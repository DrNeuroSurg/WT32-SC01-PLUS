/* ***********************************************

by DrNeurosurg 2024

Arduino IDE Version: 2.3.2

Libraries used (Arduino):

LovyanGFX by lovyan03
Version 1.1.12

lvgl by kisvegabor
Version 9.1.0

ESP32-AudioI2S
Version 3.0.7c
https://github.com/schreibfaul1/ESP32-audioI2S
(to install:
    downlad ZIP from root of this Repo (https://github.com/DrNeuroSurg/WT32-SC01-PLUS/blob/main/ESP32-audioI2S.zip)
    add this ZIP-File via Arduino->Sketch->Include Library ->Add .ZIP Library..
)

Board: ESP32S3 Dev Module

Board Settings:

USB CDC On Boot: "Enabled"
CPU Frequency: "240MHz (WiFi)"
Core Debug Level: "None"
USB DFU On Boot: "Disabled"
Erase All Flash Before Sketch Upload: "Disabled"
Events Run On: "Core 1"
Flash Mode: "QIO 80MHz"
Flash Size: "8MB (64Mb)"
JTAG Adapter: "Disabled"
Arduino Runs On: "Core 1"
USB Firmware MSC On Boot: "Disabled"
Partition Scheme: "8M with spiffs (3MB APP/1.5MB SPIFFS)"
PSRAM: "QSPI PSRAM"
Upload Mode: "UARTO / Hardware CDC"
Upload Speed: "921600"
USB Mode: "Hardware CDC and JTAG"

*********************************************** */

#include <Arduino.h>
#include "WiFi.h"

//CREDENTIALS & STATIONS
#include "config.h"

uint32_t _station = 0;
uint32_t _volume = 2 ;
uint32_t _num_stations;
uint32_t _max_volume;

#define WT_USE_LVGL
#include "WT32_SC01_PLUS.h"

#include "SimpleGUI.h"
SimpleGUI myGUI;

extern SemaphoreHandle_t lvgl_mux;
#define LVGL_TASK_DELAY_MS   10

// https://github.com/schreibfaul1/ESP32-audioI2S
#include "Audio.h"
Audio audio;

//****************************************************************************************
//                                   A U D I O _ T A S K                                 *
// from: https://github.com/schreibfaul1/ESP32-audioI2S/blob/master/examples/separate_audiotask/separate_audiotask.ino
//****************************************************************************************

struct audioMessage{
    uint8_t     cmd;
    const char* txt;
    uint32_t    value;
    uint32_t    ret;
} audioTxMessage, audioRxMessage;

enum : uint8_t { SET_VOLUME, GET_VOLUME, CONNECTTOHOST, CONNECTTOSD };

QueueHandle_t audioSetQueue = NULL;
QueueHandle_t audioGetQueue = NULL;

void CreateQueues(){
    audioSetQueue = xQueueCreate(10, sizeof(struct audioMessage));
    audioGetQueue = xQueueCreate(10, sizeof(struct audioMessage));
}

void audioTask(void *parameter) {
    CreateQueues();
    if(!audioSetQueue || !audioGetQueue){
        log_e("queues are not initialized");
        while(true){;}  // endless loop
    }

    struct audioMessage audioRxTaskMessage;
    struct audioMessage audioTxTaskMessage;

    audio.setPinout(WT_I2S_BCK, WT_I2S_WS, WT_I2S_DATA);  //From WT32_SC01_PLUS.h
    audio.setVolume(0); // 0...21
    
    _max_volume = audio.maxVolume();

    while(true){
        if(xQueueReceive(audioSetQueue, &audioRxTaskMessage, 1) == pdPASS) {
            if(audioRxTaskMessage.cmd == SET_VOLUME){
                audioTxTaskMessage.cmd = SET_VOLUME;
                audio.setVolume(audioRxTaskMessage.value);
                audioTxTaskMessage.ret = 1;
                xQueueSend(audioGetQueue, &audioTxTaskMessage, portMAX_DELAY);
            }
            else if(audioRxTaskMessage.cmd == CONNECTTOHOST){
                audioTxTaskMessage.cmd = CONNECTTOHOST;
                audioTxTaskMessage.ret = audio.connecttohost(audioRxTaskMessage.txt);
                xQueueSend(audioGetQueue, &audioTxTaskMessage, portMAX_DELAY);
            }
            else if(audioRxTaskMessage.cmd == CONNECTTOSD){
                audioTxTaskMessage.cmd = CONNECTTOSD;
                audioTxTaskMessage.ret = audio.connecttoSD(audioRxTaskMessage.txt);
                xQueueSend(audioGetQueue, &audioTxTaskMessage, portMAX_DELAY);
            }
            else if(audioRxTaskMessage.cmd == GET_VOLUME){
                audioTxTaskMessage.cmd = GET_VOLUME;
                audioTxTaskMessage.ret = audio.getVolume();
                xQueueSend(audioGetQueue, &audioTxTaskMessage, portMAX_DELAY);
            }
            else{
                LV_LOG_ERROR("error");
            }
        }
        //AVOID CONFLICTS WITH LVGL  
        xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);
          audio.loop();
        xSemaphoreGiveRecursive(lvgl_mux);

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

audioMessage transmitReceive(audioMessage msg){
    xQueueSend(audioSetQueue, &msg, portMAX_DELAY);
    if(xQueueReceive(audioGetQueue, &audioRxMessage, portMAX_DELAY) == pdPASS){
        if(msg.cmd != audioRxMessage.cmd){
            LV_LOG_WARN("AUDIO_TASK: wrong reply from message queue");
        }
    }
    return audioRxMessage;
}

void audioSetVolume(uint8_t vol){
    audioTxMessage.cmd = SET_VOLUME;
    audioTxMessage.value = vol;
    audioMessage RX = transmitReceive(audioTxMessage);
}

uint8_t audioGetVolume(){
    audioTxMessage.cmd = GET_VOLUME;
    audioMessage RX = transmitReceive(audioTxMessage);
    return RX.ret;
}

bool audioConnecttohost(const char* host){
    audioTxMessage.cmd = CONNECTTOHOST;
    audioTxMessage.txt = host;
    audioMessage RX = transmitReceive(audioTxMessage);
    return RX.ret;
}

bool audioConnecttoSD(const char* filename){
    audioTxMessage.cmd = CONNECTTOSD;
    audioTxMessage.txt = filename;
    audioMessage RX = transmitReceive(audioTxMessage);
    return RX.ret;
}

//****************************************************************************************
//                                   S E T U P                                           *
//****************************************************************************************


void setup() {
    Serial.begin(115200);

    Serial.println("********* HERE WE GO ! **************");
    Serial.flush();

    //GET THE TOTAL NUMBER OF STATIONS
    _num_stations = sizeof (stations) / sizeof (stations[0]);

    //CREATE MUTEX 
    lvgl_mux = xSemaphoreCreateRecursiveMutex();

    init_display();   //LVGL
    lv_tick_set_cb(xTaskGetTickCount); //LVGL TICK



    vTaskDelay(pdMS_TO_TICKS(LVGL_TASK_DELAY_MS));

    //CREATE (SIMPLE) GUI ON ACTUAL SCREEN
    myGUI.init(lv_scr_act());
    //LET LVGL DO ITS WORK
    xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);
        lv_timer_handler();
    xSemaphoreGiveRecursive(lvgl_mux);

    WiFi.begin(ssid.c_str(), password.c_str());
    while (WiFi.status() != WL_CONNECTED) delay(1500);

    audioInit();

    // CALLED VIA AUDIOMESSAGE !
    audioConnecttohost(stations[_station].c_str());
    audioSetVolume(_volume);  // SET INITIAL VOLUME

    delay(10);

    Serial.println("********* RUNNING AUDIO **************");
    Serial.flush();

}

//*****************************************************************************************
//                                 L O O P ( Arduino )                                    *
//*****************************************************************************************

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
   audio.connecttohost(stations[_station].c_str());
  }
}

//PREVIOUS STATION
void GuiStationPrev() {
  Serial.println("GUI Station Prev.");
  if(_station > 0) {
    _station--;
   audio.connecttohost(stations[_station].c_str());
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
