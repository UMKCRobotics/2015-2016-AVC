#ifndef PATHFINDING_H
#define PATHFINDING_H
#include "RobotState.h"
namespace Pathfinding {
  //Returns the best available heading based off of the algorithm 
  //Algorithm
  //for each ray from the LIDAR, create a circle with radius a determined safe distance around anything less than a cutoff representing a 'safe area' near that object
  //Determine longest ray that is closest to desired heading using rayHeuristic
  //Return the heading of that ray as the new heading
  double bestAvailableHeading(RobotState state,double desiredHeading);

  //Determine how favorable a ray is
  //rayDistance may be inf if the ray is past cutoff
  //rayHeuristic may return inf if ray is ultimately favorable (meaning we're heading towards it already and it's got infinite distance)
  //Note: http://en.cppreference.com/w/cpp/types/numeric_limits/infinity
  double rayHeuristic(double desiredHeading, double rayHeading, double rayDistance);
}
#endif
