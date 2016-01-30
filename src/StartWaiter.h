#pragma once
#include "serial/serialib.h"

class StartWaiter {
 private:
  serialib serial;
 public:
  StartWaiter(Conf c){
    string port = configuration.data["start_waiter"]["port"].get<string>();
    int baud = configuration.data["start_waiter"]["baud"];
    char status = serial.Open(port.c_str(),baud);
    serial.FlushReceiver();
  }
  void blockUntilGoSignal(){
    char* outputByte;
    char status;
    status = serial.ReadChar(outputByte);
    while(status != 0 && outputByte != 1){
     status = serial.ReadChar(outputByte); 
    }
    serial.Close();
  }
};

