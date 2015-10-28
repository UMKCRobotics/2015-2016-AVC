#include "GPS.h"
#include <math.h>

double GPS::calculateDesiredHeading(double currentHeading, GPSNode current, GPSNode desired){
  double angleBetweenNodes = GPS::calculateAngleToNode(current,desired);
  return AngleMath::angleBetweenTwoAngles(currentHeading,angleBetweenNodes);
}

double GPS::calculateAngleToNode(GPSNode current, GPSNode desired){
  //taken from http://wikicode.wikidot.com/get-angle-of-line-between-two-points
  double xDiff = desired.longitude - current.longitude;
  double yDiff = desired.lattitude - current.lattitude;
  return atan2(yDiff,xDiff);
}

string GPS::readNMEAString(){
  char current_char;
  char code;
  string output;
  while(true){
    code = serial.ReadChar(&current_char);
    if (code == -1){
      LOG(ERROR) << "GPS error while setting timeout";
      return "";
    }
    if(code == -2){
      LOG(ERROR) << "GPS READ ERROR.";
      return "";
    }
    if(code == 0){
      LOG(ERROR) << "GPS Timeout reached";
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
   LOG(INFO) << "GPS found data: " << output;
   GPS::parseNMEAString(output,info);
  }
}
void GPS::logCurrentInfo(){
  LOG(INFO) << "GPS lat long info. LAT:" << info.node.lattitude << " LONG: " << info.node.longitude;
   LOG(INFO) << "GPS Sats in view:" << info.satsInView << " Sats in use: " << info.satsInUse;
   LOG(INFO) << "GPS Direction: " << info.heading;
   LOG(INFO) << "GPS Speed: " << info.speed;
   LOG(INFO) << "PDOP(Precision): " << info.PDOP;
}
GPS::GPS(){
  char status = serial.Open(PORT,BAUD);
  switch (status){
  case 1:
    LOG(INFO) << "GPS Serial opened successfully";
    break;
  case -1:
    LOG(ERROR) << "GPS Serial couldn't find device: " << PORT;
    break;
  case -2:
    LOG(ERROR) << "GPS Serial couldn't open device: " << PORT;
    break;
  case -3:
    LOG(ERROR) << "GPS Serial error while getting port params:" << PORT;
    break;
  case -4:
    LOG(ERROR) << "GPS Serial speed not recognized: " << BAUD;
    break;
  case -5:
    LOG(ERROR) << "GPS Serial error while writing port parameters: " << PORT;
    break;
  case -6:
    LOG(ERROR) << "GPS Error while writing timeout parameters: " << PORT;
    break;
  default:
    LOG(INFO) << "GPS Unkown error opening device" << status;
  }
}

GPS::~GPS(){
  serial.Close();
}

void GPS::parseNMEAString(string nmeastring, GPSInfo& info){
  
}
