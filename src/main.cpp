#include <Arduino.h>
#include <Audio.h>
#include <SD.h>
#include <FS.h>
#include <OneButton.h>
#include <ArduinoJson.h>

// SD card connections
#define SD_CS 5
#define SD_MOSI 23
#define SD_MISO 19
#define SD_CLK 18

// Max98357 I2S connections
#define I2S_DOUT 22
#define I2S_BCLK 26
#define I2S_LRC 25

// Buttons
OneButton button_prw = OneButton(
  2,            // Input pin for the button
  false,        // Button is active LOW
  false         // Enable internal pull-up resistor
);

OneButton button_nxt = OneButton(
  4,            // Input pin for the button
  false,        // Button is active LOW
  false         // Enable internal pull-up resistor
);

OneButton sw_shuffle = OneButton(
  32,           // Input pin for the button
  false,        // Button is active LOW
  false         // Enable internal pull-up resistor
);

bool shuffle = false;
TaskHandle_t Task1;

// Audio object
Audio audio;
int audio_volume = 0;

// File
File dir;
String startup_melody_dir = "/startup_melody";
String default_mp3;
int file_num = 0;
int file_index = 0;
String file_list[256];
int mp3_index = 0;




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
      }
    }

    file = dir.openNextFile();
  }

  return i;
}

// Stop current playing mp3
void stop_mp3(){
  Serial.println("Stopping current mp3 playback");
  audio.stopSong();
  delay(200);           // Wait for to finish
}

// Next mp3
void next_mp3(){
  stop_mp3();

  int index = mp3_index;
  Serial.print("File index:");
  Serial.println(mp3_index);

  if (mp3_index < file_num)
  {
    mp3_index++;
  }
  if (mp3_index >= file_num)
  {
    mp3_index = 0;
  }
  
  String mp3_path = startup_melody_dir + "/" + file_list[mp3_index];
  Serial.println(mp3_path);

  // Play mp3
  audio.connecttoFS(SD, mp3_path.c_str());
}

// Previous mp3
void previous_mp3(){
  stop_mp3();

  int index = mp3_index;
  Serial.print("File index:");
  Serial.println(mp3_index);

  if (mp3_index <= file_num)
  {
    mp3_index--;
  }
  if (mp3_index <= 0)
  {
    mp3_index = file_num - 1;
  }
  
  String mp3_path = startup_melody_dir + "/" + file_list[mp3_index];
  Serial.println(mp3_path);

  // Play mp3
  audio.connecttoFS(SD, mp3_path.c_str());
}

// Volume control - increase
void increase_volume(){
  if (audio_volume <= 21)
  {
    audio_volume++;
  }
  audio.setVolume(audio_volume);
  Serial.print("Volume:");
  Serial.println(audio_volume);
  delay(200);
}

// Volume control - decrease
void decrease_volume(){
  if (audio_volume > 1)
  {
    audio_volume--;
  }
  audio.setVolume(audio_volume);
  Serial.print("Volume:");
  Serial.println(audio_volume);
  delay(200);
}

void shuffle_mode_true(){
  Serial.println("Shuffle mode True");
  shuffle = true;
}

void shuffle_mode_false(){
  Serial.println("Shuffle mode False");
  shuffle = false;
}



//Buttom task
void Task_buttonHandler( void * pvParameters ){
  Serial.print("Task_buttonHandler running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    // keep watching the push buttons:
    button_prw.tick();
    button_nxt.tick();
    sw_shuffle.tick();
  }
}

void init(){
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
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    
  // Set Volume
  audio_volume = 10;
  audio.setVolume(audio_volume); //21 max volume
}

void setCurrentMp3(){
  Serial.println("setCurrentMp3");
}

void init_btn(){
  // link the button 1 functions.
  button_prw.attachClick(previous_mp3);
  button_prw.attachDuringLongPress(decrease_volume);
  button_prw.attachDoubleClick(setCurrentMp3);

  // link the button 2 functions.
  button_nxt.attachClick(next_mp3);
  button_nxt.attachDuringLongPress(increase_volume);
  button_nxt.attachDoubleClick(setCurrentMp3);

  sw_shuffle.attachLongPressStart(shuffle_mode_true);
  sw_shuffle.attachLongPressStop(shuffle_mode_false);

  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
                    Task_buttonHandler,           /* Task function. */
                    "Task_buttonHandler",         /* name of task. */
                    10000,              /* Stack size of task */
                    NULL,               /* parameter of the task */
                    tskIDLE_PRIORITY,   /* priority of the task */
                    &Task1,             /* Task handle to keep track of created task */
                    0);                 /* pin task to core 0 */     
}



void readConfigFile(){
  File jsonFile;
  DynamicJsonDocument doc(1024);

  Serial.println("Config file:");
  jsonFile = SD.open("/config.json", FILE_READ);

  if (!jsonFile) {
  Serial.println("Failed to open JSON file, reverting to default values");
  }

  // Parse JSON file
  DeserializationError error = deserializeJson(doc, jsonFile);
  if (error) {
    Serial.println("Error on parsing JSON file.");
  }

  // Extract values from JSON
  audio_volume = doc["audio_volume"].as<int>();
  default_mp3 = doc["default_mp3"].as<String>();

  Serial.println(doc["audio_volume"].as<int>());
  Serial.println(doc["default_mp3"].as<String>());

  jsonFile.close();  
}


void setup() {
  init();
  init_btn();
  readConfigFile();

  // Adds all mp3 files in directory to file_list
  file_num = get_mp3_list(SD, startup_melody_dir, file_list);
  Serial.print("Mp3 list count: ");
  Serial.println(file_num);

  String mp3_path;  

  // Picks random mp3 file
  if (shuffle)
  {
    Serial.println("Shuffle ON");

    mp3_index = random(file_num);
    Serial.print("Mp3 index: ");
    Serial.println(mp3_index);
    mp3_path = startup_melody_dir + "/" + file_list[mp3_index];  
  }
  else{
    Serial.println("Shuffle OFF");
    mp3_path = startup_melody_dir + "/" + default_mp3;
  }
  
  Serial.print(mp3_path);
  // Play mp3
  audio.connecttoFS(SD, mp3_path.c_str());
}



void loop() {
  audio.loop();
}
