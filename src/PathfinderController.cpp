#include "PathfinderController.h"

PathfinderController::PathfinderController(Conf c) : vision(c), lidar(c) {
}

Angle PathfinderController::bestAvailableHeading(Angle desiredHeading){
  Angle currentBest;
  currentBest = vision.bestAvailableHeading(desiredHeading);
  if(currentBest != 666){
    return currentBest;
  }
  else{
    return lidar.bestAvailableHeading(desiredHeading);
  }
}
