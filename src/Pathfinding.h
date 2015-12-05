

#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "serial/serialib.h"
#include "AngleMath.h"
#include "logger.h"
#include "Conf.hpp"

#include <string>
#include <map>
#include <cmath>
#include <thread>
#include <stdlib.h>
#include <stdexcept>
#include <sstream>

using namespace std;

typedef int Angle;
typedef int Distance;
typedef map<Angle,Distance> ReadingContainer;

class Pathfinding {
 private:
  serialib serial;
  ReadingContainer readings; //key is direction, value is distance
  void parseReadingAndInsertIntoReadings();
  thread pathfinding_serial_thread;
  void readAllInQueue();
  void openSerial();
  ReadingContainer performObstactleGrowth();
  bool threadContinue;
  Conf configuration;
  Distance computeGrowthLength(Angle sourceAngle, Distance sourceDistance, Angle otherAngle, Distance otherDistance);
  Distance rayMax;
  Distance safeLength;
 public:
  string prettyPrint();
  string prettyPrintWithHeuristicValues(Angle desiredHeading);
  string prettyPrintHeuristics(Angle desiredAngle);
  Pathfinding();
  Pathfinding(Conf c);
  ~Pathfinding();
  
  //Returns the best available heading based off of the algorithm
  //Algorithm
  //for each ray from the LIDAR, create a circle with radius a determined safe distance around anything less than a cutoff representing a 'safe area' near that object
  //Determine longest ray that is closest to desired heading using rayHeuristic
  //Return the heading of that ray as the new heading
  Angle bestAvailableHeading(Angle desiredHeading);

  //Determine how favorable a ray is
  double rayHeuristic(double desiredHeading, double rayHeading, int rayDistance);
};
#endif
