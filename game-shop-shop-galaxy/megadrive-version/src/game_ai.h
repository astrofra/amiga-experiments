#include <genesis.h>
#include "utils.h"
#include "game_shufflepuck.h"
#include "resources.h"

#ifndef SHUFFLE_AI
#define SHUFFLE_AI

void ai_setPosition(fix32 x,fix32 z);
void ai_reset(void);
void ai_updateGameData(fix32 ball_pos_x, fix32 ball_pos_z);
void ai_update(fix32 dt);

#endif