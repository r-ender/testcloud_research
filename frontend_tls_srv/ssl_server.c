#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <malloc.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <resolv.h>
#include "openssl/ssl.h"
#include "openssl/err.h"
#include <sys/un.h>


#define SOCK_MongoDB_srvc "/tmp/mongodbsrvc.socket"
#define SOCK_endecrypt_srvc "/tmp/endecryptsrvc.socket"
#define BUFFERSIZE 50


int client[10] = {-1};	//initialize clients as not-connected
SSL *ssl, *ssl2;

int root_user()
{
	int root = getuid();
	if (root != 0)		//get user id
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

int create_socket(int port)
{
    int s;
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
	perror("Unable to create socket");
	exit(EXIT_FAILURE);
    }

    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
	perror("Unable to bind");
	exit(EXIT_FAILURE);
    }

    //since this server --> listening/waiting for client to express wishes
    if (listen(s, 1) < 0) {
	perror("Unable to listen");
	exit(EXIT_FAILURE);
    }

    return s;
}

//initialize openSSL by loading the strings used for error messages, and setting up the algorithms needed for TLS
void initialize_openssl()
{ 
    SSL_load_error_strings();	
    OpenSSL_add_ssl_algorithms();
}

void clean_up_openssl()
{
    EVP_cleanup();
}

/* create an SSL_CTX or SSL context. This is created using the SSLv23_server_method which despite its name 
actually creates a server that will negotiate the highest version of SSL/TLS supported by the client it is connecting to */
SSL_CTX *create_ctx()
{
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = SSLv23_server_method();

    ctx = SSL_CTX_new(method);	//SSL_CTX_new() creates a new SSL_CTX object as framework to establish TLS/SSL enabled connections.
    if (!ctx) {
	perror("Unable to create SSL context");
	ERR_print_errors_fp(stderr);
	exit(EXIT_FAILURE);
    }

    return ctx;
}

/* The context is then configured - we use SSL_CTX_set_ecdh_auto to tell openssl to handle selecting 
the right elliptic curves for us (this function isn't available in older versions of openssl which required this to be done manually). 
The final step of configuring the context is to specify the certificate and private key to use. */
void config_ctx(SSL_CTX *ctx)
{
    SSL_CTX_set_ecdh_auto(ctx, 1);

    //set the local certificate from CertFile
    if (SSL_CTX_use_certificate_file(ctx, "ssl_server_cert.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
	exit(EXIT_FAILURE);
    }

	//set the private key from KeyFile (might be the same as Certificate)
    if (SSL_CTX_use_PrivateKey_file(ctx, "ssl_server_cert.pem", SSL_FILETYPE_PEM) <= 0 ) {
        ERR_print_errors_fp(stderr);
	exit(EXIT_FAILURE);
    }
}

void response(int n)
{
	    char buf[10000] = {0};
	    char backup_buf[10000] = {0};
	    int bytes=0;
	    int fd=open("index.html",O_RDONLY);
	    struct stat filestat;
	    fstat(fd,&filestat);
	    bytes = SSL_read(ssl, buf, sizeof(buf)); 	//get request
	    buf[bytes] = '\0';
	    printf("\"%s\"\n", buf);
	    strncpy(backup_buf,buf,bytes);

	    //parser of http-input
	    strncpy(backup_buf, buf, bytes);			//required for POST-method
	    char delim[] = " \t\n";
	    char *string_piece[3];
	    string_piece[0] = strtok(buf,delim);		//strtok brakes string into pieces
	    string_piece[1] = strtok(NULL,delim);		//the argument NULL tells strtok to continue parsing the same string as before where it stopped
	    char *clean_string = ++string_piece[1];		//skip the first char of string 
	    string_piece[2] = strtok(NULL,delim);

	    char request[300] = {0};
	    if(strncmp(string_piece[2],"HTTP/1.1",8) == 0 || strncmp(string_piece[2],"HTTP/1.0",8) == 0)
	    { 
		if (strncmp(string_piece[0],"GET",3) == 0)
	    	{
			if(strcmp(clean_string,"") == 0 || strcmp(clean_string,string_piece[1]) == 0 )
			{
				if( strcmp(clean_string,"") == 0) string_piece[1] = "index.html";
				int fd=open(string_piece[1],O_RDONLY);
				struct stat filestat;
				char sendbuf[300] = {0};
				fstat(fd,&filestat);
	    			strcpy(request, "HTTP/1.1 200 OK\r\nContent-Type: text/html Content-Length: 300\r\n\r\n"); //send http-header
	    			SSL_write(ssl, request, strlen(request));				
				SSL_write(ssl, sendbuf, read(fd,sendbuf,filestat.st_size+1));		//read parses file and returns number of parsed bytes 
			}
			else
			{
				char imgbuf[1000000];
				struct stat filestat;
				int pic;
				if ((pic = open(clean_string,O_RDONLY)) <0) puts("Error opening File\n");
				fstat(pic,&filestat);					
				strcpy(request, "HTTP/1.1 200 OK\r\nContent-Type: image/png Content-Length: 300000\r\n\r\n"); //send http-header
	    			SSL_write(ssl, request, strlen(request));
				SSL_write(ssl,imgbuf,read(pic,imgbuf,(size_t)filestat.st_size+1));
			}
		}
		if(strncmp(string_piece[0], "PUT", 3) == 0)	puts("No PUT-method available yet!\n");
		if(strncmp(string_piece[0], "POST", 4) == 0 && ( strcmp(string_piece[1],"index.html") == 0 || strcmp(string_piece[1],"") == 0) )
		{
			puts("POST-method in action!\n");
			FILE *fp;

			char *buffer2;
			buffer2 = (char *)(malloc(sizeof(char *) * (strlen(backup_buf)+1)));
			strncpy(buffer2,backup_buf,strlen(backup_buf)+1);

			char *edit_buf = strstr(backup_buf, "filename");  		//search for string filename in buf
			if (edit_buf != NULL) printf("return: %s\n", edit_buf);

			char *str_piece[2];
			str_piece[0] = strtok(edit_buf,"\"");				//filter the filename
			str_piece[1] = strtok(NULL,"\"");

			for(int i=1; i<18; i++){ 					//separate the text of file from the header
				strtok_r(buffer2,"\n",&buffer2);
			}

			char *tmpbuf;
			tmpbuf = (char *) malloc(sizeof(char *) * strlen(buffer2));
			strcpy(tmpbuf, buffer2);
			tmpbuf[strlen(buffer2)-291] = '\0';  

			fp = fopen(str_piece[1],"a+");
			fwrite(tmpbuf,1,strlen(tmpbuf),fp);
			fclose(fp);

			//create directory and move uploaded files to persistent home-folder
			if (system("mkdir -m777 -p /tmp/upload_cloud") != 0) puts("ERROR---ERROR---ERROR: error creating directory\n");	

			char upload_folder[200] = {0};
			sprintf(upload_folder, "mv -f %s /tmp/upload_cloud", str_piece[1]);
			if (system(upload_folder) != 0) puts("ERROR---ERROR---ERROR: move file to home\n");
			sprintf(upload_folder, "chmod 666 /tmp/upload_cloud/%s", str_piece[1]);
			if (system(upload_folder) != 0) puts("ERROR---ERROR---ERROR: change mode to 666\n");


			edit_buf = strstr(buffer2, "sxcrxt") + 10;
			if (edit_buf == NULL) puts("----- Error parsing secret! ---------");		
	 		
			if( atoi(edit_buf) == 1 ) 
			{
				char *edit_buf = strstr(buffer2,"pxsswd") + 11;
				if (edit_buf == NULL) puts("----- Error parsing passwd! ---------");
				memset(edit_buf+4, '\0', 1);
				
							/*	call endecrypt to encrypt	*/
				
				struct sockaddr_un client_sock;
				int data_sock;	
				char crypt_buffer[150] = {0};						

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
				strncpy(client_sock.sun_path, SOCK_endecrypt_srvc, sizeof(client_sock.sun_path)-1);

				if(connect(data_sock, (const struct sockaddr *) &client_sock, sizeof(struct sockaddr_un)) == -1)
				{
					fprintf(stderr, "The server of endecrypt is down.\n");
					puts("---- continue might result in corrupted data ----\n");
				}

				sprintf(crypt_buffer,"encrypt,/tmp/upload_cloud/%s,%s",str_piece[1],edit_buf);
				if(write(data_sock,crypt_buffer, strlen(crypt_buffer)+1) == -1) perror("write:");

				//wait for server to be ready again
				if(read(data_sock,crypt_buffer, BUFFERSIZE) == -1)
				{
					perror("read:");
					exit(EXIT_FAILURE);
				}

				close(data_sock);
				
			}


			int fd=open("upload.html",O_RDONLY);
			struct stat filestat;
			char sendbuf[300] = {0};
			fstat(fd,&filestat);
    			strcpy(request, "HTTP/1.1 200 OK\r\nContent-Type: text/html Content-Length: 300\r\n\r\n"); //send http-header
    			SSL_write(ssl, request, strlen(request));				
			SSL_write(ssl, sendbuf, read(fd,sendbuf,filestat.st_size+1));		//read parses file and returns number of parsed bytes				
			
		}
			
		if(strncmp(string_piece[0], "POST", 4) == 0 && strcmp(string_piece[1],"db_employee.html") == 0)
		{
			puts("This is db_employee page!\n");

			char *edit_buf = strstr(backup_buf, "persnr");  		//search for string persnr in buf
			if (edit_buf == NULL) puts("----- Error parsing! ---------");

			char *persnr, *username, *stelle, *entrydate, *secret, *passwd;
			strtok(edit_buf,"=");
			persnr = strtok(NULL,"&");
			
			strtok(NULL,"=");
			username = strtok(NULL,"&");

			strtok(NULL,"=");
			stelle = strtok(NULL,"&");

			strtok(NULL,"=");
			entrydate = strtok(NULL,"&");

			strtok(NULL,"=");
			secret = strtok(NULL,"&");

			strtok(NULL,"=");
			passwd = strtok(NULL,"\n");

			printf("persnr: %s\n", persnr);		
			printf("username: %s\n", username);	
			printf("stelle: %s\n", stelle);	
			printf("Entry-Date: %s\n", entrydate);	
			printf("secret: %s\n", secret);
			printf("passwd: %s\n", passwd);

			//to do: checking of input data !!!

			//send parsed inputs to MongoDB-insert-service, functions as client and sends data to CRUDinsert-service
			struct sockaddr_un client_sock;
			int data_sock;
			char buffer[BUFFERSIZE] = {0};

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
			strncpy(client_sock.sun_path, SOCK_MongoDB_srvc, sizeof(client_sock.sun_path)-1);

			if(connect(data_sock, (const struct sockaddr *) &client_sock, sizeof(struct sockaddr_un)) == -1)
			{
				fprintf(stderr, "The server of CRUDinsert is down.\n");
				exit(EXIT_FAILURE);
			}

			//send database-sets
			if(write(data_sock,persnr, strlen(persnr)+1) == -1) perror("write:");
			if(write(data_sock,username, strlen(username)+1) == -1) perror("write:");
			if(write(data_sock,stelle, strlen(stelle)+1) == -1) perror("write:");
			if(write(data_sock,entrydate, strlen(entrydate)+1) == -1) perror("write:");
			if(write(data_sock,secret, strlen(secret)+1) == -1) perror("write:");

			if(atoi(secret) == 1){
				if(write(data_sock,passwd, strlen(passwd)+1) == -1) perror("write:");
			}

			strcpy(buffer, "DOWN");
			if(write(data_sock,buffer, strlen(buffer)+1) == -1) perror("write:");
			close(data_sock);		//only close data_sock, conn_sock muss von server geschlossen werden

			//open index.html again
			int fd=open("index.html",O_RDONLY);
			struct stat filestat;
			char sendbuf[300] = {0};
			fstat(fd,&filestat);
    			strcpy(request, "HTTP/1.1 200 OK\r\nContent-Type: text/html Content-Length: 300\r\n\r\n"); //send http-header
    			SSL_write(ssl, request, strlen(request));				
			SSL_write(ssl, sendbuf, read(fd,sendbuf,filestat.st_size+1));		//read parses file and returns number of parsed bytes 

				}
				if(strncmp(string_piece[0], "PATCH", 5) == 0)	puts("No PATCH-method available yet!\n");
				if(strncmp(string_piece[0], "DELETE", 6) == 0)	puts("No DELETE-method available yet!\n");
			    }
			    else
			    {
				puts("Wrong HTTP-Version!\n");
			    }


}


int main(int argc, char *Argc[])
{

    int sock;
    SSL_CTX *ctx;

	//only root user have permission to run server
	if(!root_user())
	{
		printf("Program must be executed as root!");
		exit(0);
	}
	if (argc != 2 )
	{
		printf("Wrong usage: %s <portnumber>\n", Argc[0]);
		exit(0);
	}


    initialize_openssl();			//initialize openSSL
    ctx = create_ctx();				//create SSL context
    config_ctx(ctx);

    //Next we perform some normal socket programming and create a new server socket, there's nothing openssl specific about this code.
    static unsigned int counter = 0;
    sock = create_socket(atoi(Argc[1]));		//parse the arguments and convert string to integer

    /* Handle connections */
    while(1) {
        struct sockaddr_in addr;
        uint len = sizeof(addr);

        client[counter] = accept(sock, (struct sockaddr*)&addr, &len);	//get a new connection, call accept as normal.
        if (client[counter] < 0) {
            perror("Unable to accept");
            exit(EXIT_FAILURE);
        }
	else{
	    printf("Connection-Nr %d: %s:%d\n", counter, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
	}
	/* To handle the TLS we create a new SSL structure, this holds the information related to this particular connection.*/
        ssl = SSL_new(ctx);	/* SSL_new() creates a new SSL structure which is needed to hold the data for a TLS/SSL connection. The new structure inherits the settings of the underlying 						context ctx: connection method (SSLv2/v3/TLSv1), options, verification settings, timeout settings. */
        SSL_set_fd(ssl, client[counter]); //We use SSL_set_fd to tell openssl the file descriptor to use for the communication.

        if (SSL_accept(ssl) <= 0) {	//call SSL_accept to handle the server side of the TLS handshake
            ERR_print_errors_fp(stderr);
        }
        else {
		response(counter);
	}

        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(client[counter]);
	counter++;
    }

    close(sock);
    SSL_CTX_free(ctx);
    clean_up_openssl();
}

