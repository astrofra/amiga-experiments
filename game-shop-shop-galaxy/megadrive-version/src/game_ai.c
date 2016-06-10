#include <genesis.h>
#include "utils.h"
#include "game_shufflepuck.h"
#include "resources.h"

/*
	Enemy game object
*/
shuffle_racket ai;

void ai_setPosition(fix32 x,fix32 z)
{
	ai.pos_x = x;
	ai.pos_z = z;
	ai.target_pos_x = x;
	ai.target_pos_z = z;
}

void ai_reset(void)
{
	ai.pos_x = ai.initial_pox_x;
	ai.pos_z = ai.initial_pox_z;
	ai.prev_pos_x = ai.pos_x;
	ai.prev_pos_z = ai.pos_z;
	ai.width = FIX32(2.0);
	ai.length = FIX32(0.5);
	ai.max_racket_speed = FIX32(50.0); 
	ai.racket_speed = FIX32(50.0);
}

void ai_updateGameData(fix32 ball_pos_x, fix32 ball_pos_z)
{
	ai.target_pos_x = ball_pos_x;
	ai.target_pos_z = fix32Mul(board_length, FIX32(-0.5));

	ai.racket_speed = fix32RangeAdjust(ball_pos_z, fix32Mul(board_length, FIX32(-0.5)), fix32Mul(board_length, FIX32(-0.35)), FIX32(0.0), FIX32(1.0));
	ai.racket_speed = fix32Clamp(ai.racket_speed, FIX32(0.0), FIX32(1.0));
	ai.racket_speed = fix32RangeAdjust(ai.racket_speed, FIX32(0.0), FIX32(1.0), ai.max_racket_speed, fix32Mul(ai.max_racket_speed, FIX32(0.01)));
}

void ai_update(fix32 dt)
{
	ai.prev_pos_x = ai.pos_x;
	ai.prev_pos_z = ai.pos_z;

	ai.pos_x = fix32Add(ai.pos_x, fix32Mul(fix32Sub(ai.target_pos_x, ai.pos_x), fix32Mul(dt, ai.racket_speed)));
	ai.pos_z = fix32Add(ai.pos_z, fix32Mul(fix32Sub(ai.target_pos_z, ai.pos_z), fix32Mul(dt, ai.racket_speed)));

	ai.velocity_x = fix32Sub(ai.pos_x, ai.prev_pos_x);
	ai.velocity_z = fix32Sub(ai.pos_z, ai.prev_pos_z);
}