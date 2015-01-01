## png_to_copper_list.py

import os
import png
import math
import colorsys
import random

transparent_color = [255,0,255]

def create_dummy_palette(n_color):
	tmp_pal = []
	color_component = 0
	for i in range(0,n_color - 1):
		tmp_pal.append([int(color_component),int(color_component),int(color_component)])
		color_component += (255 / n_color)

	return tmp_pal

def get_depth_from_color_amount(color_amout):
	tmp_colors_amount = 2 * int(color_amout / 2)
	if tmp_colors_amount < color_amout:
		tmp_colors_amount = tmp_colors_amount + 1
	depth = int(math.log(tmp_colors_amount, 2))
	print('get_depth_from_color_amount(%d) = %d\n' % (color_amout, depth))
	return depth

def color_distance(color1, color2):
	return math.sqrt(sum([(e1-e2)**2 for e1, e2 in zip(color1, color2)]))

def color_best_match(sample, colors):
	by_distance = sorted(colors, key=lambda c: color_distance(c, sample))
	return by_distance[0]

def create_palette_signature(pal_array):
	pal_signature = 'pal_'
	for col_index in pal_array:
		pal_signature += str(col_index)

	return pal_signature

filename_in = ['cat.png']

def color_compute_EHB_value(_color):
	_new_color = [0,0,0]
	_new_color[0] = int(_color[0] / 2)
	_new_color[1] = int(_color[1] / 2)
	_new_color[2] = int(_color[2] / 2)
	return _new_color

def quantize_color_as_OCS(_color):
	_new_color = [0,0,0]
	_new_color[0] = 2 * int(_color[0] / 2)
	_new_color[1] = 2 * int(_color[1] / 2)
	_new_color[2] = 2 * int(_color[2] / 2)
	return _new_color

def sort_palette_by_luminance(colors):
	return sorted(colors, key=lambda c: colorsys.rgb_to_hls(1.0 - c[0] / 255.0,1.0 - c[1] / 255.0,1.0 - c[2] / 255.0)[1]) ##[::-1]

global g_max_color_per_line 
g_max_color_per_line = 8

def main():
	print('png_to_copper_list')

	for _filename in filename_in:

		filename_out = _filename.replace('.png', '')

		print('Loading bitmap : ' + _filename)
		##	Loads the PNG image
		png_buffer = png.Reader(filename = _filename)
		b = png_buffer.read()
		# print(b)

		##	Get size & depth
		w = b[0]
		h = b[1]
		print('w = ' + str(w) + ', h = ' + str(h))
		print('bitdepth = ' + str(b[3]['bitdepth']))

		if b[3]['greyscale']:
			print('!!!Error, cannot process a greyscale image :(')
			return 0

		if b[3]['bitdepth'] > 8:
			print('!!!Error, cannot process a true color image :(')
			return 0

		original_palette  =	b[3]['palette']

		png_out_buffer = []
		prev_optimized_line_palette = []

		fc = open(os.path.join('../../source/', filename_out) + '_copper_list.c', 'w')
		fc.write('/* Copper list scanline palettes */\n')
		fc.write('\n')
		fc.write('#include <exec/types.h>\n')
		fc.write('#include <intuition/intuition.h>\n')
		fc.write('\n')

		fc.write('UWORD ' + filename_out + '_copper_palette[] =\n')
		fc.write('{\n')
		fc.write('\t/* line number, number of colors for this scanline, color0, color1, color2, ... */\n')

		##	Prepare the PNG out buffer
		png_writer = png.Writer(width=w, height=h, colormap=True, palette=create_dummy_palette(g_max_color_per_line), bitdepth=8)
		png_buffer_out = []

		buffer_in = list(b[2])
		##	For each line of the image
		for j in range(0, h):
			line_stat = {}
			optimized_line_palette = []
			line_out_buffer = []

			##	Calculate the occurrence of each color index
			##	used in the current pixel row
			for p in buffer_in[j]:
				if str(p) in line_stat:
					line_stat[str(p)] += 1
				else:
					line_stat[str(p)] = 1

			##  Force the occurrence of the background color
			##  so that it is always picked in priority
			line_stat['0'] = 32768

			original_line_palette = []
			for color_index_by_occurence in reversed(sorted(line_stat, key=line_stat.get)):
				original_line_palette.append(original_palette[int(color_index_by_occurence)])

			if len(original_line_palette) <= g_max_color_per_line:
				##	If there's less than 32 colors on the same row
				##	the hardware can handle it natively, there's no need
				##	to reduce the palette.
				optimized_line_palette = original_line_palette
			else:
				##	If there's more than 32 colors on the same row
				##	the hardware cannot handle it, so the palette
				##	must be reduced.
				print('Line %d : color count overflow!' % j)
				optimized_line_palette = original_line_palette[0:g_max_color_per_line]

			if create_palette_signature(optimized_line_palette) == create_palette_signature(prev_optimized_line_palette):
				print('Line %d : palette is similar to the previous line.' % j)
			else:
				print('Line %d : palette is different to the previous line.' % j)
				print('Palette signature = ' + create_palette_signature(optimized_line_palette))

				fc.write('\t')
				fc.write(str(j) + ',')
				fc.write(str(len(optimized_line_palette)) + ',')
				for color_val in optimized_line_palette:
					color_hex = ((color_val[0] / 16) << 8) + ((color_val[1] / 16) << 4) + (color_val[2] / 16)
					color_hex = hex(color_hex)
					fc.write(str(color_hex) + ',')

				fc.write('\n')

			for p in buffer_in[j]:
				# line_out_buffer.append(int(7 * random.random()))
				_col_idx = 0
				for _col in optimized_line_palette:
					if _col == original_palette[p]:
						line_out_buffer.append(_col_idx)
						break
					_col_idx += 1

			png_buffer_out.append(line_out_buffer)

			prev_optimized_line_palette = optimized_line_palette

		fc.write('}\n')

		fc.close()

		png_file_out = open(filename_out + '_copper.png', 'wb')
		png_writer.write(png_file_out, png_buffer_out)
		png_file_out.close()

	return 1


main()