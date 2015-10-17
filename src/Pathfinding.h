#ifndef PATHFINDING_H
#define PATHFINDING_H
#include "RobotState.h"
#include "serial/serialib.h"
#include "AngleMath.h"

#include <string>
#include <unordered_map>
using namespace std;

#define MAX_SERIAL_LENGTH 10000000000

namespace Pathfinding {
  serialib serial;
  unordered_map<double,double> readings; //key is direction, value is distance

  const char* DEVICE = "/dev/ACM0";
  const unsigned int BAUD = 9600;

  void init(){
    Pathfinding::serial.Open(DEVICE,BAUD);
  }
  void readAllInQueue();
  void parseReadingAndInsertIntoReadings(string);
  
  //Returns the best available heading based off of the algorithm 
  //Algorithm
  //for each ray from the LIDAR, create a circle with radius a determined safe distance around anything less than a cutoff representing a 'safe area' near that object
  //Determine longest ray that is closest to desired heading using rayHeuristic
  //Return the heading of that ray as the new heading
  double bestAvailableHeading(double desiredHeading);

  //Determine how favorable a ray is
  //rayDistance may be inf if the ray is past cutoff
  //rayHeuristic may return inf if ray is ultimately favorable (meaning we're heading towards it already and it's got infinite distance)
  //Note: http://en.cppreference.com/w/cpp/types/numeric_limits/infinity
  double rayHeuristic(double desiredHeading, double rayHeading, double rayDistance);
};
#endif
