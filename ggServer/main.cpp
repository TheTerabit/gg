#include "src/headers/Server.h"

using namespace std;


int main(int argc, char* argv[])
{
	Server server = Server();
	server.setup();
	server.start();
	
	return(0);
}
