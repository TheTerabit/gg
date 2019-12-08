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
		int sender;
		int receiver;
		string content;
	
	public:
		Message();
};