#include "../headers/Server.h"


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
		fprintf(stderr, "Erro while assigning ip addres and port for socket...\n");
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


void Server::handleConnection(int connection_socket_descriptor) {
    //function return that creates new thread
    int create_result = 0;

    //thread reference
    pthread_t thread1;

	//create new instance of structure and fill with data that will be send to thread function
	struct thread_data_t *t_data = new thread_data_t;
	(*t_data).clientFd = connection_socket_descriptor;
	
    create_result = pthread_create(&thread1, NULL, Server::readFromClient, (void *)t_data);
    if (create_result){
       printf("Error while creating new thread: %d\n", create_result);
       exit(-1);
    }

	cout << "Created thread - " << connection_socket_descriptor << endl;
}


void* Server::readFromClient(void *t_data)
{
	//free resources when killed
    pthread_detach(pthread_self());
    struct thread_data_t *th_data = (struct thread_data_t*)t_data;

	int readResult = 1;
	char buffer[BUFFER_SIZE];
	
	//reading from client
	while(readResult > 0)
	{
		memset(buffer, 0, sizeof(buffer));
		readResult = read((*th_data).clientFd, buffer, sizeof(buffer));
		
		if(readResult > 0)
		{
			cout << "In thread - " << (*th_data).clientFd << endl;
			cout << "Message: " << buffer;

			string messageHeader = buffer;
			int messageType = buffer[0] - '0'; //type of the message
			int messageSize = atoi(messageHeader.substr(1, messageHeader.length() - 1).c_str()); //size of the message

			string messageBody = getMessageBody((*th_data).clientFd, messageSize);
			
			if(messageType == 0)
			{
				registerUser(messageBody);
			}
			else if(messageType == 1)
			{
				loginUser(messageBody);
			}
			else
			{
				
			}
			
		}
	}
	
	cout << "Killed thread: " << (*th_data).clientFd << endl; 
    pthread_exit(NULL);
}


string Server::getMessageBody(int fd, int size)
{
	char buffer[BUFFER_SIZE];
	int receivedSize = 0;
	int readResult = 1;
	string message;

	while(receivedSize < size && readResult > 0)
	{
		memset(buffer, 0, sizeof(buffer));
		readResult = read(fd, buffer, sizeof(buffer));
		receivedSize += readResult;
		message += buffer;
	}
	
	return message;
}


bool Server::registerUser(string messageBody)
{
	int delimiterIndex = messageBody.find("&");
	
	string username = messageBody.substr(0, delimiterIndex);
	string password = messageBody.substr(delimiterIndex + 1, messageBody.length());
	
	
}


bool Server::loginUser(string messageBody)
{
	
}
