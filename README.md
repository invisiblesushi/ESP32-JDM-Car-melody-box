# 🎵 ESP32-JDM-Car-melody-box 🚗🎶

Car melody box that plays a startup sound/melody when power is supplied. It's meant to be mounted in a car.

## Button Mapping
- Single press  - Next or Previous mp3
- Long press    - Increase / Decrease volume
- Double click  - Set the current mp3 as default
- Switch        - Shuffle mode on/off

![Prototype Image](/Images/Prototype.png)

![Schematic Diagram](/Images/Schematic%20diagram.png)

![PCB Image](/Images/PCB.png)

## Part List
- ESP32 wroom 32-u USB-C
- Max98357 I2S 3W Class D Amplifier
- Micro SD Storage Expansion Board
- Speaker
- SD card
- MP3 sound files

## Libraries
- [ESP32-audioI2S](https://github.com/schreibfaul1/ESP32-audioI2S)
- [OneButton](https://github.com/mathertel/OneButton)

## Sound File Source Examples:
- [Example 1](https://ko-fi.com/s/0c76c753a3)
- [Example 2](https://ko-fi.com/s/8723a46daa)

## SD Card Directory Structure:
- /startup_melody (Mp3 to be played at startup random)

## IDE
- ![Visual Studio Code](https://img.shields.io/badge/Visual%20Studio%20Code-0078d7.svg?style=for-the-badge&logo=visual-studio-code&logoColor=white)
- ![PlatformIO](https://img.shields.io/badge/-PlatformIO-FF7F00.svg?style=for-the-badge&logoColor=white)

[![Hits](https://hits.seeyoufarm.com/api/count/incr/badge.svg?url=https%3A%2F%2Fgithub.com%2Finvisiblesushi%2FESP32-JDM-Car-melody-box&count_bg=%2379C83D&title_bg=%23555555&icon=&icon_color=%23E7E7E7&title=hits&edge_flat=false)](https://hits.seeyoufarm.com)
