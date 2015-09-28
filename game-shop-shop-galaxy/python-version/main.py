import gs
import gs.plus.render as render
import gs.plus.input as input

import math
import ball_physics as ball

def RangeAdjust(val, in_lower, in_upper, out_lower, out_upper):
    return (val - in_lower) / (in_upper - in_lower) * (out_upper - out_lower) + out_lower

def mapValueToArray(val, in_lower, in_upper, mapping_array):
	val = RangeAdjust(val, in_lower, in_upper, 0.0, 1.0)
	array_pos = val * (len(mapping_array) - 1)
	ceil_pos = math.ceil(array_pos)
	floor_pos = math.floor(array_pos)
	return mapping_array[floor_pos] * float(ceil_pos - array_pos) + mapping_array[ceil_pos] * (1.0 - (float(ceil_pos - array_pos)))

def project3DTo2D():
	top_left_x = 120
	top_right_x = 320 - 120
	top_y = 8

	bottom_left_x = 0
	bottom_right_x = 320
	bottom_y = 130

	norm_x = (ball.pos_x / ball.board_width) + 0.5
	norm_y = (ball.pos_z / ball.board_length) + 0.5

	persp_coef = [2, 5, 9, 13, 17, 22, 27, 34, 42, 51, 64, 80, 102, 132]
	persp_coef = list(map(lambda z: z / 132.0, persp_coef))
	norm_y = mapValueToArray(norm_y, 0.0, 1.0, persp_coef)
	
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
