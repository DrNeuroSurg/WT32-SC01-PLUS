#ifndef _SIMPLE_GUI_H
#define _SIMPLE_GUI_H

#include <lvgl.h>

// CUSTOM FONT
extern const lv_font_t Berlin25_4;  // FROM Berlin25_4.c

using namespace std;

extern __attribute__((weak)) void GuiVolumeUp();
extern __attribute__((weak)) void GuiVolumeDown();
extern __attribute__((weak)) void GuiStationNext();
extern __attribute__((weak)) void GuiStationPrev();

class SimpleGUI {
  
  public:
    void init(lv_obj_t * parent);

    void setStationLabel(const char * station);
    void setTitleLabel(const char * title );

  private:

    lv_obj_t * _titleLabel;
    lv_obj_t * _stationLabel;
    lv_obj_t * _buttonMatrix;

};

#endif // _SIMPLE_GUI_H