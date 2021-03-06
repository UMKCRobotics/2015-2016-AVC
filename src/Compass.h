#ifndef COMPASS_H
#define COMPASS_H

#include <string>
#include <thread>
#include <unistd.h>
#include "serial/serialib.h"
#include "Conf.hpp"
#include "logger.h"
#include "AngleMath.h"

class Compass{
 public:
  int curHeading;
  Compass(Conf c);
~Compass();
 private:
  double pi = 3.14159265358979323846;
  double declination_deg;
  double declination_rad;
  void readAllInQueue();
  bool threadContinue;
  double bias[2];
  double scalar[2];
  thread compass_serial_thread;
  string PORT;
  unsigned int BAUD;
  serialib serial;
  void openSerial();
};

#endif

