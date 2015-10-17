#include "logger.h"

INITIALIZE_EASYLOGGINGPP

int main(int argv, char* argc[]){
  LOG(INFO) << "started program";
	return 0;
}
