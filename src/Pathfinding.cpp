#include "Pathfinding.h"

void Pathfinding::readAllInQueue(){
    parseReadingAndInsertIntoReadings();
}
void Pathfinding::parseReadingAndInsertIntoReadings(){
  char out_char = ' ';
  string s_key, s_value;
  while(true){
    char code = serial.ReadChar(&out_char); //add error reporting
    if(code < 0){
      switch(code){
      case 0:
        CLOG(ERROR,"pathfinding")  << "timeout reached in serial read";
        break;
      case -1:
        CLOG(ERROR,"pathfinding") << "erorr while setting the timeout";
        break;
      case -2:
        CLOG(ERROR,"pathfinding") << "error while reading the byte";
        break;
      default:
        CLOG(ERROR,"pathfinding") << "unkown serial error 2003";
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
        CLOG(ERROR,"pathfinding")  << "timeout reached in serial read";
        break;
      case -1:
        CLOG(ERROR,"pathfinding") << "erorr while setting the timeout";
        break;
      case -2:
        CLOG(ERROR,"pathfinding") << "error while reading the byte";
        break;
      default:
        CLOG(ERROR,"pathfinding") << "unkown serial error 2003";
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
  double direction,distance;
  direction = stoi(s_key);
  distance = stoi(s_value);
  int rayMax = configuration.data["pathfinding"]["ray_maximum"];
  readings[direction] = (distance >= rayMax)? rayMax : distance;
  CLOG(INFO,"pathfinding") << "Found Direction: " << direction << " and Distance: " << distance;
}

double Pathfinding::bestAvailableHeading(double desiredHeading){
  double best_heading = -1;
  double best_heuristic = -1;
  if(configuration.data["pathfinding"]["perform_growth"]){
    unordered_map<int,int> grown_readings = performObstactleGrowth();
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
        if(heuristic_value > best_heuristic || best_heuristic == -1){
          best_heading = it->first;
          best_heuristic = heuristic_value;
        }
    }
  }
  CLOG(INFO,"pathfinding") << "best" << best_heading;
  return best_heading;
}

unordered_map<int,int> Pathfinding::performObstactleGrowth(){
  unordered_map<int,int> newMap (readings);
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

double Pathfinding::rayHeuristic(double desiredHeading, double rayHeading, int rayDistance){
  //took absolute value -- 
  double deviationHeading = abs(AngleMath::angleBetweenTwoAngles(desiredHeading,rayHeading));
  double inverseDev;
  if(deviationHeading == 0){
    inverseDev = 1;
   }
  else{
    inverseDev = (1/deviationHeading);
  }
  if(configuration.data["pathfinding"]["add_degree_coefficient"]){return rayDistance*inverseDev*(desiredHeading/60);}//60 should be sweep angle
  return rayDistance*inverseDev;
}

void Pathfinding::openSerial(){
  string port = configuration.data["pathfinding"]["port"].get<string>();
  int baud = configuration.data["pathfinding"]["baud"];
  char status = serial.Open(port.c_str(),baud);
  serial.FlushReceiver();
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

string Pathfinding::prettyPrint(){
  string output;
  int max = configuration.data["pathfinding"]["ray_maximum"];
  for(auto it = readings.begin(); it != readings.end();++it){
    output += it->first;
    int percent = it->second / max;
    for(int i = 0; i < percent; ++i){
      output += "|";
    }
    output += "\n";
  }
  return output;
}
