/* ***********************************************

by DrNeurosurg 2024

Arduino IDE Version: 2.3.2

Libraries used (Arduino):

LovyanGFX by lovyan03
Version 1.1.12

lvgl by kisvegabor
Version 9.0.0

Board: ESP32S3 Dev Module

Board Settings:

USB CDC On Boot: "Disabled"
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

#define  WT_USE_LVGL
#include "WT32_SC01_PLUS.h"

lv_obj_t *label1;         // LGVL Label

void setup() {

  Serial.begin(115200);

  delay(100);


  Serial.println("Hello again!");
  
  init_display();


  //SCREEN BLACK
  lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(lv_screen_active(), 255, LV_PART_MAIN| LV_STATE_DEFAULT);

  //LABEL 
  label1 = lv_label_create(lv_scr_act());
  lv_obj_set_width( label1, 480);     
  lv_obj_set_height( label1, 30);
  lv_obj_set_x( label1, 0 );
  lv_obj_set_y( label1, -244 );
  lv_obj_set_align( label1, LV_ALIGN_BOTTOM_LEFT );
  lv_obj_set_style_text_color(label1, lv_color_hex(0xFFFEFE), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_text_opa(label1, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);
   
  lv_label_set_text(label1, "HELLO LVGL_9 !");
  

}

void loop() {
    /* let the GUI do its work */
    lv_task_handler(); 
    lv_tick_inc(5);
    delay(5);

}
