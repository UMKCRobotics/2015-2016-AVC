#ifndef LOG_SERVER_H
#define LOG_SERVER_H

#include "mongoose/mongoose.h"
//#include "logger.h"
#include "conf.h"
#include <string>
#include <iostream>

class LogServer
{
	private:
		const char *http_port = "8000"; //TODO: make this a config option

		Conf configuration;
		
		//handler for requests
		static void eventHandler(struct mg_connection *connection, int ev, void *p);

	public:

		LogServer(Conf C);
		
		//begin serving webpage
		void StartServer();



};

#endif