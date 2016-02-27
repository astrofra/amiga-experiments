#include <genesis.h>

#define board_scale 2
#define shuffle_speed_scale 1
#define board_width FIX32(16.0 * board_scale)
#define board_length FIX32(24.0 * board_scale)

typedef struct {
	fix32	max_racket_speed;
	fix32	racket_speed;

	fix32	velocity_x,
			velocity_z;

	fix32	initial_pox_x,
			initial_pox_z;

	fix32	pos_x,
			pos_z;

	fix32	prev_pos_x,
			prev_pos_z;

	fix32	width,
			length;
}shuffle_racket;