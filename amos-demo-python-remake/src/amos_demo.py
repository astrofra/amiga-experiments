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

import gs
import gs.plus.render as render
import gs.plus.clock as clock
import math
from utils import *

demo_screen_size = [1280, 720]
amiga_screen_size = [320, 220]
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


def render_credits():
	strings = [["THE AMOS DEMO", 25, 1, 1, "dustismo-roman-bold", 42],
			["PROGRAMMING BY", 60, 1, 0 ,"dustismo-roman-bold", 42],
			["Peter Hickman", 75, 1, 0, "dustismo-roman-bold", 42],
			["MUSIC BY", 105, 1, 0, "dustismo-roman-bold", 42],
			["Allister Brimble", 120, 1, 0, "dustismo-roman-bold", 42],
			["IF YOU WOULD LIKE SOME CONTROL", 155, 1, 0, "amiga4ever-pro2", 18],
			["OVER THIS DEMO PRESS SPACE NOW", 165, 1, 0, "amiga4ever-pro2", 18],
			["There is an IMPORTANT text file", 185, 1,0, "amiga4ever-pro2", 18],
			["on this disk, please read it.", 195, 1,0, "amiga4ever-pro2", 18]]

	render_text_screen(strings)

	strings = [["On some of the",40,1,0, "dustismo-roman-bold", 42],
	           ["demonstrations you",65,1,0, "dustismo-roman-bold", 42],
	           ["will be able to move",90,1,0, "dustismo-roman-bold", 42],
	           ["some of the SPRITES or",115,1,0, "dustismo-roman-bold", 42],
	           ["BOBS with the mouse.",140,1,0, "dustismo-roman-bold", 42]]

	render_text_screen(strings)


def render_title_page():
	strings = [["Let your imagination",40,1,0, "dustismo-roman-bold", 42],
	           ["take control and",65,1,0, "dustismo-roman-bold", 42],
	           ["design the game",90,1,0, "dustismo-roman-bold", 42],
	           ["of your dreams.",115,1,0, "dustismo-roman-bold", 42]]

	render_text_screen(strings)

	strings = [["Over 400 commands",40,1,0, "dustismo-roman-bold", 42],
	           ["to unleash the raw power",65,1,0, "dustismo-roman-bold", 42],
	           ["hidden inside your Amiga.",90,1,0, "dustismo-roman-bold", 42]]

	render_text_screen(strings)


def render_hardsprite():
	strings = [["By pushing your Amiga to",40,1,0, "dustismo-roman-bold", 42],
	           ["its limit AMOS allows you",65,1,0, "dustismo-roman-bold", 42],
	           ["to exceed the maximum",90,1,0, "dustismo-roman-bold", 42],
	           ["amount of hardware",115,1,0, "dustismo-roman-bold", 42],
	           ["SPRITES normally",140,1,0, "dustismo-roman-bold", 42],
	           ["available to the user.",165,1,0, "dustismo-roman-bold", 42]]

	render_text_screen(strings)


def render_text_screen(strings = None, duration = 4.0, fade_duration = 1.0):

	if strings is None:
		return

	sys = render.get_render_system()

	fonts_dict = {}
	fx_timer = 0.0
	while fx_timer < duration:
		dt_sec = clock.update()
		fx_timer += dt_sec

		if fx_timer < fade_duration:
			fade = RangeAdjust(fx_timer, 0.0, fade_duration, 0.0, 1.0)
		else:
			if fx_timer < duration - fade_duration:
				fade = 1.0
			else:
				fade = RangeAdjust(fx_timer, duration - fade_duration, duration, 1.0, 0.0)

		render.clear()
		render.get_renderer().EnableBlending(True)

		for line in strings:
			font_key = line[4] + "_" + str(line[5])
			if not font_key in fonts_dict:
				fonts_dict[font_key] = gs.RasterFont("@assets/" + line[4] + ".ttf", line[5], 512)

			rect = fonts_dict[font_key].GetTextRect(sys, line[0])
			x = (demo_screen_size[0] - rect.GetWidth()) * 0.5
			y = (amiga_screen_size[1] - line[1]) * zoom_size
			fonts_dict[font_key].Write(sys, line[0], gs.Vector3(x, y, 0.5), gs.Color.White * fade)

		sys.DrawRasterFontBatch()

		render.get_renderer().EnableBlending(False)

		# underline ?
		for line in strings:
			if line[3] == 1:
				font_key = line[4] + "_" + str(line[5])
				rect = fonts_dict[font_key].GetTextRect(sys, line[0])
				x = (demo_screen_size[0] - rect.GetWidth()) * 0.5
				y = (amiga_screen_size[1] - line[1]) * zoom_size - (line[5] * 0.2)
				render.line2d(x, y, x + rect.GetWidth(), y, gs.Color.White * fade, gs.Color.White * fade)

		update_music()
		render.flip()

def render_title_page_bouncing():
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
render_credits()
render_title_page()
render_hardsprite()
