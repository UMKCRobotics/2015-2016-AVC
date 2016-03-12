#define DEBUG false

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
#include <sstream>

INITIALIZE_EASYLOGGINGPP

int main(int argv, char* argc[]){

  Conf conf("/home/umkc/2015-2016-AVC/conf.json");
  
  //Setting up logging stuff
  el::Configurations loggingConf;
  loggingConf.setToDefault();
  loggingConf.set(el::Level::Global, el::ConfigurationType::Filename, conf.data["logfile"].get<string>());
  el::Loggers::getLogger("gps");
  el::Loggers::getLogger("lidar");
  el::Loggers::getLogger("motorcontrol");
  el::Loggers::reconfigureAllLoggers(loggingConf); //has to bafter all logging conf

 LoggerDispatchGlobals::setConfiguration(conf);
  el::Helpers::installLogDispatchCallback<LoggerDispatch>("LoggerDispatch");
  GPS gps(conf);
  MotorController motor(conf);
 // Lidar pathfinding(conf);
  //Vision seethings(conf);


  StartWaiter startwaiter(&LoggerDispatchGlobals::serial);
  int speed = conf.data["testSpeed"].get<int>();
  startwaiter.blockUntilGoSignal();
  GPSNodelist nodelist(conf);
  string ps = "iayylmao" + '\n';
  const char * os = ps.c_str();
  LoggerDispatchGlobals::serial.WriteString(os);
  gps.blockUntilFixed(); 
  while(true){
	usleep(1000000);
	GPSNode nod = gps.info.node;
	stringstream ss;
	string outputString;
	ss << "i" << nod.latitude << " " << nod.longitude << endl;
	getline(ss,outputString);
	outputString = outputString + '\n';
	const char * output = outputString.c_str();
	LoggerDispatchGlobals::serial.WriteString(output);
  }
  /*
  GPSNode node = nodelist.getNextNode();
  while(!nodelist.allNodesVisited()){
   if(gps.isOverlapping(node)){
     node = nodelist.getNextNode();
    }
   else{
     double desiredHeading = gps.calculateHeadingToNode(node); 
     //double bestPossibleHeading = pathfinding.bestAvailableHeading(desiredHeading);
     motor.commandTurn(bestPossibleHeading);
     usleep(5000);
     motor.commandForward(speed);
     usleep(5000);
    }
  }
  */
  return 0;
}
