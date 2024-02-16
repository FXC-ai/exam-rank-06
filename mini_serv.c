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

#include "test.h"

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

void send_to_all (int socketServer, int socketSender, int max_sd, char *buf)
{
	int send_status;

	for (int j = 0; j < max_sd + 1; j++)
	{
		if ( j != socketServer && j!= socketSender)
		{
			printf("Envoie du message sur la socket %d \n", j);
			send_status = send(j, buf, recv_status, 0);
			(void) send_status;
			//printf("send_status = %d\n", send_status);
		}
	}
}


void run_server ()
{

	/* cients[id] = sock */
	int clients[1024];

	memset(clients, 0, sizeof(clients));

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
	
	FD_SET(socketServer, &cpy_readfds);


	int bind_result = bind(socketServer, (struct sockaddr*)&addrServer, sizeof(addrServer));
	if (bind_result < 0)
	{
		perror("bind() failed");
		printf("Bind result : %d\n", bind_result);
		return;
	}
	int listen_result = listen(socketServer, 10);
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
			printf("something happend with select select : %d\n", result_select);
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

					display_clients(clients);

			
				}
				else
				{

					if (FD_ISSET(i, &readfds))
					{
						
						memset(buf, 0, sizeof(buf));
						recv_status = recv(i, buf, 1024, 0);

				
						printf("recv_status : %d\n", recv_status);
						if (recv_status == 0)
						{
						 	printf("Client %d disconnected\n", i);
						 	close(i);

						 	FD_CLR(i, &cpy_readfds);
							clients[find_id_from_socket(clients, i)] = 0;

							display_clients(clients);
						}
						else if (recv_status > 0)
						{
							display_clients(clients);


							printf(GREEN);
							printf("Received (%d): %s from %d\n",recv_status, buf, find_id_from_socket(clients, i));
							printf(RESET);

							for (int j = 0; j < max_sd + 1; j++)
							{
								if ( j != socketServer && j!= i)
								{
									printf("Envoie du message sur la socket %d \n", j);
									send_status = send(j, buf, recv_status, 0);
									(void) send_status;
									//printf("send_status = %d\n", send_status);
								}
							}

							printf("Socket %d is in readfds\n", i);
						}
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