#include <genesis.h>
#include <gfx.h>

#define CST_DOT_UNPACK_2 TILE_BITS
#define CST_DOT_UNPACK_3 (TILE_BITS * 2)
#define CST_DOT_UNPACK_4 (TILE_BITS * 3)

#define CST_DOT_MASK_1 ((1 << TILE_BITS) - 1)
#define CST_DOT_MASK_2 ((1 << (TILE_BITS * 2)) - 1)
#define CST_DOT_MASK_3 ((1 << (TILE_BITS * 3)) - 1)
#define CST_DOT_MASK_4 ((1 << (TILE_BITS * 4)) - 1)

#define CST_GFX_WRITE_VRAM_ADDR(adr)    ((0x4000 + ((adr) & 0x3FFF)) << 16) + (((adr) >> 14) | 0x00)
#define CST_CTRL_PORT 0xC00004
#define CST_DATA_PORT 0xC00000

static void starsGrid();

int main()
{
	starsGrid();
	return 0;
}

static void starsGrid()
{
	u16 vblCount = 0;
	u16 vramIndex = TILE_USERINDEX;

	static void inline drawStarsGrid(void)
	{
		u16 i = 0, j = 0, luma;
		u16 plan_width_by_y = 0;

	    vu32 *plctrl;
	    vu16 *pwdata;
	    u32 addr;

		plctrl = (u32 *) CST_CTRL_PORT;
	    pwdata = (u16 *) CST_DATA_PORT;

		while(j < (240 >> 3))
		{
			luma = ((i + vblCount) & 30) << 2;
			if ((j & 0x1) != 0)
				luma += 2;
		
			addr = VDP_PLAN_A + ((i + plan_width_by_y) << 1);
		    *plctrl = CST_GFX_WRITE_VRAM_ADDR(addr);
		    *pwdata = TILE_USERINDEX + luma;
		    i++;

			addr = VDP_PLAN_A + ((i + plan_width_by_y) << 1);
		    *plctrl = CST_GFX_WRITE_VRAM_ADDR(addr);
		    *pwdata = TILE_USERINDEX + luma + 1;
		    i++;		    	
	
			if (i >= 40)
			{
				i = 0;
				j++;
				plan_width_by_y = VDP_getPlanWidth() * j;
			}
		}
	}

	SYS_disableInts();

	/* Set a larger tileplan to be able to scroll */
	VDP_setPlanSize(64, 32);

	VDP_clearPlan(APLAN, 0);
	VDP_clearPlan(BPLAN, 0);

	/* Load the fond tiles */
	VDP_drawImageEx(APLAN, &stars_anim, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, vramIndex), 0, 0, FALSE, FALSE);
	vramIndex += stars_anim.tileset->numTile;

	VDP_setPalette(PAL0, stars_anim.palette->data);

    VDP_setHilightShadow(0);

	SYS_enableInts();

	while (TRUE)
	{
		VDP_waitVSync();
		drawStarsGrid();
		vblCount++;
	}
}