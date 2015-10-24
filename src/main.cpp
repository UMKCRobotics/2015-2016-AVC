#include "logger.h"
#include "GPS.h"

INITIALIZE_EASYLOGGINGPP

int main(int argv, char* argc[]){
  LOG(INFO) << "started program";
  GPS gps;
  while(true){
    gps.debug(); 
  }
	return 0;
}
