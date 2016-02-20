#include "serial/serialib.h"
#include "Conf.hpp"

#ifndef START_WAITER_H
#define START_WAITER_H 

class StartWaiter {
 private:
  serialib serial;
 public:
  StartWaiter(Conf configuration){
    string port = configuration.data["start_waiter"]["port"].get<string>();
    int baud = configuration.data["start_waiter"]["baud"];
    char status = serial.Open(port.c_str(),baud);
    serial.FlushReceiver();
  }
  void blockUntilGoSignal(){
    char* outputByte;
    char status = -1;
    while(status == -1){ 
      status = serial.ReadChar(outputByte);
    }
    serial.Close();
  }
};

#endif
