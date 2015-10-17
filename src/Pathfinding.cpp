#include "Pathfinding.h"

void Pathfinding::readAllInQueue(){
  LOG(INFO) << "Reaeading all queued data, size:" << Pathfinding::serial.Peek();
  while(Pathfinding::serial.Peek() != 0){
    char* out_charp;
    unsigned int stop_throwing_warnings = 128;
    Pathfinding::serial.ReadString(out_charp,'$',stop_throwing_warnings);
    Pathfinding::parseReadingAndInsertIntoReadings(out_charp);
  }
}
void Pathfinding::parseReadingAndInsertIntoReadings(string out_string){
  LOG(INFO) << "Pathfinding trying to parse" << out_string;
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
  double key,value;
  key = stod(s_key);
  value = stod(s_value);
  Pathfinding::readings[key] = value;
}

double Pathfinding::bestAvailableHeading(double desiredHeading){
  double best_heading;
  for(auto it = Pathfinding::readings.begin(); it != Pathfinding::readings.end();++it){
    double heuristic_value = Pathfinding::rayHeuristic(desiredHeading,it->first,it->second);
    if(heuristic_value > best_heading){
      best_heading = heuristic_value;
    }
  }
  LOG(INFO) << "PATHFINDING best" << best_heading;
  return best_heading;
}
double Pathfinding::rayHeuristic(double desiredHeading, double rayHeading, double rayDistance){
  //TODO: actually implement this
  return rayDistance - AngleMath::angleBetweenTwoAngles(desiredHeading,rayHeading);
}
