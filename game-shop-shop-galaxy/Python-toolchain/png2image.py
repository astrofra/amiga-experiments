#!/usr/bin/python

"""
For license, see gpl-3.0.txt
"""
from PIL import Image
import os
import argparse
import math
import pickle

input_path = "../work/amiga_assets/to_encode/"

def chunks(l, n):
	for i in xrange(0, len(l), n):
		yield l[i:i + n]


def color_to_plane_bits(color, depth):
	"""returns the bits for a given pixel in a list, lowest to highest plane"""
	result = [0] * depth
	for bit in range(depth):
		if color & (1 << bit) != 0:
			result[bit] = 1
	return result


def color_to_RGB4(rgb_color_triplet):
	final_color = 0
	final_color += rgb_color_triplet[2] / 16
	final_color += ((rgb_color_triplet[1] / 16) << 4)
	final_color += ((rgb_color_triplet[0] / 16) << 8)
	return final_color


def write_amiga_image(image, destfile):
	destfile = os.path.splitext(destfile)[0] 

	imdata = im.getdata()
	width, height = im.size
	colors = [i for i in chunks(map(ord, im.palette.tostring()), 3)]
	colors_amount = 2 * int(len(colors) / 2)
	if colors_amount < len(colors):
		colors_amount = len(colors) + 1
	depth = int(math.log(colors_amount, 2))
	print("width, height, colors, depth = %d, %d, %d, %d." % (width, height, len(colors), depth))

	map_words_per_row = width / 16
	if width % 16 > 0:
		map_words_per_row += 1

	# create the converted planar data
	planes = [[0] * (map_words_per_row * height) for _ in range(depth)]
	for y in range(height):
		x = 0
		while x < width:
			# build a word for each plane
			for i in range(min(16, width - x)):
				# get the palette index for pixel (x + i, y)
				color = imdata[y * width + x + i]  # color index
				planebits = color_to_plane_bits(color, depth)
				# now we need to "or" the bits into the words in their respective planes
				wordidx = (x + i) / 16  # word number in current row
				pos = y * map_words_per_row + wordidx  # list index in the plane
				for planeidx in range(depth):
					if planebits[planeidx]:
						planes[planeidx][pos] |= (1 << (15 - (x + i) % 16)) # 1 << ((x + i) % 16)
			x += 16

	##  Header file
	with open(destfile + '.h', 'w') as h_outfile:
		h_outfile.write('#include <intuition/intuition.h>\n\n')
		h_outfile.write('extern UWORD %s_palRGB4[%d];\n' % (destfile, len(colors)))
		h_outfile.write('extern UWORD %s_img[%d];\n' % (destfile, depth * map_words_per_row * height))
		h_outfile.write('\n')

		planepick = 2 ** depth - 1  # currently we always use all of the planes
		h_outfile.write('struct Image %s_image = {\n' % destfile)
		h_outfile.write('\t0, 0, %d, %d, %d, %s_imdata,\n' % (width, height, depth, destfile));
		h_outfile.write('\t%d, 0, NULL\n' % planepick)  # PlanePick, PlaneOnOff
		h_outfile.write('};\n')

	with open(destfile + '.bin', 'wb') as bin_outfile:
		for plane in planes:
			print(plane)
		# 	for chunk in chunks(plane, 1):
		# 		# for int_b in chunk:
		# 		# 	b = bin(int_b)
		# 		# 	bin_outfile.write(b)
			pickle.dump(plane, bin_outfile, pickle.HIGHEST_PROTOCOL)

	##  C file
	with open(destfile + '.c', 'w') as c_outfile:
		c_outfile.write('#include <intuition/intuition.h>\n\n')

		##  Palettes
		c_outfile.write("/* Image palette, RGB4 format (OCS/ECS machines) */\n");
		c_outfile.write('UWORD %s_palRGB4[] = {\n' % destfile)
		c_outfile.write('\t')
		tpal = im.palette.tostring()
		tpal = map(ord, tpal)
		tpal = chunks(tpal, 3)
		for color_rgb in chunks(map(ord, im.palette.tostring()), 3):
			c_outfile.write(str(hex(color_to_RGB4(color_rgb))) + ',')
		c_outfile.write('\n')
		c_outfile.write('};\n\n')

		##  Bitplanes
		c_outfile.write("/* Ensure that this data is within chip memory or you'll see nothing !!! */\n");
		c_outfile.write('UWORD %s_img[] = {\n' % destfile)
		plane_idx = 0
		for plane in planes:
			c_outfile.write("\t/* Bitplane #%d */\n" % (plane_idx));
			for chunk in chunks(plane, map_words_per_row):
				c_outfile.write('\t')
				c_outfile.write(','.join(map(str, map(hex, chunk))))
				c_outfile.write(',\n')
			c_outfile.write('\n')
			plane_idx += 1
		c_outfile.write('};\n\n')

if __name__ == '__main__':
	for filename in os.listdir(input_path):
		path_filename = os.path.join(input_path, filename)
		print(path_filename)
		if filename.find('.png') > -1:
			im = Image.open(path_filename)
			write_amiga_image(im, os.path.join(input_path, "out.c"))

# if __name__ == '__main__':
# 	parser = argparse.ArgumentParser(description='Amiga Image converter')
# 	parser.add_argument('--pngfile', required=True)
# 	parser.add_argument('--destfile', required=True)

# 	args = parser.parse_args()
# 	im = Image.open(args.pngfile)
# 	write_amiga_image(im, args.destfile)
# 	# im = Image.open('ilkke_font.png') ##args.pngfile)
# 	# write_amiga_image(im, 'test2.h') ##args.destfile)

