/* ***********************************************

by DrNeurosurg 2024

PlatformIO

Libraries used (Arduino):

LovyanGFX by lovyan03 installed via lib_deps
Version 1.1.12

lvgl by kisvegabor installed via lib_deps
Version 9.1.0

ESP32-AudioI2S by schreibfaul1 (wolle) included in "lib"
Version 3.0.7c

Array by Peter Polidoro  installed via lib_deps
*********************************************** */

#include <Arduino.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <SPI.h>
#include <SD.h>
#include <SD_MMC.h>
#include <SPIFFS.h>
#include <FFat.h>
#include <Array.h>

// **************************  EDIT FOR YOUR WiFi  ****************************************
String ssid =     "YOUR_WIFI_SSID";
String password = "YOUR_WIFI_PASSWORD";


#define WT_USE_LVGL
#include "WT32_SC01_PLUS.h"

#include <Audio.h>
Audio audio;

#include "SimpleGUI.h"
SimpleGUI myGUI;

extern SemaphoreHandle_t lvgl_mux;
#define LVGL_TASK_DELAY_MS   10

#include "Stations.h"



uint32_t _station = 0;
uint32_t _volume = 2 ;
uint32_t _num_stations;
uint32_t _max_volume;

void audioTask(void *parameter) {

    audio.setPinout(WT_I2S_BCK, WT_I2S_WS, WT_I2S_DATA);  //From WT32_SC01_PLUS.h
    audio.setVolume(0); // 0...21
    
    while(true){
        //AVOID CONFLICTS WITH LVGL  
        xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);
          audio.loop();
        xSemaphoreGiveRecursive(lvgl_mux);
        vTaskDelay(pdMS_TO_TICKS(1));
        if (!audio.isRunning()) {
          sleep(1);
        }
    }
}


void audioInit() {
    //CREATE TASK
    xTaskCreatePinnedToCore(
        audioTask,             /* Function to implement the task */
        "audioplay",           /* Name of the task */
        5000,                  /* Stack size in words */
        NULL,                  /* Task input parameter */
        2 | portPRIVILEGE_BIT, /* Priority of the task */
        NULL,                  /* Task handle. */
        1                      /* Core where the task should run */
    );
}

void setup() {

  Serial.begin(115200);
  Serial.println("********* HERE WE GO ! **************");
  Serial.flush();

  //INITIALIZE ARRAY OF STATIONS
  initStations(retroStations);

  //GET NUMBER OF STATIONS
  _num_stations = retroStations.size();

  //CREATE SEMAPHORE FOR LVGL
  lvgl_mux = xSemaphoreCreateRecursiveMutex();

  //LVGL
  init_display();   
  lv_tick_set_cb(xTaskGetTickCount); //LVGL TICK

  //CREATE (SIMPLE) GUI ON ACTUAL SCREEN
  myGUI.init(lv_scr_act());

  //LET LVGL DO ITS WORK
  xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);
      lv_timer_handler();
  xSemaphoreGiveRecursive(lvgl_mux);

  //INIT WIFI
  WiFi.begin(ssid.c_str(), password.c_str());

  //WAIT FOR CONNECT
  while (WiFi.status() != WL_CONNECTED) delay(1500);

  // INIT AUDIO & I2S-PINS & AUDIO_TASK
  audioInit();

  //GET MAX VOLUME
  _max_volume = audio.maxVolume();

  //CONNECT TO FIRST STATION 
  audio.connecttohost(retroStations[_station].URL.c_str());
  
  // SET INITIAL VOLUME
  audio.setVolume(_volume);  

  //SHORT DELAY
  delay(10);

  Serial.println("********* RUNNING AUDIO **************");
  Serial.flush();
  delay(10);


}

void loop() {

  //LET LVGL DO ITS WORK
  xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);
      lv_timer_handler();
  xSemaphoreGiveRecursive(lvgl_mux);

  vTaskDelay(pdMS_TO_TICKS(LVGL_TASK_DELAY_MS));
}

//*****************************************************************************************
//                                  E V E N T S  ( GUI )                                *
//*****************************************************************************************

// VOLUME UP
void GuiVolumeUp() {
   Serial.println("GUI Volume Up");
   if(_volume < _max_volume) {
      _volume++;
     audio.setVolume(_volume);
   }
}

// VOLUME DOWN
void GuiVolumeDown() {
   Serial.println("GUI Volume Down");
   if(_volume > 0) {
      _volume--;
      audio.setVolume(_volume);
   }
}

//NEXT STATION
void GuiStationNext() {
  Serial.println("GUI Station Next");
  if(_station < _num_stations) {
    _station++;
   audio.connecttohost(retroStations[_station].URL.c_str());
  }
}

//PREVIOUS STATION
void GuiStationPrev() {
  Serial.println("GUI Station Prev.");
  if(_station > 0) {
    _station--;
  audio.connecttohost(retroStations[_station].URL.c_str());
  }
}

//*****************************************************************************************
//                                  E V E N T S  ( AUDIO )                                *
//*****************************************************************************************

/* PUSH META_DATA TO GUI */
void audio_showstation(const char *info){
    Serial.print("station     ");Serial.println(info);

    xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);
      myGUI.setStationLabel(info);
    xSemaphoreGiveRecursive(lvgl_mux);

}

void audio_showstreamtitle(const char *info){
    Serial.print("streamtitle ");Serial.println(info);

    xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);
      myGUI.setTitleLabel(info);
    xSemaphoreGiveRecursive(lvgl_mux);

}

// optional
// void audio_info(const char *info){
//     Serial.print("info        "); Serial.println(info);
// }

// void audio_id3data(const char *info){  //id3 metadata
//     Serial.print("id3data     ");Serial.println(info);
// }
// void audio_eof_mp3(const char *info){  //end of file
//     Serial.print("eof_mp3     ");Serial.println(info);
// }

// void audio_bitrate(const char *info){
//     Serial.print("bitrate     ");Serial.println(info);
// }
// void audio_commercial(const char *info){  //duration in sec
//     Serial.print("commercial  ");Serial.println(info);
// }
// void audio_icyurl(const char *info){  //homepage
//     Serial.print("icyurl      ");Serial.println(info);
// }
// void audio_lasthost(const char *info){  //stream URL played
//     Serial.print("lasthost    ");Serial.println(info);
// }
// void audio_eof_speech(const char *info){
//     Serial.print("eof_speech  ");Serial.println(info);
// }
