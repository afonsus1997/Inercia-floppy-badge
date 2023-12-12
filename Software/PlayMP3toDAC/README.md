# Inércia 2023 Floppy Badge MP3 Support

This little *CircuitPython* script allows for the playback of .mp3 (and .wav if you really want to) from local storage and micro SD card.

## Usage

Flash CircuitPython 8.x from [here](https://circuitpython.org/board/raspberry_pi_pico/) onto the badge.

After the reboot copy the `lib` directory and `code.py` to the root of the, now available, `CIRCUITPY` device/directory on your computer.

The mixer in the firmware expects a single channel with a sample rate of 22050 Hz, so there is a `resampler.py` script that expects mp3 files in a mp3 folder and resamples everything to the required format. There's also a `requirements.txt` file. I recommend you use `venv` or something of the sorts.

Place your mp3 files into an `mp3` directory on you micro SD card.

That's pretty much it. Power the badge and it should play sound.

## Dependencies

- CircuitPython 8.x
- Adafruit CircuitPython Bundle 8.x