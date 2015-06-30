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

struct UCopList *copper;

UWORD chip blank_pointer[4]=
{
    0x0000, 0x0000,
    0x0000, 0x0000
};

void setCityCopperList(struct ViewPort *vp)
{
	UWORD loop, c_loop, line_index;

    copper = (struct UCopList *)
    AllocMem( sizeof(struct UCopList), MEMF_PUBLIC|MEMF_CHIP|MEMF_CLEAR );

    CINIT(copper, 32);
    CWAIT(copper, 0, 0);

    CMOVE(copper, *((UWORD *)SPR0PTH_ADDR), (LONG)&blank_pointer);
    CMOVE(copper, *((UWORD *)SPR0PTL_ADDR), (LONG)&blank_pointer);

    loop = 0;
    while(loop < CL_CITY_LEN)
    {
    	/*	Scanline position */
    	CWAIT(copper, cl_city[loop++], 0);

    	/*	How many color registers ? */
    	for (c_loop = 0; c_loop < cl_city[loop]; c_loop++)
    		CMOVE(copper, custom.color[++loop], cl_city[++loop]);
    }
    // CWAIT(copper, 63, 0);
    // CMOVE(copper, custom.color[0], 0xF0F);

    // CWAIT(copper, 96, 0);
    // CMOVE(copper, custom.color[0], 0xFF0);

    CEND(copper);

    vp->UCopIns = copper;
}