#ifndef LOGGER_DISPATCH_H
#define LOGGER_DISPATCH_H
#include "serial/serialib.h"
#include "logger.h"
#include "Conf.hpp"
#include <string>
#include <iostream>
using namespace std;

namespace LoggerDispatchGlobals{ //hacky shit to make this work
  extern string port;
  extern int baud;
  extern serialib serial;
  extern bool hasBeenInitialized;
  //YOU HAVE TO CALL THIS BEFORE YOU REGISTER IT
  void setConfiguration(Conf c);
};
class LoggerDispatch : public el::LogDispatchCallback{
 public:
  LoggerDispatch();

  void handle(const el::LogDispatchData* data);

  char levelToChar(el::Level level);

};
#endif
