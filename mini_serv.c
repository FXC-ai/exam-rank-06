#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#include <string.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define MAGENTA "\033[1;95m"
#define RED "\033[1;91m"
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

void cpy_fd_set(fd_set* dest, fd_set* src)
{
	for (int i = 0; i < 1024; i++)
	{
		if (FD_ISSET(i, src))
		{
			FD_SET(i, dest);
		}
	}
}

void display_fd_set(char *title, fd_set* fd_set)
{
	printf("fd_set : %s :", title);
	printf(MAGENTA);
	for (int i = 0; i < 1024; i++)
	{
		if (FD_ISSET(i, fd_set))
		{
			printf(" %d ", i);
		}
	}
	printf(RESET);
	printf("\n");
}

void run_server ()
{
	int socketServer;
	int socketClient;

	socketClient = 0;

	int max_sd;
	char buf[1024];

	int recv_status;
	int send_status;

	fd_set readfds;
	fd_set writefds;

	fd_set cpy_readfds;
	fd_set cpy_writefds;

	FD_ZERO(&readfds);
	FD_ZERO(&writefds);

	FD_ZERO(&cpy_readfds);
	FD_ZERO(&cpy_writefds);

	struct sockaddr_in addrServer;
	int addrServer_len = sizeof(addrServer);

	memset(&addrServer, 0, addrServer_len);

	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(PORT);
	addrServer.sin_addr.s_addr = INADDR_ANY;

	printf("Socket server addrServer : \n");
	displaySockaddr_in(&addrServer);

	socketServer = socket(AF_INET, SOCK_STREAM, 0);
	printf("Socket server created : %d\n", socketServer);	
	
	FD_SET(socketServer, &readfds);


	int bind_result = bind(socketServer, (struct sockaddr*)&addrServer, sizeof(addrServer));
	if (bind_result < 0)
	{
		perror("bind() failed");
		printf("Bind result : %d\n", bind_result);
		return;
	}
	int listen_result = listen(socketServer, 10);
	printf("Listen result : %d\n", listen_result);



	while (42)
	{
		cpy_fd_set(&readfds, &cpy_readfds);
		cpy_fd_set(&writefds, &cpy_writefds);

		max_sd = socketServer;

		for (int i = 0; i < 1024; i++)
		{
			if (FD_ISSET(i, &readfds) && i > max_sd)
			{
				max_sd = i;
			}
			if (FD_ISSET(i, &writefds) && i > max_sd)
			{
				max_sd = i;
			}
		}
		printf(RED);
		printf("max_sd : %d\n", max_sd);
		printf(RESET);
		printf(MAGENTA);
		printf("Before select\n");
		printf(RESET);
		/* select() Delete from readfds and writefds all the sockets not "ready" for an I/O operation. */
		/* Mais c est pas parceque le socket n est pas ready qu il faut le supprimer de readfds et writefds */
		display_fd_set("    readfds", &readfds);
		display_fd_set("cpy_readfds", &cpy_readfds);
		display_fd_set("    writefds", &writefds);
		display_fd_set("cpy_writefds", &cpy_writefds);
		
		if (select(max_sd + 1, &readfds, &writefds, NULL, NULL) > 0)
		{
			printf(MAGENTA);
			printf("After select\n");
			printf(RESET);
			display_fd_set("    readfds", &readfds);
			display_fd_set("cpy_readfds", &cpy_readfds);
			display_fd_set("    writefds", &writefds);
			display_fd_set("cpy_writefds", &cpy_writefds);


			printf("Something happened in select\n");

			for (int i = 0; i < max_sd + 1; i++)
			{
				// printf("i : %d\n", i);
				if (FD_ISSET(i, &readfds) && i == socketServer)
				{
					socketClient = accept(socketServer, (struct sockaddr*)&addrServer, (socklen_t *)&addrServer_len);
					printf("Socket client created : %d\n", socketClient);
					FD_SET(socketClient, &readfds);
					FD_SET(socketClient, &cpy_readfds);
				}
				else
				{
					printf("Socket %d need to be read or written %d \n", i, FD_ISSET(i, &readfds));
				
					if (FD_ISSET(i, &readfds))
					{
						printf("MAIS PUTAIN\n");
						recv_status = recv(socketClient, buf, 1024, 0);
						printf("recv_status : %d\n", recv_status);
						// if (recv_status == 0)
						// {
						// 	printf("Client %d disconnected\n", i);
						// 	close(i);
						// 	FD_CLR(i, &readfds);
						// 	FD_CLR(i, &cpy_readfds);
						// 	FD_CLR(i, &writefds);
						// 	FD_CLR(i, &cpy_writefds);
						// }
						// else if (recv_status < 0)
						// {
						// 	printf("Error while receiving\n");
						// 	close(i);
						// 	FD_CLR(i, &readfds);
						// 	FD_CLR(i, &cpy_readfds);
						// 	FD_CLR(i, &writefds);
						// 	FD_CLR(i, &cpy_writefds);
						// }
						if (recv_status > 0)
						{
							printf("Received (%d): %s from %d\n",recv_status, buf, i);
							printf("Socket %d is in readfds\n", i);
						}


						//FD_CLEAR(i, &readfds);
					}
					if (FD_ISSET(i, &writefds))
					{
						send_status = send(socketClient, "MSG", 3, 0);
						printf("Socket %d is in writefds\n", i);
					}
				}
			}
		}
		
	}
	close(socketServer);
	close(socketClient);
}


int main()
{
	
	printf("--------------Server start-------------------\n");
	run_server();


	return 0;
}