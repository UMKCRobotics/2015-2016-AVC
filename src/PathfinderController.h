
#ifndef PATHFINDER_CONTROLLER_H
#define PATHFINDER_CONTROLLER_H

#include "Pathfinder.h"
#include "Vision.h"
#include "Lidar.h"
#include "Conf.hpp"
#include "logger.h"


class PathfinderController : public Pathfinder{
 private:
  Vision vision;
  Lidar lidar;
 public:
  PathfinderController(Conf c);
  Angle bestAvailableHeading(Angle desiredHeading);
};
#endif
