# Usefull Commands

# Mute whole system
Mute the system volume 	nircmd.exe mutesysvolume 1
Unmute the system volume 	nircmd.exe mutesysvolume 0
Switch the system volume between the mute and normal state. 	nircmd.exe mutesysvolume 2 

# Application volume
from 0 - 1. Depending on loudness
.\nircmd.exe setappvolume spotify.exe 1

## change Application volume
from 0 - 1. Depending on loudness
.\nircmd.exe changeappvolume spotify.exe 1


# Change system volume by X (from 0 to 65535)
nircmd.exe changesysvolume 2000 

# set system volume (from 0 to 65535)
nircmd.exe setsysvolume 65535 
