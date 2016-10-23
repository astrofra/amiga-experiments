#include <genesis.h>
#include <gfx.h>
#include "utils.h"
#include "fix32_patch.h"
#include "game_shufflepuck.h"
#include "game_ai.h"
#include "game_player.h"
#include "resources.h"

extern shuffle_racket ai;
extern shuffle_racket player;

void game_ShufflePuck()
{
	char str[32];	/* debug string */
	u16 vblCount = 0;
	u16 vramIndex = TILE_USERINDEX;
	Sprite *sprites[16];
	Vect3D_f32 pvect;
	fix32 joy_x, joy_y;

	/* Ball sprite coordinates */
	int ball_2d_x,
		ball_2d_y,
		ball_2d_scale;

	/* AI racket sprite coordinates */
	int ai_2d_x,
		ai_2d_y,
		ai_2d_scale;

	/* Player racket sprite coordinates */
	int player_2d_x,
		player_2d_y,
		player_2d_scale;

	/* Gameplay data */
	short sfl_game_state;
	int player_score = 0,
		ai_score = 0;

	u16 slf_game_timer = 0;

	const fix32 persp_coef[] = {fix32DivFloats(1.0, 132.0), fix32DivFloats(5, 132.0), fix32DivFloats(9, 132.0), fix32DivFloats(13, 132.0), fix32DivFloats(17, 132.0), 
								fix32DivFloats(22, 132.0), fix32DivFloats(27, 132.0), fix32DivFloats(34, 132.0), fix32DivFloats(42, 132.0), fix32DivFloats(51, 132.0), 
								fix32DivFloats(64, 132.0), fix32DivFloats(80, 132.0), fix32DivFloats(102, 132.0), fix32DivFloats(132, 132.0)};

	/*	Specific 3D -> 2D Projection */
	static Vect3D_f32 inline project3DTo2D(fix32 x, fix32 z)
	{
		const fix32 top_left_x = FIX32(120);
		const fix32 top_right_x = FIX32(320 - 120);
		const fix32 top_y = FIX32(8);

		const fix32 bottom_left_x = FIX32(0);
		const fix32 bottom_right_x = FIX32(320);
		const fix32 bottom_y = FIX32(130);

		fix32 top_2d_x, bottom_2d_x;

		Vect3D_f32 ret_tuple;

		fix32 norm_x = fix32Add(RSE_fix32Div(x, board_width), FIX32(0.5));
		fix32 norm_y = fix32Add(RSE_fix32Div(z, board_length), FIX32(0.5));

		norm_y = fix32mapValueToArray(norm_y, FIX32(0.0), FIX32(1.0), persp_coef, 14);
		
		top_2d_x = fix32Add(RSE_fix32Mul(fix32InvCoef(norm_x), top_left_x), RSE_fix32Mul(norm_x, top_right_x));
		bottom_2d_x = fix32Add(RSE_fix32Mul(fix32InvCoef(norm_x), bottom_left_x), RSE_fix32Mul(norm_x, bottom_right_x));

		ret_tuple.x = fix32Add(RSE_fix32Mul(fix32InvCoef(norm_y), top_2d_x),  RSE_fix32Mul(norm_y, bottom_2d_x)); /* proj_2d_x */
		ret_tuple.y = fix32Add(RSE_fix32Mul(fix32InvCoef(norm_y), top_y), RSE_fix32Mul(norm_y, bottom_y));	/* proj_2d_y */
		ret_tuple.z = fix32Sub(FIX32(1.0), norm_y); // fix32RangeAdjust(norm_y, FIX32(0.0), FIX32(1.0), FIX32(0.285), FIX32(1.0));	/* proj_scale */

		return ret_tuple;
	}

	/*	Ball logic */
	void ball_reset(void){
		// global pos_x, pos_z, initial_pox_x, initial_pox_z
		ball.inertia = FIX32(0.0); // FIX32(0.1);
		ball.radius = FIX32(0.5 * board_scale);
		ball.initial_pox_x = 0;
		ball.initial_pox_z = 0;
		ball.pos_x = ball.initial_pox_x;
		ball.pos_z = ball.initial_pox_z;
		ball.prev_pos_x = ball.pos_x;
		ball.prev_pos_z = ball.pos_z;
		ball.velocity_x = 0;
		ball.velocity_z = 0;
	}

	void ball_setImpulse(fix32 x, fix32 z){
		ball.velocity_x = x;
		ball.velocity_z = z;
	}

	void ball_bounceX(void){
		ball.velocity_x = RSE_fix32Mul(ball.velocity_x, FIX32(-1.0));
	}	

	void ball_bounceZ(void){
		ball.velocity_z = RSE_fix32Mul(ball.velocity_z, FIX32(-1.0));
	}

	void ball_setPosition(fix32 x, fix32 z){
		ball.pos_x = x;
		ball.pos_z = z;
	}

	void ball_update(fix32 dt){
		// global pos_x, pos_z, velocity_x, velocity_z, prev_pos_x, prev_pos_z

		/*  Keep track of the ball's previous position */
		ball.prev_pos_x = ball.pos_x;
		ball.prev_pos_z = ball.pos_z;

		/* Move the ball according to its velocity */
		ball.pos_x = fix32Add(ball.pos_x, RSE_fix32Mul(ball.velocity_x, dt));
		ball.pos_z = fix32Add(ball.pos_z, RSE_fix32Mul(ball.velocity_z, dt));

		/* basic dynamics & collision */
		if (ball.pos_x > RSE_fix32Mul(board_width, FIX32(0.5))){
			ball.pos_x = RSE_fix32Mul(board_width, FIX32(0.5));
			ball_bounceX();
		}
		else{
			if (ball.pos_x < RSE_fix32Mul(board_width, FIX32(-0.5))){
				ball.pos_x = RSE_fix32Mul(board_width, FIX32(-0.5));
				ball_bounceX();
			}
		}

		/*	Ball hits player's goal */
		if (ball.pos_z > RSE_fix32Mul(board_length, FIX32(0.5))){
			ball.pos_z = RSE_fix32Mul(board_length, FIX32(0.5));
			ball_bounceZ();
			ball.inertia = FIX32(10.0);
			ai_score++;
			sfl_game_state = SFL_GAME_GOAL;
		}
		else{
			/*	Ball hits ai's goal */
			if (ball.pos_z < RSE_fix32Mul(board_length, FIX32(-0.5))){
				ball.pos_z = RSE_fix32Mul(board_length, FIX32(-0.5));
				ball_bounceZ();
				ball.inertia = FIX32(10.0);
				player_score++;
				sfl_game_state = SFL_GAME_GOAL;
			}
		}

		// if (ball.pos_z > RSE_fix32Mul(board_length, FIX32(0.5))){
		// 	ball.pos_x = ball.initial_pox_x;
		// 	ball.pos_z = ball.initial_pox_z;
		// 	ball.prev_pos_x = ball.pos_x;
		// 	ball.prev_pos_z = ball.pos_z;
		// }

		/*	Limit the friction/damping to the areas
			where the puck can be reached by one of the players */
		if (ball.inertia > FIX32(0.0))
		{
			if (abs(ball.pos_z) > RSE_fix32Mul(board_length , FIX32(0.25)))
			{
				ball.velocity_x = fix32Sub(ball.velocity_x, RSE_fix32Mul(RSE_fix32Mul(ball.velocity_x, ball.inertia), dt));
				ball.velocity_z = fix32Sub(ball.velocity_z, RSE_fix32Mul(RSE_fix32Mul(ball.velocity_z, ball.inertia), dt));
			}
		}

		// BMP_drawText("dt = ", 0, 0);
		// fix32ToStr(dt, str, 8);
		// BMP_drawText(str, 6, 0);	

		// fix32ToStr(ball.velocity_x, str, 8);
		// BMP_drawText(str, 0, 1);	
		// fix32ToStr(ball.velocity_z, str, 8);
		// BMP_drawText(str, 10, 1);
		// fix32ToStr(ball.pos_x, str, 8);
		// BMP_drawText(str, 0, 2);	
		// fix32ToStr(ball.pos_z, str, 8);
		// BMP_drawText(str, 10, 2);			
	}

	void renderBall(int ball_2d_x, int ball_2d_y, int ball_2d_scale){
		// ball_2d_y += (240 - 136);

		// intToStr(ball_2d_x, str, 0);
		// BMP_drawText(str, 0, 3);	
		// intToStr(ball_2d_y, str, 0);
		// BMP_drawText(str, 10, 3);
		SPR_setPosition(sprites[1], ball_2d_x - 12, ball_2d_y + ((224 - 136) - 6));
		SPR_setFrame(sprites[1], ball_2d_scale);
	}

	void renderPlayer(int player_2d_x, int player_2d_y, int player_2d_scale){
		SPR_setPosition(sprites[0], player_2d_x - 32, player_2d_y + ((224 - 136) - 16));
		SPR_setFrame(sprites[0], player_2d_scale);
		// render.sprite2d(SCR_MARGIN_X + player_2d_x, player_2d_y - (65 * SCR_SCALE_FACTOR), 64 * SCR_SCALE_FACTOR * player_2d_scale, "@assets/game_racket.png")
	}

	void renderAI(int ai_2d_x, int ai_2d_y, int ai_2d_scale){
		SPR_setPosition(sprites[2], ai_2d_x - 32, ai_2d_y + ((224 - 136) - 16));
		SPR_setFrame(sprites[2], ai_2d_scale);
		// render.sprite2d(SCR_MARGIN_X + ai_2d_x, ai_2d_y - (65 * SCR_SCALE_FACTOR), 64 * SCR_SCALE_FACTOR * ai_2d_scale, "@assets/game_racket.png")
	}

	/*	Collision detection */
	/* 	Player */
	u8 ballIsBehindRacket(void){
		if (ball.pos_z < player.pos_z)
			return TRUE;
		else
			return FALSE;
	}

	u8 BallIsWithinXReach(void){
		if (fix32Add(ball.pos_x, ball.radius) > fix32Sub(player.pos_x, RSE_fix32Mul(player.width, FIX32(0.5))) 
			&& fix32Sub(ball.pos_x, ball.radius) < fix32Add(player.pos_x, RSE_fix32Mul(player.width, FIX32(0.5))))
			return TRUE;
		else
			return FALSE;
	}

	u8 BallWasWithinXReach(void){
		if (fix32Add(ball.prev_pos_x, ball.radius) > fix32Sub(player.prev_pos_x,  RSE_fix32Mul(player.width, FIX32(0.5))) 
			&& fix32Sub(ball.prev_pos_x, ball.radius) < fix32Add(player.prev_pos_x, RSE_fix32Mul(player.width, FIX32(0.5))))
			return TRUE;
		else
			return FALSE;
	}

	/* AI */
	u8 ballIsBehindAIRacket(void){
		if (ball.pos_z > ai.pos_z)
			return TRUE;
		else
			return FALSE;
	}

	u8 BallIsWithinAIXReach(void){
		if (fix32Add(ball.pos_x, ball.radius) > fix32Sub(ai.pos_x, RSE_fix32Mul(ai.width, FIX32(0.5))) 
			&& fix32Sub(ball.pos_x, ball.radius) < fix32Add(ai.pos_x, RSE_fix32Mul(ai.width, FIX32(0.5))))
			return TRUE;
		else
			return FALSE;
	}

	u8 BallWasWithinAIXReach(void){
		if (fix32Add(ball.prev_pos_x, ball.radius) > fix32Sub(ai.prev_pos_x,  RSE_fix32Mul(ai.width, FIX32(0.5))) 
			&& fix32Sub(ball.prev_pos_x, ball.radius) < fix32Add(ai.prev_pos_x, RSE_fix32Mul(ai.width, FIX32(0.5))))
			return TRUE;
		else
			return FALSE;
	}


	void gameReset(void){		
		ai_reset();
		player_reset();
		ball_reset();
	}	

	void gameMainLoop(fix32 dt, u8 upd_player_motion){
		/* Update the ball motion */
		ball_update(dt);

		if (upd_player_motion)
		{
			/* Update the player motion */
			joy_x = 0;
			if (JOY_readJoypad(0) & BUTTON_LEFT)
				joy_x = FIX32(-1.0);
			else
			if (JOY_readJoypad(0) & BUTTON_RIGHT)
				joy_x = FIX32(1.0);

			joy_y = 0;
			if (JOY_readJoypad(0) & BUTTON_UP)
				joy_y = FIX32(-1.0);
			else
			if (JOY_readJoypad(0) & BUTTON_DOWN)
				joy_y = FIX32(1.0);		

			player_setControler(joy_x, joy_y);
			player_update(dt);

			/* Update the AI */
			ai_updateGameData(ball.pos_x, ball.pos_z);
			ai_update(dt);
		}

		/* Collisions */
		if (ball.velocity_z > FIX32(0.0))
		{
			/* Ball vs Player */
			if ((!ballIsBehindRacket()) && (BallWasWithinXReach() || BallIsWithinXReach()))
			{
				ball_setPosition(ball.pos_x, player.pos_z - RSE_fix32Mul(ball.velocity_z, dt) + RSE_fix32Mul(fix32Min(FIX32(0.0), player.velocity_z), dt));
				player_setTargetPosition(player.pos_x, ball.pos_z + player.length);
				ball_bounceZ();
			}
		}
		else
		{
			/* Ball vs AI */
			if ((!ballIsBehindAIRacket()) && (BallWasWithinAIXReach() || BallIsWithinAIXReach()))
			{
				ball_setPosition(ball.pos_x, ai.pos_z + RSE_fix32Mul(ball.velocity_z, dt) - RSE_fix32Mul(fix32Min(FIX32(0.0), ai.velocity_z), dt));
				ai_setTargetPosition(ai.pos_x, ball.pos_z - ai.length);
				ball_bounceZ();
			}
		}

		/* Compute 3D/2D projections */
		pvect = project3DTo2D(ball.pos_x, ball.pos_z);
		ball_2d_x = pvect.x;
		ball_2d_y = pvect.y;
		ball_2d_scale = fix32ToInt(RSE_fix32Mul(pvect.z, FIX32(16.0)));

		pvect = project3DTo2D(player.pos_x, player.pos_z);
		player_2d_x = pvect.x;
		player_2d_y = pvect.y;
		player_2d_scale =  fix32ToInt(RSE_fix32Mul(pvect.z, FIX32(16.0)));

		pvect = project3DTo2D(ai.pos_x, ai.pos_z);
		ai_2d_x = pvect.x;
		ai_2d_y = pvect.y;
		ai_2d_scale =fix32ToInt(RSE_fix32Mul(pvect.z, FIX32(16.0)));

		// /* Opponent */
		// render.sprite2d(SCR_MARGIN_X + (320 * 0.5) * SCR_SCALE_FACTOR, (SCR_PHYSIC_HEIGHT - 96 * 0.5) * SCR_SCALE_FACTOR, 106 * SCR_SCALE_FACTOR, "@assets/robot5.png")

		// /* Game board */
		// render.image2d(SCR_MARGIN_X, 0, SCR_SCALE_FACTOR, "@assets/game_board.png")

		// /* Score panel */
		// render.image2d(SCR_MARGIN_X, SCR_DISP_HEIGHT - (32 * SCR_SCALE_FACTOR), SCR_SCALE_FACTOR, "@assets/game_score_panel.png")

		/* Render moving items according to their Z position */
		renderAI(fix32ToInt(ai_2d_x), fix32ToInt(ai_2d_y), ai_2d_scale);

		// if (ball.pos_z - ball.radius < player.pos_z + player.length)
		// {
			renderBall(fix32ToInt(ball_2d_x), fix32ToInt(ball_2d_y), ball_2d_scale);
			renderPlayer(fix32ToInt(player_2d_x), fix32ToInt(player_2d_y), player_2d_scale);
		// }
		// else
		// {
		// 	renderPlayer(player_2d_x, player_2d_y, player_2d_scale)
		// 	renderBall(ball_2d_x, ball_2d_y, ball_2d_scale)
		// }

		// render.set_blend_mode2d(render.BlendOpaque)
	}

	void gameUpdateScoreDisplay(void)
	{
		intToStr(player_score, str, 2);
		BMP_drawText(str, 6, 0);
		intToStr(ai_score, str, 2);
		BMP_drawText(str, 6, 1);
	}

	/*	System stuff */
	SYS_disableInts();

	/* Set a larger tileplan to be able to scroll */
	VDP_setPlanSize(64, 32);
	SPR_init(0,0,0);

	VDP_clearPlan(PLAN_A, 0);
	VDP_clearPlan(PLAN_B, 0);

	/* Load the fond tiles */
	VDP_drawImageEx(PLAN_A, &game_robot_5, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, vramIndex), (320 - 112) / 16, (224 - 112) / 64	, FALSE, FALSE);
	vramIndex += game_robot_5.tileset->numTile;

	VDP_drawImageEx(PLAN_B, &game_board, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, vramIndex), 0, (224 - 136) / 8, FALSE, FALSE);
	vramIndex += game_board.tileset->numTile;	

	VDP_setPalette(PAL0, game_robot_5.palette->data);
	VDP_setPalette(PAL1, game_board.palette->data);
	VDP_setPalette(PAL2, game_ball.palette->data);

	/* Ball sprite */
	sprites[1] = SPR_addSprite(&game_ball, 0, 0, TILE_ATTR_FULL(PAL2, TRUE, FALSE, FALSE, 0));
	SPR_setPosition(sprites[1], 64, 64);

	/* Player racket sprite */
	sprites[0] = SPR_addSprite(&game_racket, 0, 0, TILE_ATTR_FULL(PAL2, TRUE, FALSE, FALSE, 0));
	SPR_setPosition(sprites[0], (320 / 2) - 32, 224 - 32);

	/* AI racket sprite */
	sprites[2] = SPR_addSprite(&game_racket, 0, 0, TILE_ATTR_FULL(PAL2, TRUE, FALSE, FALSE, 0));
	SPR_setPosition(sprites[2], (320 / 2) - 32, 64);

 	SPR_update();	

	SYS_enableInts();

	// SND_startPlay_XGM(maak_music_2);
	// SND_setMusicTempo_XGM(50);		

	gameReset();
	gameUpdateScoreDisplay();

	sfl_game_state = SFL_GAME_PRELAUNCH;

	while (TRUE)
	{
		VDP_waitVSync();
		slf_game_timer++;
		// BMP_showFPS(1);
		// utils_unit_tests();

		switch(sfl_game_state)
		{
			case SFL_GAME_PRELAUNCH:
				gameReset();
				slf_game_timer = 0;
				sfl_game_state = SFL_GAME_LAUNCH;
				break;

			case SFL_GAME_LAUNCH:
				gameMainLoop(FIX32(1.0/(shuffle_speed_scale * 60.0)), FALSE);

				if (slf_game_timer > 1 * 60)
				{
					ball_setImpulse(FIX32(10.0 * board_scale * shuffle_speed_scale), FIX32(10.0 * board_scale * shuffle_speed_scale));
					sfl_game_state = SFL_GAME_PLAY;
				}

				break;

			case SFL_GAME_PLAY:
				gameMainLoop(FIX32(1.0/(shuffle_speed_scale * 60.0)), TRUE);
				break;

			case SFL_GAME_GOAL:
				slf_game_timer = 0;
				sfl_game_state = SFL_GAME_POSTGOAL;
				break;

			case SFL_GAME_POSTGOAL:
				gameMainLoop(FIX32(1.0/(shuffle_speed_scale * 60.0)), FALSE);

				if (slf_game_timer > 2 * 60)
					sfl_game_state = SFL_GAME_SCORE_UPD;

				break;

			case SFL_GAME_SCORE_UPD:
				gameUpdateScoreDisplay();
				sfl_game_state = SFL_GAME_PRELAUNCH;
				break;
		}

		SPR_update();	
		// vblCount++;
	}
}