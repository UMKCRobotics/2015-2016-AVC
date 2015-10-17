#ifndef ANGLE_MATH_H
#define ANGLE_MATH_H
#include <math.h>

namespace AngleMath{
  
  double angleBetweenTwoAngles(double source, double target);

  static double radiansToDegrees(double radians) {return radians * (180/M_PI);}
};

#endif
