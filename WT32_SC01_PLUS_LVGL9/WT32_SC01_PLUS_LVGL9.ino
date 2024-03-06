
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


//TTF font
#include "ErikaType.h"


//BUTTON CALLBACK
static void btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = (lv_obj_t*)lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        static uint8_t cnt = 0;
        cnt++;

        /*Get the first child of the button which is the label and change its text*/
        lv_obj_t * label = lv_obj_get_child(btn, 0);
        lv_label_set_text_fmt(label, "Button: %d", cnt);
    }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  init_display();

    /*Create style with the new font*/
  static lv_style_t style;
  lv_style_init(&style);
  lv_font_t * font = lv_tiny_ttf_create_data(erikaType_font, erikaType_font_size, 40); //TTF Font from array

  lv_style_set_text_font(&style, font);
  lv_style_set_text_color(&style,  lv_color_hex(0xFFFFFF));
  lv_style_set_text_align(&style, LV_TEXT_ALIGN_CENTER);

  lv_obj_t * btn = lv_button_create(lv_screen_active());      /*Add a button the current screen*/
  lv_obj_set_pos(btn, 10, 10);                                /*Set its position*/
  lv_obj_set_size(btn,300, 100);                              /*Set its size*/
  lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL); /*Assign a callback to the button*/

  lv_obj_t * label = lv_label_create(btn);                    /*Add a label to the button*/
  lv_label_set_text(label, "Button");                         /*Set the labels text*/
  lv_obj_add_style(label, &style, 0);

}

void loop() {

    lv_task_handler(); 
    lv_tick_inc(5);
    delay(5);
}
