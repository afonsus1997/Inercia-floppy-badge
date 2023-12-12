import os

# import time
import array
import math
import audiocore
import board
import audiobusio
from analogio import AnalogIn
import adafruit_sdcard
import digitalio
import busio
import storage
from audiomp3 import MP3Decoder
import audiomixer
import random
from adafruit_debouncer import Debouncer

SD_CS = board.GP13
volume_pot = AnalogIn(board.A0)
top_button = digitalio.DigitalInOut(board.GP5)
mid_button = digitalio.DigitalInOut(board.GP6)
bottom_button = digitalio.DigitalInOut(board.GP4)
top_switch = Debouncer(top_button, interval=0.01)
mid_switch = Debouncer(mid_button, interval=0.01)
bottom_switch = Debouncer(bottom_button, interval=0.01)

spi = busio.SPI(board.GP14, board.GP15, board.GP12)
cs = digitalio.DigitalInOut(SD_CS)
card = adafruit_sdcard.SDCard(spi, cs)
vfs = storage.VfsFat(card)
storage.mount(vfs, "/sd")


def print_directory(path, tabs=0):
    for file in os.listdir(path):
        print(file)


audio = audiobusio.I2SOut(board.GP20, board.GP21, board.GP22)
mixer = audiomixer.Mixer(
    voice_count=1, sample_rate=22050, channel_count=2, buffer_size=32768
)
sample_rate = 22000
wave = array.array("H", [0] * sample_rate)


def get_voltage(pin):
    return pin.value / 65536


def play_tone(frequency):
    volume = get_voltage(volume_pot)
    period = int(sample_rate / frequency)
    length = period
    wave = array.array("h", [0] * length)

    for i in range(length):
        wave[i] = int(math.sin(math.pi * 2 * i / period) * (2**15) * volume)

    wave = audiocore.RawSample(wave, sample_rate=sample_rate)
    audio.play(wave, loop=True)


def play_random(mp3):
    mixer.stop_voice(0)
    mp3.deinit()
    random_file = random.choice(song_list)
    mp3 = MP3Decoder(open("/sd/mp3/" + random_file, "rb"))
    mixer.voice[0].play(mp3)
    mixer.voice[0].level = volume
    return mp3


def play_next(currently_playing, mp3):
    mixer.stop_voice(0)
    mp3.deinit()
    currently_playing += 1
    if currently_playing >= len(song_list):
        currently_playing = 0
    song_name = song_list[currently_playing]
    print(song_name)
    mp3 = MP3Decoder(open("/sd/mp3/" + song_name, "rb"))
    mixer.voice[0].play(mp3)
    mixer.voice[0].level = volume
    return currently_playing, mp3


def list_mp3s():
    return os.listdir("/sd/mp3")


# play_tone(440)
# time.sleep(1)
currently_playing = 0
song_list = list_mp3s()
song_name = song_list[currently_playing]
print(song_name)
volume = get_voltage(volume_pot)
mp3 = MP3Decoder(open("/sd/mp3/" + song_name, "rb"))
audio.play(mixer)
mixer.voice[0].play(mp3, loop=True)
while True:
    top_switch.update()
    mid_switch.update()
    bottom_switch.update()
    if top_switch.fell:
        currently_playing, mp3 = play_next(currently_playing, mp3)
    if mid_switch.fell:
        if audio.paused:
            audio.resume()
        else:
            audio.pause()
    if bottom_switch.fell:
        mp3 = play_random(mp3)
    volume = get_voltage(volume_pot)
    mixer.voice[0].level = volume
    pass
