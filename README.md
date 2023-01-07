# ESP32-JDM-Car-melody-box

Car melody box that plays an startup sound/melody when power is supplied. It's ment to be mounted in a car.

<img src="/Images/Prototype.png" width="500" />
<img src="/Images/Schematic%20diagram.png" width="800" />
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
- OneButton https://github.com/mathertel/OneButton

# Sound file source examples:
- https://ko-fi.com/s/0c76c753a3
- https://ko-fi.com/s/8723a46daa

# SD card directory structure:
- /startup_melody (Mp3 to be played at startup random)

# IDE
- ![Visual Studio Code](https://img.shields.io/badge/Visual%20Studio%20Code-0078d7.svg?style=for-the-badge&logo=visual-studio-code&logoColor=white)
- ![PlatformIO](https://img.shields.io/badge/-PlatformIO-FF7F00.svg?style=for-the-badge&logoColor=white)

[![Hits](https://hits.seeyoufarm.com/api/count/incr/badge.svg?url=https%3A%2F%2Fgithub.com%2Finvisiblesushi%2FESP32-JDM-Car-melody-box&count_bg=%2379C83D&title_bg=%23555555&icon=&icon_color=%23E7E7E7&title=hits&edge_flat=false)](https://hits.seeyoufarm.com)
