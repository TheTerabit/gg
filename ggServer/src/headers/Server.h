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

#include "User.h"
#include "Message.h"

#define SERVER_PORT 1234
#define QUEUE_SIZE 5
#define BUFFER_SIZE 50
#define SERVER_ID -1

#define MESSAGE_TYPE_REGISTER 0
#define MESSAGE_TYPE_LOGIN 1
#define MESSAGE_TYPE_CLIENT_CLIENT 2
#define MESSAGE_TYPE_SERVER_CLIENT 3
#define MESSAGE_TYPE_FOR_NEW_CLIENT 4
#define MESSAGE_TYPE_NOTIFICATIONS 5
#define USER_ONLINE 1
#define USER_OFFLINE 0


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
		static pthread_mutex_t messagesMutex;
		static pthread_mutex_t usersMutex;
	
		//handle connection thread	
		void handleConnection(int connection_socket_descriptor);
	
		//sending messages to all clients thread and its methods
		static void *sendToAllClientsThread(void *t_data);
		static bool isReceiverOnline(int receiverId);
		static bool sendMessage(int messageType, int senderFd, int receiverFd, string content);
		static int getUserFdById(int userId);
		static void createMessageForNewClient(int loggedUserId);
		static void createNotificationMessageUserStatus(int userId, int userStatus);
		static string getUsernameById(int userId);

		
		//reading from single client thread and its methods
		static void *readFromSingleClientThread(void *t_data);
		static string getMessageBody(int clienFd, int size);
		static int registerUser(int clientFd, string messageBody);
		static int loginUser(int clientFd, string messageBody);
		static bool createMessageToSend(int loggedUserId, string messageContent);
		static void userGoOnlineById(int userId);
		static void userGoOfflineById(int userId);
	
	
	public:
		Server();
		void setup();
		void start();
};