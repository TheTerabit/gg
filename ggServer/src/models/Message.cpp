#include "../headers/Message.h"


Message::Message(int senderFd, int receiverFd, string content)
{
	this->senderFd = senderFd;
	this->receiverFd = receiverFd;
	this->content = content;
}


int Message::getSenderFd()
{
	return this->senderFd;
}


int Message::getReceiverFd()
{
	return this->receiverFd;
}


string Message::getContent()
{
	return this->content;
}