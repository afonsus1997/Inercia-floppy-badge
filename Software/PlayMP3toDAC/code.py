import os
import time
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

# SD card pin SPI pin
SD_CS = board.GP13

spi = busio.SPI(board.GP14, board.GP15, board.GP12)
cs = digitalio.DigitalInOut(SD_CS)
card = adafruit_sdcard.SDCard(spi, cs)
vfs = storage.VfsFat(card)
storage.mount(vfs, "/sd")


def print_directory(path, tabs=0):
    for file in os.listdir(path):
        print(file)


volume_pot = AnalogIn(board.A0)
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


def play_random():
    files = os.listdir("/sd/mp3")
    random_file = random.choice(files)
    mp3 = MP3Decoder(open("/sd/mp3/" + random_file, "rb"))
    mixer.voice[0].play(mp3)
    mixer.voice[0].level = volume


# play_tone(440)
# time.sleep(1)
print_directory("/sd/mp3")
volume = get_voltage(volume_pot)
mp3 = MP3Decoder(open("/sd/mp3/ducktales.mp3", "rb"))
play_random()
audio.play(mixer)
mixer.voice[0].play(mp3, loop=True)
while True:
    volume = get_voltage(volume_pot)
    mixer.voice[0].level = volume
    pass
