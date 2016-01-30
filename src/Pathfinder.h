#ifndef PATHFINDER_H
#define PATHFINDER_H

typedef int Angle;
typedef int Distance;

class Pathfinder{
 public:
  virtual Angle bestAvailableHeading(Angle desiredHeading) = 0;
};
#endif
