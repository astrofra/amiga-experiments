import math

velocity_x = 0.0
velocity_z = 0.0

pos_x = 0.0
pos_z = 0.0

radius = 1.0

board_width = 16.0
board_length = 24.0

def reset():
	pos_x = 0.0
	pos_z = 0.0
	velocity_x = 0.0
	velocity_z = 0.0

def applyImpulse(x,z):
	global velocity_x, velocity_z
	velocity_x = x
	velocity_z = z

def update(dt):
	global pos_x, pos_z, velocity_x, velocity_z, board_width, board_length
	# pos_x = board_width * 0.5
	pos_x += velocity_x * dt
	pos_z += velocity_z * dt

	# basic dynamics & collision
	if pos_x > board_width * 0.5:
		pos_x = board_width * 0.5
		velocity_x *= -1
	else:
		if pos_x < board_width * -0.5:
			pos_x = board_width * -0.5
			velocity_x *= -1

	if pos_z > board_length * 0.5:
		pos_z = board_length * 0.5
		velocity_z *= -1
	else:
		if pos_z < board_length * -0.5:
			pos_z = board_length * -0.5
			velocity_z *= -1

	# pos_z = 0.5
