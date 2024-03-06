/* ***********************************************

by DrNeurosurg 2024

Arduino IDE Version: 2.3.2

Libraries used (Arduino):

LovyanGFX by lovyan03
Version 1.1.12

lvgl by kisvegabor
Version 9.0.0

TGX (https://github.com/vindar/tgx ... download "Code" and install in ArduinoIDE as Library)
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

//#define  WT_USE_LVGL

//#define TFT_PORTRAIT // UNCOMMENT FOR PORTRAIT 

#include "WT32_SC01_PLUS.h"

// 3D graphic library
#include <tgx.h>

// the mesh to draw
#include "naruto.h"

// let's not burden ourselves with the tgx:: prefix
using namespace tgx;

// size of the drawing framebuffer
// (limited by the amount of memory in the ESP32. Reduce, if you need more RAM !)
#define SLX 420
#define SLY 320

// the framebuffer we draw onto
uint16_t fb[SLX * SLY];

// the z-buffer in 16 bits precision
uint16_t* zbuf;

// the image that encapsulate framebuffer fb
Image<RGB565> imfb(fb, SLX, SLY);

// only load the shaders we need.
const int LOADED_SHADERS = TGX_SHADER_PERSPECTIVE | TGX_SHADER_ZBUFFER | TGX_SHADER_FLAT | TGX_SHADER_GOURAUD | TGX_SHADER_NOTEXTURE | TGX_SHADER_TEXTURE_NEAREST |TGX_SHADER_TEXTURE_WRAP_POW2;

// the renderer object that performs the 3D drawings
Renderer3D<RGB565, LOADED_SHADERS, uint16_t> renderer;


void setup() {
    Serial.begin(115200);
    delay(500);

  //HERE WE GO !
    #ifdef PRINT_DEBUG_WT
      Serial.println();
      Serial.println("************ HERE WE GO ! ********************");
      Serial.println();
    #endif

    init_display();

    // allocate the zbuffer
    zbuf = (uint16_t*)malloc(SLX * SLY * sizeof(uint16_t));
    while (zbuf == nullptr)
    {
        Serial.println("Error: cannot allocate memory for zbuf");
        delay(1000);
    }

    // setup the 3D renderer.
    renderer.setViewportSize(SLX,SLY);
    renderer.setOffset(0, 0);    
    renderer.setImage(&imfb); // set the image to draw onto (ie the screen framebuffer)
    renderer.setZbuffer(zbuf); // set the z buffer for depth testing
    renderer.setPerspective(45, ((float)SLX) / SLY, 1.0f, 100.0f);  // set the perspective projection matrix.     
    renderer.setMaterial(RGBf(0.85f, 0.55f, 0.25f), 0.2f, 0.7f, 0.8f, 64); // bronze color with a lot of specular reflexion. 
    renderer.setCulling(1);
    renderer.setTextureQuality(TGX_SHADER_TEXTURE_NEAREST);
    renderer.setTextureWrappingMode(TGX_SHADER_TEXTURE_WRAP_POW2);      

}


/** Compute the model matrix according to the current time */
tgx::fMat4 moveModel(int& loopnumber)
{
    const float end1 = 6000;
    const float end2 = 2000;
    const float end3 = 6000;
    const float end4 = 2000;

    int tot = (int)(end1 + end2 + end3 + end4);
    int m = millis();

    loopnumber = m / tot;
    float t = m % tot;

    const float dilat = 9; // scale model
    const float roty = 360 * (t / 4000); // rotate 1 turn every 4 seconds        
    float tz, ty;
    if (t < end1)
    { // far away
        tz = -25;
        ty = 0;
    }
    else
    {
        t -= end1;
        if (t < end2)
        { // zooming in
            t /= end2;
            tz = -25 + 18 * t;
            ty = -6.5f * t;
        }
        else
        {
            t -= end2;
            if (t < end3)
            { // close up
                tz = -7;
                ty = -6.5f;
            }
            else
            { // zooming out
                t -= end3;
                t /= end4;
                tz = -7 - 18 * t;
                ty = -6.5 + 6.5 * t;
            }
        }
    }

    fMat4 M;
    M.setScale({ dilat, dilat, dilat }); // scale the model
    M.multRotate(-roty, { 0,1,0 }); // rotate around y
    M.multTranslate({ 0,ty, tz }); // translate
    return M;
}

void _rgb565_swap(void * buf, uint32_t buf_size_px)
{
  
    uint32_t u32_cnt = buf_size_px / 2;
    uint16_t * buf16 = (uint16_t *)buf;
    uint32_t * buf32 = (uint32_t *)buf;

    while(u32_cnt >= 8) {
        buf32[0] = ((buf32[0] & 0xff00ff00) >> 8) | ((buf32[0] & 0x00ff00ff) << 8);
        buf32[1] = ((buf32[1] & 0xff00ff00) >> 8) | ((buf32[1] & 0x00ff00ff) << 8);
        buf32[2] = ((buf32[2] & 0xff00ff00) >> 8) | ((buf32[2] & 0x00ff00ff) << 8);
        buf32[3] = ((buf32[3] & 0xff00ff00) >> 8) | ((buf32[3] & 0x00ff00ff) << 8);
        buf32[4] = ((buf32[4] & 0xff00ff00) >> 8) | ((buf32[4] & 0x00ff00ff) << 8);
        buf32[5] = ((buf32[5] & 0xff00ff00) >> 8) | ((buf32[5] & 0x00ff00ff) << 8);
        buf32[6] = ((buf32[6] & 0xff00ff00) >> 8) | ((buf32[6] & 0x00ff00ff) << 8);
        buf32[7] = ((buf32[7] & 0xff00ff00) >> 8) | ((buf32[7] & 0x00ff00ff) << 8);
        buf32 += 8;
        u32_cnt -= 8;
    }

    while(u32_cnt) {
        *buf32 = ((*buf32 & 0xff00ff00) >> 8) | ((*buf32 & 0x00ff00ff) << 8);
        buf32++;
        u32_cnt--;
    }

    if(buf_size_px & 0x1) {
        uint32_t e = buf_size_px - 1;
        buf16[e] = ((buf16[e] & 0xff00) >> 8) | ((buf16[e] & 0x00ff) << 8);
    }

}

int loopnumber = 0;
int prev_loopnumber = -1;

void loop() {
    // compute the model position
    fMat4  M = moveModel(loopnumber);
    renderer.setModelMatrix(M);

    // draw the 3D mesh
    imfb.fillScreen(RGB565_Cyan);              // clear the framebuffer (black background)
    renderer.clearZbuffer();                    // clear the z-buffer

// choose the shader to use
    switch (loopnumber % 4)
        {
        case 0: renderer.setShaders(TGX_SHADER_GOURAUD | TGX_SHADER_TEXTURE);
                renderer.drawMesh(&naruto_1, false); 
                break;
                
        case 1: renderer.drawWireFrameMesh(&naruto_1, true);
                break;
        
        case 2: renderer.setShaders(TGX_SHADER_FLAT);
                renderer.drawMesh(&naruto_1, false); 
                break;
                
        case 3: renderer.setShaders(TGX_SHADER_GOURAUD);
                renderer.drawMesh(&naruto_1, false); 
                break;        
        }

    if (prev_loopnumber != loopnumber)
    {
      prev_loopnumber = loopnumber;

      // tft.fillRect(0, 300, 240, 20, TFT_BLACK);
      // tft.setCursor(5, 305);

      // switch (loopnumber % 4)
      //     {
      //     case 0: tft.print("Gouraud shading / texturing"); break;
      //     case 1: tft.print("Wireframe"); break;
      //     case 2: tft.print("Flat Shading"); break;
      //     case 3: tft.print("Gouraud shading"); break;
      //     }
    }


    // upload the framebuffer to the screen (async. via DMA)
    if (tft.getStartCount() == 0)
    {   // Processing if not yet started
          tft.startWrite();
    }

    _rgb565_swap(fb, SLX * SLY);
    tft.pushImageDMA((tft.width() - SLX) / 2, (tft.height() - SLY) / 2, SLX, SLY, fb);
    delay(10);

}
