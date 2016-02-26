#include <genesis.h>
#include <gfx.h>
#include "utils.h"
#include "game_shufflepuck.h"

static void game_ShufflePuck();

int main()
{
	game_ShufflePuck();
	return 0;
}

/*	
	Ball game object
*/
struct {
	fix16	inertia;

	fix16	velocity_x,
			velocity_z;

	fix16	initial_pox_x,
			initial_pox_z;

	fix16	pos_x,
			pos_z;

	fix16	prev_pos_x,
			prev_pos_z;

	fix16 	radius;
}ball;

/*
	Player game object
*/
struct {
	fix16	racket_speed;

	fix16	velocity_x,
			velocity_z;

	fix16	initial_pox_x,
			initial_pox_z;

	fix16	pos_x,
			pos_z;

	fix16	prev_pos_x,
			prev_pos_z;

	fix16	width,
			length;
}racket;

/*
	Enemy game object
*/
struct {
	fix16	max_racket_speed;
	fix16	racket_speed;

	fix16	velocity_x,
			velocity_z;

	fix16	initial_pox_x,
			initial_pox_z;

	fix16	pos_x,
			pos_z;

	fix16	prev_pos_x,
			prev_pos_z;

	fix16	width,
			length;
}ai;

static void game_ShufflePuck()
{
	char str[16];	/* debug string */
	u16 vblCount = 0;
	u16 vramIndex = TILE_USERINDEX;
	Sprite sprites[16];

	/* Ball sprite coordinates */
	s16 ball_2d_x,
		ball_2d_y,
		ball_2d_scale;

	const fix16 persp_coef[] = {fix16DivFloats(1.0, 132.0), fix16DivFloats(5, 132.0), fix16DivFloats(9, 132.0), fix16DivFloats(13, 132.0), fix16DivFloats(17, 132.0), 
								fix16DivFloats(22, 132.0), fix16DivFloats(27, 132.0), fix16DivFloats(34, 132.0), fix16DivFloats(42, 132.0), fix16DivFloats(51, 132.0), 
								fix16DivFloats(64, 132.0), fix16DivFloats(80, 132.0), fix16DivFloats(102, 132.0), fix16DivFloats(132, 132.0)};

	/*	Specific 3D -> 2D Projection */
	static Vect3D_f16 inline project3DTo2D(fix16 x, fix16 z)
	{
		const fix16 top_left_x = FIX16(120);
		const fix16 top_right_x = FIX16(320 - 120);
		const fix16 top_y = FIX16(8);

		const fix16 bottom_left_x = FIX16(0);
		const fix16 bottom_right_x = FIX16(320);
		const fix16 bottom_y = FIX16(130);

		fix16 top_2d_x, bottom_2d_x;

		Vect3D_f16 ret_tuple;

		fix16 norm_x = fix16Add(fix16Div(x, board_width), FIX16(0.5));
		fix16 norm_y = fix16Add(fix16Div(z, board_length), FIX16(0.5));

		norm_y = fix16mapValueToArray(norm_y, FIX16(0.0), FIX16(1.0), persp_coef, 14);
		
		top_2d_x = fix16Add(fix16Mul(fix16InvCoef(norm_x), top_left_x), fix16Mul(norm_x, top_right_x));
		bottom_2d_x = fix16Add(fix16Mul(fix16InvCoef(norm_x), bottom_left_x), fix16Mul(norm_x, bottom_right_x));

		ret_tuple.x = fix16Add(fix16Mul(fix16InvCoef(norm_y), top_2d_x),  fix16Mul(norm_y, bottom_2d_x)); /* proj_2d_x */
		ret_tuple.y = fix16Add(fix16Mul(fix16InvCoef(norm_y), top_y), fix16Mul(norm_y, bottom_y));	/* proj_2d_y */
		ret_tuple.z = fix16RangeAdjust(norm_y, FIX16(0.0), FIX16(1.0), FIX16(0.285), FIX16(1.0));	/* proj_scale */

		return ret_tuple;
	}

	/*	Ball logic */
	void ball_reset(void){
		// global pos_x, pos_z, initial_pox_x, initial_pox_z
		ball.inertia = FIX16(0.1);
		ball.radius = FIX16(0.5);
		ball.initial_pox_x = 0;
		ball.initial_pox_z = 0;
		ball.pos_x = ball.initial_pox_x;
		ball.pos_z = ball.initial_pox_z;
		ball.prev_pos_x = ball.pos_x;
		ball.prev_pos_z = ball.pos_z;
		ball.velocity_x = 0;
		ball.velocity_z = 0;
	}

	void ball_setImpulse(fix16 x, fix16 z){
		ball.velocity_x = x;
		ball.velocity_z = z;
	}

	void ball_bounceX(void){
		fix16Mul(ball.velocity_x, FIX16(-1.0));
	}	

	void ball_bounceZ(void){
		fix16Mul(ball.velocity_z, FIX16(-1.0));
	}

	void ball_setPosition(fix16 x, fix16 z){
		ball.pos_x = x;
		ball.pos_z = z;
	}

	void ball_update(fix16 dt){
		// global pos_x, pos_z, velocity_x, velocity_z, prev_pos_x, prev_pos_z

		/*  Keep track of the ball's previous position */
		ball.prev_pos_x = ball.pos_x;
		ball.prev_pos_z = ball.pos_z;

		/* Move the ball according to its velocity */
		ball.pos_x = fix16Add(ball.pos_x, fix16Mul(ball.velocity_x, dt));
		ball.pos_z = fix16Add(ball.pos_z, fix16Mul(ball.velocity_z, dt));

		/* basic dynamics & collision */
		if (ball.pos_x > fix16Mul(board_width, FIX16(0.5))){
			ball.pos_x = fix16Mul(board_width, FIX16(0.5));
			ball_bounceX();
		}
		else{
			if (ball.pos_x < fix16Mul(board_width, FIX16(-0.5))){
				ball.pos_x = fix16Mul(board_width, FIX16(-0.5));
				ball_bounceX();
			}
		}

		if (ball.pos_z < fix16Mul(board_length, FIX16(-0.5))){
			ball.pos_z = fix16Mul(board_length, FIX16(-0.5));
			ball_bounceZ();
		}

		if (ball.pos_z > fix16Mul(board_length, FIX16(0.5))){
			ball.pos_x = ball.initial_pox_x;
			ball.pos_z = ball.initial_pox_z;
			ball.prev_pos_x = ball.pos_x;
			ball.prev_pos_z = ball.pos_z;
		}

		/*	Limit the friction/damping to the areas
			where the puck can be reached by one of the players */
		// if (abs(pos_z) > board_length * 0.25):
		// 	friction_x, friction_z = mulVectorByScalar(velocity_x, velocity_z, -inertia * dt)
		// 	velocity_x, velocity_z = addVectors(velocity_x, velocity_z, friction_x, friction_z)	
	}

	void gameReset(void){		
		ball_reset();
		ball_setImpulse(FIX16(10.0), FIX16(10.0));
	}

	void renderBall(u16 ball_2d_x,u16  ball_2d_y, u16 ball_2d_scale){
		// ball_2d_y += (240 - 136);

		intToStr(ball_2d_x, str, 0);
		BMP_drawText(str, 0, 0);	
		intToStr(ball_2d_y, str, 0);
		BMP_drawText(str, 10, 0);		
		SPR_setPosition(&sprites[0], ball_2d_x, ball_2d_y); //  - 65);
		// render.sprite2d(SCR_MARGIN_X + ball_2d_x, ball_2d_y - (65 * SCR_SCALE_FACTOR), 24 * SCR_SCALE_FACTOR * ball_2d_scale, "@assets/game_ball.png")
	}

	void renderPlayer(u16 player_2d_x, u16 player_2d_y, u16 player_2d_scale){
		// render.sprite2d(SCR_MARGIN_X + player_2d_x, player_2d_y - (65 * SCR_SCALE_FACTOR), 64 * SCR_SCALE_FACTOR * player_2d_scale, "@assets/game_racket.png")
	}

	void renderAI(u16 ai_2d_x, u16 ai_2d_y, u16 ai_2d_scale){
		// render.sprite2d(SCR_MARGIN_X + ai_2d_x, ai_2d_y - (65 * SCR_SCALE_FACTOR), 64 * SCR_SCALE_FACTOR * ai_2d_scale, "@assets/game_racket.png")
	}

	u8 ballIsBehindRacket(void){
		if (ball.pos_z < racket.pos_z)
			return TRUE;
		else
			return FALSE;
	}

	u8 BallIsWithinXReach(void){
		if (fix16Add(ball.pos_x, ball.radius) > fix16Sub(racket.pos_x, fix16Mul(racket.width, FIX16(0.5))) 
			&& fix16Sub(ball.pos_x, ball.radius) < fix16Add(racket.pos_x, fix16Mul(racket.width, FIX16(0.5))))
			return TRUE;
		else
			return FALSE;
	}

	u8 BallWasWithinXReach(void){
		if (fix16Add(ball.prev_pos_x, ball.radius) > fix16Sub(racket.prev_pos_x,  fix16Mul(racket.width, FIX16(0.5))) 
			&& fix16Sub(ball.prev_pos_x, ball.radius) < fix16Add(racket.prev_pos_x, fix16Mul(racket.width, FIX16(0.5))))
			return TRUE;
		else
			return FALSE;
	}

	void gameMainLoop(fix16 dt){
		/* Update the ball motion */
		ball_update(dt);

		/* Update the player motion */
		// player_setMouse(mouse_device.GetValue(gs.InputDevice.InputAxisX) / SCR_DISP_WIDTH, mouse_device.GetValue(gs.InputDevice.InputAxisY) / SCR_DISP_HEIGHT);
		// player_update(dt);

		/* Update the AI */
		// ai_updateGameData(ball.pos_x, ball.pos_z, board.board_width, board.board_length);
		// ai_update(dt);

		/* Collisions */
		// if (ball.velocity_z > 0.0)
		// {
		// 	if (!ballIsBehindRacket(ball, player)) && (BallWasWithinXReach(ball, player) or BallIsWithinXReach(ball, player))
		// 	{
		// 		ball.setPosition(ball.pos_x, player.pos_z - ball.velocity_z * dt + min(0.0, player.velocity_z) * dt);
		// 		player.setPosition(player.pos_x, ball.pos_z + player.length);
		// 		ball.bounceZ();
		// 	}
		// }

		/* Compute 3D/2D projections */
		Vect3D_f16 pvect; 
		pvect = project3DTo2D(ball.pos_x, ball.pos_z);
		ball_2d_x = pvect.x;
		ball_2d_y = pvect.y;
		ball_2d_scale = pvect.z;

		// ball_2d_x *= SCR_SCALE_FACTOR
		// ball_2d_y = SCR_DISP_HEIGHT - (ball_2d_y * SCR_SCALE_FACTOR)

		// player_2d_x, player_2d_y, player_2d_scale = project3DTo2D(player.pos_x, player.pos_z, board.board_width, board.board_length)
		// player_2d_x *= SCR_SCALE_FACTOR
		// player_2d_y = SCR_DISP_HEIGHT - (player_2d_y * SCR_SCALE_FACTOR)

		// ai_2d_x, ai_2d_y, ai_2d_scale = project3DTo2D(ai.pos_x, ai.pos_z, board.board_width, board.board_length)
		// ai_2d_x *= SCR_SCALE_FACTOR
		// ai_2d_y = SCR_DISP_HEIGHT - (ai_2d_y * SCR_SCALE_FACTOR)

		// render.clear()
		// render.set_blend_mode2d(render.BlendAlpha)
		// /* Opponent */
		// render.sprite2d(SCR_MARGIN_X + (320 * 0.5) * SCR_SCALE_FACTOR, (SCR_PHYSIC_HEIGHT - 96 * 0.5) * SCR_SCALE_FACTOR, 106 * SCR_SCALE_FACTOR, "@assets/robot5.png")

		// /* Game board */
		// render.image2d(SCR_MARGIN_X, 0, SCR_SCALE_FACTOR, "@assets/game_board.png")

		// /* Score panel */
		// render.image2d(SCR_MARGIN_X, SCR_DISP_HEIGHT - (32 * SCR_SCALE_FACTOR), SCR_SCALE_FACTOR, "@assets/game_score_panel.png")

		/* Render moving items according to their Z position */
		// renderAI(ai_2d_x, ai_2d_y, ai_2d_scale)

		// if (ball.pos_z - ball.radius < player.pos_z + player.length)
		// {
			renderBall(fix16ToInt(ball_2d_x), fix16ToInt(ball_2d_y), ball_2d_scale);
		// 	renderPlayer(player_2d_x, player_2d_y, player_2d_scale)
		// }
		// else
		// {
		// 	renderPlayer(player_2d_x, player_2d_y, player_2d_scale)
		// 	renderBall(ball_2d_x, ball_2d_y, ball_2d_scale)
		// }

		// render.set_blend_mode2d(render.BlendOpaque)
	}

	/*	System stuff */
	SYS_disableInts();

	/* Set a larger tileplan to be able to scroll */
	VDP_setPlanSize(64, 32);
	SPR_init(256);

	VDP_clearPlan(APLAN, 0);
	VDP_clearPlan(BPLAN, 0);

	/* Load the fond tiles */
	VDP_drawImageEx(APLAN, &game_robot_5, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, vramIndex), (320 - 112) / 16, (240 - 112) / 64	, FALSE, FALSE);
	vramIndex += game_robot_5.tileset->numTile;

	VDP_drawImageEx(BPLAN, &game_board, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, vramIndex), 0, (240 - 136) / 8, FALSE, FALSE);
	vramIndex += game_board.tileset->numTile;	

	VDP_setPalette(PAL0, game_robot_5.palette->data);
	VDP_setPalette(PAL1, game_board.palette->data);
	VDP_setPalette(PAL2, game_ball.palette->data);

    SPR_initSprite(&sprites[0], &game_ball, 0, 0, TILE_ATTR_FULL(PAL2, FALSE, FALSE, FALSE, 0));
 //    // SPR_initSprite(&sprites[1], &rse_logo_shadow_alt, 0, 0, TILE_ATTR_FULL(PAL2, FALSE, FALSE, FALSE, 0));
	SPR_setPosition(&sprites[0], 64, 64);
 	SPR_update(sprites, 1);	

	SYS_enableInts();

	gameReset();

	while (TRUE)
	{
		VDP_waitVSync();
		gameMainLoop(FIX16(1.0/60.0));
	    SPR_update(sprites, 1);	
		vblCount++;
	}
}
