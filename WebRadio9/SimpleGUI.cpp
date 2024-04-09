#include <Arduino.h>
#include "SimpleGUI.h"

#include <lvgl.h>

//EVENTHANDLER FOR BUTTON_MATRIX
static void _event_handler_buttonmatrix(lv_event_t * e) {
lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = (lv_obj_t *) lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        uint32_t id = lv_buttonmatrix_get_selected_button(obj);
        switch (id) {

          case 0:   //LEFTMOST
            Serial.println("VOL --");
            if(GuiVolumeDown) { GuiVolumeDown();} //IF CALLBACK -> CALL IT
            break;

          case 1:
            Serial.println("PREV");
            if(GuiStationPrev) { GuiStationPrev();} //IF CALLBACK -> CALL IT
            break;

          case 2:
            Serial.println("NEXT");
            if(GuiStationNext) { GuiStationNext();} //IF CALLBACK -> CALL IT
            break;

          case 3:   //RIGHTMOST
            Serial.println("VOL ++");
            if(GuiVolumeUp) { GuiVolumeUp();} //IF CALLBACK -> CALL IT
            break;

          default:
            break;

        }

    }
}


// FOR MORE INFORMATION ABOUT CRAETE AND STYLE THE GUI (LVGL)
// VISIT https://docs.lvgl.io/master/

void SimpleGUI::init(lv_obj_t * parent ) {

  //CREATE LABEL FOR STATION_NAME
  _stationLabel = lv_label_create(parent);
  lv_obj_set_size(_stationLabel, lv_pct(90), LV_SIZE_CONTENT); //90% WIDTH OF PARENT
  lv_obj_align(_stationLabel, LV_ALIGN_CENTER, 0, -50);   //CENTER ON PARENT 
  lv_obj_set_style_text_font(_stationLabel, &Berlin25_4, 0);  //CUSTOM FONT
  lv_label_set_text(_stationLabel, "STATION") ;
  lv_obj_set_style_text_align(_stationLabel, LV_TEXT_ALIGN_CENTER, 0);
  lv_label_set_long_mode(_stationLabel, LV_LABEL_LONG_SCROLL_CIRCULAR);  //SCROLL IF TOO LONG
  lv_obj_clear_flag(_stationLabel, LV_OBJ_FLAG_SCROLLABLE); //DON'T USE SCROLLBARS
  lv_label_set_text(_stationLabel, "CONNECTING TO WiFi") ;

  //CREATE LABEL FOR TITLE
  _titleLabel = lv_label_create(parent);
  lv_obj_set_size(_titleLabel, lv_pct(90), LV_SIZE_CONTENT); //90% WIDTH OF PARENT
  lv_obj_align(_titleLabel, LV_ALIGN_CENTER, 0, 0); // CENTER
  lv_obj_set_style_text_font(_titleLabel, &Berlin25_4, 0);  //CUSTOM FONT
  lv_label_set_text(_titleLabel, "TITLE") ;
  lv_label_set_long_mode(_titleLabel, LV_LABEL_LONG_SCROLL_CIRCULAR);  //SCROLL IF TOO LONG
   lv_obj_set_style_text_align(_titleLabel, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_clear_flag(_titleLabel, LV_OBJ_FLAG_SCROLLABLE); //DON'T USE SCROLLBARS
  lv_label_set_text(_titleLabel, "PLEASE WAIT") ;

  // BUTTON MATRIX
  static const char * btnm_map[] = {LV_SYMBOL_VOLUME_MID, LV_SYMBOL_PREV, LV_SYMBOL_NEXT, LV_SYMBOL_VOLUME_MAX ,""}; //4 BUTTONS
  _buttonMatrix = lv_buttonmatrix_create(parent);
  lv_buttonmatrix_set_map(_buttonMatrix, btnm_map);
  lv_obj_set_size(_buttonMatrix, lv_pct(100), lv_pct(30)); //100% WIDTH OF PARENT, 30% HEIGHT OF PARENT
  lv_obj_align(_buttonMatrix, LV_ALIGN_BOTTOM_MID, 0, 0); // BOTTOM ON PARENT
   lv_obj_clear_flag(_buttonMatrix, LV_OBJ_FLAG_SCROLLABLE); //DON'T USE SCROLLBARS
  lv_obj_add_event_cb(_buttonMatrix, _event_handler_buttonmatrix, LV_EVENT_ALL, NULL); // ADD EVENT_HANDLER
}

// SET STATION_LABEL
void SimpleGUI::setStationLabel(const char * station) { 
    if(_stationLabel) {
      lv_label_set_text(_stationLabel, station) ;
      }
  }

// SET TITLE_LABEL
void SimpleGUI::setTitleLabel(const char * title ) {  
  if(_titleLabel) {
      lv_label_set_text(_titleLabel, title) ;
      }
}