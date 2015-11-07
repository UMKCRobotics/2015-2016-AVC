#include "MotorController.h"

using namespace std;

MotorController::MotorController(Conf c) {
	configuration = c;
	string port_str = configuration.data["motorcontroller"]["port"];
	int baud_int = configuration.data["motorcontroller"]["baud"];
	const char * port_fin = port_str.c_str();
	int suc = serial.Open(port_fin,baud_int);
	if (suc != 1)
	{
		cout << "Error while opening port\n";
	}
	cout << "Serial started. \n";
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