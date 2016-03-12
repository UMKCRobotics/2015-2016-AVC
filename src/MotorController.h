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
    double turn_coefficient;
    int gillens_limit;

  public:
    MotorController(Conf c);
    void commandStop();
    void commandStraight();
    void commandTurn(int turn);
    void commandForward(int throttle);
    void commandBackward(int throttle);
    /*
     * This function will turn and move the car according to an equation 
     * which is 
     * gillens_constant = turn_coefficient * turn * throttle
     * such that the car will slow down when starting a turn, and speed up out of it.
     * since we know turn, turn_coefficient and gillens_constant
     * the rearannged equation is
     * gillens_constant / (turn_coefficient * turn) = throttle
     * the motors are then set to that turn and the resulting throttle
     */
    void runGillensEquation(int turn);

    /*
     * this computes the resulting throttle, but does not actually cause any side effects
     */
    int computeGillensThrottle(int turn);
};

#endif
