#include "../headers/User.h"

User::User(string username, string password)
{
	this->username = username;
	this->password = password;
}

string User::getUsername()
{
	return this->username;
}


bool User::validatePassword(string password)
{
	if(this->password == password)
		return true;
	else
		return false;
}