void run_server ()
{

	int socketServer;
	int socketClient;

	struct sockaddr_in addrServer;
	int addrServer_len = sizeof(addrServer);

	socketServer = socket(AF_INET, SOCK_STREAM, 0);
	printf("Socket server created : %d\n", socketServer);	
	

	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(PORT);
	addrServer.sin_addr.s_addr = INADDR_ANY;

	printf("Socket server addrServer : \n");
	displaySockaddr_in(&addrServer);

	int bind_result = bind(socketServer, (struct sockaddr*)&addrServer, sizeof(addrServer));
	printf("Bind result : %d\n", bind_result);

	int listen_result = listen(socketServer, 10);
	printf("Listen result : %d\n", listen_result);

	int connect_status = connect(socketOutside, (struct sockaddr*)&addrOutside, sizeof(addrOutside));
	printf("Connect status : %d\n", connect_status);


	socketClient = accept(socketServer, (struct sockaddr*)&addrServer, (socklen_t *)&addrServer_len);
	printf("Socket client created : %d\n", socketClient);


	char buf[1024];
	int recv_status = recv(socketClient, buf, 1024, 0);

	printf("Received (%d): %s\n",recv_status, buf);

	close(socketServer);
	close(socketClient);

}