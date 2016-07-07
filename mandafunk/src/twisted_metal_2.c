#include <genesis.h>
#include <gfx.h>

#define TWISTER_TABLE_SIZE 1024
#define CST_CTRL_PORT 0xC00004
#define CST_DATA_PORT 0xC00000
#define CST_WRITE_VSRAM_ADDR(adr)   ((0x4000 + ((adr) & 0x3FFF)) << 16) + (((adr) >> 14) | 0x10)

s16 twister_jump_table[TWISTER_TABLE_SIZE];
// s16 scroll_jump_table_bg[TWISTER_TABLE_SIZE];
// s16 hscroll_jump_table[TWISTER_TABLE_SIZE];
// s16 hscroll_jump_table_bg[TWISTER_TABLE_SIZE];
// s16 hblank_table[TWISTER_TABLE_SIZE];
// s16 hblank_table_bg[TWISTER_TABLE_SIZE];
// s16 hscroll_table[256];
// s16 hscroll_table_bg[256];


void twistedMetal2(void)
{
	u16 vramIndex = TILE_USERINDEX;
	s16 i, rot_y = 0, ang_speed_y = 4, twist_y = 0, twist_inc = 0;
	u16 vcount = 0;

    vu16 *pw;
    vu32 *pl;

    pw = (u16 *) GFX_DATA_PORT;
    pl = (u32 *) GFX_CTRL_PORT;

	static void hBlank(){
		// VDP_setVerticalScroll(PLAN_A, scroll_jump_table[(GET_VCOUNTER + rot_y) & ((TWISTER_TABLE_SIZE >> 2) - 1)] + (64 + sinFix16(rot_y << 1)) << 1);

	    *pl = CST_WRITE_VSRAM_ADDR(0);
	    *pw = twister_jump_table[(GET_VCOUNTER + rot_y + (twist_y >> 6)) & ((TWISTER_TABLE_SIZE >> 1) - 1)];
	    twist_y += twist_inc;

	    // *pl = CST_WRITE_VSRAM_ADDR(2);
	    // *pw = scroll_jump_table_bg[(GET_VCOUNTER + rot_y) & ((TWISTER_TABLE_SIZE >> 1) - 1)] + hblank_table_bg[rot_y];	    
	}

	SYS_disableInts();

	/* Set a larger tileplan to be able to scroll */
	VDP_setPlanSize(64, 64);

	VDP_clearPlan(APLAN, 0);
	VDP_clearPlan(BPLAN, 0);
	VDP_setScrollingMode(HSCROLL_LINE, VSCROLL_PLANE);

	/* Load the fond tiles */
	VDP_drawImageEx(APLAN, &twister, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, vramIndex), 8, 0, FALSE, FALSE);
	VDP_drawImageEx(APLAN, &twister, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, vramIndex), 8, 256 / 8, FALSE, FALSE);
	vramIndex += twister.tileset->numTile;

	VDP_setPalette(PAL0, twister.palette->data);
	VDP_setPaletteColor(0, 0x80A);

	// /* Load the fond tiles */
	// VDP_drawImageEx(BPLAN, &twister_bg, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, vramIndex), 8, 0, FALSE, FALSE);
	// VDP_drawImageEx(BPLAN, &twister_bg, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, vramIndex), 8, 256 / 8, FALSE, FALSE);
	// vramIndex += twister_bg.tileset->numTile;

	// VDP_setPalette(PAL1, twister_bg.palette->data);


    VDP_setHilightShadow(0);

    for(i = 0; i < TWISTER_TABLE_SIZE; i++)
    	twister_jump_table[i] = -i;

	SYS_enableInts();

	VDP_setHInterrupt(1);
	SYS_setHIntCallback(&hBlank);

	while (TRUE)
	{
		VDP_waitVSync();
		ang_speed_y = sinFix16(vcount) >> 4;
		rot_y += ang_speed_y;
		rot_y = rot_y & 0xFF;
		twist_y = 0;
		twist_inc = sinFix16(vcount);
		vcount += 4;
	}	
}