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
        CLOG(ERROR,"pathfinding") << "unkown serial error 2003, so emotional";
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
        CLOG(ERROR,"pathfinding") << "unkown serial error 2003, pls alert yung lean :(";
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
  Angle direction;
  Distance distance;
  try{
    direction = stoi(s_key);
    distance = stoi(s_value);
    readings[direction] = (distance >= rayMax)? rayMax : distance;
    //CLOG(INFO,"pathfinding") << "Found Direction: " << direction << " and Distance: " << distance;
  }catch(const invalid_argument& e){
    CLOG(ERROR,"pathfinding") << "Bad conversion to number: " << s_key << " " << s_value;
  }
}

Angle Pathfinding::bestAvailableHeading(Angle desiredHeading){
  Angle best_heading = -1;
  double best_heuristic = -1;
  ReadingContainer used_reading;
  if(configuration.data["pathfinding"]["perform_growth"]){
    used_reading = performObstactleGrowth();
  }
  else{
    used_reading = readings;
  }
  for(auto it = used_reading.begin(); it != used_reading.end();++it){
        double heuristic_value = rayHeuristic(desiredHeading,it->first,it->second);
        if(heuristic_value > best_heuristic || best_heuristic == -1){
          best_heading = it->first;
          best_heuristic = heuristic_value;
        }
  }
  CLOG(INFO,"pathfinding") << "Computed best heading: " << best_heading;
  return best_heading;
}

ReadingContainer Pathfinding::performObstactleGrowth(){
  ReadingContainer newMap (readings); //copy the current readings
  Distance leftRayDistance = -1;
  Angle leftRayAngle = -1;
  Distance rightRayDistance = -1;
  Angle rightRayAngle = -1;
  bool first = true;
  for(auto ray = newMap.begin(); ray != newMap.end(); ++ray){
    leftRayDistance = -1;
    leftRayAngle = -1;
    rightRayDistance = -1;
    rightRayAngle = -1;
    Distance outDistance = ray->second;
    //try to find a right ray
    auto rightRay =  *(&ray + 1);
    if(rightRay != readings.end()){ //there is a right ray
      rightRayAngle = rightRay->first;
      rightRayDistance = rightRay->second;
    }
    if (!first){
      auto leftRay = *(&ray - 1);
      leftRayAngle = leftRay->first;
      leftRayDistance = leftRay->second;
    }
    //check left
    if(leftRayAngle != -1 && leftRayDistance != -1){ //there is a left ray
      outDistance = computeGrowthLength(ray->first,ray->second,leftRayAngle,leftRayDistance);
    }
    //check right
    if(rightRayAngle != -1 && rightRayDistance != -1){ //there is a left ray
      Distance rightDistance;
      rightDistance = computeGrowthLength(ray->first,ray->second,rightRayAngle,rightRayDistance);
      if(rightDistance < outDistance || outDistance == -1){
        outDistance = rightDistance;
      }
    }
    leftRayAngle = ray->first; //change the left to the current one
    leftRayDistance = ray->second;
    newMap[ray->first] = outDistance;
    
  }
  return newMap;
}

Distance Pathfinding::computeGrowthLength(Angle sourceAngle, Distance sourceDistance, Angle otherAngle, Distance otherDistance){
    if(otherDistance <= rayMax){
        double beta = abs(atan2(safeLength/2,otherAngle));
        double angle_between_two_rays = abs(AngleMath::angleBetweenTwoAngles(sourceAngle,otherAngle));
        if(angle_between_two_rays < beta){
            Distance v = otherDistance - safeLength/2;
            return v;
        }
    }
    return sourceDistance;
}

double Pathfinding::rayHeuristic(double desiredHeading, double rayHeading, int rayDistance){
  //took absolute value -- 
  if(rayDistance < safeLength){
    return 0;
  }
  double deviationHeading = abs(AngleMath::angleBetweenTwoAngles(desiredHeading,rayHeading));
  double inverseDev;
  if(deviationHeading == 0){
    inverseDev = 1;
   }
  else{
    inverseDev = (1/deviationHeading);
  }
  //if(configuration.data["pathfinding"]["add_degree_coefficient"]){return rayDistance*inverseDev*(desiredHeading/60);}//60 should be sweep angle
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
Pathfinding::Pathfinding(){
  ///Only used for mocking
  safeLength = 100;
  rayMax = 100;
}
Pathfinding::Pathfinding(Conf c){
  configuration = c;
  safeLength = configuration.data["pathfinding"]["safe_length"];
  rayMax = configuration.data["pathfinding"]["ray_maximum"];
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
  stringstream output;
  int max = configuration.data["pathfinding"]["ray_maximum"];
  for(auto it = readings.begin(); it != readings.end();++it){
    output << it->first;
    double percent = (it->second / ((double)max)) * 100;
    int rounded_percent = round(percent);
    for(int i = 0; i < rounded_percent; ++i){
      output << "|";
    }
    output << "\n";
  }
  return output.str();
}
string Pathfinding::prettyPrintWithHeuristicValues(Angle desiredHeading){
  stringstream output;
  int max = configuration.data["pathfinding"]["ray_maximum"];
  Angle bestHeading = bestAvailableHeading(desiredHeading);
  for(auto it = readings.begin(); it != readings.end();++it){
    double heuristic = rayHeuristic(desiredHeading,it->first,it->second);
    output << it->first;
    double percent = (it->second / ((double)max)) * 100;
    int rounded_percent = round(percent);
    double h_percent = (heuristic / ((double)max)) * 100;
    int heuristic_rounded_percent = round(h_percent);
    output << "\033[0;31m";
    for(int i = 1; i <= rounded_percent; ++i){
      output << "|";
      if(i >= heuristic_rounded_percent){
        output << "\033[0;37m";
      }
    }
    output << "R: " << it->second << ", H: "<< heuristic << ((bestHeading == it->first)? " ***" : "")<<"\n";
  }
  return output.str();
}
