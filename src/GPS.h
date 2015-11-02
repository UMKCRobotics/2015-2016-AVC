#ifndef GPS_H
#define GPS_H

#include <math.h>
#include <string>
#include <thread>
#include <stdlib.h>

#include "serial/serialib.h"
#include "nmea/minmea.h"
#include "AngleMath.h"
#include "logger.h"
#include "GPSParser.h"
#include "Conf.hpp"

using namespace std;
class GPS{
 private:
  void readAllInQueue();
  serialib serial;
  string readNMEAString();
  thread gps_serial_thread;
  void openSerial();
  bool threadContinue;
  string PORT;
  unsigned int BAUD;
 public:
  GPSInfo info;
  //Given our current heading, calculate in radians what we'd like our heading to be 
  //assuming the angle we are facing currently is 0
  //Basically, returns the radian value we need to adjust to
  static double calculateDesiredHeading(double currentHeading, GPSNode current, GPSNode desired);
  static double calculateAngleToNode(GPSNode current, GPSNode desired);
  void blockUntilFixed(){
    while(info.lastFix == 0 || info.node.latitude != info.node.latitude || info.node.longitude != info.node.longitude){ CLOG(INFO,"gps") << "Waiting on fix...";}
  }
  
  bool isOverlapping(GPSNode node){ 
    //todo implement
    return true;
  }
  
  void logCurrentInfo();

  //Calculate from where we are to the desired node
  double calculateHeadingToNode(GPSNode node);

  //TODO: should spawn a new thread to do all this
  GPS(Conf c);
  ~GPS();
  void debug(){
      readAllInQueue();
   };
   
};


#endif
