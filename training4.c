#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct client 
{
	int id;
	char msg[10000];
} t_client;

t_client clients[1024];
char buffer_read[424242];
char buffer_write[424242];


void exitError()
{
	write(2, "Fatal error\n", 12);
	exit(1);
}

void sendAll(int max_sd, int sock_server, int sock_client, char *buffer_write)
{
	for (int i = 0; i < max_sd + 1; i++)
	{
		if (i != sock_server && i != sock_client)
		{
			send(i, buffer_write, strlen(buffer_write), 0);
		}
	}
}

int main(int argc, char *argv[]) 
{
	(void ) argc;
	(void) argv;

	if (argc != 2)
	{
		write(2, "Wrong number of arguments\n", 26);
		exit(1);
	}
	int port = atoi(argv[1]);

	int sock_server, sock_client;
	int len;
	struct sockaddr_in servaddr, cli; 

	// socket create and verification 
	sock_server = socket(AF_INET, SOCK_STREAM, 0); 
	if (sock_server == -1) 
		exitError();

	bzero(&servaddr, sizeof(servaddr)); 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(port); 
  
	// Binding newly created socket to given IP and verification 
	if ((bind(sock_server, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) 
		exitError();

	if (listen(sock_server, 128) != 0) 
		exitError();

	int max_sd = 0;
	int last_id = 0;
	fd_set readfds, writefds, active;
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&active);

	FD_SET(sock_server, &active);

	max_sd = sock_server;



	bzero(buffer_read, 424242);
	bzero(buffer_write, 424242);

	while (42)
	{
		readfds = writefds = active;

		if (select(max_sd + 1, &readfds, &writefds, NULL, NULL) <= 0) /*<= ?*/
			continue;

		for (int fd = 0; fd < max_sd + 1; fd++)
		{
			if (FD_ISSET(fd, &readfds) && fd == sock_server)
			{
				len = sizeof(cli);
				sock_client = accept(sock_server, (struct sockaddr *)&cli, (socklen_t *) &len);
				if (sock_client < 0) 
					continue;

				FD_SET(sock_client, &active);

				clients[sock_client].id = last_id;
				last_id++;
				printf("fd = %d\n", clients[sock_client].id);

				if (max_sd < sock_client)
					max_sd = sock_client;

				sprintf(buffer_write, "server: client %d just arrived\n", clients[sock_client].id);
				sendAll(max_sd, sock_server, sock_client, buffer_write);

				break;
			}
			else if (FD_ISSET(fd, &readfds) && fd != sock_server)
			{
				int read = recv(fd, buffer_read, 424242, 0);

				if (read == 0)
				{
					FD_CLR(fd, &active);
					close(fd);
					bzero(&buffer_write, 424242);
					sprintf(buffer_write, "server: client %d just left\n", clients[fd].id);
					sendAll(max_sd, sock_server, fd, buffer_write);

					break;
				}
				else
				{
					bzero(clients[fd].msg, 10000);
					for (int i = 0, j = strlen(clients[fd].msg); i < read; i++, j++ )
					{
						clients[fd].msg[j] = buffer_read[i];
						if (buffer_read[i] == '\n')
						{
							clients[fd].msg[j] = '\0';
							sprintf(buffer_write, "client %d: %s\n", clients[fd].id, clients[fd].msg);
							sendAll(max_sd, sock_server, fd, buffer_write);
							bzero(clients[fd].msg, strlen(clients[fd].msg));
							j = -1;
						}
					}
					break;
				}
			}
		}
	}
	return 0;
}
