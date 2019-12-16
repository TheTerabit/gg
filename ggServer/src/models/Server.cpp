#include "../headers/Server.h"


vector <User> Server::users;
vector <Message> Server::messages;


Server::Server()
{
	reuse_addr_val = 1;
}


void Server::setup()
{
	int bind_result;
	int listen_result;
	struct sockaddr_in server_address;
	
	memset(&server_address, 0, sizeof(struct sockaddr));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(SERVER_PORT);

	//create socket
	server_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket_descriptor < 0)
	{
		fprintf(stderr, "Error while creating a socket...\n");
		exit(1);
	}
	setsockopt(server_socket_descriptor, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse_addr_val, sizeof(reuse_addr_val));

	//bind socket
	bind_result = bind(server_socket_descriptor, (struct sockaddr*)&server_address, sizeof(struct sockaddr));
	if (bind_result < 0)
	{
		fprintf(stderr, "Error while assigning ip addres and port for socket...\n");
		exit(1);
	}

	//set mode to listen
	listen_result = listen(server_socket_descriptor, QUEUE_SIZE);
	if (listen_result < 0) {
		fprintf(stderr, "Error while setting up queue size...\n");
		exit(1);
	}
}


void Server::start()
{
	//function return that creates new thread
    int create_result = 0;

    //thread reference
    pthread_t thread1;
	
    create_result = pthread_create(&thread1, NULL, Server::sendToAllClientsThread, (void *) 0);
    if (create_result){
       printf("Error while creating sender thread: %d\n", create_result);
       exit(-1);
    }
	cout << "Created sender thread." << endl;
	
	
	int connection_socket_descriptor;	
	//main loop
	while(1)
	{
		//accept new connection
		connection_socket_descriptor = accept(server_socket_descriptor, NULL, NULL);
		if (connection_socket_descriptor < 0)
		{
			fprintf(stderr, "Error while accepting new connection...\n");
			exit(1);
		}
		
		//handle new connection
		handleConnection(connection_socket_descriptor);
	}

	close(server_socket_descriptor);
}


void Server::handleConnection(int connection_socket_descriptor) 
{
    //function return that creates new thread
    int create_result = 0;

    //thread reference
    pthread_t thread1;

	//create new instance of structure and fill with data that will be send to thread function
	struct thread_data_t *t_data = new thread_data_t;
	(*t_data).clientFd = connection_socket_descriptor;
	
    create_result = pthread_create(&thread1, NULL, Server::readFromSingleClientThread, (void *)t_data);
    if (create_result){
       printf("Error while creating new thread: %d\n", create_result);
       exit(-1);
    }

	cout << "Created thread: " << connection_socket_descriptor << endl;
}


void* Server::sendToAllClientsThread(void *t_data)
{
	//free resources when killed
    pthread_detach(pthread_self());
	bool sendResult;
	
	while(1)
	{
		if(messages.size() > 0)
		{
			vector <Message> messagesToSend;
			
			for(size_t i = 0; i < messages.size(); i++)
			{
				if(isReceiverOnline(messages[i].getReceiverId()) == true)
				{
					messagesToSend.push_back(messages[i]);
					messages.erase(messages.begin() + i);
				}
			}
			
			for(size_t i = 0; i < messagesToSend.size(); i++)
			{
				sendResult = sendMessage(messagesToSend[i].getSenderId(), messages[i].getReceiverId(), messages[i].getContent());
				
				if(sendResult == true)
				{
					cout << "Message has been sent from: " << messagesToSend[i].getSenderId() << " to: " << messagesToSend[i].getReceiverId() << " with content: " << messagesToSend[i].getContent() << endl;
					
					//remove message from vector when has been sent
					messagesToSend.erase(messagesToSend.begin() + i);
				}
				else cout << "Sending message went wrong from: " << messagesToSend[i].getSenderId() << " to: " << messagesToSend[i].getReceiverId() << " with content: " << messagesToSend[i].getContent() << endl;
			}
		}
	}

	cout << "Killed sender thread." << endl; 
    pthread_exit(NULL);
}


void* Server::readFromSingleClientThread(void *t_data)
{
	//free resources when killed
    pthread_detach(pthread_self());
    struct thread_data_t *th_data = (struct thread_data_t*)t_data;

	int loggedUserId = -1;
	int readResult = 1;
	char buffer[BUFFER_SIZE];
	string messageBody;
	
	//reading from client
	while(readResult > 0)
	{
		memset(buffer, 0, sizeof(buffer));
		readResult = read((*th_data).clientFd, buffer, sizeof(buffer));
		
		if(readResult > 0)
		{
			cout << "In thread: " << (*th_data).clientFd << endl;
			cout << "Message: " << buffer;

			string messageHeader = buffer; //buffer to string
			int delimiterIndex = messageHeader.find("&"); //split message by &
			int messageType = atoi(messageHeader.substr(0, delimiterIndex).c_str()); //type of the message
			int messageSize = atoi(messageHeader.substr(delimiterIndex + 1, messageHeader.length()).c_str()); //size of the message
			
			if(messageType == MESSAGE_TYPE_REGISTER)
			{
				messageBody = getMessageBody((*th_data).clientFd, messageSize);
				loggedUserId = registerUser((*th_data).clientFd, messageBody);
			}
			else if(messageType == MESSAGE_TYPE_LOGIN)
			{
				messageBody = getMessageBody((*th_data).clientFd, messageSize);
				loggedUserId = loginUser((*th_data).clientFd, messageBody);
			}
			else if(messageType == MESSAGE_TYPE_SEND)
			{
				if(loggedUserId != -1)
				{
					messageBody = getMessageBody((*th_data).clientFd, messageSize);
					createMessageToSend(loggedUserId, messageBody);
				}
				else
					cout << "You must be logged in to send messages." << endl;
			}
		}
	}
	
	cout << "Killed thread: " << (*th_data).clientFd << endl; 
    pthread_exit(NULL);
}


int Server::registerUser(int clientFd, string messageBody)
{
	int delimiterIndex = messageBody.find("&");
	
	string username = messageBody.substr(0, delimiterIndex);
	string password = messageBody.substr(delimiterIndex + 1, messageBody.length());
	
	//checks for existing username
	for(size_t i = 0; i < users.size(); i++)
		if(users[i].getUsername() == username)
		{
			cout << "User already exists: " << username << endl;
			return -1;	
		}
	
	//push new user to vector and assign his id to variable
	users.push_back(User(clientFd, username, password));
	int loggedUserId = users[users.size() - 1].getId();
	userGoOnlineById(loggedUserId);
	
	cout << "User has been created with login: " << username << " and password: " << password << endl;
	return loggedUserId;
}


int Server::loginUser(int clientFd, string messageBody)
{
	int delimiterIndex = messageBody.find("&");
	
	string username = messageBody.substr(0, delimiterIndex);
	string password = messageBody.substr(delimiterIndex + 1, messageBody.length());
	
	for(size_t i = 0; i < users.size(); i++)
	{
		if(users[i].getUsername() == username && users[i].validatePassword(password))
		{
			users[i].setFd(clientFd); //change user's fd
			int loggedUserId = users[i].getId();
			userGoOnlineById(loggedUserId);
			
			cout << "User has logged in with login: " << username << " and password: " << password << endl;
			
			return loggedUserId;
		}
	}
	
	cout << "Wrong username or password." << endl;
	return -1;
}


bool Server::createMessageToSend(int loggedUserId, string messageBody)
{
	int delimiterIndex = messageBody.find("&");
	
	int receiverId = atoi(messageBody.substr(0, delimiterIndex).c_str());
	int senderId = loggedUserId;
	string messageContent = messageBody.substr(delimiterIndex + 1, messageBody.length());
	
	messages.push_back(Message(MESSAGE_CLIENT_CLIENT, senderId, receiverId, messageContent));
	
	cout << "Message has been been created from: " << senderId << " to: " << receiverId << " content: " << messageContent << endl;
	
	return true;
}


bool Server::sendMessage(int senderId, int receiverId, string content)
{
	int receiverFd = getUserFdById(receiverId);
	if(receiverFd == -1) return false;
	
	string bodyMessage = to_string(senderId) + "&" + content;
	string headerMessage = "2" + to_string(bodyMessage.length());
	
	//send message with type and size of the message
	//bufferHeader contains type of the message, sign & and length of the incoming message
	char bufferHeader[headerMessage.length() + 1];
	strcpy(bufferHeader, headerMessage.c_str());
	cout << bufferHeader << endl;
	
	int writeHeaderResult = write(receiverFd, bufferHeader, sizeof(bufferHeader));
	if(writeHeaderResult == -1) return false;
	
	//send message with content
	char bufferBody[bodyMessage.length() + 1];
	strcpy(bufferBody, bodyMessage.c_str());
	
	int writeBodyResult = write(receiverFd, bufferBody, sizeof(bufferBody));
	
	if(writeBodyResult == -1) return false;
	else return true;
}


bool Server::sendNotificationUserOnline()


string Server::getMessageBody(int clientFd, int size)
{
	char buffer[BUFFER_SIZE];
	int receivedSize = 0;
	int readResult = 1;
	string message;

	//read until received size is smaller than size of the message from client
	while(receivedSize < size && readResult > 0)
	{
		memset(buffer, 0, sizeof(buffer));
		readResult = read(clientFd, buffer, sizeof(buffer));
		receivedSize += readResult;
		message += buffer;
	}
	
	return message;
}


bool Server::isReceiverOnline(int receiverId)
{
	for(size_t i = 0; i < users.size(); i++)
	{
		if(users[i].getId() == receiverId && users[i].isOnline() == true)
			return true;
	}
	
	return false;
}


int Server::getUserFdById(int userId)
{
	for(size_t i = 0; i < users.size(); i ++)
	{
		if(users[i].getId() == userId)
			return users[i].getFd();
	}
	
	return -1;
}


void Server::userGoOnlineById(int userId)
{
	for(size_t i = 0; i < users.size(); i++)
	{
		if(users[i].getId() == userId)
			users[i].goOnline();
	}
	
}


void Server::userGoOfflineById(int userId)
{
	for(size_t i = 0; i < users.size(); i++)
	{
		if(users[i].getId() == userId)
			users[i].goOffline();
	}
}