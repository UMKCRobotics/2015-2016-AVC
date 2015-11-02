#include "logger.h"
#include "GPS.h"
#include "Pathfinding.h"
#include "GPSNodelist.h"
#include "Conf.hpp"

#include <string>

INITIALIZE_EASYLOGGINGPP

int main(int argv, char* argc[]){
  Conf conf("./conf.json");
  
  //Setting up logging stuff
  el::Configurations loggingConf;
  loggingConf.setToDefault();
  loggingConf.parseFromText("GLOBAL:\n FILENAME = " + conf.data["logfile"].get<string>());
  el::Loggers::getLogger("gps");
  el::Loggers::getLogger("pathfinding");
  LOG(INFO) << "started program";

  GPS gps(conf);
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
