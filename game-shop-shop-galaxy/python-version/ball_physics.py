import math

inertia = 0.1

velocity_x = 0.0
velocity_z = 0.0

pos_x = 0.0
pos_z = 0.0

radius = 0.5

board_width = 16.0
board_length = 24.0

def reset():
	pos_x = 0.0
	pos_z = 0.0
	velocity_x = 0.0
	velocity_z = 0.0

def setImpulse(x,z):
	global velocity_x, velocity_z
	velocity_x = x
	velocity_z = z

def addVectors(ax, az, bx, bz):
	return ax + bx, az + bz

def mulVectorByScalar(ax, az, s):
	return ax * s, az * s

def getVectorLength(ax, az):
	return math.sqrt(ax * ax + az * az)


def update(dt):
	global pos_x, pos_z, velocity_x, velocity_z, board_width, board_length

	pos_x += velocity_x * dt
	pos_z += velocity_z * dt

	# basic dynamics & collision
	if pos_x > (board_width * 0.5) - radius:
		pos_x = (board_width * 0.5) - radius
		velocity_x *= -1
	else:
		if pos_x < (board_width * -0.5) + radius:
			pos_x = (board_width * -0.5) + radius
			velocity_x *= -1

	if pos_z > (board_length * 0.5) - radius:
		pos_z = (board_length * 0.5) - radius
		velocity_z *= -1
	else:
		if pos_z < (board_length * -0.5) + radius:
			pos_z = (board_length * -0.5) + radius
			velocity_z *= -1

	# Limit the friction/damping to the areas
	# where the puck can be reached by one of the players
	if abs(pos_z) > board_length * 0.25:
		friction_x, friction_z = mulVectorByScalar(velocity_x, velocity_z, -inertia * dt)
		velocity_x, velocity_z = addVectors(velocity_x, velocity_z, friction_x, friction_z)

	# pos_z = 0.5
