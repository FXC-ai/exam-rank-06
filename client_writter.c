#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>

#define MAGENTA "\033[1;95m"
#define RESET "\033[0m"

#include "test.h"

void displaySockaddr_in(struct sockaddr_in* addr)
{
	printf(MAGENTA);
	printf("Family : %d\n", addr->sin_family);
	printf("Port : %d\n", ntohs(addr->sin_port));
	printf("IP : %s\n", inet_ntoa(addr->sin_addr));
	printf(RESET);

}

int main ()
{
	//char buf[1024];

	int socketOutside = socket(AF_INET, SOCK_STREAM, 0);
	printf("Socket Outside created : %d\n", socketOutside);

	struct sockaddr_in addrOutside;
	addrOutside.sin_family = AF_INET;
	addrOutside.sin_port = htons(PORT);
	addrOutside.sin_addr.s_addr = inet_addr("127.0.0.1");

	printf("Socket Outside addrServer : \n");
	displaySockaddr_in(&addrOutside);

	int connect_status = connect(socketOutside, (struct sockaddr*)&addrOutside, sizeof(addrOutside));
	printf("Connect status : %d\n", connect_status);

	while (42)
	{
		printf("Message to send : ");
		sleep(5);
		int send_status = send(socketOutside, "Les cousins il viennent de partout. Putain j ai peur.", 1024, 0);
		printf("Send status : %d\n", send_status);
	}

	// int recv_status = recv(socketOutside, buf, 1024, 0);
	// printf("Recv status : %d\n", recv_status);

	close(socketOutside);

	return 0;
}