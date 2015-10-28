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
    //LOG(INFO) << "Current char: " << current_char;
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
      return output += "\r\n";
    }
    else if(current_char == ' '){}
    else{
      output += current_char;
    }
  }
}

void GPS::readAllInQueue(){
  string output =  readNMEAString();
   LOG(INFO) << "GPS found data: " << output;
   int parsed = nmea_parse(&parser,output.c_str(),(int)strlen(output.c_str()),&info);
   LOG(INFO) << "GPS parsed " << parsed << " packets.";
   LOG(INFO) << "GPS lat long info. LAT:" << info.lat << " LONG: " << info.lon;
   LOG(INFO) << "GPS Sats in view:" << info.satinfo.inview << " Sats in use: " << info.satinfo.inuse;
   LOG(INFO) << "GPS Direction: " << info.direction;
   LOG(INFO) << "GPS Speed: " << info.speed;
   LOG(INFO) << "PDOP(Precision): " << info.PDOP;
}

GPS::GPS(){
  nmea_zero_INFO(&info);
  nmea_parser_init(&parser);
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
  serial.FlushReceiver();
}

GPS::~GPS(){
  serial.Close();
  nmea_parser_destroy(&parser);
}
 
