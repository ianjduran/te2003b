import os
import vlc
import tkinter
from time import sleep
from glob import glob
import serial

#play button function
def on_play():
    player.play()
    sleep(0.1)
    if (player.is_playing() == 0):
        title.config(text='not work. jelp')
    else:
        # get song info
        Media.parse_with_options(parse_flag=vlc.MediaParseFlag.local, timeout=100)
        # set text
        title.config(text=str(Media.get_meta(0)))
        status_label.config(text="NOW PLAYING:")
    show_label()

#pause button function
def on_pause():
    status_label.config(text="PAUSED")
    player.pause()

#stop button function
def on_stop():
    player.stop()
    title.config(text="")
    status_label.config(text="STOPPED")

#next button function
def on_next():
    global current_song
    global Media
    player.stop()
    next_song=current_song+1
    # check if there is next song
    if(next_song<len(files)):
        current_song=next_song
        # Set media to next song and attach to player
        Media = vlc_instance.media_new(files[current_song])
        player.set_media(Media)
        on_play()
    else: # there is no next song
        title.config(text="fin :)")

#previous button function
def on_previous():
    global current_song
    global Media
    player.stop()
    next_song=current_song-1
    # Check if there is a previous song
    if(next_song>-1):
        current_song=next_song
        # set media to previous song and attach to player
        Media = vlc_instance.media_new(files[current_song])
        player.set_media(Media)
    on_play() # If there is no previous song, it will replay the first one.

# Show labels
def show_label():
    status_label.grid(row=0,column=0,padx=20,pady=20)
    title.grid(row=1,column=1)

ser = serial.Serial('/dev/pts/3', 115200, timeout=0)

def handle_serial_rx(input):
    # TODO Add more commands
    input = input.strip()
    if input == 'p':
        on_play()
    elif input == 'n':
        on_next()

prev_name = ""
def handle_serial_tx():
    name = os.path.basename(files[current_song])
    name = name.strip()[:16]

    ## TODO Get song data
    global prev_name
    if prev_name == name:
        return

    prev_name = name
    print(name)
    #ser.write(name)

def handle_serial():
    input = ser.readlines()
    if len(input) == 1:
        handle_serial_rx(input[0])
    handle_serial_tx()
    tkinter_root.after(200, handle_serial)

# Create vlc instance and media player
player = vlc.MediaPlayer()
# Set Window Handle
tkinter_root = tkinter.Tk()
tkinter_root.title("Best Reproductor ever ft vlc")
player.set_hwnd(tkinter_root.winfo_id())


# Get Directory
directory = os.getcwd()
print("Actual Directory: " )
print(directory)
print()
# Get List of files in str array
files = glob('./*.mp3')
for i in files:
    print(directory + i)
print()
print(str(len(files)) + ' files')


vlc_instance = vlc.Instance()
#Create and Attach new media file
current_song = 0
Media  = vlc_instance.media_new(files[current_song])
player.set_media(Media)

print()

# Set Labels
status_label = tkinter.Label(tkinter_root, text="PONELE A PLAY CHE")
title = tkinter.Label(tkinter_root)
show_label()

# Create play Button
play_btn = tkinter.Button(tkinter_root, text ="Play", command = on_play).grid(row=2,column=0,padx=20,pady=20)
# Create pause Button
pause_btn = tkinter.Button(tkinter_root, text ="Ponele pausa bro", command = on_pause).grid(row=2,column=1,padx=20,pady=20)
# Create stop Button
stop_btn = tkinter.Button(tkinter_root, text ="stap", command = on_stop).grid(row=2,column=2,padx=20,pady=20)
# Create next Button
next_btn = tkinter.Button(tkinter_root, text="siguiente joven", command = on_next).grid(row=3,column=0,padx=20,pady=20)
# Create previous Button
prev_btn = tkinter.Button(tkinter_root, text="volvé bro", command = on_previous).grid(row=3,column=2,padx=20,pady=20)

tkinter_root.after(200, handle_serial)
tkinter_root.mainloop()
ser.close()