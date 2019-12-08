#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <iostream>
#include <vector>

#include "Message.h"

#define SERVER_PORT 1234
#define QUEUE_SIZE 5

using namespace std;

struct thread_data_t
{
	int x;
};

class Server
{
	private:
		int server_socket_descriptor;
		char reuse_addr_val;
		vector <Message> messages;
	

	
	public:
		Server();
		void setup();
		void start();	
		void handleConnection(int connection_socket_descriptor);
		static void *readFromClient(void *t_data);
};