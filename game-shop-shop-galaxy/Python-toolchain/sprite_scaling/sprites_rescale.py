import gs
import gs.plus.render as render
# import gs.plus.input as input
import os
from utils import *

gs.LoadPlugins(gs.get_default_plugins_path())

# provide access to the data folder
gs.MountFileDriver(gs.StdFileDriver("data/"), "@data/")

scaled_copies = 16
min_factor = 0.25

for spr_file in os.listdir("data/"):

	if not spr_file.startswith("scaled_"):
		render.init(512, 512, "../pkg.core")

		spr_pic = gs.LoadPicture("@data/" + spr_file)
		w, h = spr_pic.GetWidth(), spr_pic.GetHeight()

		render.uninit()

		render.init(w * scaled_copies, h, "../pkg.core")

		capture_buffer = gs.Picture(w * scaled_copies, h, gs.Picture.RGBA8)

		for i in range(2):
			render.clear(gs.Color(1, 0, 1))
			for i in range(scaled_copies):
				sprite_scale_factor = RangeAdjust(float(i), 0.0, float(scaled_copies), 1.0, min_factor)
				render.image2d(i * w + (w * (1.0 - sprite_scale_factor) * 0.5), h * (1.0 - sprite_scale_factor) * 0.5, sprite_scale_factor, "@data/" + spr_file)
			# render.sprite2d(512 - 64, 512 - 64, 128, "@data/blink.jpg")
			# render.blit2d(0, 0, 512, 512, 80, 80, 512 - 160, 512 - 160, "@data/owl.jpg")
			render.flip()

		render.get_renderer().CaptureFramebuffer(capture_buffer)
		gs.SavePicture(capture_buffer, "@data/" + "scaled_" + spr_file, 'STB', 'format:png')