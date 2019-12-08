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
	(*t_data).x = connection_socket_descriptor;
	
    create_result = pthread_create(&thread1, NULL, Server::readFromClient, (void *)t_data);
    if (create_result){
       printf("Error while creating new thread: %d\n", create_result);
       exit(-1);
    }

	cout << "Po utworzeniu procesu - " << connection_socket_descriptor << endl;

    //TODO (przy zadaniu 1) odbieranie -> wyświetlanie albo klawiatura -> wysyłanie
}


void* Server::readFromClient(void *t_data)
{
	//free resources when killed
    pthread_detach(pthread_self());
    struct thread_data_t *th_data = (struct thread_data_t*)t_data;
    //dostęp do pól struktury: (*th_data).pole
    //TODO (przy zadaniu 1) klawiatura -> wysyłanie albo odbieranie -> wyświetlanie
	char buf[50];
	
	while(1)
	{
		cout << "W procesie - " << (*th_data).x << endl;
		sleep(1);
	}
	
	while(1)
	{
		read((*th_data).x, buf, 50);
		cout << (*th_data).x << endl;	
	}
	
    pthread_exit(NULL);
}
