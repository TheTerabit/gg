#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

using namespace std;

class Message
{
	private:
		char type;
		int senderFd;
		int receiverFd;
		string content;
	
	public:
		Message();
};