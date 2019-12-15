#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "User.h"

using namespace std;

class Message
{
	private:
		//char type;
		int senderFd;
		int receiverFd;
		User sender;
		User receiver;
		string content;
	
	public:
		Message(int senderFd, int receiverFd, string content);
	
		int getSenderFd();
		int getReceiverFd();
		string getContent();
};