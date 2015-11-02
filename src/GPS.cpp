#include "GPS.h"
#include <math.h>

double GPS::calculateDesiredHeading(double currentHeading, GPSNode current, GPSNode desired){
  double angleBetweenNodes = GPS::calculateAngleToNode(current,desired);
  return AngleMath::angleBetweenTwoAngles(currentHeading,angleBetweenNodes);
}

double GPS::calculateAngleToNode(GPSNode current, GPSNode desired){
  //taken from http://wikicode.wikidot.com/get-angle-of-line-between-two-points
  double xDiff = desired.longitude - current.longitude;
  double yDiff = desired.latitude - current.latitude;
  return atan2(yDiff,xDiff);
}

string GPS::readNMEAString(){
  char current_char;
  char code;
  string output;
  while(true){
    code = serial.ReadChar(&current_char);
    if (code == -1){
      CLOG(ERROR,"gps") << "error while setting timeout";
      return "";
    }
    if(code == -2){
      CLOG(ERROR,"gps") << "READ ERROR.";
      return "";
    }
    if(code == 0){
      CLOG(ERROR,"gps") << "Timeout reached while attempting to read char";
      return "";
    }
    if(current_char == 13){ //We found a cr
      serial.ReadChar(&current_char); //Read an lf 
      return output;
    }
    else if(current_char == ' '){}
    else{
      output += current_char;
    }
  }
}

void GPS::readAllInQueue(){
  while(serial.Peek() > 0){
   string output =  readNMEAString();
   CLOG(INFO,"gps") << "found string: " << output;
   GPSParser::parseNMEAString(output,info);
   logCurrentInfo();
  }
}
void GPS::logCurrentInfo(){
  //TODO: make sure this actually works
  info.log();
}
GPS::GPS(){
  threadContinue = true;
  gps_serial_thread = thread([this]{
      openSerial();
      while(threadContinue){
        readAllInQueue();
      }
    });
}
void GPS::openSerial(){
  char status = serial.Open(PORT,BAUD);
  switch (status){
  case 1:
    CLOG(INFO,"gps") << "Serial opened successfully";
    break;
  case -1:
    CLOG(ERROR,"gps") << "FATAL: Serial couldn't find device: " << PORT;
    exit(EXIT_FAILURE);
    break;
  case -2:
    CLOG(ERROR,"gps") << "FATAL: Serial couldn't open device: " << PORT;
    exit(EXIT_FAILURE);
    break;
  case -3:
    CLOG(ERROR,"gps") << "FATAL: Serial error while getting port params:" << PORT;
    exit(EXIT_FAILURE);
    break;
  case -4:
    CLOG(ERROR,"gps") << "FATAL: Serial speed not recognized: " << BAUD;
    exit(EXIT_FAILURE);
    break;
  case -5:
    CLOG(ERROR,"gps") << "FATAL: Serial error while writing port parameters: " << PORT;
    exit(EXIT_FAILURE);
    break;
  case -6:
    CLOG(ERROR,"gps") << "FATAL: Error while writing timeout parameters: " << PORT;
    exit(EXIT_FAILURE);
    break;
  default:
    CLOG(INFO,"gps") << "FATAL: Unkown error opening device" << status;
    exit(EXIT_FAILURE);
  }

}
GPS::~GPS(){
  threadContinue = false;
  gps_serial_thread.join();
  serial.Close();
}

double GPS::calculateHeadingToNode(GPSNode node){
  return calculateAngleToNode(info.node,node);
}
