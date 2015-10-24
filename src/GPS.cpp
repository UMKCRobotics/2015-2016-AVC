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


char GPS::readAChar(){
  char data;
  int output = serial.ReadString(&data,13,1000,5000);
  if (output == -3){
      LOG(ERROR) << "GPS max amount of bytes read";
      return -1;
    }
    if(output == -2){
      LOG(ERROR) << "GPS READ ERROR.";
      return -1;
    }
    if(output == -1){
      LOG(ERROR) << "GPS Erorr setting timeout";
      return -1;
    }
    return data;
}
void GPS::readAllInQueue(){
    LOG(INFO) << "GPS bytes in serial buffer: " << serial.Peek();
    while(true){
      LOG(INFO) << readAChar();
    }
    string data;
    while(readAChar()!='G');
    bool working = true;
    while (working){
      char nextChar = readAChar();
      if(nextChar == 13){ //CR
        readAChar(); //read the lf
        working = false;
      }
      else{
        data += nextChar;
      }
    }
    
   LOG(INFO) << "GPS found data: " << string(data);
   nmea_parse(&parser,data.c_str(),data.length(),&info);
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
 
