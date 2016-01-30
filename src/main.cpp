#include "logger.h"
#include "GPS.h"
#include "Pathfinding.h"
#include "GPSNodelist.h"
#include "MotorController.h"
#include "Conf.hpp"

#include <string>

INITIALIZE_EASYLOGGINGPP

int main(int argv, char* argc[]){
  Conf conf("./conf.json");
  
  
  
  //Setting up logging stuff
  el::Configurations loggingConf;
  loggingConf.setToDefault();
  loggingConf.set(el::Level::Global, el::ConfigurationType::Filename, conf.data["logfile"].get<string>());
  el::Loggers::getLogger("gps");
  el::Loggers::getLogger("pathfinding");
  el::Loggers::getLogger("motorcontrol");
  el::Loggers::reconfigureAllLoggers(loggingConf); //has to bafter all logging conf

  LOG(INFO) << "started program";

  GPS gps(conf);
  MotorController motor(conf);
  Pathfinding pathfinding(conf);
  GPSNodelist nodelist(conf);
  gps.blockUntilFixed();
  GPSNode node = nodelist.getNextNode();
  while(!nodelist.allNodesVisited()){
    if(gps.isOverlapping(node)){
      node = nodelist.getNextNode();
    }
    else{
      double desiredHeading = gps.calculateHeadingToNode(node); 
      double bestPossibleHeading = pathfinding.bestAvailableHeading(desiredHeading);
      //tell motor commander to turn to there
    }
  }

  return 0;
}
