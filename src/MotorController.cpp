#include "MotorController.h"

using namespace std;

MotorController::MotorController(Conf c) {
	configuration = c;
        turn_coefficient = c.data["motorcontroller"]["gillens_equation"]["turn_coefficient"];
        gillens_limit = c.data["motorcontroller"]["gillens_equation"]["gillens_limit"];
	openSerial();
}

void MotorController::openSerial() {
	string port_str = configuration.data["motorcontroller"]["port"];
	int baud = configuration.data["motorcontroller"]["baud"];
	const char * port = port_str.c_str();
	int status = serial.Open(port,baud);
	switch (status){
	  case 1:
	    CLOG(INFO,"motorcontrol") << "Serial opened successfully";
	    break;
	  case -1:
	    CLOG(FATAL,"motorcontrol") << "Serial couldn't find device: " << port;
	    break;
	  case -2:
	    CLOG(FATAL,"motorcontrol") << "Serial couldn't open device: " << port;
	    break;
	  case -3:
	    CLOG(FATAL,"motorcontrol") << "Serial error while getting port params:" << port;
	    break;
	  case -4:
	    CLOG(FATAL,"motorcontrol") << "Serial speed not recognized: " << baud;
	    break;
	  case -5:
	    CLOG(FATAL,"motorcontrol") << "Serial error while writing port parameters: " << port;
	    break;
	  case -6:
	    CLOG(FATAL,"motorcontrol") << "Error while writing timeout parameters: " << port;
	    break;
	  default:
	    CLOG(FATAL,"motorcontrol") << "Unkown error opening device" << status;
	    break;
	}
}

void MotorController::commandStop() {
	string full_s = "s$";
	const char * s = full_s.c_str();
	int suc = serial.WriteString(s);
}

void MotorController::commandStraight() {
	string full_s = "c$";
	const char * s = full_s.c_str();
	int suc = serial.WriteString(s);
}

void MotorController::commandTurn(int turn) {
	string turn_s = to_string(turn);
	if (turn >= 0)
	{
		turn_s = "+" + turn_s;
	}
	string full_s = "t" + turn_s + "$";
	const char * s = full_s.c_str();
	int suc = serial.WriteString(s);
}

void MotorController::commandForward(int throttle) {
	string throttle_s = to_string(throttle);
	string full_s = "f" + throttle_s + "$";
	const char * s = full_s.c_str();
	int suc = serial.WriteString(s);
}

void MotorController::commandBackward(int throttle) {
	string throttle_s = to_string(throttle);
	string full_s = "b" + throttle_s + "$";
	const char * s = full_s.c_str();
	int suc = serial.WriteString(s);
}

int MotorController::computeGillensThrottle(int turn){
  if(turn == 0){
    return gillens_limit;
  }
  return gillens_limit / (turn_coefficient * turn );
}

void MotorController::runGillensEquation(int turn){
  commandTurn(turn);
  commandForward(computeGillensThrottle(turn));
}
