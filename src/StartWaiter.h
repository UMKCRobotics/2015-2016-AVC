#include "serial/serialib.h"
#include "Conf.hpp"
#include <time.h>

#ifndef START_WAITER_H
#define START_WAITER_H 

class StartWaiter {
 private:
  serialib* serial;
 public:
  StartWaiter(serialib* serial_){
    serial = serial_;
  }
  void blockUntilGoSignal(){
    char* outputByte;
    char status = -1;
    while(status == -1){ 
      status = serial->ReadChar(outputByte);
      usleep(10);
    }
    usleep(100);
  }
};

#endif
