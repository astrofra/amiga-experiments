.prl.gst:
	sc NOLINK CODE=F DATA=F OPTI MGST $*.gst $*.prl

.c.o:
	sc NOLINK CODE=F DATA=F OPTI TO $@ $*.c

OBJ = main.o bitmap_routines.o input_routines.o cosine_table.o image_bg_fish.o cat_copper_imgdata.o ilkke_font.o ilkke_font_coords.o

rog.exe: $(OBJ) includes.gst
	sc LINK CODE=F DATA=F OBJ $(OBJ) TO rog.exe

includes.gst: includes.prl
main.o: main.c bitmap_routines.h input_routines.h cosine_table.h image_bg_fish.h cat_copper_imgdata.h ilkke_font.h common.h includes.gst
bitmap_routines.o: bitmap_routines.c includes.gst
input_routines.o: input_routines.c includes.gst
cosine_table.o: cosine_table.c includes.gst
image_bg_fish.o: image_bg_fish.c
cat_copper_imgdata.o: cat_copper_imgdata.c
ilkke_font.o: ilkke_font.c
ilkke_font_coords.o: ilkke_font_coords.c
