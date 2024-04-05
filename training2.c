#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct client 
{
	int id;
	char msg[5000];
} t_client ;

int main (int argc, char *argv[])
{

	if (argc != 2)
	{
		write (2,"Wrong number of arguments\n", 26);
		exit(1);
	}
	int port = atoi(argv[1]);

	int socket_server;
	struct sockaddr_in servaddr, cliaddr;

	t_client clients[1024];

	// socket create and verification 
	socket_server = socket(AF_INET, SOCK_STREAM, 0); 
	if (socket_server == -1) 
	{ 
		write (2,"Fatal error\n", 12);
		exit(1); 
	}

	bzero(&servaddr, sizeof(servaddr)); 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(port); 
  
	// Binding newly created socket to given IP and verification 
	if ((bind(socket_server, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) 
	{ 
		write (2,"Fatal error\n", 12);
		exit(1); 
	} 

	if (listen(socket_server, 128) != 0) {
		write (2,"Fatal error\n", 12);
		exit(1);
	}

	fd_set readfds, writefds, active;
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&active);
	FD_SET(socket_server, &active);

	int max_sd = socket_server;
	int last_id = 0;

	char buffer_read [5000];
	char buffer_write [5000];

	while (42)
	{
		readfds = writefds = active;

		if (select (max_sd + 1, &readfds, &writefds, NULL, NULL) <= 0)
			continue;

		for (int fd = 0; fd < max_sd + 1; fd++)
		{

			if (FD_ISSET(fd, &readfds) && fd == socket_server)
			{
				socklen_t len = sizeof(cliaddr);
				int client_fd = accept(socket_server, (struct sockaddr *)&cliaddr, &len);
				if (client_fd < 0) 
				{ 
					write (2,"Fatal error\n", 12);
					exit(1);
				} 

				FD_SET(client_fd, &active);
				clients[client_fd].id = last_id;
				last_id++;
				if (client_fd > max_sd)
					max_sd = client_fd;

				bzero(&buffer_write, sizeof(buffer_write));
				sprintf(buffer_write, "server: client %d just arrived\n", clients[client_fd].id);
				for (int i = 0; i < max_sd + 1; i++)
				{
					if (i != client_fd && i != socket_server)
					{
						send(i, buffer_write, strlen(buffer_write), 0);
					}
				}
				
				break;
			}

			if (FD_ISSET(fd, &readfds) && fd != socket_server)
			{
				
				int read = recv(fd, buffer_read, 5000, 0);

				if (read <= 0)
				{
					FD_CLR(fd, &active);
					close(fd);
					bzero(&buffer_write, sizeof(buffer_write));
					sprintf(buffer_write, "server: client %d just left\n", clients[fd].id);
					for (int i = 0; i < max_sd + 1; i++)
					{
						if (i != fd && i != socket_server)
						{
							send(i, buffer_write, strlen(buffer_write), 0);
						}
					}					
				}
				else
				{
					printf("buffer_read = %s\n", buffer_read);
					int j = 0;
					while (buffer_read[j] != '\n')
					{
						// printf("%c (%d)");
						buffer_write[j] = buffer_read[j];
						j++;
					}
					buffer_write[j] = '\n';
					buffer_write[j + 1] = '\0';
					printf("buffer_write = %s\n", buffer_write);
					
					
					sprintf(clients[fd].msg, "client %d: %s", clients[fd].id, buffer_write);
					
					for (int i = 0; i < max_sd + 1; i++)
					{
						if (i != fd && i != socket_server)
						{
							send(i, clients[fd].msg, strlen(clients[fd].msg), 0);
						}
					}
				}
				break;
			}	
		}
	}
	close(socket_server);
	return 0;
}