#include "GPS.h"
#include <math.h>

double GPS::calculateDesiredHeading(double currentHeading, GPSNode current, GPSNode desired){
  
  double desiredAbsoluteHeading = calculateAbsoluteHeading(currentHeading,current,desired);
  double desiredRelativeHeading = desiredAbsoluteHeading - currentHeading;
  //if(abs(desiredRelativeHeading) > 180)
  //  desiredRelativeHeading += 360;
  //if(desiredRelativeHeading >= 360)
  //  desiredRelativeHeading -= 360;
  return desiredRelativeHeading;
}

double GPS::calculateAbsoluteHeading(double currentHeading, GPSNode current, GPSNode desired){
  double angleBetweenNodes = calculateAngleToNode(current,desired);
  //double tempAngle = -AngleMath::angleBetweenTwoAngles(currentHeading,angleBetweenNodes)+90;
  //tempAngle = round(tempAngle);
  //if (tempAngle >= 360)
  //  tempAngle -= 360;
  //else if (tempAngle < 0)
  //  tempAngle += 360;
  //return angleBetweenNodes;
  angleBetweenNodes = round(angleBetweenNodes);
  double tempAngle;
  if (angleBetweenNodes <= 90 && angleBetweenNodes > 0)
  {
    tempAngle = 270 - angleBetweenNodes;
  }
  else if (angleBetweenNodes <= 0 && angleBetweenNodes > -90)
  {
    tempAngle = 270 - angleBetweenNodes;
  }
  else if (angleBetweenNodes <= -90 && angleBetweenNodes > -180)
  {
    tempAngle = (-angleBetweenNodes) - 90;
  }
  else if (angleBetweenNodes <= 180 && angleBetweenNodes > 90)
  {
    tempAngle = (180 - angleBetweenNodes) + 90;
  }
  if (tempAngle == -180)
    tempAngle = -tempAngle;
  return tempAngle;
}

double GPS::calculateAngleToNode(GPSNode current, GPSNode desired){
  //taken from http://wikicode.wikidot.com/get-angle-of-line-between-two-points
  double xDiff = desired.longitude - current.longitude;
  double yDiff = desired.latitude - current.latitude;
  return atan2(yDiff,xDiff) * 180 / 3.14159265;
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
   //CLOG(INFO,"gps") << "found string: " << output;
   GPSParser::parseNMEAString(output,info);
  }
  logCurrentInfo();
  usleep(100);
}
void GPS::logCurrentInfo(){
  info.log();
}
GPS::GPS(Conf c){
  PORT = c.data["gps"]["port"].get<string>();
  BAUD = c.data["gps"]["baud"];
  min_overlap_distance = c.data["gps"]["min_overlap_dist"].get<long double>();
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

double GPS::calculateHeadingToNode(double heading, GPSNode node){
  return calculateDesiredHeading(heading, info.node,node);
}
void GPS::blockUntilFixed(){
    while(info.lastFix == 0 || info.node.latitude != info.node.latitude || info.node.longitude != info.node.longitude){ 
      CLOG_EVERY_N(10,INFO,"gps") << "Waiting on fix...";
      usleep(1000);
   }
}

bool GPS::isOverlapping(GPSNode node){
  long double euclideanDist = sqrt(pow(info.node.latitude - node.latitude,2) + pow(info.node.longitude - node.longitude,2));
  return euclideanDist < min_overlap_distance;
}
