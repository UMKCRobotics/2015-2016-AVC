#include "Compass.h"

void Compass::readAllInQueue(){
  char out_char = ' ';
  string s_key, s_value;
  serial.WriteString("1");
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
  double radHeading;
  int tempHeading;
  try{
    x = stod(s_key);
    y = stod(s_value);
    //tempHeading = stod(s_value);
    radHeading = atan2(y,x) + declination_rad;
    if (radHeading < 0) //reverse signs if necessary
      radHeading += 2*pi;
    if (radHeading > 2*pi);
      radHeading -= 2*pi;
    curHeading = AngleMath::radiansToDegrees(radHeading);

    tempHeading += declination_deg;
    tempHeading += 180;
    if (tempHeading < 0)
       tempHeading += 360;
    else if (tempHeading > 360)
       tempHeading -= 360;
    curHeading = tempHeading;
    CLOG_EVERY_N(10,INFO,"compass") << "heading: " << curHeading;
  } catch(const invalid_argument& e){
    CLOG(ERROR,"compass") << "Couldn't parse a double";
  }
  usleep(250000);
}

Compass::Compass(Conf c){
  PORT = c.data["compass"]["port"].get<string>();
  BAUD = c.data["compass"]["baud"];
  declination_deg = c.data["compass"]["declination_deg"].get<double>();
  declination_rad = declination_deg*pi/180;
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

