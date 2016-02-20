#include "logger.h"
#include "GPS.h"
#include "Lidar.h"
#include "GPSNodelist.h"
#include "MotorController.h"
#include "Conf.hpp"
#include "LoggerDispatch.h"
#include "Vision.h"
#include <unistd.h>
#include <ctime>

using namespace std;
#include <string>

INITIALIZE_EASYLOGGINGPP

int main(int argv, char* argc[]){

  Conf conf("./conf.json");
  
  //Setting up logging stuff
  el::Configurations loggingConf;
  loggingConf.setToDefault();
  loggingConf.set(el::Level::Global, el::ConfigurationType::Filename, conf.data["logfile"].get<string>());
  //el::Loggers::getLogger("gps");
  el::Loggers::getLogger("lidar");
  el::Loggers::getLogger("motorcontrol");
  el::Loggers::reconfigureAllLoggers(loggingConf); //has to bafter all logging conf

  LoggerDispatchGlobals::setConfiguration(conf);
  el::Helpers::installLogDispatchCallback<LoggerDispatch>("LoggerDispatch");
  //GPS gps(conf);
  MotorController motor(conf);
  Lidar pathfinding(conf);
  //Vision seethings(conf);

//GPSNodelist nodelist(conf);
  // gps.blockUntilFixed();
  //GPSNode node = nodelist.getNextNode();
  //while(!nodelist.allNodesVisited()){
  // if(gps.isOverlapping(node)){
  //   node = nodelist.getNextNode();
  // }
  // else{
  //   double desiredHeading = gps.calculateHeadingToNode(node); 
  //   double bestPossibleHeading = pathfinding.bestAvailableHeading(desiredHeading);
  //   motor.commandTurn(bestPossibleHeading);
  time_t start = time(nullptr);
  while(time(nullptr)-start < 10) {
       //double bestPossibleHeading = pathfinding.bestAvailableHeading(0);
       //LOG(INFO) << "Best Heading: " << bestPossibleHeading;
       //LOG(INFO) << pathfinding.prettyPrintWithHeuristicValues(0);
       double bestPossibleHeading = pathfinding.bestAvailableHeading(0);
	motor.commandForward(1);
	motor.commandTurn(bestPossibleHeading);
  }

  return 0;
}
