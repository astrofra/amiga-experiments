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
# import gs.plus.render as render
# import gs.plus.clock as clock
import math
import sys
import random
import pymsgbox
import json
from utils import *
from graphic_routines import *
from screen_specs import *
from os.path import dirname, realpath
from random import uniform

plus = None

def resolution_requester(open_gui=True):
	if open_gui:
		window_mode = pymsgbox.confirm(text='Select your screen mode', title='AMOS DEMO', buttons=['Windowed', 'Fullscreen'])

		if window_mode == 'Windowed':
			demo_screen_size[2] = gs.Window.Windowed
			screen_resolutions = ['640x480', '720x568', '800x600', '1280x800']
		elif window_mode == 'Fullscreen':
			demo_screen_size[2] = gs.Window.Fullscreen
			screen_resolutions = ['640x480', '800x600', '1280x720', '1280x800', '1920x1080']
		else:
			return False

		screen_res = pymsgbox.confirm(text='Select your screen resolution', title='AMOS DEMO',
									   buttons=screen_resolutions)

		if screen_res is not None:
			demo_screen_size[0] = int(screen_res.split('x')[0])
			demo_screen_size[1] = int(screen_res.split('x')[1])
		else:
			return False

	return True


def demo_exit_test():
	global plus
	if plus.IsAppEnded(plus.EndOnDefaultWindowClosed) or plus.KeyPress(gs.InputDevice.KeyEscape):
		plus.RenderUninit()
		exit()


def engine_init():
	global al, channel, plus
	try:
		gs.LoadPlugins(gs.get_default_plugins_path())
	except:
		pass

	plus = gs.GetPlus()
	plus.RenderInit(demo_screen_size[0], demo_screen_size[1], 1, demo_screen_size[2])

	# mount the system file driver
	gs.MountFileDriver(gs.StdFileDriver("assets/"), "@assets/")

	al = None
	channel = None


def startup_sequence():
	global plus

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
	global al, channel, plus
	# create an OpenAL mixer and wrap it with the MixerAsync interface
	al = gs.MixerAsync(gs.ALMixer())
	al.Open()
	future_channel = al.Stream("@assets/amos_demo_music.xm")
	channel = future_channel.get()


def render_credits():
	global plus

	strings = [["THE AMOS DEMO", 25, 1, 1, "bilko-opti-bold", 42],
			["PROGRAMMING BY", 60, 1, 0 ,"bilko-opti-bold", 42],
			["Peter Hickman", 75, 1, 0, "bilko-opti-bold", 42],
			["MUSIC BY", 105, 1, 0, "bilko-opti-bold", 42],
			["Allister Brimble", 120, 1, 0, "bilko-opti-bold", 42],
			["IF YOU WOULD LIKE SOME CONTROL", 155, 1, 0, "amiga4ever-pro2", 18],
			["OVER THIS DEMO PRESS SPACE NOW", 165, 1, 0, "amiga4ever-pro2", 18],
			["There is an IMPORTANT text file", 185, 1,0, "amiga4ever-pro2", 18],
			["on this disk, please read it.", 195, 1,0, "amiga4ever-pro2", 18]]

	render_text_screen(strings, duration=9.0, plus=plus, exit_callback=demo_exit_test)

	strings = [["On some of the",40,1,0, "bilko-opti-bold", 42],
			["demonstrations you",65,1,0, "bilko-opti-bold", 42],
			["will be able to move",90,1,0, "bilko-opti-bold", 42],
			["some of the SPRITES or",115,1,0, "bilko-opti-bold", 42],
			["BOBS with the mouse.",140,1,0, "bilko-opti-bold", 42]]

	render_text_screen(strings, duration=5.0, plus=plus, exit_callback=demo_exit_test)


def render_title_page():
	strings = [["Let your imagination",40,1,0, "bilko-opti-bold", 42],
			["take control and",65,1,0, "bilko-opti-bold", 42],
			["design the game",90,1,0, "bilko-opti-bold", 42],
			["of your dreams.",115,1,0, "bilko-opti-bold", 42]]

	render_text_screen(strings, plus=plus, exit_callback=demo_exit_test)

	strings = [["Over 400 commands",40,1,0, "bilko-opti-bold", 42],
			["to unleash the raw power",65,1,0, "bilko-opti-bold", 42],
			["hidden inside your Amiga.",90,1,0, "bilko-opti-bold", 42]]

	render_text_screen(strings, plus=plus, exit_callback=demo_exit_test)


def render_hardsprite():
	global plus

	strings = [["By pushing your Amiga to",40,1,0, "bilko-opti-bold", 42],
			["its limit AMOS allows you",65,1,0, "bilko-opti-bold", 42],
			["to exceed the maximum",90,1,0, "bilko-opti-bold", 42],
			["amount of hardware",115,1,0, "bilko-opti-bold", 42],
			["SPRITES normally",140,1,0, "bilko-opti-bold", 42],
			["available to the user.",165,1,0, "bilko-opti-bold", 42]]

	render_text_screen(strings, duration=6.0, plus=plus, exit_callback=demo_exit_test)

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
		demo_exit_test()
		dt_sec = clock.update()
		fx_timer += dt_sec

		plus.Clear()

		plus.SetBlend2D(gs.BlendAlpha)

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
				render_strings_array(strings, plus=plus)
			else:
				strings = [["MOVE IT!", 13 * 8, 1, 0, "amiga4ever-pro2", 18]]
				render_strings_array(strings, plus=plus)

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

			plus.Image2D((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5 + (sprite_data[spr_index]['x'] * zoom_size()),
						(amiga_screen_size[1] - sprite_data[spr_index]['y']) * zoom_size(), zoom_size() / 2,
						"@assets/" + spr_name + str(int(sprite_data[spr_index]['frame_index'])) + ".png")
		plus.SetBlend2D(gs.BlendOpaque)

		plus.Flip()


def render_hotdog_screen():
	global plus

	strings = [["Software SPRITES (BOBS)",30,1,0, "bilko-opti-bold", 42],
			["are also available. Their",55,1,0, "bilko-opti-bold", 42],
			["size and range of colours",80,1,0, "bilko-opti-bold", 42],
			["are limited only by the",105,1,0, "bilko-opti-bold", 42],
			["amount of free memory!",130,1,0, "bilko-opti-bold", 42]]

	render_text_screen(strings, duration=5.0, plus=plus, exit_callback=demo_exit_test)

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
		demo_exit_test()
		dt_sec = clock.update()
		fx_timer += dt_sec
		plus.Clear()
		plus.Image2D((demo_screen_size[0] - amiga_screen_size[0] * zoom_size() * 3) * 0.5, 0, zoom_size(), "@assets/backgr.png")
		plus.Image2D((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5, 0, zoom_size(), "@assets/backgr.png")
		plus.Image2D((demo_screen_size[0] + amiga_screen_size[0] * zoom_size()) * 0.5, 0, zoom_size(), "@assets/backgr.png")
		plus.SetBlend2D(gs.BlendAlpha)
		for b in bobs:
			plus.Image2D((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5 + b[0] * zoom_size(),
						   (amiga_screen_size[1] - b[1]) * zoom_size(), zoom_size(), "@assets/" + b[2] + ".tga")
			b[0] += dt_sec * 60.0 * b[3]
			if b[0] > 440:
				b[0] = -340
			else:
				if b[0] < -340:
					b[0] = 440

		plus.SetBlend2D(gs.BlendOpaque)
		plus.Flip()


def render_gipper():
	global plus

	strings = [["Software SPRITES (BOBS)",40,1,0, "bilko-opti-bold", 42],
			  ["can be used in many",65,1,0, "bilko-opti-bold", 42],
			   ["different ways.",90,1,0, "bilko-opti-bold", 42]]

	render_text_screen(strings, plus=plus, exit_callback=demo_exit_test)

	fx_timer = 0.0
	run_duration = 1.5
	brake_duration = 0.5
	shoot_duration = 2.5
	fx_duration = 7.0
	plus.SetBlend2D(gs.BlendAlpha)

	x, y, sprite_index = -55, 75 + 48, 0

	while fx_timer < fx_duration:
		demo_exit_test()
		dt_sec = clock.update()
		fx_timer += dt_sec
		plus.Clear()

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

		plus.Image2D((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5 + x * zoom_size(),
					   (amiga_screen_size[1] - y) * zoom_size(), zoom_size() / 2.0, "@assets/sprite_gipper_" + str(int(sprite_index)) + ".png")

		plus.Flip()

	plus.SetBlend2D(gs.BlendOpaque)


def render_gippers():
	global plus

	strings = [["There is no limit to the",40,1,0, "bilko-opti-bold", 42],
				["amount of images you can",65,1,0, "bilko-opti-bold", 42],
				["generate. By using just",90,1,0, "bilko-opti-bold", 42],
				["one BOB dozens can be",115,1,0, "bilko-opti-bold", 42],
				["displayed with ease.",140,1,0, "bilko-opti-bold", 42]]

	render_text_screen(strings, duration=5.0, plus=plus, exit_callback=demo_exit_test)

	fx_timer = 0.0
	run_duration = 1.5
	brake_duration = 0.5
	shoot_duration = 2.5
	fx_duration = 8.5
	plus.SetBlend2D(gs.BlendAlpha)

	x, y, sprite_index = -55, 74 + 50, 0

	while fx_timer < fx_duration:
		demo_exit_test()
		dt_sec = clock.update()
		fx_timer += dt_sec
		plus.Clear()

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
				plus.Image2D((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5 + xx * zoom_size(),
							   (amiga_screen_size[1] - yy) * zoom_size(), zoom_size() / 2.0, "@assets/sprite_gipper_" + str(int(sprite_index)) + ".png")

		plus.Flip()

	plus.SetBlend2D(gs.BlendOpaque)


def rvect(r):
	return gs.Vector3(uniform(-r, r), uniform(-r, r), uniform(-r, r))


def render_star():
	global plus

	strings = [["Collision detection in AMOS",30,1,0, "bilko-opti-bold", 42],
			   ["uses special masks.",55,1,0, "bilko-opti-bold", 42],
			   ["This method is very",80,1,0, "bilko-opti-bold", 42],
			   ["fast and gives 100%",105,1,0, "bilko-opti-bold", 42],
			   ["accuracy.",130,1,0, "bilko-opti-bold", 42]]

	render_text_screen(strings, duration=5.0, plus=plus, exit_callback=demo_exit_test)

	strings = [["Watch the balls in this",30,1,0, "bilko-opti-bold", 42],
			   ["next demo. They only",55,1,0, "bilko-opti-bold", 42],
			   ["change colour when in",80,1,0, "bilko-opti-bold", 42],
			   ["contact with a solid",105,1,0, "bilko-opti-bold", 42],
			   ["part of the large star.",130,1,0, "bilko-opti-bold", 42]]

	render_text_screen(strings, duration=5.0, plus=plus, exit_callback=demo_exit_test)

	scn = plus.NewScene()
	scn.Load("@assets/star.scn", gs.SceneLoadContext(plus.GetRenderSystem()))
	cam = plus.AddCamera(scn, gs.Matrix4.TranslationMatrix(gs.Vector3(0, -45, -100) * 1))
	cam.GetTransform().SetRotation(gs.Vector3(math.pi * -24 / 180, 0, 0))
	cam.GetComponent("Camera").SetZoomFactor(22.0) ## * 0.5)
	# plus.AddEnvironment(scn, gs.Color.Black, gs.Color.White)
	plus.AddLight(scn, gs.Matrix4.TranslationMatrix((6, 4, -6)))

	while not scn.IsReady():
		dt = plus.UpdateClock()
		plus.UpdateScene(scn, dt)

	_w = 15

	pos = gs.Vector3(0, -_w * 0.75, 0)
	rb = plus.AddPhysicCube(scn, gs.Matrix4.TranslationMatrix(pos), _w * 2, 1, 1, 0)
	rb[1].SetRestitution(1.0)

	pos = gs.Vector3(0, _w * 0.75, 0)
	rb = plus.AddPhysicCube(scn, gs.Matrix4.TranslationMatrix(pos), _w * 2, 1, 1, 0)
	rb[1].SetRestitution(1.0)

	pos = gs.Vector3(_w, 0, 0)
	rb = plus.AddPhysicCube(scn, gs.Matrix4.TranslationMatrix(pos), 1, _w * 2, 1, 0)
	rb[1].SetRestitution(1.0)

	pos = gs.Vector3(-_w, 0, 0)
	rb = plus.AddPhysicCube(scn, gs.Matrix4.TranslationMatrix(pos), 1, _w * 2, 1, 0)
	rb[1].SetRestitution(1.0)

	_w = 8

	scn.GetPhysicSystem().SetForceRigidBodyAxisLockOnCreation(gs.LockZ + gs.LockRotX + gs.LockRotY + gs.LockRotZ)
	scn.GetPhysicSystem().SetGravity(gs.Vector3(0, 0, 0))

	balls = {}
	prev_pos = None
	for i in range(15):
		_rnd = uniform(0, 1)
		if _rnd < 0.25:
			pos = gs.Vector3(-_w, -_w, 0)
		elif _rnd < 0.5:
			pos = gs.Vector3(_w, -_w, 0)
		elif _rnd< 0.75:
			pos = gs.Vector3(_w, _w, 0)
		else:
			pos = gs.Vector3(-_w, _w, 0)

		while prev_pos is not None and gs.Vector3.Dist(prev_pos, pos) < 2.0:
			pos += rvect(2.0)
			pos.z = 0

		prev_pos = pos

		ball = plus.AddPhysicSphere(scn, gs.Matrix4.TranslationMatrix(pos), 0.5, 6, 16, 1, "@assets/blue.mat")

		ball[1].ApplyLinearImpulse(pos * -1.0 * uniform(0.05, 0.5))
		ball[1].SetRestitution(1.1)

		col_ball = plus.AddSphere(scn, gs.Matrix4.TranslationMatrix(gs.Vector3(0,0,0)), 0.5, 6, 16, "@assets/red.mat")
		col_ball.GetTransform().SetParent(ball[0])
		col_ball.GetComponent("Object").SetEnabled(False)

		plus.UpdateScene(scn, plus.UpdateClock())
		balls[str(ball[0].GetUid())] = [ball[0], col_ball, 0.0]

	star_mesh_edges = scn.GetNode("star_mesh_edges")
	star_mesh_edges.RemoveComponent(star_mesh_edges.GetComponent("Object"))

	star = scn.GetNode("star_mesh")
	rb = gs.MakeRigidBody()
	rb.SetRestitution(1.0)
	star.AddComponent(rb)
	star.RemoveComponent(star.GetComponent("Object"))

	star_bitmap = scn.GetNode("star_bitmap")
	star_bitmap.GetTransform().SetRotation(gs.Vector3(-0.15, 0, 0))

	star_geo = gs.LoadCoreGeometry("@assets/star_mesh.geo")
	star_col = gs.MakeMeshCollision()
	star_col.SetGeometry(star_geo)
	star_col.SetMass(10)
	star.AddComponent(star_col)

	fx_timer = 0.0
	fx_duration = 12.0

	ps = scn.GetPhysicSystem()
	while fx_timer < fx_duration:
		demo_exit_test()
		dt = plus.UpdateClock()
		plus.UpdateScene(scn, dt)
		fx_timer += dt.to_sec()

		for colp in ps.GetCollisionPairs(star):
			if colp.GetNodeA().GetName == "star_mesh":
				col_node = colp.GetNodeB()
			else:
				col_node = colp.GetNodeA()

			balls[str(col_node.GetUid())][2] = 1.0

		for ball_key in balls:
			ball = balls[ball_key]
			if ball[2] > 0.0:
				ball[1].GetComponent("Object").SetEnabled(True)
				ball[0].GetComponent("Object").SetEnabled(False)
			else:
				ball[1].GetComponent("Object").SetEnabled(False)
				ball[0].GetComponent("Object").SetEnabled(True)

			ball[2] = max(0.0, ball[2] - dt.to_sec())


		plus.Flip()



def render_overlay():
	global plus

	strings = [["AMOS allows up to",40,1,0, "bilko-opti-bold", 42],
			   ["eight screens to be",65,1,0, "bilko-opti-bold", 42],
			   ["displayed at any",90,1,0, "bilko-opti-bold", 42],
			   ["one time.",115,1,0, "bilko-opti-bold", 42]]

	render_text_screen(strings, plus=plus, exit_callback=demo_exit_test)

	strings = [["Each screen can",40,1,0, "bilko-opti-bold", 42],
			   ["be manipulated in a",65,1,0, "bilko-opti-bold", 42],
			   ["variety of unusual ways.",90,1,0, "bilko-opti-bold", 42]]

	render_text_screen(strings, plus=plus, exit_callback=demo_exit_test)

	strings = [["OVERLAPPING SCREENS WITH DIFFERENT SIZES, RESOLUTIONS AND COLOURS IS EASY", 40, 1, 0, "topaz-a500", 20]]

	fx_timer = 0.0
	fx_duration = 8.0
	screen_0 = [0, 200, -2]
	screen_1 = [0, 0, 3]
	screen_2 = [0, 0, 1.25]

	while fx_timer < fx_duration:
		demo_exit_test()
		dt_sec = clock.update()
		fx_timer += dt_sec
		plus.Clear()

		# back image
		plus.Image2D((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5 + screen_0[0] * zoom_size(),
					   (amiga_screen_size[1] - screen_0[1]) * zoom_size(),
					   zoom_size() * 0.5, "@assets/multiscreen_pixelart.png")
		plus.Image2D((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5 + (screen_0[0] + amiga_screen_size[0]) * zoom_size(),
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
		plus.Image2D((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5 + screen_1[0] * zoom_size(),
			   (amiga_screen_size[1] - screen_1[1]) * zoom_size(),
			   zoom_size() * 0.5, "@assets/multiscreen_ham.png")

		screen_1[1] += dt_sec * 60.0 * screen_1[2]
		if screen_1[1] > amiga_screen_size[1] * 2:
			screen_1[1] = -amiga_screen_size[1]

		text_bg_y = (amiga_screen_size[1] - screen_2[1]) * zoom_size() - 3
		text_bg_col = gs.Color(0.1, 0.1, 0.1, 1.0)
		plus.Quad2D(0, text_bg_y, 0, text_bg_y + 32, demo_screen_size[0], text_bg_y + 32, demo_screen_size[0], text_bg_y,
					text_bg_col, text_bg_col, text_bg_col, text_bg_col)
		plus.Commit2D()
		strings[0][1] = screen_2[1]
		render_strings_array(strings, plus=plus)

		screen_2[1] += dt_sec * 60.0 * screen_2[2]
		if screen_2[1] > amiga_screen_size[1] * 1.25:
			screen_2[1] = -amiga_screen_size[1] * 0.25

		plus.Flip()


def render_change_fonts():
	global plus

	strings = [["AMOS is much more flexible",40,1,0, "bilko-opti-bold", 42],
			   ["then any other Amiga",65,1,0, "bilko-opti-bold", 42],
			   ["programming language.",90,1,0, "bilko-opti-bold", 42]]

	render_text_screen(strings, plus=plus, exit_callback=demo_exit_test)

	strings = [["You can even use",40,1,0, "bilko-opti-bold", 42],
			   ["standard Deluxe Paint",65,1,0, "bilko-opti-bold", 42],
			   ["compatible fonts.",90,1,0, "bilko-opti-bold", 42]]

	render_text_screen(strings, plus=plus, exit_callback=demo_exit_test)

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

		render_text_screen(strings, fade_duration=0.1, plus=plus, exit_callback=demo_exit_test)


def render_price_mandarin_logo():
	global plus

	x, y = 114, 67
	plus.SetBlend2D(gs.BlendAlpha)
	plus.Image2D((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5 + x * zoom_size(),
					(amiga_screen_size[1] - y) * zoom_size(), zoom_size() / 2.0, "@assets/mandarin_logo.png")
	plus.SetBlend2D(gs.BlendOpaque)


def render_price():
	global plus

	y = 8
	strings = [["AMOS will be available in",25 - y,1,0, "bilko-opti-bold", 40],
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

	render_text_screen(strings, duration=len(strings), fade_duration=0.2, render_callback=render_price_mandarin_logo,
					   plus=plus, exit_callback=demo_exit_test)


def render_hardscroll():
	global plus

	strings = [["Software and hardware",40,1,0, "bilko-opti-bold", 42],
			   ["scrolling are present in",65,1,0, "bilko-opti-bold", 42],
			   ["AMOS. Each type can be",90,1,0, "bilko-opti-bold", 42],
			   ["activated with a single",115,1,0, "bilko-opti-bold", 42],
			   ["command.",140,1,0, "bilko-opti-bold", 42]]

	render_text_screen(strings, duration=len(strings), plus=plus, exit_callback=demo_exit_test)

	strings = [["It is possible to use",40,1,0, "bilko-opti-bold", 42],
			   ["both SPRITES and BOBS",65,1,0, "bilko-opti-bold", 42],
			   ["on any type of",90,1,0, "bilko-opti-bold", 42],
			   ["scrolling screen.",115,1,0, "bilko-opti-bold", 42]]

	render_text_screen(strings, duration=len(strings), plus=plus, exit_callback=demo_exit_test)

	fx_timer = 0.0
	phase_1_duration = 3.0
	phase_2_duration = 2.0
	phase_3_duration = 2.0
	phase_4_duration = 2.5
	phase_5_duration = 2.0
	phase_6_duration = 1.5
	phase_7_duration = 0.5
	phase_8_duration = 2.0
	fx_duration = 17.5
	screen_1 = [0, 200, 0]
	sprite_0 = [-100, 110, 0, 0, 0]
	prev_y = sprite_0[1]
	sprite_1 = [400, 110, 0, 0, 15]
	enemy_amount = 10
	enemy_sprites = [[400, 110, 0, 0, 15] for _ in range(enemy_amount)]
	for i in range(enemy_amount):
		enemy_sprites[i][0] = random.randint(400, 500)
		enemy_sprites[i][1] = random.randint(64, 200)
		enemy_sprites[i][2] = -(1 + random.randint(1, 5) + random.randint(0, 1) * 0.5 + random.randint(0, 1) * 0.25)
		enemy_sprites[i][4] = random.randint(15, 20)

	while fx_timer < fx_duration:
		demo_exit_test()
		dt_sec = clock.update()
		fx_timer += dt_sec

		if fx_timer < phase_1_duration:
			screen_1[2] = 0
			if sprite_0[0] < 60:
				sprite_0[2] = 2
			elif sprite_0[0] < 120:
				sprite_0[2] = 2
				sprite_0[3] = -2
			elif sprite_0[1] < 180:
				sprite_0[2] = 0
				sprite_0[3] = 2
		elif fx_timer < phase_1_duration + phase_2_duration:
			screen_1[2] = -1
			if sprite_0[1] > 110:
				sprite_0[2] = -2
				sprite_0[3] = -2
			else:
				sprite_0[2] = 0
				sprite_0[3] = 0
		elif fx_timer < phase_1_duration + phase_2_duration + phase_3_duration:
			screen_1[2] = -2
			if sprite_0[0] > 10:
				sprite_0[2] = -1
			else:
				sprite_0[2] = 0
		elif fx_timer < phase_1_duration + phase_2_duration + phase_3_duration + phase_4_duration:
			screen_1[2] = -3
		elif fx_timer < phase_1_duration + phase_2_duration + phase_3_duration + phase_4_duration + phase_5_duration:
			screen_1[2] = -2
		elif fx_timer < phase_1_duration + phase_2_duration + phase_3_duration + phase_4_duration + phase_5_duration + phase_6_duration:
			screen_1[2] = -1
		elif fx_timer < phase_1_duration + phase_2_duration + phase_3_duration + phase_4_duration + phase_5_duration + phase_6_duration + phase_7_duration:
			screen_1[2] = -0.5
			sprite_0[2] = 2
		elif fx_timer < phase_1_duration + phase_2_duration + phase_3_duration + phase_4_duration + phase_5_duration + phase_6_duration + phase_7_duration + phase_8_duration:
			screen_1[2] = 0
			if abs(sprite_0[0] - sprite_1[0]) > 10:
				sprite_1[2] = -3
			else:
				sprite_0[2] = sprite_1[2] = 0

				sprite_0[4] += dt_sec * 10.0
				if sprite_0[4] < 6:
					sprite_0[4] = 6
				elif sprite_0[4] > 14:
					sprite_0[4] = 14

				sprite_1[4] = sprite_0[4] * 1.05
		else:
			sprite_0[4] = -1
			sprite_1[4] = -1

		plus.Clear()
		plus.SetBlend2D(gs.BlendAlpha)

		# front image
		plus.Image2D((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5 + (screen_1[0] - amiga_screen_size[0]) * zoom_size(),
					   (amiga_screen_size[1] - screen_1[1]) * zoom_size(),
					   zoom_size() * 0.5, "@assets/DPLAY1.png")
		plus.Image2D((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5 + screen_1[0] * zoom_size(),
					   (amiga_screen_size[1] - screen_1[1]) * zoom_size(),
					   zoom_size() * 0.5, "@assets/DPLAY1.png")
		plus.Image2D((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5 + (screen_1[0] + amiga_screen_size[0]) * zoom_size(),
					   (amiga_screen_size[1] - screen_1[1]) * zoom_size(),
					   zoom_size() * 0.5, "@assets/DPLAY1.png")
		plus.Image2D((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5 + (screen_1[0] + amiga_screen_size[0] * 2) * zoom_size(),
					   (amiga_screen_size[1] - screen_1[1]) * zoom_size(),
					   zoom_size() * 0.5, "@assets/DPLAY1.png")

		screen_1[0] += dt_sec * 60.0 * screen_1[2]
		if screen_1[0] < -amiga_screen_size[0]:
			screen_1[0] = 0

		sprite_0[0] += dt_sec * 60.0 * sprite_0[2]
		sprite_0[1] += dt_sec * 60.0 * sprite_0[3]
		ship_flicker = int((fx_timer*2)%2)
		spr_vel_y = prev_y - sprite_0[1]
		prev_y = sprite_0[1]
		if spr_vel_y > 0:
			ship_flicker += 2
		elif spr_vel_y < 0:
			ship_flicker += 4

		if sprite_0[4] >= 0:
			plus.Image2D((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5 + sprite_0[0] * zoom_size(),
						(amiga_screen_size[1] - sprite_0[1]) * zoom_size(), zoom_size() / 2.0, "@assets/sprite_spaceship_" + str(int(sprite_0[4] + ship_flicker)) + ".png")

		sprite_1[0] += dt_sec * 60.0 * sprite_1[2]
		sprite_1[1] += dt_sec * 60.0 * sprite_1[3]
		if sprite_1[4] >= 0:
			plus.Image2D((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5 + sprite_1[0] * zoom_size(),
						(amiga_screen_size[1] - sprite_1[1]) * zoom_size(), zoom_size() / 2.0, "@assets/sprite_spaceship_" + str(int(sprite_1[4])) + ".png")

		for i in range(enemy_amount):
			enemy_sprites[i][0] += dt_sec * 60.0 * enemy_sprites[i][2]
			enemy_sprites[i][1] += dt_sec * 60.0 * enemy_sprites[i][3]
			enemy_sprites[i][4] += dt_sec * 10.0
			if enemy_sprites[i][4] > 22:
				enemy_sprites[i][4] = 15
			plus.Image2D((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5 + enemy_sprites[i][0] * zoom_size(),
						(amiga_screen_size[1] - enemy_sprites[i][1]) * zoom_size(), zoom_size() / 2.0, "@assets/sprite_spaceship_" + str(int(enemy_sprites[i][4])) + ".png")

		plus.SetBlend2D(gs.BlendOpaque)
		plus.Flip()


def render_dual_playfield():
	global plus

	strings = [["With AMOS you can easily",30,1,0, "bilko-opti-bold", 42],
				["utilise the unique Amiga",55,1,0, "bilko-opti-bold", 42],
				["Dual Playfield mode, Which",80,1,0, "bilko-opti-bold", 42],
				["allows two screens to be",105,1,0, "bilko-opti-bold", 42],
				["overlayed on top of each",130,1,0, "bilko-opti-bold", 42],
				["each other.",155,1,0, "bilko-opti-bold", 42]]

	render_text_screen(strings, duration=len(strings), plus=plus, exit_callback=demo_exit_test)

	strings = [["This allows you to create",30,1,0, "bilko-opti-bold", 42],
				["stunning parallax scrolling",55,1,0, "bilko-opti-bold", 42],
				["games like XENON II and",80,1,0, "bilko-opti-bold", 42],
				["SILKWORM.",105,1,0, "bilko-opti-bold", 42]]

	render_text_screen(strings, duration=len(strings), plus=plus, exit_callback=demo_exit_test)

	fx_timer = 0.0
	fx_duration = 15.0
	screen_0 = [0, 200, -1]
	screen_1 = [0, 200, -2]

	while fx_timer < fx_duration:
		global plus

		demo_exit_test()
		dt_sec = clock.update()
		fx_timer += dt_sec
		plus.Clear()
		plus.SetBlend2D(gs.BlendAlpha)

		# back image
		plus.Image2D((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5 + (screen_0[0] - amiga_screen_size[0]) * zoom_size(),
					(amiga_screen_size[1] - screen_0[1]) * zoom_size(),
					zoom_size() * 0.5, "@assets/DPLAY2.png")
		plus.Image2D((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5 + screen_0[0] * zoom_size(),
					(amiga_screen_size[1] - screen_0[1]) * zoom_size(),
					zoom_size() * 0.5, "@assets/DPLAY2.png")
		plus.Image2D((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5 + (screen_0[0] + amiga_screen_size[0]) * zoom_size(),
					(amiga_screen_size[1] - screen_0[1]) * zoom_size(),
					zoom_size() * 0.5, "@assets/DPLAY2.png")
		plus.Image2D((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5 + (screen_0[0] + amiga_screen_size[0] * 2) * zoom_size(),
					(amiga_screen_size[1] - screen_0[1]) * zoom_size(),
					zoom_size() * 0.5, "@assets/DPLAY2.png")

		screen_0[0] += dt_sec * 60.0 * screen_0[2]
		if screen_0[0] < -amiga_screen_size[0]:
			screen_0[0] = 0

		# front image
		plus.Image2D((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5 + (screen_1[0] - amiga_screen_size[0]) * zoom_size(),
					(amiga_screen_size[1] - screen_1[1]) * zoom_size(),
					zoom_size() * 0.5, "@assets/DPLAY1.png")
		plus.Image2D((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5 + screen_1[0] * zoom_size(),
					(amiga_screen_size[1] - screen_1[1]) * zoom_size(),
					zoom_size() * 0.5, "@assets/DPLAY1.png")
		plus.Image2D((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5 + (screen_1[0] + amiga_screen_size[0]) * zoom_size(),
					(amiga_screen_size[1] - screen_1[1]) * zoom_size(),
					zoom_size() * 0.5, "@assets/DPLAY1.png")
		plus.Image2D((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5 + (screen_1[0] + amiga_screen_size[0] * 2) * zoom_size(),
					(amiga_screen_size[1] - screen_1[1]) * zoom_size(),
					zoom_size() * 0.5, "@assets/DPLAY1.png")

		screen_1[0] += dt_sec * 60.0 * screen_1[2]
		if screen_1[0] < -amiga_screen_size[0]:
			screen_1[0] = 0

		plus.SetBlend2D(gs.BlendOpaque)
		plus.Flip()


def render_title_page_bouncing():
	global plus

	fx_timer = 0.0
	fx_duration = math.pi
	while fx_timer < 4.0:
		demo_exit_test()
		dt_sec = clock.update()
		fx_timer += dt_sec
		y_damping = RangeAdjust(fx_timer, 0.0, fx_duration, 1.0, 0.0)
		y_damping = Clamp(y_damping, 0.0, 1.0)
		y_damping = EaseInOutQuick(y_damping)
		y_scr = abs(math.sin((fx_timer + 1.2) * 4.0)) * y_damping
		plus.Clear()
		plus.Image2D((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5, y_scr * demo_screen_size[1], zoom_size(), "@assets/titlepage.png")
		plus.Flip()


def render_title_page_still():
	global plus

	fx_timer = 0.0
	while fx_timer < 4.0:
		demo_exit_test()
		dt_sec = clock.update()
		fx_timer += dt_sec
		plus.Clear()
		plus.Image2D((demo_screen_size[0] - amiga_screen_size[0] * zoom_size()) * 0.5, 0, zoom_size(), "@assets/titlepage.png")
		plus.Flip()


def main():
	if getattr(sys, 'frozen', False):
	    # frozen
	    dir_ = dirname(sys.executable)
	else:
	    # unfrozen
	    dir_ = dirname(realpath(__file__))

	startup_sequence()
	if resolution_requester(open_gui=True):
		engine_init()
		render_title_page_bouncing()
		play_music()
		render_title_page_still()
		render_credits()
		render_title_page()
		render_hardsprite()
		render_hotdog_screen()
		render_gipper()
		render_gippers()
		render_star()
		render_hardscroll()
		render_dual_playfield()
		render_overlay()
		render_change_fonts()
		render_price()

if __name__ == "__main__":
	main()