#include <genesis.h>
#include <gfx.h>

void twistedMetal(void)
{
	u16 vramIndex = TILE_USERINDEX;
	u16 scroll_jump_table[512];
	u16 i, vcount = 0;

	static void hBlank(){
		// VDP_setVerticalScroll(PLAN_A, scroll_jump_table[GET_VCOUNTER]);
		VDP_setVerticalScroll(PLAN_A, scroll_jump_table[(GET_VCOUNTER + vcount) & 511] + ((vcount << 2) & 255));
	}

	for(i = 0; i < 512; i++)
		scroll_jump_table[i] = (i * (sinFix16(i << 1) + 64)) >> 8;

	SYS_disableInts();

	/* Set a larger tileplan to be able to scroll */
	VDP_setPlanSize(32, 128);

	VDP_clearPlan(APLAN, 0);
	VDP_clearPlan(BPLAN, 0);

	/* Load the fond tiles */
	VDP_drawImageEx(APLAN, &twister, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, vramIndex), 0, 0, FALSE, FALSE);
	VDP_drawImageEx(APLAN, &twister, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, vramIndex), 0, 320 / 8, FALSE, FALSE);
	vramIndex += twister.tileset->numTile;

	VDP_setPalette(PAL0, twister.palette->data);

    VDP_setHilightShadow(0);

	SYS_enableInts();

	VDP_setHInterrupt(1);
	SYS_setHIntCallback(&hBlank);

	while (TRUE)
	{
		VDP_waitVSync();
		vcount++;
	}	
}