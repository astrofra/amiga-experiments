import gs
import gs.plus.render as render
import gs.plus.input as input

import math
import ball_physics as ball

def RangeAdjust(val, in_lower, in_upper, out_lower, out_upper):
    return (val - in_lower) / (in_upper - in_lower) * (out_upper - out_lower) + out_lower

def project3DTo2D():
	top_left_x = 120
	top_right_x = 320 - 120
	top_y = 8

	## Polynomial[{(0.0, 0.0), (0.25, 0.143), (0.5, 0.31), (0.75, 0.53), (1.0, 1.0)}]
	## Polynomial[{(0.0, 0.0), (0.0833, 0.0), (0.1667, 0.0), (0.25, 0.0), (0.3333, 0.0), (0.4167, 0.0), (0.5, 0.0), (0.5833, 0.0), (0.6667, 0.0), (0.75, 0.0), (0.8333, 0.0), (0.9167, 0.0), (1.0, 0.0)}]
	# Polynomial[{(0.0, 0.0), (0.0769, 0.0458), (0.1538, 0.0687), (0.2308, 0.0916), (0.3077, 0.1298), (0.3846, 0.1603), (0.4615, 0.2061), (0.5385, 0.2595), (0.6154, 0.3206), (0.6923, 0.3893), (0.7692, 0.4809), (0.8462, 0.6107), (0.9231, 0.7786), (1.0, 1.0) }]
# Polynomial[{(0.0, 0.0), (0.0769, 0.0458), (0.1538, 0.0687), (0.3077, 0.1298), (0.4615, 0.2061), (0.6154, 0.3206), (0.7692, 0.4809), (0.9231, 0.7786), (1.0, 1.0) }]
	persp = [0.0, 0.143, 0.31, 0.53, 1.0]

	bottom_left_x = 0
	bottom_right_x = 320
	bottom_y = 130

	norm_x = (ball.pos_x / ball.board_width) + 0.5
	norm_y = (ball.pos_z / ball.board_length) + 0.5

	norm_y = 1.79 * math.pow(norm_y, 4.0) - 2.38 * math.pow(norm_y, 3.0) + 1.19 * math.pow(norm_y, 2.0) + 0.39 * norm_y
	# norm_y = - 37.23 * math.pow(norm_y, 7.0) + 132.21 * math.pow(norm_y, 6.0) - 183.2 * math.pow(norm_y, 5.0) + 125.88 * math.pow(norm_y, 4.0) - 43.8 * math.pow(norm_y, 3.0) + 7.11 * math.pow(norm_y, 2.0) + 0.02 * norm_y

	# if norm_y < 0.25:
	# 	norm_y = RangeAdjust(norm_y, 0.0, 0.25, 0.0, 0.143)
	# else:
	# 	if norm_y > 0.25 and norm_y < 0.5:
	# 		norm_y = RangeAdjust(norm_y, 0.25, 0.5, 0.143, 0.31)
	# 	else:
	# 		if norm_y > 0.5 and norm_y < 0.75:
	# 			norm_y = RangeAdjust(norm_y, 0.5, 0.75, 0.31, 0.53)
	# 		else:
	# 			norm_y = RangeAdjust(norm_y, 0.75, 1.0, 0.53, 1.0)

	top_2d_x = (1.0 - norm_x) * top_left_x + norm_x * top_right_x
	bottom_2d_x = (1.0 - norm_x) * bottom_left_x + norm_x * bottom_right_x

	proj_2d_x = (1.0 - norm_y) * top_2d_x + norm_y * bottom_2d_x
	proj_2d_y = (1.0 - norm_y) * top_y + norm_y * bottom_y

	proj_scale = (norm_y + 1.0) * 0.5

	return proj_2d_x, proj_2d_y, proj_scale

SCR_PHYSIC_WIDTH = 320
SCR_PHYSIC_HEIGHT = 200

SCR_DISP_WIDTH = 320 * 2
SCR_DISP_HEIGHT = 200 * 2

SCR_SCALE_FACTOR = min(SCR_DISP_WIDTH / SCR_PHYSIC_WIDTH, SCR_DISP_HEIGHT / SCR_PHYSIC_HEIGHT)

render.init(SCR_DISP_WIDTH, SCR_DISP_HEIGHT, "pkg.core")

# provide access to the data folder
gs.MountFileDriver(gs.StdFileDriver("assets/"), "@assets/")

ball.reset()
ball.applyImpulse(10.0, 10.0)

while not input.key_press(gs.InputDevice.KeyEscape):
	scr_margin_x = (SCR_DISP_WIDTH - (SCR_PHYSIC_WIDTH * SCR_SCALE_FACTOR)) / 2.0
	ball.update(1.0 / 60.0)
	ball_2d_x, ball_2d_y, ball_2d_scale = project3DTo2D()
	ball_2d_x *= SCR_SCALE_FACTOR
	ball_2d_y = SCR_DISP_HEIGHT - (ball_2d_y * SCR_SCALE_FACTOR)

	render.clear()
	render.image2d(scr_margin_x, 0, SCR_SCALE_FACTOR, "@assets/game_board.png")
	render.image2d(scr_margin_x, SCR_DISP_HEIGHT - (32 * SCR_SCALE_FACTOR), SCR_SCALE_FACTOR, "@assets/game_score_panel.png")
	render.sprite2d(scr_margin_x + ball_2d_x, ball_2d_y - 130, 36 * SCR_SCALE_FACTOR * ball_2d_scale, "@assets/game_ball.png")
	render.flip()
