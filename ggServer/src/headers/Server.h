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

#define MESSAGE_TYPE_REGISTER 0
#define MESSAGE_TYPE_LOGIN 1
#define MESSAGE_TYPE_SEND 2

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
		static vector <Message> messages;
		static vector <User> users;
	
		//handle connection thread	
		void handleConnection(int connection_socket_descriptor);
	
		//sending messages to all clients thread and its methods
		static void *sendToAllClientsThread(void *t_data);
		static bool sendMessage(int senderFd, int receiverFd, string content);
		
		//reading from single client thread and its methods
		static void *readFromSingleClientThread(void *t_data);
		static string getMessageBody(int clienFd, int size);
		static bool registerUser(int clientFd, string messageBody, User* &loggedUser);
		static bool loginUser(int clientFd, string messageBody, User* &loggedUser);
		static bool createMessageToSend(string messageContent, User* &loggedUser);
	
	
	public:
		Server();
		void setup();
		void start();
};