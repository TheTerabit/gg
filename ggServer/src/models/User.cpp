#include "../headers/User.h"


int User::numberOfUsers = 0;


User::User()
{
	this->id = numberOfUsers;
	numberOfUsers++;
}


User::User(int fd, string username, string password)
{
	this->username = username;
	this->password = password;
	this->fd = fd;
	
	this->id = numberOfUsers;
	numberOfUsers++;
	
	cout << "USER ID: " << numberOfUsers << endl;
}


int User::getId()
{
	return this->id;
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


void User::setFd(int fd)
{
	this->fd = fd;
}


int User::getFd()
{
	return this->fd;
}


void User::goOnline()
{
	this->online = true;
}


void User::goOffline()
{
	this->online = false;
}


bool User::isOnline()
{
	return this->online;
}