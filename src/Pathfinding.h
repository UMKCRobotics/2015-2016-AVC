

#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "RobotState.h"
#include "serial/serialib.h"
#include "AngleMath.h"
#include "logger.h"

#include <string>
#include <unordered_map>
#include <cmath>
#include <thread>

using namespace std;

#define MAX_SERIAL_LENGTH 10000000000

class Pathfinding {
 private:
  serialib serial;
  unordered_map<double,double> readings; //key is direction, value is distance
  void parseReadingAndInsertIntoReadings(string);
  thread pathfinding_serial_thread;
  void readAllInQueue();
  void openSerial();
  unordered_map<double,double> performObstactleGrowth();
 public:
  const char* PORT = "/dev/ACM0";
  const unsigned int BAUD = 9600;

  const int SAFE_LENGTH = 10; //meters wide the car is

  Pathfinding();
  
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
  static double rayHeuristic(double desiredHeading, double rayHeading, double rayDistance);
};
#endif
