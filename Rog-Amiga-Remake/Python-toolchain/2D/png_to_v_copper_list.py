## png_to_copper_list.py

import png
import math
import colorsys

filename_in = ['gradient.png', 'blue_gradient.png']
filename_out = '../../Assets/vert_copper_palettes'
global fc, fh

def main():
	print('png_to_vertical_copper_list')

	fc = open(filename_out + '.c', 'w')
	fc.write('/* Vertical copper list palettes */\n')
	fc.write('\n')
	fc.write('#include <exec/types.h>\n')
	fc.write('#include <intuition/intuition.h>\n')
	fc.write('\n')

	fh = open(filename_out + '.h', 'w')
	fh.write('/* Vertical copper list palettes (headers) */\n')
	fh.write('\n')
	fh.write('#include <exec/types.h>\n')
	fh.write('#include <intuition/intuition.h>\n')	
	fh.write('\n')

	for _filename in filename_in:
		print('Loading bitmap : ' + _filename)
		fc.write('UWORD vertical_copper_pal_' + _filename.replace('.png', '') + '[] =\n')
		fc.write('{\n')
		fc.write('\t')

		##	Loads the PNG image
		png_buffer = png.Reader(filename = _filename)
		b = png_buffer.read()
		# print(b)

		##	Get size & depth
		w = b[0]
		h = b[1]
		print('w = ' + str(w) + ', h = ' + str(h))

		fh.write('extern UWORD vertical_copper_pal_' + _filename.replace('.png', '') + '[' + str(w) + '];\n')

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

		buffer_in = list(b[2])
		##	For each line of the image

		first_line = buffer_in[0]
		for x in range(0,49):
			color_idx = first_line[x]
			color_val = original_palette[color_idx]
			color_hex = ((color_val[0] / 16) << 8) + ((color_val[1] / 16) << 4) + (color_val[2] / 16)
			color_hex = hex(color_hex)
			print('color index = ' + str(color_idx) + ', color = ' + str(color_val) + ', color_hex = ' + str(color_hex))

			fc.write(str(color_hex) + ',')
			if ((x+1)%16) == 0:
				fc.write('\n\t')

		fc.write('\n')
		fc.write('};\n')
		fc.write('\n')

	fc.close()
	return 1


main()