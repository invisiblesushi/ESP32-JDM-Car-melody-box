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
#define I2S_DOUT 22
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


// This statement will declare pin 15 as digital input 
pinMode(15, INPUT);
pinMode(2, INPUT);
pinMode(4, INPUT);
pinMode(32, INPUT);

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
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    
  // Set Volume
  audio.setVolume(64); //{ 0, 1, 2, 3, 4 , 6 , 8, 10, 12, 14, 17, 20, 23, 27, 30 ,34, 38, 43 ,48, 52, 58, 64}

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


  //audio.loop();

  /*
  if (audio.isRunning() == false)
  {
    audio.connecttoFS(SD, "/startup_melody/2Tofu delivery machine ready to go!.mp3");
  }
  */

 // digitalRead function stores the Push button state 
// in variable push_button_state
int Push_button15_state = digitalRead(15);
int Push_button2_state = digitalRead(2);
int Push_button4_state = digitalRead(4);
int Push_button32_state = digitalRead(32);
// if condition checks if push button is pressed
// if pressed LED will turn on otherwise remain off 
if ( Push_button15_state == HIGH )
{
  Serial.print("15 High");
  sleep(1);
}

if ( Push_button2_state == HIGH )
{
  Serial.print("2 High");
  sleep(1);
}

if ( Push_button4_state == HIGH )
{
  Serial.print("4 High");
  sleep(1);
}

if ( Push_button32_state == HIGH )
{
  audio.loop();
}

}





