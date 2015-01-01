## png_to_copper_list.py

import png
import math
import colorsys
import codecs

filename_in = 'ilkke_font'
filename_out = '../../source/fonts'

def quantize_color_as_OCS(_color):
	_new_color = [0,0,0]
	_new_color[0] = 2 * int(_color[0] / 2)
	_new_color[1] = 2 * int(_color[1] / 2)
	_new_color[2] = 2 * int(_color[2] / 2)
	return _new_color

def main():
	print('Font converter')

		##	Creates the header
	print('Output in : ' + filename_out + '.c')

	f = codecs.open(filename_out + '.c', 'w')
	f.write('/* Font descriptor */' + '\n\n')

	f.write('const char ' + filename_in + '_glyph_array[] = ' + '\n')
	f.write('{' + '\n')

	alphabet = 'ABCDEFGHIJKLMNOPQRSTUVWXZ'
	## \ = |
	## `= ^

	out_str = ''
	_count = 0
	for _letter in alphabet:
		_letter = _letter.replace('\\', '\\\\')
		_letter = _letter.replace('"', '\\"')
		_letter = _letter.replace('\'', '\\\'')
		# print(_letter)
		out_str += '\'' + _letter + '\', '
		_count += 1
		if _count >= 20:
			_count = 0
			f.write('\t' + out_str + '\n')
			out_str = ''

	if out_str != '':
		f.write('\t' + out_str + '\n')

	f.write('\t\'' + '\\0' + '\'\n')

	f.write('};' + '\n')

	f.write('\n')

	print('Loading bitmap : ' + filename_in + '.png')
	##	Loads the PNG image
	png_buffer = png.Reader(filename = filename_in + '.png')
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

	x_table = []
	buffer_in = list(b[2])
	for x in range(0,w):
		current_pixel = buffer_in[0][x]
		if current_pixel == 0:
			x_table.append(x)

	x_table.append(w)

	f.write('const int ' + filename_in + '_x_pos_array[] = ' + '\n')
	f.write('{' + '\n')

	out_str = ''
	_count = 0
	for x in x_table:
		out_str += str(x) + ', '
		_count += 1
		if _count >= 20:
			_count = 0
			f.write('\t' + out_str + '\n')
			out_str = ''

	if out_str != '':
		f.write('\t' + out_str + '\n')

	f.write('};' + '\n')

	f.close()

	f = codecs.open(filename_out + '.h', 'w')
	f.write('/* Font descriptor headers */' + '\n\n')

	f.write('extern const char ' + filename_in + '_glyph_array[' + str(len(alphabet)) + '];' + '\n')	
	f.write('extern const int ' + filename_in + '_x_pos_array[' + str(len(x_table)) + '];' + '\n')	

	return 1


main()