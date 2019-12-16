#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#define MESSAGE_CLIENT_CLIENT 0
#define NOTIFICATION_CLIENT_ALL_CLIENTS 1


using namespace std;

class Message
{
	private:
		int type; //0 - message from client to client || 1 - notification sending to all clients
		int senderFd;
		int receiverFd;
		int senderId;
		int receiverId;
		string content;
	
	public:
		Message(int type, int senderFd, int receiverFd, string content);
	
		int getType();
		int getSenderId();
		int getReceiverId();
		int getSenderFd();
		int getReceiverFd();
		string getContent();
};