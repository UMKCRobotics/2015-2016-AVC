#include "Pathfinding.h"

void Pathfinding::readAllInQueue(){
  while(Pathfinding::serial.Peek() != 0){
    char* out_charp;
    Pathfinding::serial.ReadString(out_charp,'$',MAX_SERIAL_LENGTH);
    Pathfinding::parseReadingAndInsertIntoReadings(str(out_charp));
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
  Pathfinding::readings[key] = value;
}

double Pathfinding::bestAvailable(double desiredHeading){
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
  //TODO: actually implement this
  return rayDistance - AngleMath::angleBetweenTwoAngles(desiredHeading,rayHeading);
}
