import math
import game_puck_board as board
from utils import *

max_racket_speed = 50
racket_speed = max_racket_speed

velocity_x = 0.0
velocity_z = 0.0

initial_pox_x = 0.0
initial_pox_z = 0.0

pos_x = 0.0
pos_z = 0.0

target_pos_x = 0.0
target_pos_z = 0.0

prev_pos_x = 0.0
prev_pos_z = 0.0

width = 2.0
length = 0.5

def setPosition(x,z):
	global pos_x, pos_z, target_pos_x, target_pos_z
	pos_x, pos_z = x, z
	target_pos_x, target_pos_z = x, z


def reset():
	global pos_x, pos_z, initial_pox_x, initial_pox_z, velocity_x, velocity_z
	pos_x = initial_pox_x
	pos_z = initial_pox_z
	prev_pos_x = pos_x
	prev_pos_z = pos_z	


def updateGameData(ball_pos_x, ball_pos_z, board_width, board_length):
	global target_pos_x, target_pos_z, racket_speed

	target_pos_x = ball_pos_x
	target_pos_z = board_length * -0.5

	racket_speed = RangeAdjust(ball_pos_z, board_length * -0.5, board_length * -0.35, 0.0, 1.0)
	racket_speed = Clamp(racket_speed, 0.0, 1.0)
	racket_speed = RangeAdjust(racket_speed, 0.0, 1.0, max_racket_speed, max_racket_speed * 0.01)


def update(dt):
	global pos_x, pos_z, velocity_x, velocity_z, prev_pos_x, prev_pos_z

	prev_pos_x = pos_x
	prev_pos_z = pos_z

	pos_x += (target_pos_x - pos_x) * dt * racket_speed
	pos_z += (target_pos_z - pos_z) * dt * max_racket_speed

	velocity_x = pos_x - prev_pos_x
	velocity_z = pos_z - prev_pos_z
