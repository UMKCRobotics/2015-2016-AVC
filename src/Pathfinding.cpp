#include "Pathfinding.h"

void Pathfinding::readAllInQueue(){
  while(serial.Peek() != 0){
    char* out_charp;
    unsigned int stop_throwing_warnings = 128;
    serial.ReadString(out_charp,'$',stop_throwing_warnings);
    parseReadingAndInsertIntoReadings(out_charp);
  }
}
void Pathfinding::parseReadingAndInsertIntoReadings(string out_string){
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
  readings[key] = value;
}

double Pathfinding::bestAvailableHeading(double desiredHeading){
  double best_heading;
  for(auto it = Pathfinding::readings.begin(); it != Pathfinding::readings.end();++it){
    double heuristic_value = Pathfinding::rayHeuristic(desiredHeading,it->first,it->second);
    if(heuristic_value > best_heading){
      best_heading = heuristic_value;
    }
  }
  return best_heading;
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
