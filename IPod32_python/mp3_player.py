import os
import vlc
import tkinter
from time import sleep
from glob import glob
import serial
import status

meta_info =  {vlc.Meta.Title: None,
            vlc.Meta.Artist: None,
            vlc.Meta.Album: None,
            vlc.Meta.Date: None}


rfid_songs = {
    '1': None,
    '2': None,
    '3': None
}

### INIT ###
# Create vlc instance and media player
vlc_instance = vlc.Instance()
player = vlc.MediaPlayer()
# Set Window Handle
tkinter_root = tkinter.Tk()
tkinter_root.title("Mp3 player")
player.set_hwnd(tkinter_root.winfo_id())

current_song_index = 0
current_song : vlc.Media

def set_btn_img( btn: tkinter.Button, img: tkinter.PhotoImage):
    btn.configure(image= img)
    btn.image = img

def set_song(instance_player, instance_vlc, song_path):
    Media = instance_vlc.media_new(song_path)
    instance_player.set_media(Media)

def get_metadata(song):
    temp = meta_info.copy()
    for key in meta_info.keys():
        v = song.get_meta(key)
        temp[key] = v
    return temp

#play button function
def on_play():
    global current_song
    if (player.is_playing() == 0 and player.get_media() == None): #Not playing and has no song
        title.config(text='Whoops. Something\'s wrong. Let\'s try again')
        return
    if(player.is_playing() == 0): #Play Action
        player.play()
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
        # set image
        set_btn_img(play_btn,pause_icon)
        # Write to Serial
        write_status_to_lcd(current_song, status.PLAY)
    else: # Pause Action
        player.pause()
        play_btn["text"]="Play"
        status_label.config(text="PAUSED")
        # set image
        set_btn_img(play_btn, play_icon)
        write_status_to_lcd(current_song, status.PAUSE)
        
    show_label()

#stop button function
def on_stop():
    player.stop()
    title.config(text="")
    status_label.config(text="STOPPED")
    play_btn["text"]="Play"
    # set image
    play_btn.configure(image=play_icon)
    play_btn.image = play_icon
    write_status_to_lcd(current_song, status.STOP)

#next button function
def on_next():
    global current_song_index
    player.stop()
    next_song=current_song_index+1
    # check if there is next song
    if(next_song<len(file_paths)):
        current_song_index=next_song
        # Set media to next song and attach to player
        set_song(player, vlc_instance, file_paths[current_song_index])
        on_play()
    else: # there is no next song
        title.config(text="You reached the end!")
        # set image
        play_btn.configure(image=play_icon)
        play_btn.image = play_icon
        write_str_to_lcd("Phew.. you made it this far! Press play to listen to the last song!", status.END)

#previous button function
def on_previous():
    global current_song_index
    player.stop()
    next_song=current_song_index-1
    # Check if there is a previous song
    if(next_song>-1):
        current_song_index=next_song
        # set media to previous song and attach to player
        set_song(player, vlc_instance, file_paths[current_song_index])
    on_play() # If there is no previous song, it will replay the first one.

# Show labels
def show_label():
    status_label.grid(row=0,columnspan=4,padx=20,pady=20)
    title.grid(row=1,columnspan=4)

ser = serial.Serial('/dev/ttyS0', 9600, timeout=0)

def handle_serial_rx(input):
    global current_song_index
    input = input.strip().decode()
    print(input)
    if input == '*':
        on_play()
    elif input =='0':
        on_previous()
    elif input == '#':
        on_next()
    elif input == 'D':
        on_stop()
    elif input== '1' or input== '2' or input== '3':
        print(rfid_songs)
        if(rfid_songs[input]!=None):
            current_song_index=rfid_songs[input]
            print(file_paths[current_song_index])
            set_song(player, vlc_instance, file_paths[current_song_index])
            player.stop()
            on_play()
        else:
            print(current_song_index, "->", input)
            rfid_songs[input] = current_song_index


def write_status_to_lcd(song, status_text):
    track_info = get_metadata(song)
    lcd1_text = " - ".join([str(e) for e in track_info.values()])
    data_out = ("{},{}".format(lcd1_text,status_text) + '\0').encode('utf-8') 
    ser.write(data_out)

def write_str_to_lcd(text1,text2):
    data_out = ("{},{}".format(text1,text2) + '\0').encode('utf-8') 
    ser.write(data_out)

def handle_serial():
    input = ser.readlines()
    if len(input) == 1:
        handle_serial_rx(input[0])
    # handle_serial_tx()
    tkinter_root.after(100, handle_serial)




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

set_song(player, vlc_instance, file_paths[current_song_index])
print()

# Set Labels
status_label = tkinter.Label(tkinter_root, text="WELCOME!")
title = tkinter.Label(tkinter_root, text="Let\'s start! Just hit Play!")
show_label()

# Create photoImages
play_icon = tkinter.PhotoImage(file='./img/play_img.png')
pause_icon = tkinter.PhotoImage(file='./img/pause_img.png')
prev_icon = tkinter.PhotoImage(file='./img/prev_img.png')
next_icon = tkinter.PhotoImage(file='./img/next_img.png')
stop_icon = tkinter.PhotoImage(file='./img/stop_img.png')

# Create play Button
play_btn = tkinter.Button(tkinter_root, text ="Play", image=play_icon, command = on_play)
play_btn.grid(row=2,column=0,padx=20,pady=20)
# Create previous Button
prev_btn = tkinter.Button(tkinter_root, text="Previous", image=prev_icon, command = on_previous)
prev_btn.grid(row=2,column=1,padx=20,pady=20)
# Create next Button
next_btn = tkinter.Button(tkinter_root, text="Next", image=next_icon, command = on_next)
next_btn.grid(row=2,column=2,padx=20,pady=20)
# Create stop Button
stop_btn = tkinter.Button(tkinter_root, text ="Stop", image=stop_icon, command = on_stop)
stop_btn.grid(row=2,column=3,padx=20,pady=20)

tkinter_root.after(100, handle_serial)
tkinter_root.mainloop()
ser.close()
