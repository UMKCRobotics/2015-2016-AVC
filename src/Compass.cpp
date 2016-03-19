#include "Compass.h"

void Compass::readAllInQueue(){
  char out_char = ' ';
  string s_key, s_value;
  while(true){
    char code = serial.ReadChar(&out_char); //add error reporting
    if(code < 0){
      switch(code){
      case 0:
        CLOG(ERROR,"compass")  << "timeout reached in serial read";
        break;
      case -1:
        CLOG(ERROR,"compass") << "erorr while setting the timeout";
        break;
      case -2:
        CLOG(ERROR,"compass") << "error while reading the byte";
        break;
      default:
        CLOG(ERROR,"compass") << "unkown serial error 2003, so emotional";
        break;
      }
    }
    else if(out_char != ','){
      s_key += out_char;
    }
    else{
      break;
    }
  }
  while(true){
    char code =serial.ReadChar(&out_char); //add error reporting
    if(code < 0){
      switch(code){
      case 0:
        CLOG(ERROR,"compass")  << "timeout reached in serial read";
        break;
      case -1:
        CLOG(ERROR,"compass") << "erorr while setting the timeout";
        break;
      case -2:
        CLOG(ERROR,"compass") << "error while reading the byte";
        break;
      default:
        CLOG(ERROR,"compass") << "unkown serial error 2003, pls alert yung lean :(";
        break;
      }
    }
    else if(out_char != '$'){
      s_value += out_char;
    }
    else{
      break;
    }
  }
  double x;
  double y;
  try{
    x = stod(s_key);
    y = stod(s_value);
    curHeading = AngleMath::radiansToDegrees(atan2(y,x));
  } catch(const invalid_argument& e){
    CLOG(ERROR,"compass") << "Couldn't parse a double";
  }
  usleep(100);
}

Compass::Compass(Conf c){
  PORT = c.data["compass"]["port"].get<string>();
  BAUD = c.data["compass"]["baud"];
  threadContinue = true;
  compass_serial_thread = thread([this]{
      openSerial();
      while(threadContinue){
        readAllInQueue();
      }
    });
}
void Compass::openSerial(){
  char status = serial.Open(PORT.c_str(),BAUD);
  switch (status){
  case 1:
    CLOG(INFO,"compass") << "Serial opened successfully";
    break;
  case -1:
    CLOG(FATAL,"compass") << "Serial couldn't find device: " << PORT;
    break;
  case -2:
    CLOG(FATAL,"compass") << "Serial couldn't open device: " << PORT;
    break;
  case -3:
    CLOG(FATAL,"compass") << "Serial error while getting port params:" << PORT;
    break;
  case -4:
    CLOG(FATAL,"compass") << "Serial speed not recognized: " << BAUD;
    break;
  case -5:
    CLOG(FATAL,"compass") << "Serial error while writing port parameters: " << PORT;
    break;
  case -6:
    CLOG(ERROR,"compass") << "FATAL: Error while writing timeout parameters: " << PORT;
    break;
  default:
    CLOG(FATAL,"compass") << "Unkown error opening device" << status; exit(EXIT_FAILURE);}
}

Compass::~Compass(){
  threadContinue = false;
  compass_serial_thread.join();
  serial.Close();
}

