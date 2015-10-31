#include "logger.h"
#include "GPS.h"
#include "Pathfinding.h"
#include "GPSNodelist.h"

INITIALIZE_EASYLOGGINGPP

int main(int argv, char* argc[]){
  el::Loggers::getLogger("gps");
  el::Loggers::getLogger("pathfinding");
  LOG(INFO) << "started program";
  GPS gps;
  Pathfinding pathfinding;
  GPSNodelist nodelist("./nodes");
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
