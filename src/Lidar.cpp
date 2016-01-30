#include "Lidar.h"

void Lidar::readAllInQueue(){
    parseReadingAndInsertIntoReadings();
}
void Lidar::parseReadingAndInsertIntoReadings(){
  char out_char = ' ';
  string s_key, s_value;
  while(true){
    char code = serial.ReadChar(&out_char); //add error reporting
    if(code < 0){
      switch(code){
      case 0:
        CLOG(ERROR,"lidar")  << "timeout reached in serial read";
        break;
      case -1:
        CLOG(ERROR,"lidar") << "erorr while setting the timeout";
        break;
      case -2:
        CLOG(ERROR,"lidar") << "error while reading the byte";
        break;
      default:
        CLOG(ERROR,"lidar") << "unkown serial error 2003, so emotional";
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
        CLOG(ERROR,"lidar")  << "timeout reached in serial read";
        break;
      case -1:
        CLOG(ERROR,"lidar") << "erorr while setting the timeout";
        break;
      case -2:
        CLOG(ERROR,"lidar") << "error while reading the byte";
        break;
      default:
        CLOG(ERROR,"lidar") << "unkown serial error 2003, pls alert yung lean :(";
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
    if(direction > sweepAngle / 2) {
      CLOG(WARNING, "lidar") << "Found angle outside sweep angle" << direction;
    }
    readings[direction] = (distance >= rayMax)? rayMax : distance;
    //CLOG(INFO,"lidar") << "Found Direction: " << direction << " and Distance: " << distance;
  }catch(const invalid_argument& e){
    CLOG(WARNING,"lidar") << "Bad conversion to number: " << s_key << " " << s_value;
  }
}

Angle Lidar::bestAvailableHeading(Angle desiredHeading){
  TIMED_FUNC(timerobj);
  Angle best_heading = -1;
  double best_heuristic = -1;
  ReadingContainer used_reading;
  if(configuration.data["pathfinding"]["lidar"]["perform_growth"]){
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
  CLOG(INFO,"lidar") << "Computed best heading: " << best_heading;
  return best_heading;
}

ReadingContainer Lidar::performObstactleGrowth(){
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

Distance Lidar::computeGrowthLength(Angle sourceAngle, Distance sourceDistance, Angle otherAngle, Distance otherDistance){
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

double Lidar::rayHeuristic(double desiredHeading, double rayHeading, int rayDistance){
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

void Lidar::openSerial(){
  string port = configuration.data["pathfinding"]["lidar"]["port"].get<string>();
  int baud = configuration.data["pathfinding"]["lidar"]["baud"];
  char status = serial.Open(port.c_str(),baud);
  serial.FlushReceiver();
  switch (status){
  case 1:
    CLOG(INFO,"lidar") << "Serial opened successfully";
    break;
  case -1:
    CLOG(FATAL,"lidar") << "Serial couldn't find device: " << port;
    break;
  case -2:
    CLOG(FATAL,"lidar") << "Serial couldn't open device: " << port;
    break;
  case -3:
    CLOG(FATAL,"lidar") << "Serial error while getting port params:" << port;
    break;
  case -4:
    CLOG(FATAL,"lidar") << "Serial speed not recognized: " << baud;
    break;
  case -5:
    CLOG(FATAL,"lidar") << "Serial error while writing port parameters: " << port;
    break;
  case -6:
    CLOG(FATAL,"lidar") << "Error while writing timeout parameters: " << port;
    break;
  default:
    CLOG(FATAL,"lidar") << "Unkown error opening device" << status;
    break;
  }
}
Lidar::Lidar(){
  ///Only used for mocking
  safeLength = 100;
  rayMax = 100;
}
Lidar::Lidar(Conf c){
  configuration = c;
  safeLength = configuration.data["pathfinding"]["lidar"]["safe_length"];
  rayMax = configuration.data["pathfinding"]["lidar"]["ray_maximum"];
  sweepAngle = configuration.data["pathfinding"]["lidar"]["sweep_angle"];
  threadContinue = true;
  pathfinding_serial_thread = thread([this]{
      openSerial();
      while(threadContinue){
        readAllInQueue();
      }
    });
}
Lidar::~Lidar(){
  threadContinue= false;
  pathfinding_serial_thread.join();
  serial.Close();
}

string Lidar::prettyPrint(){
  stringstream output;
  int max = rayMax;
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
string Lidar::prettyPrintWithHeuristicValues(Angle desiredHeading){
  stringstream output;
  int max = rayMax;
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
