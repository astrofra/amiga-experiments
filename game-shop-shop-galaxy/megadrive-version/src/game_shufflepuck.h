#include <genesis.h>

#ifndef SHUFFLE_GAME
#define SHUFFLE_GAME

#define board_scale 2
#define shuffle_speed_scale 3
#define board_width FIX32(16.0 * board_scale)
#define board_length FIX32(24.0 * board_scale)

#define SFL_GAME_PRELAUNCH 0
#define SFL_GAME_LAUNCH 1
#define SFL_GAME_PLAY 2
#define SFL_GAME_GOAL 3
#define SFL_GAME_POSTGOAL 4
#define SFL_GAME_SCORE_UPD 5

/*
	Racket game object
*/
typedef struct {
	fix32	max_racket_speed;
	fix32	racket_speed;

	fix32	velocity_x,
			velocity_z;

	fix32	initial_pox_x,
			initial_pox_z;

	fix32	pos_x,
			pos_z;

	fix32	target_pos_x,
			target_pos_z;

	fix32	prev_pos_x,
			prev_pos_z;

	fix32	width,
			length;
}shuffle_racket;

/*	
	Ball game object
*/
struct {
	fix32	inertia;

	fix32	velocity_x,
			velocity_z;

	fix32	initial_pox_x,
			initial_pox_z;

	fix32	pos_x,
			pos_z;

	fix32	prev_pos_x,
			prev_pos_z;

	fix32 	radius;
}ball;

#endif