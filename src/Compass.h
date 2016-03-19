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
  void readAllInQueue();
  bool threadContinue; 
  thread compass_serial_thread;
  string PORT;
  unsigned int BAUD;
  serialib serial;
  void openSerial();
};

#endif

