#!/usr/bin/env python3

import vlc
import random
import serial

import time 
arduino = serial.Serial(port='/dev/cu.usbmodem101', baudrate=115200, timeout=.1) 



def play_music():
    while True:
        data = arduino.readline()
        if data == b"":
            continue
        print(data)
        if data.startswith(b"Detected"):
            x = random.random()
            if 0<=x<=.3333:
                p = vlc.MediaPlayer("file:///Users/afix/Downloads/Scav2026/Short Polka tramblanka.mp3")
                print("Playing polka")
                p.play()
                time.sleep(16.5)
            elif .3333 < x <=.6666:
                p = vlc.MediaPlayer("file:///Users/afix/Downloads/Scav2026/Short W moim ogródecku (In my garden) Polish Folk Song [Eng sub, HQ].mp3")
                print("Playing garder")
                p.play() 
                time.sleep(29.5)               
            else:
                p = vlc.MediaPlayer("file:///Users/afix/Downloads/Scav2026/Short Hej, sokoły! - Polish Folk Song.mp3")
                print("Playing Hej")
                p.play()
                time.sleep(13)  

    return


def main():
    play_music()


if __name__=="__main__":
    main()
