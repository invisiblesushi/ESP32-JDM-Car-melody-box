#include <Arduino.h>
#include <Audio.h>
#include <SD.h>
#include <FS.h>
#include <OneButton.h>

// SD card connections
#define SD_CS 5
#define SD_MOSI 23
#define SD_MISO 19
#define SD_CLK 18

// Max98357 I2S connections
#define I2S_DOUT 22
#define I2S_BCLK 26
#define I2S_LRC 25

// Button
OneButton button1 = OneButton(
  2,            // Input pin for the button
  false,        // Button is active LOW
  false         // Enable internal pull-up resistor
);

OneButton button2 = OneButton(
  4,            // Input pin for the button
  false,        // Button is active LOW
  false         // Enable internal pull-up resistor
);

// Audio object
Audio audio;

// File
File dir;
String startup_melody_dir = "/startup_melody";
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
        Serial.println(temp_file);
      }
    }

    file = dir.openNextFile();
  }

  return i;
}

// Stop current playing mp3
void stop_mp3(){
  Serial.println("Stopping mp3");
  audio.stopSong();
  delay(500);       // Wait for to finish
}

// Next mp3
void next_mp3(){
  stop_mp3();

  int index = mp3_index;
  Serial.print("Number of files:");
  Serial.println(file_num);
  Serial.print("index:");
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
  Serial.print("Number of files:");
  Serial.println(file_num);
  Serial.print("index:");
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


// This function will be called once, when the button1 is pressed for a long time.
void longPressStart1() {
  Serial.println("Button 1 longPress start");
}

// This function will be called often, while the button1 is pressed for a long time.
void longPress1() {
  Serial.println("Button 1 longPress...");
sleep(1);
}

// This function will be called once, when the button1 is released after beeing pressed for a long time.
void longPressStop1() {
  Serial.println("Button 1 longPress stop");
}



void longPressStart2() {
  Serial.println("Button 2 longPress start");
}

void longPress2() {
  Serial.println("Button 2 longPress...");
  sleep(1);
}

void longPressStop2() {
  Serial.println("Button 2 longPress stop");
}

TaskHandle_t Task1;

//Buttom task
void Task_btn( void * pvParameters ){
  Serial.print("Task_btn running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    // keep watching the push buttons:
    button1.tick();
    button2.tick();
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
  audio.setVolume(10); //21 max volume
}

void init_Btn(){
  pinMode(15, INPUT);
  pinMode(2, INPUT);
  pinMode(4, INPUT);
  pinMode(32, INPUT);

  // link the button 1 functions.
  button1.attachClick(previous_mp3);
  button1.attachLongPressStart(longPressStart1);
  button1.attachLongPressStop(longPressStop1);
  button1.attachDuringLongPress(longPress1);

  // link the button 2 functions.
  button2.attachClick(next_mp3);
  button2.attachLongPressStart(longPressStart2);
  button2.attachLongPressStop(longPressStop2);
  button2.attachDuringLongPress(longPress2);


  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
                    Task_btn,   /* Task function. */
                    "Task_btn",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    tskIDLE_PRIORITY,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */     
}


void setup() {
  init();
  init_Btn();


  // Adds all mp3 files in directory to file_list
  file_num = get_mp3_list(SD, startup_melody_dir, file_list);
  Serial.print("Mp3 list count: ");
  Serial.println(file_num);

  // Picks random mp3 file
  mp3_index = random(file_num);

  Serial.print("Mp3 index: ");
  Serial.println(mp3_index);

  String mp3_path = startup_melody_dir + "/" + file_list[mp3_index];
  Serial.print(mp3_path);

  // Play mp3
  audio.connecttoFS(SD, mp3_path.c_str());
}



void loop() {
  audio.loop();
}
