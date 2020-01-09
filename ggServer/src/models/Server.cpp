#include "../headers/Server.h"


vector <User> Server::users;
vector <Message> Server::messages;
pthread_mutex_t Server::usersMutex;
pthread_mutex_t Server::messagesMutex;

Server::Server(int serverPort)
{
	this->serverPort = serverPort;
	reuse_addr_val = 1;
	messagesMutex = PTHREAD_MUTEX_INITIALIZER;
	usersMutex = PTHREAD_MUTEX_INITIALIZER;
}


void Server::setup()
{
	int bind_result;
	int listen_result;
	struct sockaddr_in server_address;
	
	memset(&server_address, 0, sizeof(struct sockaddr));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(serverPort);

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
	
	//sending loop
	while(1)
	{
		pthread_mutex_lock(&messagesMutex);
		//if messages vector is not empty
		if(messages.size() > 0)
		{
			//create additional vector for messages ready to send
			vector <Message> messagesToSend;
			vector <int> messagesToRemove;
			
			//check all messages
			for(size_t i = 0; i < messages.size(); i++)
			{
				//if the message is type of response
				if(messages[i].getType() == MESSAGE_TYPE_RESPONSE)
				{
					messagesToSend.push_back(messages[i]);
					messagesToRemove.push_back(i);
				}
				//if message is type of notification
				else if(messages[i].getType() == MESSAGE_TYPE_NOTIFICATIONS)
				{
					pthread_mutex_lock(&usersMutex);
					//add one message of this type for all users to messagesToSend vector
					for(size_t j = 0; j < users.size(); j++)
					{
						//do not send notification to yourself
						if(messages[i].getSenderId() != users[j].getId() && isReceiverOnline(users[j].getId()))
						{
							messages[i].setReceiverId(users[j].getId());
							messagesToSend.push_back(messages[i]);
						}
					}
					pthread_mutex_unlock(&usersMutex);
					messagesToRemove.push_back(i);
				}
				//if not check if receiver is online and add message to vector that contains messages ready to send and delete from the previous one
				else
				{
					pthread_mutex_lock(&usersMutex);
					
					if(isReceiverOnline(messages[i].getReceiverId()) == true)
					{
						messagesToSend.push_back(messages[i]);
						messagesToRemove.push_back(i);
					}
					
					pthread_mutex_unlock(&usersMutex);
				}
			}
			
			//delete messages that has been added to messagesToSend vector
			for(size_t i = 0; i < messagesToRemove.size(); i++)
			{
				messages.erase(messages.begin() + messagesToRemove[i]);
			}
			
			//send all messages in vector messagesToSend
			for(size_t i = 0; i < messagesToSend.size(); i++)
			{
				if(messagesToSend[i].getType() == MESSAGE_TYPE_RESPONSE)
				{
					sendResult = sendResponseMessage(messagesToSend[i].getReceiverFd(), messagesToSend[i].getContent());
					
					if(sendResult == true)
						cout << "Response message has been sent to: " << messagesToSend[i].getReceiverFd() << "(fd) with content: " << messagesToSend[i].getContent() << endl;
					else
						cout << "Sending response message went wrong to: " << messagesToSend[i].getReceiverFd() << "(fd) with content: " << messagesToSend[i].getContent() << endl;
				}
				else
				{
					sendResult = sendMessage(messagesToSend[i].getType(), messagesToSend[i].getSenderId(), messagesToSend[i].getReceiverId(), messagesToSend[i].getContent());
					
					if(sendResult == true)
						cout << "Message has been sent from: " << messagesToSend[i].getSenderId() << " to: " << messagesToSend[i].getReceiverId() << " with content: " << messagesToSend[i].getContent() << endl;
					else 
						cout << "Sending message went wrong from: " << messagesToSend[i].getSenderId() << " to: " << messagesToSend[i].getReceiverId() << " with content: " << messagesToSend[i].getContent() << endl;
				}
			}
		}
		pthread_mutex_unlock(&messagesMutex);
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
			
			//to end connection send message -9
			if(messageType == MESSAGE_TYPE_END_CONNECTION)
			{
				readResult = 0;
			}
			else if(messageType == MESSAGE_TYPE_REGISTER)
			{
				messageBody = getMessageBody((*th_data).clientFd, messageSize);
				int newUserId = registerUser((*th_data).clientFd, messageBody);
				
				//if there is not error
				if(newUserId != -1) 
				{
					loggedUserId = newUserId;
					
					userGoOnlineById(loggedUserId);
					createResponseMessage(MESSAGE_TYPE_REGISTER, (*th_data).clientFd, SUCCESS);
					createMessageForNewClient(loggedUserId);
					createNotificationMessageUserStatus(loggedUserId, USER_ONLINE);
				}
				else
				{
					createResponseMessage(MESSAGE_TYPE_REGISTER, (*th_data).clientFd, ERROR);
					cout << "User already exists." << endl;
				}
			}
			else if(messageType == MESSAGE_TYPE_LOGIN)
			{
				messageBody = getMessageBody((*th_data).clientFd, messageSize);
				int newUserId = loginUser((*th_data).clientFd, messageBody);
				
				if(newUserId != -1)
				{
					loggedUserId = newUserId;
					
					userGoOnlineById(loggedUserId);
					createResponseMessage(MESSAGE_TYPE_LOGIN, (*th_data).clientFd, SUCCESS);
					createMessageForNewClient(loggedUserId);
					createNotificationMessageUserStatus(loggedUserId, USER_ONLINE);
				}
				else
				{
					createResponseMessage(MESSAGE_TYPE_LOGIN, (*th_data).clientFd, ERROR);
					cout << "Wrong username or password." << endl;
				}
			}
			else if(messageType == MESSAGE_TYPE_CLIENT_CLIENT)
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
	
	if(loggedUserId != -1)
	{
		userGoOfflineById(loggedUserId);
		createNotificationMessageUserStatus(loggedUserId, USER_OFFLINE);
	}
	
    pthread_exit(NULL);
}


int Server::registerUser(int clientFd, string messageBody)
{
	int loggedUserId = -2;
	int delimiterIndex = messageBody.find("&");
	
	string username = messageBody.substr(0, delimiterIndex);
	string password = messageBody.substr(delimiterIndex + 1, messageBody.length());
	
	pthread_mutex_lock(&usersMutex);
	
	//checks for existing username
	for(size_t i = 0; i < users.size(); i++)
		if(users[i].getUsername() == username)
			 loggedUserId = -1;	
	
	if(loggedUserId != -1)
	{
		//push new user to vector and assign his id to variable
		users.push_back(User(clientFd, username, password));
		loggedUserId = users[users.size() - 1].getId();	
	}
	
	pthread_mutex_unlock(&usersMutex);
	
	cout << "User has been created with login: " << username << " and password: " << password << endl;
	return loggedUserId;
}


int Server::loginUser(int clientFd, string messageBody)
{
	int loggedUserId = -1;
	int delimiterIndex = messageBody.find("&");
	
	string username = messageBody.substr(0, delimiterIndex);
	string password = messageBody.substr(delimiterIndex + 1, messageBody.length());
	
	pthread_mutex_lock(&usersMutex);
	for(size_t i = 0; i < users.size(); i++)
	{
		if(users[i].getUsername() == username && users[i].validatePassword(password))
		{
			users[i].setFd(clientFd); //change user's fd
			loggedUserId = users[i].getId();
			
			cout << "User has logged in with login: " << username << " and password: " << password << endl;
			
			break;
		}
	}
	pthread_mutex_unlock(&usersMutex);
	
	return loggedUserId;
}


void Server::createResponseMessage(int type, int clientFd, int result)
{
	pthread_mutex_lock(&messagesMutex);

	string content = to_string(type) + "&" + to_string(result);
	messages.push_back(Message(MESSAGE_TYPE_RESPONSE, clientFd, content));
	
	pthread_mutex_unlock(&messagesMutex);
}


void Server::createMessageForNewClient(int loggedUserId)
{
	string messageContent = "";
	
	pthread_mutex_lock(&usersMutex);
	for(size_t i = 0; i < users.size(); i++)
	{
		if(users[i].getId() != loggedUserId)
			messageContent += "&" + to_string(users[i].getId()) + "%" + users[i].getUsername() + "%" + to_string(users[i].isOnline());
	}
	pthread_mutex_unlock(&usersMutex);
	
	if(messageContent != "")
	{
		messageContent.erase(messageContent.begin());
		
		pthread_mutex_lock(&messagesMutex);
		messages.push_back(Message(MESSAGE_TYPE_SERVER_CLIENT, SERVER_ID, loggedUserId, messageContent));
		pthread_mutex_unlock(&messagesMutex);
	}
}


bool Server::createMessageToSend(int loggedUserId, string messageBody)
{
	int delimiterIndex = messageBody.find("&");
	
	int receiverId = atoi(messageBody.substr(0, delimiterIndex).c_str());
	int senderId = loggedUserId;
	string messageContent = messageBody.substr(delimiterIndex + 1, messageBody.length());
	
	pthread_mutex_lock(&messagesMutex);
	messages.push_back(Message(MESSAGE_TYPE_CLIENT_CLIENT, senderId, receiverId, messageContent));
	pthread_mutex_unlock(&messagesMutex);
	
	cout << "Message has been been created from: " << senderId << " to: " << receiverId << " content: " << messageContent << endl;
	
	return true;
}


bool Server::sendMessage(int messageType, int senderId, int receiverId, string content)
{
	pthread_mutex_lock(&usersMutex);
	int receiverFd = getUserFdById(receiverId);
	pthread_mutex_unlock(&usersMutex);
	
	if(receiverFd == -1) return false;
	
	string bodyMessage = to_string(senderId) + "&" + content + "\n";
	string headerMessage = to_string(messageType) + "&" + to_string(bodyMessage.length() - 1) + "\n";
	
	//send message with type and size of the message
	//bufferHeader contains type of the message, sign & and length of the incoming message
	char bufferHeader[headerMessage.length() + 1];
	strcpy(bufferHeader, headerMessage.c_str());
	
	int writeHeaderResult = write(receiverFd, bufferHeader, sizeof(bufferHeader));
	if(writeHeaderResult == -1) return false;
	
	//send message with content
	char bufferBody[bodyMessage.length() + 1];
	strcpy(bufferBody, bodyMessage.c_str());
	
	int writeBodyResult = write(receiverFd, bufferBody, sizeof(bufferBody));
	
	if(writeBodyResult == -1) return false;
	else return true;
}


bool Server::sendResponseMessage(int receiverFd, string content)
{
	content = content + "\n";
	char buffer[content.length() + 1];
	strcpy(buffer, content.c_str());
	
	int sendMessageResult = write(receiverFd, buffer, sizeof(buffer));
	
	return sendMessageResult;
}


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


void Server::createNotificationMessageUserStatus(int userId, int userStatus)
{
	pthread_mutex_lock(&usersMutex);
	string notificationContent = getUsernameById(userId) + "%" + to_string(userStatus);
	pthread_mutex_unlock(&usersMutex);
	
	pthread_mutex_lock(&messagesMutex);
	messages.push_back(Message(MESSAGE_TYPE_NOTIFICATIONS, userId, -1, notificationContent));
	pthread_mutex_unlock(&messagesMutex);
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


string Server::getUsernameById(int userId)
{	
	for(size_t i = 0; i < users.size(); i ++)
		if(users[i].getId() == userId)
			return users[i].getUsername();
		
	return "";
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
	pthread_mutex_lock(&usersMutex);
	
	for(size_t i = 0; i < users.size(); i++)
	{
		if(users[i].getId() == userId)
			users[i].goOnline();
	}
	
	pthread_mutex_unlock(&usersMutex);
}


void Server::userGoOfflineById(int userId)
{
	pthread_mutex_lock(&usersMutex);
	
	for(size_t i = 0; i < users.size(); i++)
	{
		if(users[i].getId() == userId)
			users[i].goOffline();
	}
	
	pthread_mutex_unlock(&usersMutex);
}