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
#define GREEN "\033[1;92m"
#define RESET "\033[0m"

// #include "test.h"

#include <stdlib.h>

void display_clients(int *clients)
{
	printf(MAGENTA);
	printf("Clients : ");
	printf(RESET);

	for (int i = 0; i < 1024; i++)
	{
		if (clients[i]!= 0)
		{	
			printf(" %d : %d | ", i, clients[i]);
		}
	}
	printf("\n");
}

int find_id_from_socket(int *clients, int sock)
{
	for (int i = 0; i < 1024; i++)
	{
		if (clients[i] == sock)
		{
			return i;
		}
	}
	return -1;
}

void displaySockaddr_in(struct sockaddr_in* addr)
{
	printf(MAGENTA);
	printf("Family : %d\n", addr->sin_family);
	printf("Port : %d\n", ntohs(addr->sin_port));
	printf("IP : %s\n", inet_ntoa(addr->sin_addr));
	printf(RESET);

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





void send_to_all (int socketServer, int socketSender, char *msg, int max_sd)
{
	for (int j = 0; j < max_sd + 1; j++)
	{
		if ( j != socketServer && j!= socketSender)
		{
			send(j, msg, strlen(msg), 0);
		}
	}
}

void fatal_error()
{
	write(2, "Fatal error\n", 14);
}

int main(int argc, char **argv)
{
	int port;

	if (argc!= 2)
	{
		write(2, "Wrong number of arguments\n", 26);
		return 1;
	}
	else
	{
		port = atoi(argv[1]);
	}

	printf("--------------Server start-------------------\n");

	/* cients[id] = sock */
	int clients[1024];

	memset(clients, 0, sizeof(clients));

	int socketServer;
	int socketClient;

	socketClient = 0;

	int max_sd;
	char buf[4096];

	int recv_status;
	// int send_status;

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
	addrServer.sin_port = htons(port);
	addrServer.sin_addr.s_addr = INADDR_ANY;

	printf("Socket server addrServer : \n");
	displaySockaddr_in(&addrServer);

	socketServer = socket(AF_INET, SOCK_STREAM, 0);
	if (socketServer < 0)
	{
		fatal_error();
		return -1;
	}
	printf("Socket server created : %d\n", socketServer);	
	
	FD_SET(socketServer, &cpy_readfds);


	int bind_result = bind(socketServer, (struct sockaddr*)&addrServer, sizeof(addrServer));
	if (bind_result < 0)
	{
		fatal_error();
		return 1;
	}

	int listen_result = listen(socketServer, 10);
	if (listen_result < 0)
	{
		fatal_error();
		return 1;
	}
	printf("Listen result : %d\n", listen_result);

	int last_id = 0;

	while (42)
	{
		readfds = cpy_readfds;
		writefds = cpy_writefds;


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

		display_fd_set("     readfds", &readfds);
		display_fd_set(" cpy_readfds", &cpy_readfds);

		int result_select = select(max_sd + 1, &readfds, &writefds, NULL, NULL);
		
		
		if (result_select > 0)
		{

			printf(MAGENTA);
			printf("something happend with select : %d\n", result_select);
			printf(RESET);
			display_fd_set("     readfds", &readfds);
			display_fd_set(" cpy_readfds", &cpy_readfds);

			for (int i = 0; i < max_sd + 1; i++)
			{
				printf("i : %d\n", i);
				if (FD_ISSET(i, &readfds) && i == socketServer)
				{
					socketClient = accept(socketServer, (struct sockaddr*)&addrServer, (socklen_t *)&addrServer_len);
					printf("Socket client created : %d\n", socketClient);

					FD_SET(socketClient, &cpy_readfds);
					clients[last_id] = socketClient;
					++last_id;

					printf("last_id : %d\n", last_id);

					display_clients(clients);

					sprintf(buf, "server: client %d just arrived\n", find_id_from_socket(clients, socketClient));

					send_to_all(socketServer, socketClient, buf, max_sd);

					memset(buf, 0, sizeof(buf));

				}
				else
				{

					if (FD_ISSET(i, &readfds))
					{
						memset(buf, 0, sizeof(buf));

						recv_status = recv(i, buf, 4096, 0);

						printf("recv_status : %d\n", recv_status);
						if (recv_status == 0)
						{
						 	printf("Client %d disconnected\n", i);
						 	close(i);

						 	FD_CLR(i, &cpy_readfds);
							
							sprintf(buf, "server: client %d just left\n", find_id_from_socket(clients, i));
							
							send_to_all(socketServer, i, buf, max_sd);
							clients[find_id_from_socket(clients, i)] = 0;
							display_clients(clients);
						}
						else if (recv_status > 0)
						{
							display_clients(clients);

							printf(GREEN);
							printf("Received (%d): %s from %d\n",recv_status, buf, find_id_from_socket(clients, i));
							printf(RESET);

							char msg[4128];
							memset(msg, 0, sizeof(msg));
							sprintf(msg, "client %d : %s", find_id_from_socket(clients, i), buf);

							send_to_all(socketServer, i, msg, max_sd);

							printf("Socket %d is in readfds\n", i);
						}
						else if (recv_status < 0)
						{
						 	close(i);
						 	FD_CLR(i, &cpy_readfds);

							sprintf(buf, "server: client %d just left\n", find_id_from_socket(clients, i));

							send_to_all(socketServer, i, buf, max_sd);			
							clients[find_id_from_socket(clients, i)] = 0;

						}
					}
				}
			}
		}
	}
	close(socketServer);
	//close(socketClient);
 
	return 0;

	// https://www.youtube.com/watch?v=OxErBK-I2E4
}