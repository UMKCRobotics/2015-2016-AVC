#ifndef GPS_H
#define GPS_H

#include <math.h>
#include <string>
#include "serial/serialib.h"
#include "AngleMath.h"
#include "logger.h"

using namespace std;

struct GPSNode{
  double lattitude, longitude; 
}
struct GPSInfo{
  GPSNode node; //lat long
  int lastFix; //last gps fix taken
  int satsInView, satsInUse; //sat data
  double heading; // true north heading
  double speed; //landspeed
  double PDOP; //precision
};

class GPS{
 private:
  void readAllInQueue();
  serialib serial;
  string readNMEAString();
  GPSInfo info;
 public:
  const char* PORT = "/dev/ttyUSB0";
  const unsigned int BAUD = 38400;
  //Given our current heading, calculate in radians what we'd like our heading to be 
  //assuming the angle we are facing currently is 0
  //Basically, returns the radian value we need to adjust to
  static double calculateDesiredHeading(double currentHeading, GPSNode current, GPSNode desired);
  static double calculateAngleToNode(GPSNode current, GPSNode desired);
  
  static void parseNMEAString(string nmeastring, GPSInfo& info);
  
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

/*
NMEA EXAMPLE
--------------
#include <nmea/nmea.h>
#include <string.h>

int main()
{
    const char *buff[] = {
        "$GPRMC,173843,A,3349.896,N,11808.521,W,000.0,360.0,230108,013.4,E*69\r\n",
        "$GPGGA,111609.14,5001.27,N,3613.06,E,3,08,0.0,10.2,M,0.0,M,0.0,0000*70\r\n",
        "$GPGSV,2,1,08,01,05,005,80,02,05,050,80,03,05,095,80,04,05,140,80*7f\r\n",
        "$GPGSV,2,2,08,05,05,185,80,06,05,230,80,07,05,275,80,08,05,320,80*71\r\n",
        "$GPGSA,A,3,01,02,03,04,05,06,07,08,00,00,00,00,0.0,0.0,0.0*3a\r\n",
        "$GPRMC,111609.14,A,5001.27,N,3613.06,E,11.2,0.0,261206,0.0,E*50\r\n",
        "$GPVTG,217.5,T,208.8,M,000.00,N,000.01,K*4C\r\n"
    };

    int it;
    nmeaINFO info;
    nmeaPARSER parser;

    nmea_zero_INFO(&info);
    nmea_parser_init(&parser);

    for(it = 0; it < 6; ++it)
        nmea_parse(&parser, buff[it], (int)strlen(buff[it]), &info);

    nmea_parser_destroy(&parser);

    return 0;
}
-----------------------------------
*/
