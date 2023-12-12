import os
import librosa
import soundfile as sf

os.chdir("Software/PlayMP3toDAC")
target_sr = 22050
folder_name = "mp3"
os.chdir(folder_name)
try:
    os.mkdir("processed")
except OSError as error:
    print(error)
for file in os.listdir():
    if file.endswith(".mp3"):
        sr = librosa.get_samplerate(file)
        y, sr = librosa.load(file, sr=sr, mono=True)
        y_resampled = librosa.resample(y, orig_sr=sr, target_sr=target_sr)
        sf.write(
            os.path.join("processed", file),
            y_resampled,
            samplerate=target_sr,
            format="mp3",
        )
        print("resampled:", file)
