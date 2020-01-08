#include "src/headers/Server.h"

using namespace std;


int main(int argc, char* argv[])
{
	int serverPort;
	
	if(argc == 1) 
		serverPort = 1234;
	else 
		serverPort = atoi(argv[1]);
	
	Server server = Server(serverPort);
	server.setup();
	server.start();
	
	return(0);
}
