#include <genesis.h>
#include <gfx.h>

void twistedMetal(void)
{
	u16 vramIndex = TILE_USERINDEX;

	SYS_disableInts();

	/* Set a larger tileplan to be able to scroll */
	VDP_setPlanSize(64, 64);

	VDP_clearPlan(APLAN, 0);
	VDP_clearPlan(BPLAN, 0);

	/* Load the fond tiles */
	VDP_drawImageEx(APLAN, &twister, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, vramIndex), 0, 32, FALSE, FALSE);
	vramIndex += twister.tileset->numTile;

	VDP_setPalette(PAL0, twister.palette->data);

    VDP_setHilightShadow(0);

	SYS_enableInts();

	while (TRUE)
	{
		VDP_waitVSync();
	}	
}