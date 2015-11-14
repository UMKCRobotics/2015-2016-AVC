#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include "serial/serialib.h"
#include "logger.h"
#include "Conf.hpp"
#include <string>
#include <iostream>

class MotorController{
  private:
    void openSerial();
    serialib serial;
    Conf configuration;
  public:
    MotorController(Conf c);
    void commandStop();
    void commandStraight();
    void commandTurn(int turn);
    void commandForward(int throttle);
    void commandBackward(int throttle);
};

#endif
