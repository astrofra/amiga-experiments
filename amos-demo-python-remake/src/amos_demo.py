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
import easygui
from utils import *
from graphic_routines import *
from screen_specs import *


def resolution_requester():
	res_list = ["320x200", "640x400", "800x600", "1280x720", "1600x900", "1920x1080", "1920x1200"]
	choice = easygui.choicebox(msg='Select your screen resolution', title='Screen Resolution', choices=(res_list))
	if choice is not None:
		demo_screen_size[0] = int(choice.split("x")[0])
		demo_screen_size[1] = int(choice.split("x")[1])


def engine_init():
	global al, channel
	gs.LoadPlugins(gs.get_default_plugins_path())
	render.init(demo_screen_size[0], demo_screen_size[1], "pkg.core")

	# mount the system file driver
	gs.MountFileDriver(gs.StdFileDriver("assets/"), "@assets/")

	al = None
	channel = None


def startup_sequence():
	print("")
	print("                           =========")
	print("                           AMOS DEMO")
	print("                           =========")
	print("")
	print("                    AMOS by Francois Lionet")
	print("")
	print("                 AMOS DEMO by Peter J. Hickman")
	print("")
	print("                      Music by A. Brimble")
	print("")
	print("    THIS DEMO WILL NOT WORK ON AN A1000, THE CAUSE OF THE")
	print("    PROBLEM IS CURRENTLY BEING RECTIFIED AND WHEN AMOS IS")
	print("    RELEASED IT WILL WORK ON ALL COMMODORE AMIGA MACHINES.")
	print("")
	print("There is a text file on this disc called IMPORTANT_TEXT_FILE")
	print("            Please read it before running this demo")
	print("             Press <control> + <C> to stop the demo")
	print("")
	print("                 For More Information contact:")
	print(" MANDARIN SOFTWARE, EUROPA HOUSE, ADLINGTON PARK, ADLINGTON,")
	print("                    MACCLESFIELD SK10 4NP")
	print("                     PHONE (0625) 859333")
	print("")
	print("                   Python port by Astrofra")
	print("")



def play_music():
	global al, channel
	# create an OpenAL mixer and wrap it with the MixerAsync interface
	al = gs.MixerAsync(gs.ALMixer())
	al.Open()
	future_channel = al.Stream("@assets/amos_demo_music.xm")
	channel = future_channel.get()


def render_credits():
	strings = [["THE AMOS DEMO", 25, 1, 1, "bilko-opti-bold", 42],
			["PROGRAMMING BY", 60, 1, 0 ,"bilko-opti-bold", 42],
			["Peter Hickman", 75, 1, 0, "bilko-opti-bold", 42],
			["MUSIC BY", 105, 1, 0, "bilko-opti-bold", 42],
			["Allister Brimble", 120, 1, 0, "bilko-opti-bold", 42],
			["IF YOU WOULD LIKE SOME CONTROL", 155, 1, 0, "amiga4ever-pro2", 18],
			["OVER THIS DEMO PRESS SPACE NOW", 165, 1, 0, "amiga4ever-pro2", 18],
			["There is an IMPORTANT text file", 185, 1,0, "amiga4ever-pro2", 18],
			["on this disk, please read it.", 195, 1,0, "amiga4ever-pro2", 18]]

	render_text_screen(strings, duration=9.0)

	strings = [["On some of the",40,1,0, "bilko-opti-bold", 42],
			["demonstrations you",65,1,0, "bilko-opti-bold", 42],
			["will be able to move",90,1,0, "bilko-opti-bold", 42],
			["some of the SPRITES or",115,1,0, "bilko-opti-bold", 42],
			["BOBS with the mouse.",140,1,0, "bilko-opti-bold", 42]]

	render_text_screen(strings, duration=5.0)


def render_title_page():
	strings = [["Let your imagination",40,1,0, "bilko-opti-bold", 42],
			["take control and",65,1,0, "bilko-opti-bold", 42],
			["design the game",90,1,0, "bilko-opti-bold", 42],
			["of your dreams.",115,1,0, "bilko-opti-bold", 42]]

	render_text_screen(strings)

	strings = [["Over 400 commands",40,1,0, "bilko-opti-bold", 42],
			["to unleash the raw power",65,1,0, "bilko-opti-bold", 42],
			["hidden inside your Amiga.",90,1,0, "bilko-opti-bold", 42]]

	render_text_screen(strings)


def render_hardsprite():
	strings = [["By pushing your Amiga to",40,1,0, "bilko-opti-bold", 42],
			["its limit AMOS allows you",65,1,0, "bilko-opti-bold", 42],
			["to exceed the maximum",90,1,0, "bilko-opti-bold", 42],
			["amount of hardware",115,1,0, "bilko-opti-bold", 42],
			["SPRITES normally",140,1,0, "bilko-opti-bold", 42],
			["available to the user.",165,1,0, "bilko-opti-bold", 42]]

	render_text_screen(strings, duration=6.0)

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
				x, y = image_data[spr_index * 3], image_data[spr_index * 3 + 1] * 0.925 + 32
				vel = (1.0 if x < amiga_screen_size[0] * 0.5 else -1)
				sprite_data[spr_index] = {'frame_index': 0.0, 'max_frames': get_sprite_seq_max_frame(spr_name), 'x':x, 'y':y, 'x_velocity':vel}

			if fx_timer > intro_duration:
				sprite_data[spr_index]['frame_index'] = math.fmod((sprite_data[spr_index]['frame_index'] + dt_sec * 10.0), sprite_data[spr_index]['max_frames'])
			if fx_timer > intro_duration + expose_duration:
				sprite_data[spr_index]['x'] += dt_sec * 60.0 * sprite_data[spr_index]['x_velocity']

			render.image2d((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5 + (sprite_data[spr_index]['x'] * zoom_size()),
						(amiga_screen_size[1] - sprite_data[spr_index]['y']) * zoom_size(), zoom_size() / 2,
						"@assets/" + spr_name + str(int(sprite_data[spr_index]['frame_index'])) + ".png")
		render.set_blend_mode2d(render.BlendOpaque)

		render.flip()


def render_hotdog_screen():
	strings = [["Software SPRITES (BOBS)",30,1,0, "bilko-opti-bold", 42],
			["are also available. Their",55,1,0, "bilko-opti-bold", 42],
			["size and range of colours",80,1,0, "bilko-opti-bold", 42],
			["are limited only by the",105,1,0, "bilko-opti-bold", 42],
			["amount of free memory!",130,1,0, "bilko-opti-bold", 42]]

	render_text_screen(strings, duration=5.0)

	bobs = [[120, 40, "jumbo_dog_quarter", 0.5],
			[400, 60, "jumbo_dog_quarter", -0.5],
			[400, 120, "jumbo_dog_quarter", 0.6],
			[-120, 190, "jumbo_dog_quarter", 0.5],
			[400, 100, "jumbo_dog_half", -1],
			[-120, 195, "jumbo_dog_half", 1],
			[-200, 150, "jumbo_dog_half", -1.2],
			[360, 150, "jumbo_dog", -2]]

	fx_timer = 0.0
	fx_duration = 15.0
	while fx_timer < fx_duration:
		dt_sec = clock.update()
		fx_timer += dt_sec
		render.clear()
		render.image2d((demo_screen_size[0] - amiga_screen_size[0] * zoom_size() * 3) * 0.5, 0, zoom_size(), "@assets/backgr.png")
		render.image2d((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5, 0, zoom_size(), "@assets/backgr.png")
		render.image2d((demo_screen_size[0] + amiga_screen_size[0] * zoom_size()) * 0.5, 0, zoom_size(), "@assets/backgr.png")
		render.set_blend_mode2d(render.BlendAlpha)
		for b in bobs:
			render.image2d((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5 + b[0] * zoom_size(),
						   (amiga_screen_size[1] - b[1]) * zoom_size(), zoom_size(), "@assets/" + b[2] + ".png")
			b[0] += dt_sec * 60.0 * b[3]
			if b[0] > 440:
				b[0] = -340
			else:
				if b[0] < -340:
					b[0] = 440

		render.set_blend_mode2d(render.BlendOpaque)
		render.flip()


def render_gipper():

	strings = [["Software SPRITES (BOBS)",40,1,0, "bilko-opti-bold", 42],
			  ["can be used in many",65,1,0, "bilko-opti-bold", 42],
			   ["different ways.",90,1,0, "bilko-opti-bold", 42]]

	render_text_screen(strings)

	fx_timer = 0.0
	run_duration = 1.5
	brake_duration = 0.5
	shoot_duration = 2.5
	fx_duration = 7.0
	render.set_blend_mode2d(render.BlendAlpha)

	x, y, sprite_index = -55, 75 + 48, 0

	while fx_timer < fx_duration:
		dt_sec = clock.update()
		fx_timer += dt_sec
		render.clear()

		if fx_timer < run_duration:
			x += dt_sec * 60.0 * 1.7
			sprite_index = math.fmod(sprite_index + dt_sec * 8.0, 5)
		elif fx_timer < run_duration + brake_duration:
			x += dt_sec * 60.0 * 1.25
			sprite_index = 7
		elif fx_timer < run_duration + brake_duration + shoot_duration:
			sprite_index += dt_sec * 10.0
			if sprite_index < 10:
				sprite_index = 10
			elif sprite_index > 17:
				sprite_index = 10
		else:
			x += dt_sec * 60.0 * 1.7
			sprite_index = math.fmod(sprite_index + dt_sec * 8.0, 5)


		render.image2d((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5 + x * zoom_size(),
					   (amiga_screen_size[1] - y) * zoom_size(), zoom_size() / 2.0, "@assets/sprite_gipper_" + str(int(sprite_index)) + ".png")

		render.flip()

	render.set_blend_mode2d(render.BlendOpaque)


def render_gippers():

	strings = [["There is no limit to the",40,1,0, "bilko-opti-bold", 42],
				["amount of images you can",65,1,0, "bilko-opti-bold", 42],
				["generate. By using just",90,1,0, "bilko-opti-bold", 42],
				["one BOB dozens can be",115,1,0, "bilko-opti-bold", 42],
				["displayed with ease.",140,1,0, "bilko-opti-bold", 42]]

	render_text_screen(strings, duration=5.0)

	fx_timer = 0.0
	run_duration = 1.5
	brake_duration = 0.5
	shoot_duration = 2.5
	fx_duration = 8.5
	render.set_blend_mode2d(render.BlendAlpha)

	x, y, sprite_index = -55, 74 + 50, 0

	while fx_timer < fx_duration:
		dt_sec = clock.update()
		fx_timer += dt_sec
		render.clear()

		if fx_timer < run_duration:
			x += dt_sec * 60.0 * 1.7
			sprite_index = math.fmod(sprite_index + dt_sec * 8.0, 5)
		elif fx_timer < run_duration + brake_duration:
			x += dt_sec * 60.0 * 1.25
			sprite_index = 7
		elif fx_timer < run_duration + brake_duration + shoot_duration:
			sprite_index += dt_sec * 10.0
			if sprite_index < 10:
				sprite_index = 10
			elif sprite_index > 17:
				sprite_index = 10
		else:
			x += dt_sec * 60.0 * 1.7
			sprite_index = math.fmod(sprite_index + dt_sec * 8.0, 5)

		for j in range(4):
			for i in range(6):
				xx = x + (i - 2.5) * 48.0
				yy = y + (j - 1.5) * 50.0
				render.image2d((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5 + xx * zoom_size(),
							   (amiga_screen_size[1] - yy) * zoom_size(), zoom_size() / 2.0, "@assets/sprite_gipper_" + str(int(sprite_index)) + ".png")

		render.flip()

	render.set_blend_mode2d(render.BlendOpaque)


def render_star():
	strings = [["Collision detection in AMOS",30,1,0, "bilko-opti-bold", 42],
			   ["uses special masks.",55,1,0, "bilko-opti-bold", 42],
			   ["This method is very",80,1,0, "bilko-opti-bold", 42],
			   ["fast and gives 100%",105,1,0, "bilko-opti-bold", 42],
			   ["accuracy.",130,1,0, "bilko-opti-bold", 42]]

	render_text_screen(strings, duration=5.0)

	strings = [["Watch the balls in this",30,1,0, "bilko-opti-bold", 42],
			   ["next demo. They only",55,1,0, "bilko-opti-bold", 42],
			   ["change colour when in",80,1,0, "bilko-opti-bold", 42],
			   ["contact with a solid",105,1,0, "bilko-opti-bold", 42],
			   ["part of the large star.",130,1,0, "bilko-opti-bold", 42]]

	render_text_screen(strings, duration=5.0)


def render_overlay():
	strings = [["AMOS allows up to",40,1,0, "bilko-opti-bold", 42],
	           ["eight screens to be",65,1,0, "bilko-opti-bold", 42],
	           ["displayed at any",90,1,0, "bilko-opti-bold", 42],
	           ["one time.",115,1,0, "bilko-opti-bold", 42]]

	render_text_screen(strings)

	strings = [["Each screen can",40,1,0, "bilko-opti-bold", 42],
	           ["be manipulated in a",65,1,0, "bilko-opti-bold", 42],
	           ["variety of unusual ways.",90,1,0, "bilko-opti-bold", 42]]

	render_text_screen(strings)

	strings = [["OVERLAPPING SCREENS WITH DIFFERENT SIZES, RESOLUTIONS AND COLOURS IS EASY", 40, 1, 0, "topaz-a500", 20]]

	fx_timer = 0.0
	fx_duration = 6.0
	screen_0 = [0, 200, -2]
	screen_1 = [0, 0, 3]
	screen_2 = [0, 0, 1.25]

	while fx_timer < fx_duration:
		dt_sec = clock.update()
		fx_timer += dt_sec
		render.clear()

		# back image
		render.image2d((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5 + screen_0[0] * zoom_size(),
		               (amiga_screen_size[1] - screen_0[1]) * zoom_size(),
		               zoom_size() * 0.5, "@assets/multiscreen_pixelart.png")
		render.image2d((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5 + (screen_0[0] + amiga_screen_size[0]) * zoom_size(),
		               (amiga_screen_size[1] - screen_0[1]) * zoom_size(),
		               zoom_size() * 0.5, "@assets/multiscreen_pixelart.png")

		screen_0[0] += dt_sec * 60.0 * screen_0[2]
		if screen_0[0] < -amiga_screen_size[0]:
			screen_0[0] = -amiga_screen_size[0]
			screen_0[2] *= -1
		elif screen_0[0] > 0:
			screen_0[0] = 0
			screen_0[2] *= -1

		# front image
		render.image2d((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5 + screen_1[0] * zoom_size(),
               (amiga_screen_size[1] - screen_1[1]) * zoom_size(),
               zoom_size() * 0.5, "@assets/multiscreen_ham.png")

		screen_1[1] += dt_sec * 60.0 * screen_1[2]
		if screen_1[1] > amiga_screen_size[1] * 2:
			screen_1[1] = -amiga_screen_size[1]

		strings[0][1] = screen_2[1]
		render_strings_array(strings)

		screen_2[1] += dt_sec * 60.0 * screen_2[2]
		if screen_2[1] > amiga_screen_size[1] * 1.25:
			screen_2[1] = -amiga_screen_size[1] * 0.25

		render.flip()

def render_change_fonts():
	strings = [["AMOS is much more flexible",40,1,0, "bilko-opti-bold", 42],
	           ["then any other Amiga",65,1,0, "bilko-opti-bold", 42],
	           ["programming language.",90,1,0, "bilko-opti-bold", 42]]

	render_text_screen(strings)

	strings = [["You can even use",40,1,0, "bilko-opti-bold", 42],
	           ["standard Deluxe Paint",65,1,0, "bilko-opti-bold", 42],
	           ["compatible fonts.",90,1,0, "bilko-opti-bold", 42]]

	render_text_screen(strings)

	fonts = [["common6", 42, 13],
	         ["bilko-opti-bold", 42, 13], ["bilko-opti-bold", 32, 17],
	         ["eka-s-handwriting", 50, 11], ["eka-s-handwriting", 32, 17],
	         ["voyage-fant", 50, 11],
	         ["limelight", 46, 12],
	         ]

	for _font in fonts:
		strings = []
		for i in range(_font[2]):
			strings.append(["AMOS FONTS", (i + 1) * _font[1] / 3, 1, 0, _font[0], _font[1]])

		render_text_screen(strings, fade_duration=0.1)


def render_price_mandarin_logo():
	x, y = 114, 67
	render.set_blend_mode2d(render.BlendAlpha)
	render.image2d((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5 + x * zoom_size(),
					(amiga_screen_size[1] - y) * zoom_size(), zoom_size() / 2.0, "@assets/mandarin_logo.png")
	render.set_blend_mode2d(render.BlendOpaque)

def render_price():

	y = 8
	strings=[["AMOS will be available in",25 - y,1,0, "bilko-opti-bold", 40],
	         ["March for £49.95 from ",38 - y,1,0, "bilko-opti-bold", 40],
	         ["FOR DETAILED SPECIFICATIONS",85 - y,1,0, "amiga4ever-pro2", 18],
	         ["WRITE TO:",95 - y,1,0, "amiga4ever-pro2", 18],
	         ["AMOS INFORMATION",105 - y,1,0, "amiga4ever-pro2", 18],
	         ["MANDARIN SOFTWARE",115 - y,1,0, "amiga4ever-pro2", 18],
	         ["EUROPA HOUSE, ADLINGTON PARK",125 - y,1,0, "amiga4ever-pro2", 18],
	         ["ADLINGTON, MACCLESFIELD",135 - y,1,0, "amiga4ever-pro2", 18],
	         ["SK10 4NP",145 - y,1,0, "amiga4ever-pro2", 18],
	         ["PHONE: (0625) 859333",160 - y,1,0, "amiga4ever-pro2", 18],
	         ["Press space for more information",175 - y,1,0, "amiga4ever-pro2", 18],
	         ["Any other key",185 - y,1,1, "amiga4ever-pro2", 18],
	         ["restarts this demo.",195 - y,1,1, "amiga4ever-pro2", 18]]

	render_text_screen(strings, duration=len(strings), fade_duration=0.2, render_callback=render_price_mandarin_logo)

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
		render.image2d((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5, y_scr * demo_screen_size[1], zoom_size(), "@assets/titlepage.png")
		render.flip()


def render_title_page_still():
	fx_timer = 0.0
	while fx_timer < 4.0:
		dt_sec = clock.update()
		fx_timer += dt_sec
		render.clear()
		render.image2d((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5, 0, zoom_size(), "@assets/titlepage.png")
		render.flip()


startup_sequence()
# resolution_requester()
engine_init()
# render_title_page_bouncing()
# play_music()
# render_title_page_still()
# render_credits()
# render_title_page()
# render_hardsprite()
# render_hotdog_screen()
# render_gipper()
# render_gippers()
# render_star()
render_overlay()
# render_change_fonts()
# render_price()