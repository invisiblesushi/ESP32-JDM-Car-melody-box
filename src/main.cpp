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
OneButton buttonPrevious = OneButton(
  2,            // Input pin for the button
  false,        // Button is active LOW
  false         // Enable internal pull-up resistor
);

OneButton buttonNext = OneButton(
  4,            // Input pin for the button
  false,        // Button is active LOW
  false         // Enable internal pull-up resistor
);

TaskHandle_t ButtonTask;

// Audio object
Audio audio;
int audio_volume = 10;

// File
File dir;
String startupMelodyDirectory = "/startup_melody";
String defaultMp3;
String currentMp3;
int fileCount = 0;
int fileIndex = 0;
String fileList[256];
int mp3Index = 0;
File jsonFile;
DynamicJsonDocument doc(1024);

int get_mp3_list(fs::FS fs, String dir_name, String fileList[256]){

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
        fileList[i] = temp_file;
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

  int index = mp3Index;
  Serial.print("File index:");
  Serial.println(mp3Index);

  if (mp3Index < fileCount)
  {
    mp3Index++;
  }
  if (mp3Index >= fileCount)
  {
    mp3Index = 0;
  }
  
  String mp3_path = startupMelodyDirectory + "/" + fileList[mp3Index];
  Serial.println(mp3_path);

  // Play mp3
  audio.connecttoFS(SD, mp3_path.c_str());
}

// Previous mp3
void previous_mp3(){
  stop_mp3();

  int index = mp3Index;
  Serial.print("File index:");
  Serial.println(mp3Index);

  if (mp3Index <= fileCount)
  {
    mp3Index--;
  }
  if (mp3Index <= 0)
  {
    mp3Index = fileCount - 1;
  }
  
  String mp3_path = startupMelodyDirectory + "/" + fileList[mp3Index];
  Serial.println(mp3_path);

  // Play mp3
  audio.connecttoFS(SD, mp3_path.c_str());
}

void readConfigFile(){
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
  audio.setVolume(audio_volume);

  defaultMp3 = doc["default_mp3"].as<String>();

  Serial.println(audio_volume);
  Serial.println(defaultMp3);

  jsonFile.close();  
}

void updateConfigFile(){
  // Open JSON file for writing
  jsonFile = SD.open("/config.json", FILE_WRITE);
  if (!jsonFile) {
    Serial.println("Failed to open JSON file for writing.");
    while (1);
  }

  // Update JSON object with new values
  doc["audio_volume"] = audio_volume;
  doc["default_mp3"] = defaultMp3;

  // Serialize JSON object and write to file
  if (serializeJson(doc, jsonFile) == 0) {
    Serial.println("Failed to write updated JSON to file.");
  }
  else{
    Serial.println("Config file updated");
  }

  // Close JSON file
  jsonFile.close();
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
  updateConfigFile();
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
  updateConfigFile();
  delay(200);
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

  dir = SD.open(startupMelodyDirectory);

  // Setup I2S 
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    
  // Set Volume
  audio.setVolume(audio_volume);      //21 max volume
}

void setCurrentMp3(){
  Serial.println("setCurrentMp3");
  defaultMp3 = currentMp3;
  updateConfigFile();
}

//Buttom task
void Task_buttonHandler( void * pvParameters ){
  Serial.print("Task_buttonHandler running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    // keep watching the push buttons:
    buttonPrevious.tick();
    buttonNext.tick();
  }
}

void init_btn(){
  // link the button 1 functions.
  buttonPrevious.attachClick(previous_mp3);
  buttonPrevious.attachDuringLongPress(decrease_volume);
  buttonPrevious.attachDoubleClick(setCurrentMp3);

  // link the button 2 functions.
  buttonNext.attachClick(next_mp3);
  buttonNext.attachDuringLongPress(increase_volume);
  buttonNext.attachDoubleClick(setCurrentMp3);


  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
                    Task_buttonHandler,           /* Task function. */
                    "Task_buttonHandler",         /* name of task. */
                    10000,                        /* Stack size of task */
                    NULL,                         /* parameter of the task */
                    tskIDLE_PRIORITY,             /* priority of the task */
                    &ButtonTask,                       /* Task handle to keep track of created task */
                    0);                           /* pin task to core 0 */     
}

String randomMp3(int fileNumber){
    mp3Index = random(fileNumber);
    Serial.print("Mp3 index: ");
    Serial.println(mp3Index);
    return fileList[mp3Index];  
}

bool checkExist(String fileName){
  if (SD.exists(startupMelodyDirectory + "/" + fileName))
  {
    Serial.println("File found");
    return true;
  }
  else{
    Serial.println("File not found");
    return false;
  }
  
}

void setup() {
  init();
  init_btn();
  readConfigFile();

  // Adds all mp3 files in directory to fileList
  fileCount = get_mp3_list(SD, startupMelodyDirectory, fileList);
  Serial.print("Mp3 list count: ");
  Serial.println(fileCount);

  String mp3_path;  

  pinMode(32, INPUT_PULLUP);
  bool shuffleSwitch = digitalRead(32);

  if(shuffleSwitch == HIGH && checkExist(defaultMp3) == true){
    Serial.println("Shuffle OFF");
    currentMp3 = defaultMp3;
    mp3_path = startupMelodyDirectory + "/" + currentMp3;
  }
  else{
    Serial.println("Shuffle ON");
    currentMp3 = randomMp3(fileCount);
    mp3_path = startupMelodyDirectory + "/" + currentMp3;
  }

  // Play mp3
  Serial.print(mp3_path);
  audio.connecttoFS(SD, mp3_path.c_str());
}

void loop() {
  audio.loop();
}