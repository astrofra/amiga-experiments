#include <genesis.h>
#include "utils.h"
#include "game_shufflepuck.h"
#include "resources.h"

#ifndef SHUFFLE_PLAYER
#define SHUFFLE_PLAYER

void player_setPosition(fix32 x,fix32 z);
void player_setTargetPosition(fix32 x,fix32 z);
void player_reset(void);
void player_setControler(fix32 joy_x, fix32 joy_z);
void player_update(fix32 dt);

#endif