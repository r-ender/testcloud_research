#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{

	char outbuf[100] = "This is en- and decryption-service for BSI Testcloud";
	int fd = open("~/testbla.c", O_CREAT, 0666);
	write(fd, outbuf,strlen(outbuf));
	close(fd);

	return 0;
}
