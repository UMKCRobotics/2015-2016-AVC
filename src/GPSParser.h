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
  int lastFix = 0; //last gps fix taken
  int fixQuality = 0; //Quality of fix
  int satsInView, satsInUse; //sat data
  long double heading = 0.0; // true north heading
  long double speed = 0.0; //landspeed 
  long double pdop = 0.0;
  void log(){
    CLOG_EVERY_N(1000,INFO,"gps") << "(" << node.latitude << "," << node.longitude << ") @ " << heading;
   // CLOG_EVERY_N(10,INFO,"gps") << "Lat Deviation: " << deviation.latitude << " Long Deviation: " << deviation.longitude;
    //CLOG_EVERY_N(10, INFO, "gps") << "Last Fix: " << lastFix << " Fix Quality: " << fixQuality << " PDOP: " << pdop << " Sats in use/total: " << satsInUse << "/" << satsInView;
    //CLOG_EVERY_N(10, INFO,"gps") << "Heading: " << heading << " Speed: " << speed;
  }
};

namespace GPSParser{
  void parseNMEAString(string nmeastring, GPSInfo& info);
}

#endif
