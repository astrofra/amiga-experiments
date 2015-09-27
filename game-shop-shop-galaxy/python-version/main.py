import gs
import gs.plus.render as render
import gs.plus.input as input

import ball_physics as ball

def project3DTo2D():
	return ball.pos_x, ball.pos_z, 0.5

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

	render.clear()
	render.image2d(scr_margin_x, 0, SCR_SCALE_FACTOR, "@assets/game_board.png")
	render.image2d(scr_margin_x, SCR_DISP_HEIGHT - (32 * SCR_SCALE_FACTOR), SCR_SCALE_FACTOR, "@assets/game_score_panel.png")
	render.sprite2d(scr_margin_x + ball_2d_x, ball_2d_y, 36 * SCR_SCALE_FACTOR * ball_2d_scale, "@assets/game_ball.png")
	render.flip()
