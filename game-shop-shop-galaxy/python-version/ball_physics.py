import math
import board
from basic_vector import *

inertia = 0.1

velocity_x = 0.0
velocity_z = 0.0

initial_pox_x = 0.0
initial_pox_z = 0.0

pos_x = 0.0
pos_z = 0.0

prev_pos_x = 0.0
prev_pos_z = 0.0

radius = 0.5

def reset():
	global pos_x, pos_z, initial_pox_x, initial_pox_z
	pos_x = initial_pox_x
	pos_z = initial_pox_z
	prev_pos_x = pos_x
	prev_pos_z = pos_z
	velocity_x = 0.0
	velocity_z = 0.0

def setImpulse(x,z):
	global velocity_x, velocity_z
	velocity_x = x
	velocity_z = z

def bounceX():
	global velocity_x
	velocity_x *= -1

def bounceZ():
	global velocity_z
	velocity_z *= -1

def setPosition(x,z):
	global pos_x, pos_z
	pos_x = x
	pos_z = z

def update(dt):
	global pos_x, pos_z, velocity_x, velocity_z, prev_pos_x, prev_pos_z

	# Keep track of the ball's previous position
	prev_pos_x = pos_x
	prev_pos_z = pos_z

	# Move the ball according to its velocity
	pos_x += velocity_x * dt
	pos_z += velocity_z * dt

	# basic dynamics & collision
	if pos_x > (board.board_width * 0.5): # - radius:
		pos_x = (board.board_width * 0.5) # - radius
		bounceX()
	else:
		if pos_x < (board.board_width * -0.5): # + radius:
			pos_x = (board.board_width * -0.5) # + radius
			bounceX()

	if pos_z > (board.board_length * 0.5): # - radius:
		pos_z = (board.board_length * 0.5) # - radius
		bounceZ()
	else:
		if pos_z < (board.board_length * -0.5): # + radius:
			pos_z = (board.board_length * -0.5) # + radius
			bounceZ()

	# Limit the friction/damping to the areas
	# where the puck can be reached by one of the players
	# if abs(pos_z) > board.board_length * 0.25:
	# 	friction_x, friction_z = mulVectorByScalar(velocity_x, velocity_z, -inertia * dt)
	# 	velocity_x, velocity_z = addVectors(velocity_x, velocity_z, friction_x, friction_z)
