#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>


using namespace std;

class User
{
	private:
		static int numberOfUsers;
	
		int id;
		int fd;
	
		string username;
		string password;	
		bool online;
	
	public:
		User();
		User(int fd, string username, string password);
	
		int getId();
		void setFd(int fd);
		int getFd();
	
		string getUsername();
		bool validatePassword(string password);
	
		void goOnline();
		void goOffline();
		bool isOnline();
};