#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>

typedef struct client {
	int id;
	char msg[4096];
} t_client;

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		write(2, "Wrong number of arguments\n", 26);
		exit(1);
	}
	int port = atoi(argv[1]);

	int last_id = 0;
	int max_sd = 0; 
	t_client clients[1024];
	fd_set readfds, active;
	char read_buffer[424242];
	char write_buffer[424242];

	bzero(read_buffer, sizeof(read_buffer));

	FD_ZERO(&readfds);
	FD_ZERO(&active);



	int socket_server = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_server < 0)
	{
	 	write (2, "Fatal error\n", 12);
	 	exit(1);
	}

	struct sockaddr_in servaddr, client_addr;
	bzero(&servaddr, sizeof(servaddr));
	bzero(&client_addr, sizeof(client_addr));

	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(2130706433);
	servaddr.sin_port = htons(port);

	if ((bind(socket_server, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
	{
		write (2, "Fatal error\n", 12);
		exit(1);
	}

	if (listen (socket_server, 255) == -1) // backlog value
	{
		write (2, "Fatal error\n", 12);
		exit(1);		
	}

	FD_SET(socket_server, &active);
	max_sd = socket_server;


	while (42)
	{

		readfds = active;

		int result = select (max_sd + 1, &readfds, NULL, NULL, NULL);
		if (result == -1)
		{
			continue;
		}


		for (int i = 0; i < max_sd + 1; i++)
		{
			if (FD_ISSET(i, &readfds) && i == socket_server)
			{
				socklen_t addr_len = sizeof(client_addr);
				int new_socket = accept(socket_server, (struct sockaddr *)&client_addr, &addr_len);
				if (new_socket == -1)
				{
					write (2, "Fatal error\n", 12);
					exit(1);				
				}
				clients[new_socket].id = last_id;
				last_id++;


				sprintf(write_buffer, "server: client %d just arrived\n", clients[new_socket].id);
				for (int j = 0; j < max_sd + 1; j++)
				{
					if (j != i && j != socket_server)
					{
						send(j, write_buffer, strlen(write_buffer), 0);
					}
				}


				FD_SET(new_socket, &active);
				if (max_sd < new_socket)
				{
					max_sd = new_socket;
				}
				break;
			}
			if (FD_ISSET(i, &readfds) && i != socket_server)
			{
				bzero(clients[i].msg, sizeof(clients[i].msg));
				int read = recv(i, clients[i].msg, 424242, 0); // ATTENTION !!!

				if (read > 0)
				{
					bzero(write_buffer, sizeof(write_buffer));
					sprintf(write_buffer, "client %d: %s", clients[i].id, clients[i].msg);
					for (int j = 0; j < max_sd + 1; j++)
					{
						if (j != i && j != socket_server)
						{
							send(j, write_buffer, strlen(write_buffer), 0);
						}
					}
					bzero(read_buffer, sizeof(read_buffer));				
				}
				else if (read <= 0)
				{
					sprintf(write_buffer, "server: client %d just left\n", clients[i].id);
					for (int j = 0; j < max_sd + 1; j++)
					{
						if (j != i && j != socket_server)
						{
							send(j, write_buffer, 40, 0);
						}
					}
					FD_CLR(i, &active);
					clients[i].id  = 0;
					bzero(clients[i].msg, sizeof(clients[i].msg));
					close(i);

				}
			}
		}
	}
	close(socket_server);
	return 0;
}