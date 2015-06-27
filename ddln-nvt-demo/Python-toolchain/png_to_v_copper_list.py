## png_to_copper_list.py

import png
import copy
import math
import colorsys


global fc, fh
max_color_index = 6

def main():

	root_name_list = ['city']

	filename_out = 'city'

	print('png_to_vertical_copper_list')

	fc = open('copper_lists.c', 'w')
	fc.write('/* Copper list palettes */\n')
	fc.write('\n')
	fc.write('#include <exec/types.h>\n')
	fc.write('#include <intuition/intuition.h>\n')
	fc.write('\n')

	fh = open('copper_lists.h', 'w')
	fh.write('/* Copper list palettes (headers) */\n')
	fh.write('\n')
	fh.write('#include <exec/types.h>\n')
	fh.write('#include <intuition/intuition.h>\n')	
	fh.write('\n')

	for root_name in root_name_list:
		png_list = []

		##	Get size & depth
		h = 256
		# print('w = ' + str(w) + ', h = ' + str(h))

		fc.write('UWORD cl_' + filename_out + '[] =\n')
		fc.write('{\n')
		fc.write('\t/* Line number, number of colors to update, color list [index, RGB4]*/\n')

		# fh.write('extern UWORD cl_' + filename_out + '[' + str(h) + '];\n')
		# fh.close()

		# Scan PNG line by line

		prev_line_palette = []

		for y in range(0, h):
			print("Line " + str(y))
			current_line_palette = []

			for color_idx in range(0, max_color_index):
				filename_in = 'in/' + root_name + str(color_idx) + '.png'
				print('Loading bitmap : ' + filename_in)

				# Loads the PNG image
				png_buffer = png.Reader(filename = filename_in)
				b = png_buffer.read()

				# print('bitdepth = ' + str(b[3]['bitdepth']))

				if b[3]['greyscale']:
					print('!!!Error, cannot process a greyscale image :(')
					return 0

				if b[3]['bitdepth'] > 8:
					print('!!!Error, cannot process a true color image :(')
					return 0

				original_palette = b[3]['palette']

				buffer_in = list(b[2])
				current_line = buffer_in[y]
				current_pixel = current_line[0]
				color_val = original_palette[current_pixel]
				color_hex = (int(color_val[0] / 16) << 8) + (int(color_val[1] / 16) << 4) + int(color_val[2] / 16)

				current_line_palette.append(color_hex)
				print('Found new color, index = ' + str(color_idx) + ', color = ' + str(color_val) + ', color_hex = ' + str(color_hex))

			# first_line = buffer_in[0]
			# for x in range(0,w-1):
			# 	color_idx = first_line[x]
			# 	color_val = original_palette[color_idx]
			# 	color_hex = ((color_val[0] / 16) << 8) + ((color_val[1] / 16) << 4) + (color_val[2] / 16)
			# 	color_hex = hex(color_hex)
			# 	print('color index = ' + str(color_idx) + ', color = ' + str(color_val) + ', color_hex = ' + str(color_hex))

			# 	fc.write(str(color_hex) + ',')
			# 	if ((x+1)%16) == 0:
			# 		fc.write('\n\t')

			packed_line_palette = []
			idx = 0
			for color_hex in current_line_palette:
				if len(prev_line_palette) == 0 or (len(prev_line_palette) > 0 and color_hex != prev_line_palette[idx]):
					packed_line_palette.append([idx, color_hex])
				idx += 1

			if len(packed_line_palette) > 0:
				fc.write('\t' + str(y) + ', ' + str(len(packed_line_palette)) + ', ')
				for color_tuple in packed_line_palette:
					fc.write(str(color_tuple[0]) + ', ' + str(hex(color_tuple[1])) + ', ')
				fc.write('\n')

			prev_line_palette = copy.deepcopy(current_line_palette)

		fc.write('\n')
		fc.write('};\n')
		fc.write('\n')

	fc.close()
	return 1


main()