#include "GPS.h"
#include <math.h>



double GPS::calculateDesiredHeading(double currentHeading, GPSNode current, GPSNode desired){
  double angleBetweenNodes = GPS::calculateAngleToNode(current,desired);
  return GPS::angleBetweenTwoAngles(currentHeading,angleBetweenNodes);
}

double GPS::calculateAngleToNode(GPSNode current, GPSNode desired){
  //taken from http://wikicode.wikidot.com/get-angle-of-line-between-two-points
  double xDiff = desired.longitude - current.longitude;
  double yDiff = desired.lattitude - current.lattitude;
  return atan2(yDiff,xDiff);
}

double customModulous(double a, double n){
  //mod without having the same sign as dividend
  return a - floor(a/n) * n;
}
double GPS::angleBetweenTwoAngles(double source, double target){
  //https://stackoverflow.com/questions/1878907/the-smallest-difference-between-2-angles
  double a  = target - source;
  return customModulous((a+180),360) - 180;
}

GPS::GPS(){
  nmea_zero_INFO(&info);
  nmea_parser_init(&parser);
}
