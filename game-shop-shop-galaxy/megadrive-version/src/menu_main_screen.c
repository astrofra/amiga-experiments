#include <genesis.h>
#include <gfx.h>
#include "menu_main_screen.h"
#include "transition_helper.h"

s16 scroll_PLAN_A[TABLE_LEN];
s16 scroll_PLAN_B[TABLE_LEN];
u8 scroll_speed[TABLE_LEN];

void resetScrolling(){
	SYS_disableInts();
	VDP_setScrollingMode(HSCROLL_PLANE,VSCROLL_PLANE);
	VDP_setHorizontalScroll(PLAN_A, 0);
	VDP_setVerticalScroll(PLAN_A, 0);
	VDP_setHorizontalScroll(PLAN_B, 0);
	VDP_setVerticalScroll(PLAN_B, 0);			
	SYS_enableInts();	
}

void menu_MainScreen()
{
	u16 vramIndex = TILE_USERINDEX;
	s16 i, ns, s;
	u16 vbl_count = 0;
	u16 press_start_vis = 0;
	u16 start_is_pressed = 0;
	u16 seq = 0;
	
	struct coordS16 title_0_coord; //shop shop logo
	title_0_coord.x = -260;
	title_0_coord.y = 0;
	
	struct coordS16 title_1_coord; //galaxy logo
	title_1_coord.x = 400;
	title_1_coord.y = 64;
	
	struct coordS16 bar_coord;
	bar_coord.x = 320;
	bar_coord.y = BAR_Y;

	//backup palettes
	u16 palettes[64];
	memcpy(&palettes[0], spaceship.palette->data, 16 * 2);
	memcpy(&palettes[16], starfield.palette->data, 16 * 2);
	memcpy(&palettes[32], title_0.palette->data, 16 * 2);

	
	Sprite *sprites[SPRITE_MAX];	

	SYS_disableInts();

	VDP_clearPlan(PLAN_A, 0);
	VDP_clearPlan(PLAN_B, 0);

	/* Set a larger tileplan to be able to scroll */
	VDP_setPlanSize(64, 32);
	VDP_setHorizontalScroll(PLAN_A, PLAN_A_X);
	SPR_init(0,0,0);

	/* Draw the foreground */
	//RSE_turn_screen_to_black();
	VDP_drawImageEx(PLAN_B, &starfield, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, vramIndex), 0, 0, TRUE, FALSE);
	VDP_drawImageEx(PLAN_B, &starfield, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, vramIndex), 256 >> 3, 0, TRUE, FALSE);
	vramIndex += starfield.tileset->numTile; 	

	VDP_drawImageEx(PLAN_A, &spaceship, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, vramIndex), (320 - 224) >> 4, ((224 - 88) >> 4) + 5, TRUE, FALSE);
	vramIndex += spaceship.tileset->numTile;

	// set all palettes to black
	VDP_setPaletteColors(0, (u16*) palette_black, 64);

	sprites[0] = SPR_addSprite(&title_0, 0, 0, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));
	SPR_setPosition(sprites[0], (320 - 256) >> 1, 0);
	
	sprites[1] = SPR_addSprite(&title_1, 0, 0, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));
	SPR_setPosition(sprites[1], (320 - 168) >> 1, 64);

	sprites[2] = SPR_addSprite(&menu_press, 0, 0, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));
	SPR_setPosition(sprites[2], 0, -8);

	sprites[3] = SPR_addSprite(&menu_start, 0, 0, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));
	SPR_setPosition(sprites[3], 0, -8);
	
	sprites[4] = SPR_addSprite(&menu_bar, 0, 0, TILE_ATTR_FULL(PAL2, FALSE, FALSE, FALSE, vramIndex));
	SPR_setVisibility(sprites[4], FALSE);
	SPR_setPosition(sprites[4], 0, 224);	

	
	SPR_update();
	VDP_setScrollingMode(HSCROLL_TILE, VSCROLL_PLANE);
	SYS_enableInts();

	VDP_fadeInAll(palettes, 20, TRUE);
	
	s = 1;
	for(i = 0; i < TABLE_LEN; i++)
	{
		scroll_PLAN_A[i] = PLAN_A_X;
		scroll_PLAN_B[i] = 0;
		do
		{
			ns = -((random() % 3) + 1);
		}
		while (ns == s);
		scroll_speed[i] = ns;
		s = ns;
	}
	while (start_is_pressed == 0 && seq <= 5) 
	{
		VDP_waitVSync();
		vbl_count++;
		BMP_showFPS(1);
		VDP_setHorizontalScrollTile(PLAN_A, 1, scroll_PLAN_A, TABLE_LEN, TRUE);	
		VDP_setHorizontalScrollTile(PLAN_B, 2, scroll_PLAN_B, TABLE_LEN, TRUE);
		VDP_setVerticalScroll(PLAN_A, -20 + (sinFix16(vbl_count & 511) >> 1));
		SPR_setPosition(sprites[0], title_0_coord.x, title_0_coord.y);
		SPR_setPosition(sprites[1], title_1_coord.x, title_1_coord.y);
		switch(seq){
			case 0:
			seq++;
			break;
			
			case 1: //scrolling logo
			title_0_coord.x += 2;
			title_1_coord.x -= 2;
			if(title_0_coord.x >= (320 - 256) >> 1 ) seq++;			
			break;
			
			case 2: //white flash
			VDP_fadeAllTo(palettes_white, 20, TRUE);
			seq++;
			break;
			
			case 3:
			if(!VDP_isDoingFade()) seq++;
			break;
			
			case 4: //restore orgiginals palettes
			VDP_fadeAllTo(palettes, 150, TRUE);
			//BPLAN change scrolling speed
			for(i = 0; i < TABLE_LEN; i++){
				//Change speed PLANB
				do{
					ns = -((random() % 6) + 1);
				}
				while (ns == s);
					scroll_speed[i] = ns;
					s = ns;			
			}
			SPR_setPosition(sprites[2], PRESS_START_X, PRESS_START_Y);
			SPR_setPosition(sprites[3], PRESS_START_X + 48, PRESS_START_Y);													
			seq++;			
			break;
			
			case 5:
			// press start flash
			if(vbl_count % 40 == 0){ 
				press_start_vis = !press_start_vis;
				SPR_setVisibility(sprites[2], press_start_vis);
				SPR_setVisibility(sprites[3], press_start_vis);
			}
			//APLAN SCROLLING 
			s16 val = sinFix16( -20 + (sinFix16(vbl_count & 511) >> 1));
			for(i = 0; i < TABLE_LEN; i++){
				scroll_PLAN_A[i] = val;
			}
			//BAR
			if(bar_coord.x > BAR_X){
				bar_coord.x -= 2;
				SPR_setPosition(sprites[4], bar_coord.x, bar_coord.y);	
			}
			/*
			if(vbl_count % TIME_TO_STORY == 0{
				seq++;
				for(i = 0; i < SPRITE_MAX; i++){
					SPR_setVisibility(sprites[0], FALSE);
				}
				VDP_waitVSync();
			}
			*/
			break;			
			
			default:
			break;
		}
		
		for(i = 0; i < TABLE_LEN; i++){
			if(scroll_PLAN_A[i] < 0) scroll_PLAN_A[i] = scroll_PLAN_A[i] + 1;
			scroll_PLAN_B[i] = (scroll_PLAN_B[i] + scroll_speed[i]) & 0xFF;
		}
		SPR_update();
		if(JOY_readJoypad(0) == BUTTON_START){
			start_is_pressed = 1;
			resetScrolling();
			SPR_end();
		}
	}
	if(start_is_pressed == 0 && seq == 6){ //clear
		resetScrolling();
		SYS_disableInts();
		VDP_clearPlan(PLAN_A, 0);
		VDP_clearPlan(PLAN_B, 0);		
		SYS_enableInts();
		VDP_waitVSync();
		seq++;
	}
	while(start_is_pressed == 0 && seq >= 7){ //story
		VDP_waitVSync();
		VDP_drawText("BLA BLA BLA BLA", 0, 0);
	}
}
