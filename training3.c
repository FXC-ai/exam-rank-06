#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

typedef struct client
{
	int id;
	char msg[5000];

} t_client ;

void send_to_all (int max_sd, int socket_server, int socket_client, char *buffer_write)
{
	for (int i = 0; i < max_sd + 1; i++)
	{
		if (i != socket_server && i != socket_client)
		{
			send(i, buffer_write, strlen(buffer_write), 0);
		}
	}	
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		write(2, "Wrong number of arguments\n", 26);
		exit(1);
	}
	int port;
	port = atoi(argv[1]);

	int max_sd;
	int read;
	int last_id;
	socklen_t len;
	int socket_server;
	int socket_client;
	char buffer_read[424242];
	bzero(&buffer_read, 424242);
	char buffer_write[424242];
	bzero(&buffer_write, 424242);
	struct sockaddr_in servaddr, cliaddr;
	fd_set readfds, writefds, active;

	t_client clients[1024];

	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&active);

	socket_server = socket(AF_INET, SOCK_STREAM, 0); 
	if (socket_server == -1) { 
		write(2, "Fatal error\n", 12);
		exit(1); 
	} 

	bzero(&servaddr, sizeof(servaddr)); 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(port); 
  
	if ((bind(socket_server, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) { 
		write(2, "Fatal error\n", 12);
		exit(1);
	} 
	if (listen(socket_server, 128) != 0) {
		write(2, "Fatal error\n", 12);
		exit(1);
	}

	max_sd = socket_server;
	FD_SET(socket_server, &active);
	last_id = 0;

	while (42)
	{
		readfds = writefds = active;

		if (select(max_sd + 1, &readfds, &writefds, NULL, NULL) <= 0)
			continue;

		for (int fd = 0; fd < max_sd + 1; fd ++)
		{
			if (FD_ISSET(fd, &readfds) && fd == socket_server)
			{
				len = sizeof(cliaddr);
				socket_client = accept(socket_server, (struct sockaddr *)&cliaddr, &len);
				if (socket_client < 0) { 
					write(2, "Fatal error\n", 12);
					exit(1); 
				}
				FD_SET(socket_client, &active);
				if (socket_client > max_sd)
					max_sd = socket_client;
				clients[socket_client].id = last_id;
				last_id++;
				bzero(&buffer_write, 424242);
				sprintf(buffer_write, "server: client %d just arrived\n", clients[socket_client].id);
				send_to_all (max_sd, socket_server, socket_client, buffer_write);
		
				break;
			}
			else if (FD_ISSET(fd, &readfds) && fd != socket_server)
			{
				read = recv(fd, &buffer_read, 424242, 0);
				if (read == 0)
				{
					bzero(&buffer_write, 424242);
					sprintf(buffer_write, "server: client %d just left\n", clients[fd].id);
					send_to_all (max_sd, socket_server, fd, buffer_write);
					FD_CLR(fd, &active);
					clients[fd].id = 0;
					close(fd);
				}
				else
				{
					int j =0;
					while (buffer_read[j] != '\n' && j <= read)
					{
						buffer_write[j] = buffer_read[j];
						j++;
					}
					buffer_write[j] = '\n';
					buffer_write[j+1] = '\0';
					sprintf(clients[fd].msg, "client %d: %s", clients[fd].id, buffer_write);
					send_to_all (max_sd, socket_server, fd, clients[fd].msg);
				}
			}
		}
	}
	for (int k = 0; k < 1024; k++)
	{
		if (clients[k].id != 0)
		{
			close (k);
		}
	}
	return 0;
}