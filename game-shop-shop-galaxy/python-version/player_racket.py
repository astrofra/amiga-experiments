import math
import board
from utils import *

racket_speed = 20

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
	global pos_x, pos_z
	pos_x = x
	pos_z = z


def reset():
	global pos_x, pos_z, initial_pox_x, initial_pox_z, velocity_x, velocity_z
	pos_x = initial_pox_x
	pos_z = initial_pox_z
	prev_pos_x = pos_x
	prev_pos_z = pos_z	


def setMouse(x,y):
	global target_pos_x, target_pos_z

	x = Clamp(x, 0, 1.0)
	y = Clamp(y, 0, 1.0)

	target_pos_x = RangeAdjust(x, 0.0, 1.0, board.board_width * -0.5 + (width * 0.5), board.board_width * 0.5 - (width * 0.5))
	target_pos_z = RangeAdjust(y, 0.0, 0.5, board.board_length * 0.5 - (length * 0.5), board.board_length * 0.35)


def update(dt):
	global pos_x, pos_z, velocity_x, velocity_z, prev_pos_x, prev_pos_z

	prev_pos_x = pos_x
	prev_pos_z = pos_z

	pos_x += (target_pos_x - pos_x) * dt * racket_speed
	pos_z += (target_pos_z - pos_z) * dt * racket_speed

	velocity_x = pos_x - prev_pos_x
	velocity_z = pos_z - prev_pos_z
