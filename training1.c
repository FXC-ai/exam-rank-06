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
	char msg [5000];

} t_client;

int main (int argc, char *argv[])
{
	if (argc != 2)
	{
		write (2, "Fatal error\n", 12);
		exit(1);
	}
	int port = atoi(argv[1]);

	t_client clients[1024];
	char write_buffer[424242];
	char read_buffer[424242];

	bzero(&read_buffer, sizeof(read_buffer));
	bzero(&read_buffer, sizeof(write_buffer));

	struct sockaddr_in server_addr, client_addr;

	fd_set readfds, writefds, active;
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&active);

	int socket_server = socket(AF_INET, SOCK_STREAM, 0); 
	if (socket_server == -1) { 
		write (2, "Fatal error\n", 12); 
		exit(1); 
	}

	bzero(&server_addr, sizeof(server_addr)); 

	server_addr.sin_family = AF_INET; 
	server_addr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	server_addr.sin_port = htons(port);

	// Binding newly created socket to given IP and verification 
	if ((bind(socket_server, (const struct sockaddr *)&server_addr, sizeof(server_addr))) != 0) 
	{ 
		write (2, "Fatal error\n", 12); 
		exit(1);
	} 


	if (listen(socket_server, 128) != 0) 
	{
		write (2, "Fatal error\n", 12); 
		exit(1); 
	}

	int max_sd = socket_server;
	int last_id = 0;
	FD_SET(socket_server, &active);

	while (42)
	{
		readfds = writefds = active;

		if (select (socket_server, &readfds, &writefds, NULL, NULL) <= 0)
		{
			continue;
		}
		printf("BOUCLE\n");

		for (int fd = 0; fd < max_sd + 1 ; fd++)
		{
			if (FD_ISSET(fd, &readfds) && fd == socket_server)
			{
				socklen_t len = sizeof(client_addr);
				int new_socket = accept(socket_server, (struct sockaddr *)&client_addr, &len);
				if (new_socket < 0) { 
					write (2, "Fatal error\n", 12); 
					exit(1); 
				}
				FD_SET(new_socket, &active);
				clients[new_socket].id = last_id;
				last_id++;
				if (max_sd < new_socket)
				{
					max_sd = new_socket;
				}

				bzero(&write_buffer, sizeof(write_buffer));
				sprintf(write_buffer, "server: client %d just arrived\n", clients[fd].id);
				for (int j = 0; j < max_sd + 1; j++)
				{
					if (j != socket_server && j != fd)
					{
						send(j, write_buffer, strlen(write_buffer), 0);
					}
				}
				break;
			}

			if (FD_ISSET(fd, &readfds) && fd != socket_server)
			{
				int read = recv(fd, read_buffer, 5000, 0); // point a ameliorer : utiliser le read_buffer...

				if (read <= 0)
				{
					bzero(&write_buffer, sizeof(write_buffer));
					sprintf(write_buffer, "server: client %d just left\n", clients[fd].id);
					for (int j = 0; j < max_sd + 1; j++)
					{
						if (j != socket_server && j != fd)
						{
							send(j, write_buffer, strlen(write_buffer), 0);
						}
					}
					FD_CLR(fd, &active);
					close(fd);
					//et max_sd ???
				}
				else
				{
					int k = 0;
					while (read_buffer[k] != '\n')
					{
						write_buffer[k] = read_buffer[k];
						k++;
					}
					read_buffer[k] = '\n';
					read_buffer[k + 1] = '\0';

					sprintf(clients[fd].msg, "client %d: %s",clients[fd].id, read_buffer);
					for (int j = 0; j < max_sd + 1; j++)
					{
						if (j != socket_server && j != fd)
						{
							send(j, clients[fd].msg, strlen(clients[fd].msg), 0);
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