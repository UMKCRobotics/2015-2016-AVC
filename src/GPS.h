#ifndef GPS_H
#define GPS_H

#include <math.h>
#include <string>
#include <thread>

#include "serial/serialib.h"
#include "nmea/minmea.h"
#include "AngleMath.h"
#include "logger.h"

using namespace std;

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

class GPS{
 private:
  void readAllInQueue();
  serialib serial;
  string readNMEAString();
  thread gps_serial_thread;
  void openSerial();
 public:
  GPSInfo info;
  const char* PORT = "/dev/ttyUSB0";
  const unsigned int BAUD = 38400;
  //Given our current heading, calculate in radians what we'd like our heading to be 
  //assuming the angle we are facing currently is 0
  //Basically, returns the radian value we need to adjust to
  static double calculateDesiredHeading(double currentHeading, GPSNode current, GPSNode desired);
  static double calculateAngleToNode(GPSNode current, GPSNode desired);
  
  void parseNMEAString(string nmeastring);
  
  void logCurrentInfo();

  //Calculate from where we are to the desired node
  double calculateToNode(GPSNode node);

  //TODO: should spawn a new thread to do all this
  GPS();
  ~GPS();
  void debug(){
      readAllInQueue();
   };
   
};


#endif
