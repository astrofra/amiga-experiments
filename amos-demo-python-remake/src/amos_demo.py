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
import time

gs.LoadPlugins(gs.get_default_plugins_path())

# mount the system file driver
gs.MountFileDriver(gs.StdFileDriver())

# create an OpenAL mixer and wrap it with the MixerAsync interface
al = gs.ALMixer()
al.Open()
channel = al.Stream(os.path.join(os.getcwd(), "assets/amos_demo_music.xm"))

# wait until the user decides to exit the program or the stream ends
print("Playing on channel %d, press Ctrl+C to stop." % channel)

while al.GetPlayState(channel) == gs.MixerPlaying:
	time.sleep(0.1)
