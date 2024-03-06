/* ***********************************************

by DrNeurosurg 2024

Arduino IDE Version: 2.3.2

Libraries used (Arduino):

LovyanGFX by lovyan03
Version 1.1.12

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

//#define TFT_PORTRAIT // UNCOMMENT FOR PORTRAIT 

#include "WT32_SC01_PLUS.h"

void drawGradient(void)
{
  tft.startWrite();
  tft.setAddrWindow(0, 0, tft.width(), tft.height());
  for (int y = 0; y < tft.height(); ++y) {
    for (int x = 0; x < tft.width(); ++x) {
      tft.writeColor(tft.color888(x>>1, (x + y) >> 2, y>>1), 1);
    }
  }
  tft.endWrite();
}

void setup() {
  Serial.begin(115200);
  delay(500);

  init_display();

  drawGradient();
  tft.fillCircle(tft.width()/2, tft.height()/2, 100, TFT_RED);
  tft.drawString("Demo by DrNeurosurg 2024", 10, 10);

}

void loop() {
  // put your main code here, to run repeatedly:

}
