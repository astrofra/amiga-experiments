import math
import board
from utils import *

velocity_x = 0.0
velocity_z = 0.0

initial_pox_x = 0.0
initial_pox_z = 0.0

pos_x = 0.0
pos_z = 0.0

racket_width = 2.0
racket_length = 0.5

def reset():
	global pos_x, pos_z, initial_pox_x, initial_pox_z
	pos_x = initial_pox_x
	pos_z = initial_pox_z

def setMouse(x,y):
	global pos_x, pos_z
	x = Clamp(x, 0, 1.0)
	y = Clamp(y, 0, 1.0)
	pos_x = RangeAdjust(x, 0.0, 1.0, board.board_width * -0.5 + (racket_width * 0.5), board.board_width * 0.5 - (racket_width * 0.5))
	pos_z = RangeAdjust(y, 0.5, 1.0, board.board_length * 0.5 - (racket_length * 0.5), board.board_length * 0.35)

def update(dt):
	global pos_x, pos_z, velocity_x, velocity_z
