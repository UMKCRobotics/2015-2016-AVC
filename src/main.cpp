#define DEBUG false

#include "logger.h"
#include "GPS.h"
#include "Lidar.h"
#include "GPSNodelist.h"
#include "MotorController.h"
#include "Conf.hpp"
#include "LoggerDispatch.h"
#include "StartWaiter.h"
#include "Compass.h"
#include <unistd.h>
#include <time.h>

using namespace std;
#include <string>
#include <sstream>

INITIALIZE_EASYLOGGINGPP

int main(int argv, char* argc[]){

  Conf conf("/home/umkc/2015-2016-AVC/conf.json");

  usleep(100000); 
  //Setting up logging stuff
  el::Configurations loggingConf;
  loggingConf.setToDefault();
  loggingConf.set(el::Level::Global, el::ConfigurationType::Filename, conf.data["logfile"].get<string>());
  el::Loggers::getLogger("gps");
  el::Loggers::getLogger("compass");
  el::Loggers::getLogger("lidar");
  el::Loggers::getLogger("motorcontrol");
  el::Loggers::reconfigureAllLoggers(loggingConf); //has to bafter all logging conf

 LoggerDispatchGlobals::setConfiguration(conf);
  el::Helpers::installLogDispatchCallback<LoggerDispatch>("LoggerDispatch");
  GPS gps(conf);
while(true){}
/*
  MotorController motor(conf);
  Compass compass(conf);
  //Lidar pathfinding(conf);
  //Vision seethings(conf);


  StartWaiter startwaiter(&LoggerDispatchGlobals::serial);
  int speed = conf.data["testSpeed"].get<int>();
  startwaiter.blockUntilGoSignal();
  GPSNodelist nodelist(conf);
  GPSNode node = nodelist.getNextNode();
//  gps.blockUntilFixed(); 
  while(!nodelist.allNodesVisited()){
   if(gps.isOverlapping(node)){
     node = nodelist.getNextNode();
    }
   else{
     double desiredHeading = gps.calculateDesiredHeading(compass.curHeading, gps.info.node, node); 
     CLOG(INFO,"gps")  << "cd: " << compass.curHeading << "dh: " << desiredHeading;
     motor.commandTurn(desiredHeading);
     usleep(100000);
     motor.commandForward(speed);
     usleep(100000);
    }
  }
  for(int i = 0; i < 10; ++i){
	  motor.commandStop();
	  usleep(5000);
  }
  return 0;
*/
}
