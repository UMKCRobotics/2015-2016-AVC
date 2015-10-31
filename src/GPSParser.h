#ifndef GPSPARSER_H
#define GPSPARSER_H

struct GPSNode{
  long latitude, longitude; 
};
struct GPSInfo{
  GPSNode node; //lat long
  GPSNode deviation; //lat long deviation
  int lastFix; //last gps fix taken
  int fixQuality; //Quality of fix
  int satsInView, satsInUse; //sat data
  double heading = 0.0; // true north heading
  double speed = 0.0; //landspeed 
  double pdop = 0.0;
  void log(){
    CLOG(INFO,"gps") << "Latitude: " << node.latitude << " Longitude:" << node.longitude;
    CLOG(INFO,"gps") << "Lat Deviation: " << deviation.latitude << " Long Deviation: " << deviation.longitude;
    CLOG(INFO, "gps") << "Last Fix: " << lastFix << " Fix Quality: " << fixQuality << " PDOP: " << pdop;
      CLOG(INFO,"gps") << "Heading: " << heading << " Speed: " << speed;
  }
};

namespace GPSParser{
  void GPSParser::parseNMEAString(string nmeastring, GPSInfo& info);
 private:
  void GPSParser::parseVTGString(string nmeastring, GPSInfo& info);
}

#endif
