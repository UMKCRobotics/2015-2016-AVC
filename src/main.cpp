#include "logger.h"
#include "GPS.h"
INITIALIZE_EASYLOGGINGPP

int main(int argv, char* argc[]){


  el::Loggers::getLogger("gps");
  el::Loggers::getLogger("pathfinding");
  LOG(INFO) << "started program";
  GPS gps;
  while(true){
    //LOG(INFO) << "test";
  }
  return 0;
}
