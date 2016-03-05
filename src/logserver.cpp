#include "logserver.h"

using namespace std;

LogServer::LogServer(Conf C)
{
	configuration = C;

}

void LogServer::eventHandler(struct mg_connection *connection, int event, void *p)
{

	static struct mg_serve_http_opts serverOptions;
	serverOptions.document_root = "./webroot"; // hackish attempt to make this work
	
	switch(event)
	{

		case MG_EV_HTTP_REQUEST: //handle HTTP requests 
		std::cout << "HTTP REQUEST RECEIVED" << std::endl;

			mg_serve_http(connection, (struct http_message*) p, serverOptions);

	}

}

void LogServer::StartServer()
{
	struct mg_mgr manager;
	struct mg_connection *connection;

	mg_mgr_init(&manager, NULL);
	connection = mg_bind(&manager, http_port, eventHandler);

	mg_set_protocol_http_websocket(connection);

	//begin polling for connections
	while(1)
	{
		mg_mgr_poll(&manager, 1000);
	}

	std::cout << "Finished" << std::endl;

	mg_mgr_free(&manager);
}