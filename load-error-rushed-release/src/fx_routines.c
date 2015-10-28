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

extern struct Image redbot;
extern struct BitMap *bitmap_redbot;

extern struct Image astronaut;
extern struct BitMap *bitmap_astronaut;

extern struct Image demo_title;
extern struct BitMap *bitmap_demo_title;

extern struct Image face;
extern struct BitMap *bitmap_face;

extern struct Image guard;
extern struct BitMap *bitmap_guard;

extern struct Image mountain;
extern struct BitMap *bitmap_mountain;

extern struct Image mummy;
extern struct BitMap *bitmap_mummy;

extern struct Image ufo;
extern struct BitMap *bitmap_ufo;

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

void __inline loadRedbotSprite(void)
{   bitmap_redbot = load_zlib_file_as_bitmap("assets/redbot.data", 0, 18432, redbot.Width, redbot.Height, redbot.Depth);  }

void __inline drawRedbotSprite(struct BitMap *dest_bitmap)
{   
    BLIT_BITMAP_S(bitmap_redbot, dest_bitmap, redbot.Width, redbot.Height, 0, 0);
    WaitBlit();
}

void __inline freeRedbotSprite(void)
{   
    FREE_ALLOCATED_BITMAP(bitmap_redbot);
    bitmap_redbot = NULL; 
}

void __inline loadAstronautprite(void)
{   bitmap_astronaut = load_zlib_file_as_bitmap("assets/astronaut.data", 0, 10560, astronaut.Width, astronaut.Height, astronaut.Depth);  }

void __inline drawAstronautSprite(struct BitMap *dest_bitmap)
{   
    BLIT_BITMAP_S(bitmap_astronaut, dest_bitmap, astronaut.Width, astronaut.Height, 0, 0);
    WaitBlit();
}

void __inline freeAstronautSprite(void)
{   
    FREE_ALLOCATED_BITMAP(bitmap_astronaut);
    bitmap_astronaut = NULL; 
}