#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>


using namespace std;

class User
{
	private:
		string username;
		string password;
	
	public:
		User(string username, string password);
		string getUsername();
		bool validatePassword(string password);
};