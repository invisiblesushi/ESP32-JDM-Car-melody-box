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
String startup_melody_dir = "/startup_melody";
int file_num = 0;
int file_index = 0;
String file_list[256];


int get_mp3_list(fs::FS fs, String dir_name, String file_list[256]){

  File dir = fs.open(dir_name);
  File file = dir.openNextFile();

  int i = 0;

  // Loops through all files in directory and add mp3 files to list
  while (file)
  {
    if (!file.isDirectory()){
      String temp_file = file.name();

      if (temp_file.endsWith(".mp3"))
      {
        file_list[i] = temp_file;
        i++;
        Serial.println(temp_file);
      }
    }

    file = dir.openNextFile();
  }

  return i;
}



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

  dir = SD.open(startup_melody_dir);

  // Setup I2S 
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DIN);
    
  // Set Volume
  audio.setVolume(30);
    
  // Open music file
  //audio.connecttoFS(SD,"mp3/jdmthankyousequence2.mp3");

  // Adds all mp3 files in directory to file_list
  file_num = get_mp3_list(SD, startup_melody_dir, file_list);
  Serial.print("Mp3 list count: ");
  Serial.println(file_num);

  // Picks random mp3 file
  int random_index = random(file_num);
  Serial.println(random_index);

  String mp3_path = startup_melody_dir + "/" + file_list[random_index];
  Serial.print(mp3_path);

  // Play mp3
  audio.connecttoFS(SD, mp3_path.c_str());
}


void loop() {
  audio.loop();
}





