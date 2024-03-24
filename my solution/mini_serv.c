#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#include <string.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <stdlib.h>

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
	port = atoi(argv[1]);
	
	/* cients[id] = sock */
	int clients[1024];
	memset(clients, 0, sizeof(clients));

	int socketClient = 0;

	int max_sd = 0;

	char buf[4096];
	memset(buf, 0, sizeof(buf));

	char msg[4196];
	memset(msg, 0, sizeof(msg));

	int recv_status = 0;

	fd_set readfds;
	fd_set cpy_readfds;
	FD_ZERO(&readfds);
	FD_ZERO(&cpy_readfds);

	struct sockaddr_in addrServer;
	int addrServer_len = sizeof(addrServer);

	memset(&addrServer, 0, addrServer_len);

	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(port);
	addrServer.sin_addr.s_addr = INADDR_ANY;

	int socketServer = socket(AF_INET, SOCK_STREAM, 0);
	if (socketServer < 0)
	{
		fatal_error();
		return 1;
	}

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

	int last_id = 0;
	int result_select = 0;

	while (42)
	{
		/*COPIE de cpy_readfds dans readfds : au départ de la boucle les 2 structures contiennent les même fd*/
		readfds = cpy_readfds;

		/*On regarde combien il y a de fd dans readfds*/
		max_sd = socketServer;
		for (int i = 0; i < 1024; i++)
		{
			if (FD_ISSET(i, &readfds) && i > max_sd)
			{
				max_sd = i;
			}
		}

		/* select ne va conserver dans readfds que les fd dans lesquels il y eu un évenement */
		result_select = select(max_sd + 1, &readfds, NULL, NULL, NULL);


		if (result_select > 0)
		{
			/*On parcours l'ensemble des fd du fd_set*/
			for (int i = 0; i < max_sd + 1; i++)
			{
				
				if (FD_ISSET(i, &readfds) && i == socketServer) /*Si on y trouve encore le fd du serveur alors il y a eu une nouvelle connection*/
				{
					socketClient = accept(socketServer, (struct sockaddr*)&addrServer, (socklen_t *)&addrServer_len);
					FD_SET(socketClient, &cpy_readfds);
					clients[last_id] = socketClient;
					++last_id;
					memset(buf, 0, sizeof(buf));
					sprintf(buf, "server: client %d just arrived\n", find_id_from_socket(clients, socketClient));
					send_to_all(socketServer, socketClient, buf, max_sd);
					memset(buf, 0, sizeof(buf));
				}
				else
				{
					if (FD_ISSET(i, &readfds)) /* Si une socket se trouve encore dans readfds MAIS que ce n'est pas la socket du serveur */
					{
						memset(buf, 0, sizeof(buf));
						recv_status = recv(i, buf, 4096, 0);
						if (recv_status <= 0)
						{
						 	close(i);
						 	FD_CLR(i, &cpy_readfds);
							sprintf(buf, "server: client %d just left\n", find_id_from_socket(clients, i));
							send_to_all(socketServer, i, buf, max_sd);
							clients[find_id_from_socket(clients, i)] = 0;
						}
						else if (recv_status > 0)
						{
							printf("recv_status : %d strlen : %lu : %s\n", recv_status, strlen(buf), buf);
							int pi;
							for(pi = 0; pi < recv_status; ++pi)
							{
								if (buf[pi] == '\n')
								{
									buf[pi] = '\0';
									break;
								}
							}
							printf("pi = %d\n", pi);
							buf[pi] = '\n';
							memset(msg, 0, sizeof(msg));
							sprintf(msg, "client %d: %s", find_id_from_socket(clients, i), buf);

							send_to_all(socketServer, i, msg, max_sd);
						}
					}
				}
			}
		}
	}
	close(socketServer);
	return 0;
}