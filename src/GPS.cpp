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
   CLOG(INFO,"gps") << "found data: " << output;
   GPS::parseNMEAString(output,info);
  }
}
void GPS::logCurrentInfo(){
  //TODO: make sure this actually works
   CLOG(INFO,"gps") << "lat long info. LAT:" << info.node.latitude << " LONG: " << info.node.longitude
                    << " Sats in view:" << info.satsInView << " Sats in use: " << info.satsInUse
                    << " Direction: " << info.heading
                    << " Speed: " << info.speed << " PDOP(Precision): " << info.PDOP;
}
GPS::GPS(){
  char status = serial.Open(PORT,BAUD);
  switch (status){
  case 1:
    CLOG(INFO,"gps") << "Serial opened successfully";
    break;
  case -1:
    CLOG(ERROR,"gps") << "Serial couldn't find device: " << PORT;
    break;
  case -2:
    CLOG(ERROR,"gps") << "Serial couldn't open device: " << PORT;
    break;
  case -3:
    CLOG(ERROR,"gps") << "Serial error while getting port params:" << PORT;
    break;
  case -4:
    cLOG(ERROR,"gps") << "Serial speed not recognized: " << BAUD;
    break;
  case -5:
    CLOG(ERROR,"gps") << "Serial error while writing port parameters: " << PORT;
    break;
  case -6:
    CLOG(ERROR,"gps") << "Error while writing timeout parameters: " << PORT;
    break;
  default:
    CLOG(INFO,"gps") << "Unkown error opening device" << status;
  }
}

GPS::~GPS(){
  serial.Close();
}

void GPS::parseNMEAString(string nmeastring){
  char * line = nmeastring.c_str();
        switch (minmea_sentence_id(line, false)) {
            case MINMEA_SENTENCE_GGA: {
                minmea_sentence_gga frame;
                if (minmea_parse_gga(&frame, line)) {
                  info.fixQuality = frame.fix_quality;
                  info.node.latitude = minmea_tocoord(&frame.latitude);
                  info.node.longitude = minmea_tocoord(&frame.longitude);
                  info.satsInUse = frame.satellites_tracked;
                }
                else {
                  CLOG(ERROR,"gps")<< "GGA sentenced not parsed"
                }
            } break;

            case MINMEA_SENTENCE_GST: {
                struct minmea_sentence_gst frame;
                if (minmea_parse_gst(&frame, line)) {
                  info.deviation.latitude = minmea_tofloat(&frame.latitude_error_deviation);
                  info.deviation.longitude = minmea_tofloat(&frame.longitude_error_deviation);
                }
                else {
                  CLOG(ERROR,"gps") << "GST sentence not parsed correctly"
                }
            } break;
            case MINMEA_SENTENCE_GLL:
              minmea_sentence_gll frame;
              if(minmea_parse_gll(&frame,line)){
                info.node.latitude = minmea_tocoord(&frame.latitude);
                info.node.longitude = minmea_tocoord(&frame.longitude);
                info.lastFix = frame.time.hours * 1000 + frame.time.minutes * 100 + frame.time.seconds; //todo improve this
              }
              else{
                CLOG(ERROR,"gps") << "GLL sentence not read correctly";
              }
              break;
           case MINMEA_SENTENCE_GSA:
             minmea_sentence_gsa frame;
             if(minmea_parse_gsa(&frame,line)){
               info.pdop = minmea_tocoord(&frame.pdop);
               //i guess figure out how many sats fromthis
             }
             else{
               CLOG(ERROR,"gps") << "error reading gsa string";
             }
             break;
            case MINMEA_INVALID: {
              CLOG(ERROR,"gps") << sentence << "is not valid";
            } break;

            default: {
              CLOG(ERROR,"gps") <<  sentence <<" is not parsed");
            } break;
        }
    }


}
