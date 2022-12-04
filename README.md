# ESP32-JDM-Car-melody-box

Car melody box that plays an startup sound/melody when power is supplied. It's ment to be mounted in a car.

<img src="/Images/Schematic.png" width="500" />
<img src="/Images/Schematic%20diagram.png" width="500" />
<img src="/Images/PCB.png" width="500" />

# Part list
- ESP32 wroom 32-u usb-c
- Max98357 I2S 3W Class D Amplifier
- Micro SD Storage Expansion Board
- Speaker
- SD card
- MP3 sound files

# Libraries
- ESP32-audioI2S https://github.com/schreibfaul1/ESP32-audioI2S

# Sound file source examples:
https://ko-fi.com/s/8723a46daa

# SD card directory structure:
- /startup_melody (Mp3 to be played at startup random)

# IDE
- Visual studio code
- Platformio
