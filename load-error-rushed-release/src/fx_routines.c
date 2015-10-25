#include "includes.prl"
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfxmacros.h>
#include <hardware/custom.h>

#include "board.h"
#include "screen_size.h"
#include "color_routines.h"
#include "bitmap_routines.h"
#include "cosine_table.h"
#include "copper_lists.h"

extern struct Custom far custom;

extern struct GfxBase *GfxBase;
extern struct ViewPort view_port1;

extern int dbuffer_offset_1;
extern int dbuffer_offset_2;

extern struct Image dog_16c;
extern struct BitMap *bitmap_dog;

struct UCopList *copper;

UWORD chip blank_pointer[4]=
{
    0x0000, 0x0000,
    0x0000, 0x0000
};

#define RASTER_CLEAR_HSTEP 64
BOOL progressiveClearRaster(struct RastPort *rp, unsigned int fx_clock, const int max_width, const int max_height, UBYTE color_index)
{
    short x, max_x;
    x = fx_clock * RASTER_CLEAR_HSTEP;

    if (x >= max_width)
        return FALSE;

    max_x = x + RASTER_CLEAR_HSTEP;
    if (max_x >= max_width)
        max_x = max_width - 1;
    SetAPen(rp, color_index);
    RectFill(rp, x, 0, x + RASTER_CLEAR_HSTEP, max_height - 1);

    return TRUE;
}

void setEmptyCopperList(struct ViewPort *vp)
{
    copper = (struct UCopList *)
    AllocMem( sizeof(struct UCopList), MEMF_PUBLIC|MEMF_CHIP|MEMF_CLEAR );

    CINIT(copper, 16);
    CWAIT(copper, 0, 0);

    CMOVE(copper, *((UWORD *)SPR0PTH_ADDR), (LONG)&blank_pointer);
    CMOVE(copper, *((UWORD *)SPR0PTL_ADDR), (LONG)&blank_pointer);

    CEND(copper);

    vp->UCopIns = copper;    
}

void __inline loadDogSprite(void)
{   bitmap_dog = load_file_as_bitmap("assets/dog.dat", 0, dog_16c.Width, dog_16c.Height, dog_16c.Depth);  }

void __inline drawDogSprite(struct BitMap *dest_bitmap)
{   
    BLIT_BITMAP_S(bitmap_dog, dest_bitmap, dog_16c.Width, dog_16c.Height, DEFAULT_WIDTH >> 1, DEFAULT_HEIGHT >> 1);
    WaitBlit();
}

void __inline freeDogSprite(void)
{   
    FREE_ALLOCATED_BITMAP(bitmap_dog);
    bitmap_dog = NULL; 
}