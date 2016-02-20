#include "serial/serialib.h"
#include "Conf.hpp"

#ifndef START_WAITER_H
#define START_WAITER_H 

class StartWaiter {
 private:
  serialib* serial;
 public:
  StartWaiter(serialib* serial_){
    serial = serial_;
    serial->FlushReceiver();
  }
  void blockUntilGoSignal(){
    char* outputByte;
    char status = -1;
    while(status == -1){ 
      status = serial->ReadChar(outputByte);
    }
    serial->Close();
  }
};

#endif
