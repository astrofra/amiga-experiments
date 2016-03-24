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
from graphic_routines import *
from screen_specs import *

gs.LoadPlugins(gs.get_default_plugins_path())
render.init(demo_screen_size[0], demo_screen_size[1], "pkg.core")

# mount the system file driver
gs.MountFileDriver(gs.StdFileDriver("assets/"), "@assets/")

al = None
channel = None


def play_music():
	global al, channel
	# create an OpenAL mixer and wrap it with the MixerAsync interface
	al = gs.MixerAsync(gs.ALMixer())
	al.Open()
	future_channel = al.Stream("@assets/amos_demo_music.xm")
	channel = future_channel.get()


def render_credits():
	strings = [["THE AMOS DEMO", 25, 1, 1, "dustismo-roman", 42],
			["PROGRAMMING BY", 60, 1, 0 ,"dustismo-roman", 42],
			["Peter Hickman", 75, 1, 0, "dustismo-roman", 42],
			["MUSIC BY", 105, 1, 0, "dustismo-roman", 42],
			["Allister Brimble", 120, 1, 0, "dustismo-roman", 42],
			["IF YOU WOULD LIKE SOME CONTROL", 155, 1, 0, "amiga4ever-pro2", 18],
			["OVER THIS DEMO PRESS SPACE NOW", 165, 1, 0, "amiga4ever-pro2", 18],
			["There is an IMPORTANT text file", 185, 1,0, "amiga4ever-pro2", 18],
			["on this disk, please read it.", 195, 1,0, "amiga4ever-pro2", 18]]

	render_text_screen(strings)

	strings = [["On some of the",40,1,0, "dustismo-roman", 42],
			["demonstrations you",65,1,0, "dustismo-roman", 42],
			["will be able to move",90,1,0, "dustismo-roman", 42],
			["some of the SPRITES or",115,1,0, "dustismo-roman", 42],
			["BOBS with the mouse.",140,1,0, "dustismo-roman", 42]]

	render_text_screen(strings)


def render_title_page():
	strings = [["Let your imagination",40,1,0, "dustismo-roman", 42],
			["take control and",65,1,0, "dustismo-roman", 42],
			["design the game",90,1,0, "dustismo-roman", 42],
			["of your dreams.",115,1,0, "dustismo-roman", 42]]

	render_text_screen(strings)

	strings = [["Over 400 commands",40,1,0, "dustismo-roman", 42],
			["to unleash the raw power",65,1,0, "dustismo-roman", 42],
			["hidden inside your Amiga.",90,1,0, "dustismo-roman", 42]]

	render_text_screen(strings)


def render_hardsprite():
	strings = [["By pushing your Amiga to",40,1,0, "dustismo-roman", 42],
			["its limit AMOS allows you",65,1,0, "dustismo-roman", 42],
			["to exceed the maximum",90,1,0, "dustismo-roman", 42],
			["amount of hardware",115,1,0, "dustismo-roman", 42],
			["SPRITES normally",140,1,0, "dustismo-roman", 42],
			["available to the user.",165,1,0, "dustismo-roman", 42]]

	render_text_screen(strings)

	sprites = ["sprite_bike_", "sprite_skate_", "sprite_glide_", "sprite_plane_"]
	image_data = [230,-1,2, 60,0,0, 180,30,3, 100,25,1,
				230,49,2, 60,50,0, 180,80,3, 100,75,1,
				230,99,2, 60,100,0, 180,130,3, 100,125,1,
				230,150,2, 60,149,0, 180,180,3, 100,175,1]
	sprite_data = {}
	max_sprites = 16

	fx_timer = 0.0
	intro_duration = 4.0
	expose_duration = 1.0
	total_duration = 10.0
	while fx_timer < total_duration:
		dt_sec = clock.update()
		fx_timer += dt_sec

		render.clear()

		render.set_blend_mode2d(render.BlendAlpha)

		if fx_timer < intro_duration:
			v_max_sprites = int(RangeAdjust(fx_timer, 0.0, intro_duration, 1, max_sprites))
			strings = [["SPRITE", 13 * 8, 1, 0, "amiga4ever-pro2", 18],
						["NUMBER", 15 * 8, 1, 0, "amiga4ever-pro2", 18],
						[str(v_max_sprites), 17 * 8, 1, 0, "amiga4ever-pro2", 18]]
			render_strings_array(strings)
		else:
			v_max_sprites = max_sprites
			if fx_timer >= intro_duration and fx_timer < intro_duration + expose_duration:
				strings = [["ANIMATE!", 13 * 8, 1, 0, "amiga4ever-pro2", 18]]
				render_strings_array(strings)
			else:
				strings = [["MOVE IT!", 13 * 8, 1, 0, "amiga4ever-pro2", 18]]
				render_strings_array(strings)

		for spr_index in range(v_max_sprites):
			spr_name = sprites[image_data[spr_index * 3 + 2]]
			if not (spr_index in sprite_data):
				x, y = image_data[spr_index * 3], image_data[spr_index * 3 + 1] + 32
				vel = (1.0 if x < amiga_screen_size[0] * 0.5 else -1)
				sprite_data[spr_index] = {'frame_index': 0.0, 'max_frames': get_sprite_seq_max_frame(spr_name), 'x':x, 'y':y, 'x_velocity':vel}

			if fx_timer > intro_duration:
				sprite_data[spr_index]['frame_index'] = math.fmod((sprite_data[spr_index]['frame_index'] + dt_sec * 10.0), sprite_data[spr_index]['max_frames'])
			if fx_timer > intro_duration + expose_duration:
				sprite_data[spr_index]['x'] += dt_sec * 60.0 * sprite_data[spr_index]['x_velocity']

			render.image2d((demo_screen_size[0] - amiga_screen_size[0] * zoom_size) * 0.5 + (sprite_data[spr_index]['x'] * zoom_size),
						(amiga_screen_size[1] - sprite_data[spr_index]['y']) * zoom_size, zoom_size / 2,
						"@assets/" + spr_name + str(int(sprite_data[spr_index]['frame_index'])) + ".png")
		render.set_blend_mode2d(render.BlendOpaque)

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
		render.flip()


render_title_page_bouncing()
play_music()
render_title_page_still()
render_credits()
render_title_page()
render_hardsprite()
