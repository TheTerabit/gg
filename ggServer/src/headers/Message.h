#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>


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
		Message(int type, int receiverFd, string content);
		Message(int type, int senderId, int receiverId, string content);
	
		int getType();
		int getSenderId();
		int getReceiverId();
		int getSenderFd();
		int getReceiverFd();
		string getContent();
		void setReceiverId(int receiverId);
};