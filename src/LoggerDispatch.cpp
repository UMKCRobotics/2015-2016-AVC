#include "LoggerDispatch.h"

char LoggerDispatch::levelToChar(el::Level level){
  switch(level){
        case el::Level::Global: 
        case el::Level::Verbose: 
        case el::Level::Info: 
            return 'i';
            break;
       case el::Level::Warning: 
            return 'w';
            break;
        default:
            return 'e';
            break;
        }
}

void LoggerDispatch::handle(const el::LogDispatchData* data){
    //Precede the message with a level indicator for led
    char beginChar = levelToChar(data->logMessage()->level());
    string outString = beginChar + data->logMessage()->logger()->id()+ data->logMessage()->message() + '\n';
    serial.WriteString(outString.c_str());
}

LoggerDispatch::LoggerDispatch(){
  serial.Open(LoggerDispatchGlobals::port.c_str(),LoggerDispatchGlobals::baud);
}

namespace LoggerDispatchGlobals {
  string port ="";
  int baud = 0;
}

void LoggerDispatchGlobals::setConfiguration(Conf c){
  port = c.data["logger_dispatch"]["port"].get<string>();
  baud = c.data["logger_dispatch"]["baud"];
}

