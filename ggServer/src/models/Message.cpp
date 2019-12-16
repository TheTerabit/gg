#include "../headers/Message.h"


Message::Message(int type, int senderId, int receiverId, string content)
{
	this->type = type;
	this->senderId = senderId;
	this->receiverId = receiverId;
	this->content = content;
}


int Message::getType()
{
	return this->type;
}


int Message::getSenderId()
{
	return this->senderId;
}


int Message::getReceiverId()
{
	return this->receiverId;
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


void Message::setReceiverId(int receiverId)
{
	this->receiverId = receiverId;
}