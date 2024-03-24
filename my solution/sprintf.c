#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main ()
{
	char buf[50];

	memset(buf, 0, 50);
	sprintf(buf, "Hello %d", 42);

	printf("buffer_size : %lu\n", strlen(buf));

	int i = 0;
	while (i < 50)
	{
		write(1, &buf[i], 1);
		write(1, "\n", 1);

		i++;
	}

	return 0;
}