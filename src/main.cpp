#include "logger.h"
#include "GPS.h"
#include "Lidar.h"
#include "GPSNodelist.h"
#include "MotorController.h"
#include "Conf.hpp"
#include "LoggerDispatch.h"
#include "StartWaiter.h"
#include <unistd.h>
#include <time.h>

using namespace std;
#include <string>

INITIALIZE_EASYLOGGINGPP

int main(int argv, char* argc[]){

  Conf conf("/home/umkc/2015-2016-AVC/conf.json");
  
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
  StartWaiter startwaiter(&LoggerDispatchGlobals::serial);
 while(true){
  startwaiter.blockUntilGoSignal();
  long runningTime = conf.data["runningTime"].get<long>();
  int speed = conf.data["testSpeed"].get<int>();
    long count = 0;
    while(count < runningTime) {
        //double bestPossibleHeading = pathfinding.bestAvailableHeading(0);
        //LOG(INFO) << "Best Heading: " << bestPossibleHeading;
        //LOG(INFO) << pathfinding.prettyPrintWithHeuristicValues(0);
	usleep(5000);
        double bestPossibleHeading = pathfinding.bestAvailableHeading(0);
	LOG(INFO) << pathfinding.prettyPrintWithHeuristicValues(0);
        motor.commandTurn(bestPossibleHeading);
	usleep(5000);
        motor.commandForward(speed);
	count++;
	LOG(INFO) << count;
    }
    LOG(INFO) << "lkawjerlkawjerlkj";
	
    	for(int i = 0; i < 10; ++i){
    		motor.commandStop();
		usleep(10000);
	}
}

  return 0;
}
