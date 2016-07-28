#include <genesis.h>
#include <gfx.h>
#include "transition_helper.h"

#define	TABLE_LEN (224 >> 3)

s16 scroll_PLAN_B[TABLE_LEN];
u8 scroll_speed[TABLE_LEN];

void menu_MainScreen()
{
	u16 vramIndex = TILE_USERINDEX;
	s16 i, ns, s;
	Sprite *sprites[2];

	SYS_disableInts();

	VDP_clearPlan(PLAN_A, 0);
	VDP_clearPlan(PLAN_B, 0);
	/* Set a larger tileplan to be able to scroll */
	VDP_setPlanSize(64, 32);
	SPR_init(0,0,0);

	/* Draw the foreground */
	// RSE_turn_screen_to_black();
	VDP_drawImageEx(PLAN_B, &starfield, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, vramIndex), 0, 0, TRUE, FALSE);
	VDP_drawImageEx(PLAN_B, &starfield, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, vramIndex), 256 >> 3, 0, TRUE, FALSE);
	vramIndex += starfield.tileset->numTile; 	

	VDP_drawImageEx(PLAN_A, &spaceship, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, vramIndex), (320 - 224) >> 4, ((224 - 88) >> 4) + 5, TRUE, FALSE);
	vramIndex += spaceship.tileset->numTile;

	VDP_setPalette(PAL2, title_0.palette->data);
	sprites[0] = SPR_addSprite(&title_0, 0, 0, TILE_ATTR_FULL(PAL2, TRUE, FALSE, FALSE, 0));
	SPR_setPosition(sprites[0], (320 - 256) >> 1, 0);

	sprites[1] = SPR_addSprite(&title_1, 0, 0, TILE_ATTR_FULL(PAL2, TRUE, FALSE, FALSE, vramIndex));
	SPR_setPosition(sprites[1], (320 - 168) >> 1, 64);	

	SPR_update();

	VDP_setScrollingMode(HSCROLL_TILE, VSCROLL_PLANE);

	SYS_enableInts();

	s = 1;
	for(i = 0; i < TABLE_LEN; i++)
	{
		scroll_PLAN_B[i] = 0;
		do
		{
			ns = -((random() % 3) + 1);
		}
		while (ns == s);
		scroll_speed[i] = ns;
		s = ns;
	}

	while (TRUE)
	{
		VDP_waitVSync();
		BMP_showFPS(1);
		VDP_setHorizontalScrollTile(PLAN_B, 2, scroll_PLAN_B, TABLE_LEN, TRUE);
		for(i = 0; i < TABLE_LEN; i++)
			scroll_PLAN_B[i] = (scroll_PLAN_B[i] + scroll_speed[i]) & 0xFF;

	}
}
