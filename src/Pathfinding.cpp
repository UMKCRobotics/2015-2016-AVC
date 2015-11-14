#include "Pathfinding.h"

void Pathfinding::readAllInQueue(){
  while(Pathfinding::serial.Peek() != 0){
    char* out_charp;
    unsigned int stop_throwing_warnings = 128;
    Pathfinding::serial.ReadString(out_charp,'$',stop_throwing_warnings);
    if (out_charp){
      Pathfinding::parseReadingAndInsertIntoReadings(out_charp);
    }
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
  bool rayMax = configuration.data["pathfinding"]["ray_maximum"];
  readings[direction] = (distance >= rayMax)? rayMax : distance;
}

double Pathfinding::bestAvailableHeading(double desiredHeading){
  double best_heading = -1;
  if(configuration.data["pathfinding"]["perform_growth"]){
    unordered_map<double,double> grown_readings = performObstactleGrowth();
    for(auto it = grown_readings.begin(); it != grown_readings.end();++it){
        double heuristic_value = rayHeuristic(desiredHeading,it->first,it->second);
        if(heuristic_value > best_heading){
          best_heading = heuristic_value;
        }
    }
  }
  else{
    for(auto it = readings.begin(); it != readings.end();++it){
        double heuristic_value = rayHeuristic(desiredHeading,it->first,it->second);
        if(heuristic_value > best_heading){
          best_heading = heuristic_value;
        }
    }
  }
  CLOG(INFO,"pathfinding") << "best" << best_heading;
  return best_heading;
}

unordered_map<double,double> Pathfinding::performObstactleGrowth(){
  unordered_map<double,double> newMap (readings);
  bool rayMax = configuration.data["pathfinding"]["ray_maximum"];
  double safeLength = configuration.data["pathfinding"]["safe_length"];
  for(auto ray = Pathfinding::readings.begin(); ray != Pathfinding::readings.end(); ++ray){
    for(auto other_ray = Pathfinding::readings.begin(); other_ray != Pathfinding::readings.end(); ++other_ray){ 
      if(other_ray->second <= rayMax){
        double beta = abs(atan2(safeLength/2,other_ray->second));
        double angle_between_two_rays = abs(AngleMath::angleBetweenTwoAngles(ray->first,other_ray->first));
        if(angle_between_two_rays < beta){
            double v = other_ray->second - safeLength/2;
            newMap[ray->first] = v;
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
  if(configuration.data["pathfinding"]["add_degree_coefficient"]){return rayDistance*inverseDev*(desiredHeading/60);}//60 should be sweep angle
  return rayDistance*inverseDev;
}

void Pathfinding::openSerial(){
  string port = configuration.data["pathfinding"]["port"].get<string>();
  int baud = configuration.data["pathfinding"]["baud"];
  char status = serial.Open(port.c_str(),baud);
  switch (status){
  case 1:
    CLOG(INFO,"pathfinding") << "Serial opened successfully";
    break;
  case -1:
    CLOG(FATAL,"pathfinding") << "Serial couldn't find device: " << port;
    break;
  case -2:
    CLOG(FATAL,"pathfinding") << "Serial couldn't open device: " << port;
    break;
  case -3:
    CLOG(FATAL,"pathfinding") << "Serial error while getting port params:" << port;
    break;
  case -4:
    CLOG(FATAL,"pathfinding") << "Serial speed not recognized: " << baud;
    break;
  case -5:
    CLOG(FATAL,"pathfinding") << "Serial error while writing port parameters: " << port;
    break;
  case -6:
    CLOG(FATAL,"pathfinding") << "Error while writing timeout parameters: " << port;
    break;
  default:
    CLOG(FATAL,"pathfinding") << "Unkown error opening device" << status;
    break;
  }
}
Pathfinding::Pathfinding(Conf c){
  configuration = c;
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
