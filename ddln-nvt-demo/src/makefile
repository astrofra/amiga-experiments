.prl.gst:
	sc NOLINK CODE=F DATA=F OPTI MGST $*.gst $*.prl

.c.o:
	sc NOLINK CODE=F DATA=F OPTI TO $@ $*.c

OBJ = main.o \
color_routines.o bitmap_routines.o font_routines.o sound_routines.o voxel_routines.o fx_routines.o bitmap_desc.o cosine_table.o font_desc.o copper_lists.o demo_strings.o font_bitmap.o

main.exe: $(OBJ) includes.gst
	sc LINK CODE=F DATA=F OBJ $(OBJ) TO main.exe

includes.gst: includes.prl
main.o: main.c tinfl.c screen_size.h sound_routines.h bitmap_routines.h color_routines.h font_routines.h fx_routines.h cosine_table.h font_desc.h copper_lists.h demo_strings.h font_bitmap.h includes.gst
color_routines.o: color_routines.c
bitmap_routines.o: bitmap_routines.c
font_routines.o: font_routines.c
sound_routines.o: sound_routines.c sound_routines.h
voxel_routines.o: voxel_routines.c voxel_routines.h	
cosine_table.o: cosine_table.c
font_desc.o: font_desc.c
bitmap_desc.o: bitmap_desc.c
copper_lists.o: copper_lists.c copper_lists.h
fx_routines.o: fx_routines.c fx_routines.h
font_bitmap.o: font_bitmap.c font_bitmap.h
demo_strings.o: demo_strings.c