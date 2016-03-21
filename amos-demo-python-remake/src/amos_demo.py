# ---------------------------------------------------------------------
#                            AMOS DEMO V3! 
#    
#                          BY Peter Hickman
#                       Python port by Astrofra
#
#            Music composed and written by Allister Brimble
#  
# You cannot change this demo, but you may view it (its a bit untidy-  
#     sorry) and you may use some of the commands from direct mode.  
#    For more information please examine the text file on this disk  
#                its called "IMPORTANT_TEXT_FILE.ASC".       
# ---------------------------------------------------------------------

import os
import gs
import gs.plus.render as render
import gs.plus.input as input
import gs.plus.clock as clock
import math
import time
from utils import *

demo_screen_size = [1280, 720]
amiga_screen_size = [320, 256]
zoom_size = demo_screen_size[1] / amiga_screen_size[1]

gs.LoadPlugins(gs.get_default_plugins_path())
render.init(demo_screen_size[0], demo_screen_size[1], "pkg.core")

# mount the system file driver
gs.MountFileDriver(gs.StdFileDriver("assets/"), "@assets/")

al = None
channel = None


def play_music():
	global al, channel
	# create an OpenAL mixer and wrap it with the MixerAsync interface
	al = gs.ALMixer()
	al.Open()
	channel = al.Stream("@assets/amos_demo_music.xm")


def update_music():
	global al, channel
	al.GetPlayState(channel)


def render_title_page_bouncing():
	global al, channel
	fx_timer = 0.0
	fx_duration = math.pi
	while fx_timer < 4.0:
		dt_sec = clock.update()
		fx_timer += dt_sec
		y_damping = RangeAdjust(fx_timer, 0.0, fx_duration, 1.0, 0.0)
		y_damping = Clamp(y_damping, 0.0, 1.0)
		y_damping = EaseInOutQuick(y_damping)
		y_scr = abs(math.sin((fx_timer + 1.2) * 4.0)) * y_damping
		render.clear()
		render.image2d((demo_screen_size[0] - amiga_screen_size[0] * zoom_size) * 0.5, y_scr * demo_screen_size[1], zoom_size, "@assets/titlepage.png")
		render.flip()


def render_title_page_still():
	fx_timer = 0.0
	while fx_timer < 4.0:
		dt_sec = clock.update()
		fx_timer += dt_sec
		render.clear()
		render.image2d((demo_screen_size[0] - amiga_screen_size[0] * zoom_size) * 0.5, 0, zoom_size, "@assets/titlepage.png")
		update_music()
		render.flip()


render_title_page_bouncing()
play_music()
render_title_page_still()
