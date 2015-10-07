#ifndef GPS_H
#define GPS_H
#include <math.h>
class GPSNode{
public:
  double lattitude, longitude;
  GPSNode(double lattitude, double longitude) : lattitude(lattitude),longitude(longitude){}
};

class GPS{
 public:
  //Given our current heading, calculate in radians what we'd like our heading to be 
  //assuming the angle we are facing currently is 0
  //Basically, returns the radian value we need to adjust to
  static double calculateDesiredHeading(double currentHeading, GPSNode current, GPSNode desired);
  static double calculateAngleToNode(GPSNode current, GPSNode desired);
  static double radiansToDegrees(double radians) {return radians * (180/M_PI);}
};

#endif
