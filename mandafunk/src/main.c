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
	u16 vblCount = 0, i;
	u16 vramIndex = TILE_USERINDEX;
	s16 tile_scroll_h[1024],
		tile_scroll_v[1024];

	static void inline drawStarsGrid(void)
	{
		u16 i = 0, j = 0, k = 0, m, luma;
		u16 plan_width_by_y = 0;

	    vu32 *plctrl;
	    vu16 *pwdata;
	    u32 addr;

		plctrl = (u32 *) CST_CTRL_PORT;
	    pwdata = (u16 *) CST_DATA_PORT;

		while(j < 32)
		{
			luma = ((i + vblCount) & 30) << 2;
			m = (vblCount & 0x1) << 1;
		
			addr = VDP_PLAN_A + ((i + k + plan_width_by_y) << 1);
		    *plctrl = CST_GFX_WRITE_VRAM_ADDR(addr);
		    *pwdata = TILE_USERINDEX + luma;

			addr += 2;
		    *plctrl = CST_GFX_WRITE_VRAM_ADDR(addr);
		    *pwdata = TILE_USERINDEX + luma + 1;

			addr = VDP_PLAN_A + ((i + k + VDP_getPlanWidth() * (j + 1)) << 1);
		    *plctrl = CST_GFX_WRITE_VRAM_ADDR(addr);
		    *pwdata = TILE_USERINDEX + luma + 2;

			addr += 2;
		    *plctrl = CST_GFX_WRITE_VRAM_ADDR(addr);
		    *pwdata = TILE_USERINDEX + luma + 3;		    

		    i += 5;	
	
			if (i >= 40)
			{
				i = 0;
				j += 3;
				if (k == 0)
					k = 3;
				else
					k = 0;
				plan_width_by_y = VDP_getPlanWidth() * j;
				// return;
			}
		}
	}

	for(i = 0; i < 1024; i++)
	{
		tile_scroll_h[i] = sinFix16(i << 2) / 2;
		tile_scroll_v[i] = cosFix16(i << 2) / 4;
	}

	SYS_disableInts();

	/* Set a larger tileplan to be able to scroll */
	VDP_setPlanSize(64, 32);

	/* Load the fond tiles */
	VDP_drawImageEx(APLAN, &stars_anim, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, vramIndex), 0, 0, FALSE, FALSE);
	vramIndex += stars_anim.tileset->numTile;

	VDP_clearPlan(APLAN, 0);
	VDP_clearPlan(BPLAN, 0);

	VDP_setPalette(PAL0, stars_anim.palette->data);

    VDP_setHilightShadow(0);

    VDP_setScrollingMode(HSCROLL_TILE, VSCROLL_2TILE);

	SYS_enableInts();

	while (TRUE)
	{
		VDP_waitVSync();
		drawStarsGrid();
		vblCount++;
		VDP_setHorizontalScrollTile(PLAN_A, 0, tile_scroll_h + (vblCount & 1023), 32, TRUE);
		VDP_setVerticalScrollTile(PLAN_A, 0, tile_scroll_v + ((vblCount << 1) & 1023), 32, TRUE);
	}
}