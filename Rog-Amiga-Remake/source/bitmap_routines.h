/*
    Misc bitmap routines headers
*/

#include "includes.prl"

PLANEPTR load_getmem(UBYTE *name, ULONG size);
PLANEPTR load_getchipmem(UBYTE *name, ULONG size);
struct BitMap *load_file_as_bitmap(UBYTE *name, ULONG byte_size, UWORD width, UWORD height, UWORD depth);
struct BitMap *load_array_as_bitmap(UWORD *bitmap_array, UWORD array_size, UWORD width, UWORD height, UWORD depth);
void load_into_existing_bitmap(struct BitMap *new_bitmap, BYTE *name, ULONG byte_size, UWORD depth);

void disp_whack(struct BitMap *src_BitMap, struct BitMap *dest_BitMap, UWORD width, UWORD height, UWORD x, UWORD y, UWORD depth);
void disp_interleaved_st_format(PLANEPTR data, struct BitMap *dest_BitMap, UWORD width, UWORD height, UWORD src_y, UWORD x, UWORD y, UWORD depth);


/*  Simple bitblit  */
#define BLIT_BITMAP_S(SRC_BITMAP, DEST_BITMAP, WIDTH, HEIGHT, X, Y) BltBitMap(SRC_BITMAP, 0, 0, \
            DEST_BITMAP, X, Y,  \
            WIDTH, HEIGHT,      \
            0xC0, 0xFF, NULL);

#define FREE_BITMAP(BITMAP_TMP) ;