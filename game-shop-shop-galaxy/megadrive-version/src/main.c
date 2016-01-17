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

static void game_ShufflePuck()
{
	u16 vblCount = 0;
	u16 vramIndex = TILE_USERINDEX;
	Sprite sprites[16];	

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

		if (ball.pos_z > fix16Mul(board_length, FIX16(-0.5))){
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

	SYS_disableInts();

	/* Set a larger tileplan to be able to scroll */
	VDP_setPlanSize(64, 32);

	VDP_clearPlan(APLAN, 0);
	VDP_clearPlan(BPLAN, 0);

	/* Load the fond tiles */
	VDP_drawImageEx(APLAN, &game_robot_5, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, vramIndex), (320 - 112) / 16, (240 - 112) / 64	, FALSE, FALSE);
	vramIndex += game_robot_5.tileset->numTile;

	VDP_drawImageEx(BPLAN, &game_board, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, vramIndex), 0, (240 - 136) / 8, FALSE, FALSE);
	vramIndex += game_board.tileset->numTile;	

	VDP_setPalette(PAL0, game_robot_5.palette->data);
	VDP_setPalette(PAL1, game_board.palette->data);

	// SPR_init(257);
 //    SPR_initSprite(&sprites[0], &rse_logo_shadow, 0, 0, TILE_ATTR_FULL(PAL2, FALSE, FALSE, FALSE, 0));
 //    // SPR_initSprite(&sprites[1], &rse_logo_shadow_alt, 0, 0, TILE_ATTR_FULL(PAL2, FALSE, FALSE, FALSE, 0));
	// SPR_setPosition(&sprites[0], (320 - 160) >> 1, ((240 - 50) >> 1) + 8);
 //    SPR_update(sprites, 1);	

	SYS_enableInts();

	while (TRUE)
	{
		VDP_waitVSync();
	    // SPR_update(sprites, 1);	
		vblCount++;
	}
}
