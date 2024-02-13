#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#include <netinet/in.h>

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


int main()
{

	int sock0;
	// char buf[1024];

	sock0 = socket(AF_INET, SOCK_STREAM, 0);
	printf("Socket created : %d\n", sock0);	

	// union mini_serv_data data;

	// data.i = 10;

	// struct mini_serv_data_struct data_struct;

	// data_struct.i = 10;

	// data_struct.data.i = 10;



	// struct sockaddr_in addr;
	
	
	










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