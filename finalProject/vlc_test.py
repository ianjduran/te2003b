import os
import vlc
import tkinter
from time import sleep
from glob import glob
import serial





def set_song(instance_player, instance_vlc, song_path):
    Media = instance_vlc.media_new(song_path)
    instance_player.set_media(Media)

#play button function
def on_play():
    if (player.is_playing() == 0 and player.get_media() == None): #Not playing and has no song
        title.config(text='Whoops. Something\'s wrong. Let\'s try again')
        ser.write(b'err')
        return
    if(player.is_playing() == 0):
        player.play()
        ser.write(b'play\r\n')
        play_btn["text"]="Pause"
        sleep(0.1)
        # get song info
        current_song = player.get_media()
        current_song.parse_with_options(
            parse_flag=vlc.MediaParseFlag.local,
            timeout=200
        )
        # set text
        title.config(text=str(current_song.get_meta(0)))
        status_label.config(text="NOW PLAYING:")
    else:
        play_btn["text"]="Play"
        ser.write(b'pause\r\n')
        status_label.config(text="PAUSED")
        player.pause()
    show_label()

#stop button function
def on_stop():
    ser.write(b'stop\r\n')
    player.stop()
    title.config(text="")
    status_label.config(text="STOPPED")
    play_btn["text"]="Play"

#next button function
def on_next():
    global Media
    global current_song
    ser.write(b'next')
    player.stop()
    next_song=current_song+1
    # check if there is next song
    if(next_song<len(file_paths)):
        current_song=next_song
        # Set media to next song and attach to player
        #Media = vlc_instance.media_new()
        #player.set_media(Media)
        set_song(player, vlc_instance, file_paths[current_song])
        on_play()
    else: # there is no next song
        title.config(text="You reached the end!")

#previous button function
def on_previous():
    global current_song
    global Media
    ser.write(b'previous')
    player.stop()
    next_song=current_song-1
    # Check if there is a previous song
    if(next_song>-1):
        current_song=next_song
        # set media to previous song and attach to player
        set_song(player, vlc_instance, file_paths[current_song])
        # Media = vlc_instance.media_new(file_paths[current_song])
        # player.set_media(Media)
    on_play() # If there is no previous song, it will replay the first one.

# Show labels
def show_label():
    status_label.grid(row=0,column=0,padx=20,pady=20)
    title.grid(row=1,column=1)

ser = serial.Serial('/dev/ttyS0', 9600, timeout=0)

def handle_serial_rx(input):
    # TODO Add more commands
    input = input.strip().decode()
    print(input)
    if input == 'z':
        on_play()
    elif input == 'n':
        on_next()
    elif input =='p':
        on_previous()
    elif input == 's':
        on_stop()

prev_name = ""
def handle_serial_tx():
    name = os.path.basename(file_paths[current_song])
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
tkinter_root.title("Mp3 player")
player.set_hwnd(tkinter_root.winfo_id())


# Get Directory
directory = os.getcwd()
print("Actual Directory: " )
print(directory)
print()


# Get List of file_paths in str array
file_paths = glob('./songs/*.mp3')
for i in file_paths:
    print(directory + i)
print()
print(str(len(file_paths)) + ' file_paths')


vlc_instance = vlc.Instance()
#Create and Attach new media file
current_song = 0
# Media  = vlc_instance.media_new(file_paths[current_song])
# player.set_media(Media)
set_song(player, vlc_instance, file_paths[current_song])
print()

# Set Labels
status_label = tkinter.Label(tkinter_root, text="Let\'s start! Just hit Play!")
title = tkinter.Label(tkinter_root)
show_label()

# Create play Button
play_btn = tkinter.Button(tkinter_root, text ="Play", command = on_play)
play_btn.grid(row=2,column=0,padx=20,pady=20)
# Create stop Button
stop_btn = tkinter.Button(tkinter_root, text ="Stop", command = on_stop)
stop_btn.grid(row=2,column=2,padx=20,pady=20)
# Create next Button
next_btn = tkinter.Button(tkinter_root, text="Next", command = on_next)
next_btn.grid(row=3,column=0,padx=20,pady=20)
# Create previous Button
prev_btn = tkinter.Button(tkinter_root, text="Previous", command = on_previous)
prev_btn.grid(row=3,column=2,padx=20,pady=20)

tkinter_root.after(200, handle_serial)
tkinter_root.mainloop()
ser.close()
