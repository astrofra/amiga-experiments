import gs
import gs.plus.render as render
import gs.plus.input as input

import ball_physics as ball

SCR_PHYSIC_WIDTH = 320
SCR_PHYSIC_HEIGHT = 200

SCR_DISP_WIDTH = 320 * 2
SCR_DISP_HEIGHT = 200 * 2

SCR_SCALE_FACTOR = min(SCR_DISP_WIDTH / SCR_PHYSIC_WIDTH, SCR_DISP_HEIGHT / SCR_PHYSIC_HEIGHT)

render.init(SCR_DISP_WIDTH, SCR_DISP_HEIGHT, "pkg.core")

# provide access to the data folder
gs.MountFileDriver(gs.StdFileDriver("assets/"), "@assets/")

while not input.key_press(gs.InputDevice.KeyEscape):
	render.clear()
	render.image2d((SCR_DISP_WIDTH - (SCR_PHYSIC_WIDTH * SCR_SCALE_FACTOR)) / 2.0, 0, SCR_SCALE_FACTOR, "@assets/game_board.png")
	render.image2d((SCR_DISP_WIDTH - (SCR_PHYSIC_WIDTH * SCR_SCALE_FACTOR)) / 2.0, SCR_DISP_HEIGHT - (32 * SCR_SCALE_FACTOR), SCR_SCALE_FACTOR, "@assets/game_score_panel.png")
	render.flip()
