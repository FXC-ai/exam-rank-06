#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#include <netinet/in.h>
#include <arpa/inet.h>


#define MAGENTA "\033[1;95m"
#define RED "\033[1;91m"
#define GREEN "\033[1;92m"
#define BLUE "\033[1;94m"
#define YELLOW "\033[1;93m"
#define CYAN "\033[1;96m"
#define RESET "\033[0m"
// union mini_serv_data
// {
// 	int i;
// 	char c;
// };

// struct mini_serv_data_struct
// {
// 	int i;
// 	char c;
// 	union mini_serv_data data;

// };

#define PORT 8080

void displaySockaddr_in(struct sockaddr_in* addr)
{
	printf(MAGENTA);
	printf("Family : %d\n", addr->sin_family);
	printf("Port : %d\n", ntohs(addr->sin_port));
	printf("IP : %s\n", inet_ntoa(addr->sin_addr));
	printf(RESET);

}


int main()
{

	int socketServer;
	int socketClient;
	// char buf[1024];

	struct sockaddr_in addrServer;

	int addrServer_len = sizeof(addrServer);

	socketServer = socket(AF_INET, SOCK_STREAM, 0);
	printf("Socket server created : %d\n", socketServer);	

	addrServer.sin_family = AF_INET;
	// printf("AF_INET = %d\n", AF_INET);	

	addrServer.sin_port = htons(PORT);
	// printf("PORT = %d\n", addrServer.sin_port);

	addrServer.sin_addr.s_addr = INADDR_ANY;
	// printf("INADDR_ANY = %d\n", INADDR_ANY);

	printf("Socket server addrServer : \n");
	displaySockaddr_in(&addrServer);



	int bind_result = bind(socketServer, (struct sockaddr*)&addrServer, sizeof(addrServer));
	printf("Bind result : %d\n", bind_result);

	int listen_result = listen(socketServer, 10);
	printf("Listen result : %d\n", listen_result);

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

	int send_status = send(socketOutside, "Hello", 6, 0);
	printf("Send status : %d\n", send_status);

	socketClient = accept(socketServer, (struct sockaddr*)&addrServer, (socklen_t *)&addrServer_len);


	char buf[1024];
	int recv_status = recv(socketClient, buf, 1024, 0);

	printf("Received (%d): %s\n",recv_status, buf);



	// union mini_serv_data data;

	// data.i = 10;

	// struct mini_serv_data_struct data_struct;

	// data_struct.i = 10;

	// data_struct.data.i = 10;

	// struct sockaddrServer_in addr;
	
	// int sock1;
	// int fd = open("test.txt", O_RDWR);

	// write(fd, "Hello", 6);
	// read(fd, buf, 1024);

	// printf("Read : %s\n", buf);

	// close(fd);

	// sock1 = socket(AF_INET, SOCK_STREAM, 0);
	

	// printf("Socket created : %d\n", sock0);
	// printf("Socket created : %d\n", sock1);

	// ssize_t nb_chr_write = write(sock0, "Hello", 6);

	// printf("Write %zd bytes\n", nb_chr_write);

	// char buf[1024];

	// int nb_char_read = read(sock0, buf, 1024);

	// printf("Read %d bytes : %s\n", nb_char_read, buf);

	// close(sock0);
	// close(sock1);

	return 0;
}