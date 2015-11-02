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
  info.log();
}
GPS::GPS(Conf c){
  PORT = c.data["gps"]["port"].get<string>();
  BAUD = c.data["gps"]["baud"];
  threadContinue = true;
  gps_serial_thread = thread([this]{
      openSerial();
      while(threadContinue){
        readAllInQueue();
      }
    });
}
void GPS::openSerial(){
  char status = serial.Open(PORT.c_str(),BAUD);
  switch (status){
  case 1:
    CLOG(INFO,"gps") << "Serial opened successfully";
    break;
  case -1:
    CLOG(FATAL,"gps") << "Serial couldn't find device: " << PORT;
    break;
  case -2:
    CLOG(FATAL,"gps") << "Serial couldn't open device: " << PORT;
    break;
  case -3:
    CLOG(FATAL,"gps") << "Serial error while getting port params:" << PORT;
    break;
  case -4:
    CLOG(FATAL,"gps") << "Serial speed not recognized: " << BAUD;
    break;
  case -5:
    CLOG(FATAL,"gps") << "Serial error while writing port parameters: " << PORT;
    break;
  case -6:
    CLOG(ERROR,"gps") << "FATAL: Error while writing timeout parameters: " << PORT;
    break;
  default:
    CLOG(FATAL,"gps") << "Unkown error opening device" << status; exit(EXIT_FAILURE);}
}

GPS::~GPS(){
  threadContinue = false;
  gps_serial_thread.join();
  serial.Close();
}

double GPS::calculateHeadingToNode(GPSNode node){
  return calculateAngleToNode(info.node,node);
}
void GPS::blockUntilFixed(){
    while(info.lastFix == 0 || info.node.latitude != info.node.latitude || info.node.longitude != info.node.longitude){ 
      CLOG_EVERY_N(10,INFO,"gps") << "Waiting on fix...";
   }
}
