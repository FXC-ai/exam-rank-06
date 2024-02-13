#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#include <netinet/in.h>
#include <arpa/inet.h>


#define MAGENTA "\033[1;95m"
#define RESET "\033[0m"

#define PORT 8083

void displaySockaddr_in(struct sockaddr_in* addr)
{
	printf(MAGENTA);
	printf("Family : %d\n", addr->sin_family);
	printf("Port : %d\n", ntohs(addr->sin_port));
	printf("IP : %s\n", inet_ntoa(addr->sin_addr));
	printf(RESET);

}


int main()
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

	int socketOutside = socket(AF_INET, SOCK_STREAM, 0);
	printf("Socket Outside created : %d\n", socketOutside);

	struct sockaddr_in addrOutside;
	addrOutside.sin_family = AF_INET;
	addrOutside.sin_port = htons(PORT);
	addrOutside.sin_addr.s_addr = inet_addr("10.11.6.3");

	printf("Socket Outside addrServer : \n");
	displaySockaddr_in(&addrOutside);

	int connect_status = connect(socketOutside, (struct sockaddr*)&addrOutside, sizeof(addrOutside));
	printf("Connect status : %d\n", connect_status);

	int send_status = send(socketOutside, "Hello", 6, 0);
	printf("Send status : %d\n", send_status);

	socketClient = accept(socketServer, (struct sockaddr*)&addrServer, (socklen_t *)&addrServer_len);
	printf("Socket client created : %d\n", socketClient);

	char buf[1024];
	int recv_status = recv(socketClient, buf, 1024, 0);

	printf("Received (%d): %s\n",recv_status, buf);

	close(socketServer);
	close(socketClient);
	close(socketOutside);
	return 0;
}