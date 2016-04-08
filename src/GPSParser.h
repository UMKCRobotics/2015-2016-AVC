#ifndef GPSPARSER_H
#define GPSPARSER_H

#include <string>

#include "nmea/minmea.h"
#include "logger.h"

using namespace std;

struct GPSNode{
  long double latitude, longitude; 
  GPSNode(){latitude = 0; longitude=0;}
  GPSNode(long double latitude, long double longitude) :longitude(longitude), latitude(latitude){}
};
struct GPSInfo{
  GPSNode node; //lat long
  GPSNode deviation; //lat long deviation
  int fix_hours = -1; //last gps fix taken
  int fix_minutes = -1;
  int fix_seconds = -1;
  int fix_microseconds = -1;
  int fixQuality = 0; //Quality of fix
  int satsInView, satsInUse; //sat data
  long double heading = 0.0; // true north heading
  long double speed = 0.0; //landspeed 
  long double pdop = 0.0;
  bool wasFixed(){
    return fix_hours != -1; //todo check this
  }
  void log(){

    CLOG_EVERY_N(1000,INFO,"gps") << "(" << node.latitude << "," << node.longitude << ") @ " << fix_hours << ":" << fix_minutes << ":" << fix_seconds << ":" << fix_microseconds;
   // CLOG_EVERY_N(10,INFO,"gps") << "Lat Deviation: " << deviation.latitude << " Long Deviation: " << deviation.longitude;
    //CLOG_EVERY_N(10, INFO, "gps") << "Last Fix: " << lastFix << " Fix Quality: " << fixQuality << " PDOP: " << pdop << " Sats in use/total: " << satsInUse << "/" << satsInView;
    //CLOG_EVERY_N(10, INFO,"gps") << "Heading: " << heading << " Speed: " << speed;
  }
};

namespace GPSParser{
  void parseNMEAString(string nmeastring, GPSInfo& info);
}

#endif
