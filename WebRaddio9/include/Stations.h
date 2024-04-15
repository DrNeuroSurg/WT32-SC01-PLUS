#ifndef _STATIONS_H_
#define _STATIONS_H_

#include <Arduino.h>
#include <Array.h>

#define MAX_STATIONS 19

typedef struct {
  String ShortName;
  String URL;     
  uint32_t startX;
  uint32_t midX;
  uint32_t endX; 
} RetroStation;


//TYPE
typedef Array<RetroStation, MAX_STATIONS> Stations;

//VAR
Stations retroStations;


void initStations(Stations & array) {
 
  array.clear();

  RetroStation station;

  station.ShortName = "baelaric";
  station.URL = "https://radio.balearic-fm.com:8000/radio.mp3";
  array.push_back(station);

  station.ShortName = "lounge.ch";
  station.URL = "http://fr1.streamhosting.ch/lounge128.mp3";
  array.push_back(station);

  station.ShortName = "kiss.fm";
  station.URL = "http://topradio-stream31.radiohost.de/kissfm_mp3-128";
  array.push_back(station);

  station.ShortName = "COSTA.D.MAR";
  station.URL = "http://radio4.cdm-radio.com:8020/stream-mp3-Chill_autodj";
  array.push_back(station);

  station.ShortName = "sun radio";
  station.URL = "http://radio2.vip-radios.fm:8068/stream-128kmp3-CafeSoulside_autodj";
  array.push_back(station);

}

#endif