#include <Arduino.h>
#include <Audio.h>
#include <SD.h>
#include <FS.h>

// SD card connections
#define SD_CS 5
#define SD_MOSI 23
#define SD_MISO 19
#define SD_CLK 18

// Max98357 I2S connections
#define I2S_DIN 22
#define I2S_BCLK 26
#define I2S_LRC 25

// Audio object
Audio audio;

File dir;



void setup() {
  // Initialize serial port
  Serial.begin(115200);

  // Initialize SD card
  pinMode(SD_CS, OUTPUT);      
  digitalWrite(SD_CS, HIGH); 
  SPI.begin(SD_CLK, SD_MISO, SD_MOSI);

  if(!SD.begin(SD_CS))
  {
    Serial.println("Error accessing microSD card");
    while(true); 
  }

  dir = SD.open("/mp3");

  // Setup I2S 
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DIN);
    
  // Set Volume
  audio.setVolume(30);
    
  // Open music file
  audio.connecttoFS(SD,"mp3/jdmsafedrivesequence2.mp3");


  
  int numberOfFiles = 0;
  String files[100] = {};

  // prints all files in directory
  while (true)
  {
    File entry = dir.openNextFile();

    if (! entry)
    {
      break;
    }

    Serial.println(entry.name());
  };

  Serial.println(numberOfFiles);
}

void loop() {
    audio.loop();    
}
