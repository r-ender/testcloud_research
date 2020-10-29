//communicates via socket with helloworld2, functions as client

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>

#define SOCKNAME "/tmp/endecrypt.socket"
#define BUFFERSIZE 50

int main()
{

	struct sockaddr_un client_sock;
	int data_sock;
	char buffer[BUFFERSIZE] = {0};
	//char outbuf[100] = "This is en- and decryption-service for BSI Testcloud\n";

	//create local socket
	data_sock = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	if(data_sock == -1)
	{
		perror("socket");
                exit(EXIT_FAILURE);
        }

	memset(&client_sock,0,sizeof(struct sockaddr_un));		//zeroize struct socket address unix
	
	//connect socket so sockaddr
	client_sock.sun_family = AF_UNIX;
	strncpy(client_sock.sun_path, SOCKNAME, sizeof(client_sock.sun_path)-1);

	if(connect(data_sock, (const struct sockaddr *) &client_sock, sizeof(struct sockaddr_un)) == -1)
	{
		fprintf(stderr, "The server is down.\n");
                exit(EXIT_FAILURE);
        }

	sleep(1);

/*
	int fd = open("/home/gast/testendecrypt.txt", O_WRONLY | O_TRUNC | O_CREAT, 0666);
	if (fd < 0)	puts("Error writing to file!\n");
	write(fd, outbuf,strlen(outbuf));
	close(fd);
*/
	//send database-location
	strcpy(buffer, "decrypt,/data/db/bla.json");
	if(write(data_sock,buffer, strlen(buffer)+1) == -1) perror("write:");

	//get result
	if(read(data_sock, buffer, BUFFERSIZE) == -1) perror("read:");
	/* Ensure buffer is 0-terminated. */
           buffer[BUFFERSIZE - 1] = 0;

	printf("Result: %s\n", buffer);


	strcpy(buffer, "DOWN");
	if(write(data_sock,buffer, strlen(buffer)+1) == -1) perror("write:");

	close(data_sock);		//only close data_sock, conn_sock muss von server geschlossen werden

	return 0;
}
