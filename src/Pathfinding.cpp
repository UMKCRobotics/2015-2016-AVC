#include "Pathfinding.h"

void Pathfinding::readAllInQueue(){
  while(Pathfinding::serial.Peek() != 0){
    char* out_charp;
    unsigned int stop_throwing_warnings = 128;
    Pathfinding::serial.ReadString(out_charp,'$',stop_throwing_warnings);
    Pathfinding::parseReadingAndInsertIntoReadings(out_charp);
  }
}
void Pathfinding::parseReadingAndInsertIntoReadings(string out_string){
  CLOG(INFO,"pathfinding") << "trying to parse" << out_string;
  string s_key,s_value;
  int i =0;
  while(out_string[i] != ','){
    s_key += out_string[i];
    ++i;
  }
  while(out_string[i] != '$'){
    s_value += out_string[i];
    ++i;
  }
  double direction,distance;
  direction = stod(s_key);
  distance = stod(s_value);
  readings[direction] = (distance >= RAY_MAXIMUM)? RAY_MAXIMUM : distance;
}

double Pathfinding::bestAvailableHeading(double desiredHeading){
  double best_heading;
  unordered_map<double,double> grown_readings = performObstactleGrowth();
  for(auto it = grown_readings.begin(); it != grown_readings.end();++it){
    double heuristic_value = Pathfinding::rayHeuristic(desiredHeading,it->first,it->second);
    if(heuristic_value > best_heading){
      best_heading = heuristic_value;
    }
  }
  CLOG(INFO,"pathfinding") << "best" << best_heading;
  return best_heading;
}

unordered_map<double,double> Pathfinding::performObstactleGrowth(){
  unordered_map<double,double> newMap (readings);
  for(auto ray = Pathfinding::readings.begin(); ray != Pathfinding::readings.end(); ++ray){
    for(auto other_ray = Pathfinding::readings.begin(); other_ray != Pathfinding::readings.end(); ++other_ray){ 
      if(other_ray->second <= RAY_MAXIMUM){
        double beta = abs(atan2(SAFE_LENGTH/2,other_ray->second));
        double angle_between_two_rays = abs(AngleMath::angleBetweenTwoAngles(ray->first,other_ray->first));
        if(angle_between_two_rays < beta){
            double v = other_ray->second - SAFE_LENGTH/2;
            readings[ray->first] = v;
        }
      }
    }
  }
  return newMap;
}

double Pathfinding::rayHeuristic(double desiredHeading, double rayHeading, double rayDistance){
  //took absolute value -- 
  double deviationHeading = abs(AngleMath::angleBetweenTwoAngles(desiredHeading,rayHeading));
  double inverseDev;
  if(deviationHeading == 0){
    inverseDev = 1;
   }
  else{
    double inverseDev = (1/deviationHeading);
  }
  return rayDistance*inverseDev;
}

void Pathfinding::openSerial(){
  char status = serial.Open(PORT,BAUD);
  switch (status){
  case 1:
    CLOG(INFO,"pathfinding") << "Serial opened successfully";
    break;
  case -1:
    CLOG(ERROR,"pathfinding") << "FATAL: Serial couldn't find device: " << PORT;
    exit(EXIT_FAILURE);
    break;
  case -2:
    CLOG(ERROR,"pathfinding") << "FATAL: Serial couldn't open device: " << PORT;
    exit(EXIT_FAILURE);
    break;
  case -3:
    CLOG(ERROR,"pathfinding") << "FATAL: Serial error while getting port params:" << PORT;
    exit(EXIT_FAILURE);
    break;
  case -4:
    CLOG(ERROR,"pathfinding") << "FATAL: Serial speed not recognized: " << BAUD;
    exit(EXIT_FAILURE);
    break;
  case -5:
    CLOG(ERROR,"pathfinding") << "FATAL: Serial error while writing port parameters: " << PORT;
    exit(EXIT_FAILURE);
    break;
  case -6:
    CLOG(ERROR,"pathfinding") << "FATAL: Error while writing timeout parameters: " << PORT;
    exit(EXIT_FAILURE);
    break;
  default:
    CLOG(INFO,"pathfinding") << "FATAL: Unkown error opening device" << status;
    exit(EXIT_FAILURE);
  }
}
Pathfinding::Pathfinding(){
  threadContinue = true;
  pathfinding_serial_thread = thread([this]{
      openSerial();
      while(threadContinue){
        readAllInQueue();
      }
    });
}
Pathfinding::~Pathfinding(){
  threadContinue= false;
  pathfinding_serial_thread.join();
  serial.Close();
}
