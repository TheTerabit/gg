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
#define BUFFER_SIZE 50

using namespace std;

struct thread_data_t
{
	int clientFd;
};

class Server
{
	private:
		int server_socket_descriptor;
		char reuse_addr_val;
		vector <Message> messages;
	
		void handleConnection(int connection_socket_descriptor);
		static void *readFromClient(void *t_data);
		static string getMessageBody(int fd, int size);
		static bool registerUser(string messageBody);
		static bool loginUser(string messageBody);
	

	
	public:
		Server();
		void setup();
		void start();		
};