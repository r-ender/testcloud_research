//To insert documents into a collection, first obtain a handle to a mongoc_collection_t via a mongoc_client_t. Then, use mongoc_collection_insert_one() to add BSON documents to the collection. 

#include <mongoc/mongoc.h>
#include <bson/bson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdbool.h>

#define SOCK_MongoDB_srvc "/tmp/mongodbsrvc.socket"
#define SOCK_endecrypt_srvc "/tmp/endecryptsrvc.socket"
#define BUFFERSIZE 100

int main()
{
	while(true)
	{
		struct sockaddr_un serv_sock;			//unix socket adress
		int conn_sock, data_sock;
		char buffer[BUFFERSIZE], persnr[10], username[40], stelle[40], entrydate[15], secret[2], passwd[5];

		//Creation of local socket
		conn_sock = socket(AF_UNIX, SOCK_SEQPACKET,0);		//UNIX adress family, SEQPACKET similiar to STREAM except for boundaries, 0 default protocol
		if (conn_sock == -1) 
		{
			perror("socket:"); 
			exit(EXIT_FAILURE);
		}

		memset(&serv_sock, 0, sizeof(struct sockaddr_un));	//zeroize unix socket addr. 

		//bind socket
		serv_sock.sun_family = AF_UNIX;				//address-family is UNIX
		strncpy(serv_sock.sun_path, SOCK_MongoDB_srvc, sizeof(serv_sock.sun_path)-1);
		if(bind(conn_sock, (const struct sockaddr *) &serv_sock, sizeof(struct sockaddr_un)) == -1) 
		{
			system("rm /tmp/mongodbsrvc.socket");
			bind(conn_sock, (const struct sockaddr *) &serv_sock, sizeof(struct sockaddr_un));	
		}

		//set backlog size to 10
		if( listen(conn_sock, 10) == -1)
		{
			perror("listen:");
			exit(EXIT_FAILURE);
		}

		//handling incoming connections
		data_sock = accept(conn_sock, NULL, NULL);
		if(data_sock == -1)
		{
			printf("accept returns value %d\n", data_sock);
			perror("accept:");
			exit(EXIT_FAILURE);
		}

		//wait for incoming data-sets from TLS server
			if(read(data_sock,persnr, BUFFERSIZE) == -1)
			{
				perror("read:");
				exit(EXIT_FAILURE);
			}
			buffer[BUFFERSIZE-1] = 0;	//zero-terminate buffer
			printf("persnr: %s\n", persnr);

			if(read(data_sock,username, BUFFERSIZE) == -1)
			{
				perror("read:");
				exit(EXIT_FAILURE);
			}
			buffer[BUFFERSIZE-1] = 0;	//zero-terminate buffer
			printf("username: %s\n", username);

			if(read(data_sock,stelle, BUFFERSIZE) == -1)
			{
				perror("read:");
				exit(EXIT_FAILURE);
			}
			buffer[BUFFERSIZE-1] = 0;	//zero-terminate buffer
			printf("stelle: %s\n", stelle);

			if(read(data_sock,entrydate, BUFFERSIZE) == -1)
			{
				perror("read:");
				exit(EXIT_FAILURE);
			}
			buffer[BUFFERSIZE-1] = 0;	//zero-terminate buffer
			printf("entrydate: %s\n", entrydate);

			if(read(data_sock,secret, BUFFERSIZE) == -1)
			{
				perror("read:");
				exit(EXIT_FAILURE);
			}
			buffer[BUFFERSIZE-1] = 0;	//zero-terminate buffer
			printf("secret: %s\n", secret);

			if(atoi(secret) == 1){
				if(read(data_sock,passwd, BUFFERSIZE) == -1)
				{
					perror("read:");
					exit(EXIT_FAILURE);
				}
				buffer[BUFFERSIZE-1] = 0;	//zero-terminate buffer
				printf("passwd: %s\n", passwd);
			}

			if(read(data_sock,buffer, BUFFERSIZE) == -1)
			{
				perror("read:");
				exit(EXIT_FAILURE);
			}

			buffer[BUFFERSIZE-1] = 0;	//zero-terminate buffer

		close(data_sock);

		close(conn_sock);
		unlink(SOCK_MongoDB_srvc);


		//from here on insert data-sets into DB

		mongoc_client_t *client;
		mongoc_collection_t *collection;
		bson_error_t error;
		bson_oid_t oid;
		bson_t *doc;

		mongoc_init();

		client = mongoc_client_new("mongodb://localhost:27017/?appname=insert-example");

		if(atoi(secret) == 0)
		{ 
			puts("Entry into unclassified collection!\n");
			collection = mongoc_client_get_collection(client, "db_employees","db_employees_unclassified-clltn");
		}
		else
		{
			puts("Entry into classified collection!\n");
			
			system("test -e /data/db/db_employees_classified-clltn.json && echo 1 > /tmp/file_existence || echo 0 > /tmp/file_existence");

			int tmp_fd=open("/tmp/file_existence",O_RDONLY);
			struct stat tmp_filestat;
			char tmp_buf[5] = {0};
			fstat(tmp_fd,&tmp_filestat);			
			read(tmp_fd,tmp_buf,tmp_filestat.st_size+1);		//read parses file and returns number of parsed bytes 
			close(tmp_fd);

			if(atoi(tmp_buf) == 1)
			{
				//previously classified collection still exists

					/*	call endecrypt to decrypt	*/

				//send parsed inputs to endecrypt-service, functions as client and sends data to endecrypt-service
				struct sockaddr_un client_sock;
				int data_sock=0;

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

				sprintf(buffer,"decrypt,/data/db/db_employees_classified-clltn.json,%s",passwd);
				if(write(data_sock,buffer, strlen(buffer)+1) == -1) perror("write:");

				//wait for server to be ready again
				if(read(data_sock,buffer, BUFFERSIZE) == -1)
				{
					perror("read:");
					exit(EXIT_FAILURE);
				}

				system("mongoimport --db db_employees --collection db_employees_classified-clltn --file /data/db/db_employees_classified-clltn.json --jsonArray");

				close(data_sock);
			}

			collection = mongoc_client_get_collection(client, "db_employees","db_employees_classified-clltn");
		}

		//generate docs with object ID
		doc = bson_new();
		bson_oid_init(&oid, NULL);
		bson_append_oid(doc, "_id", -1, &oid);
		bson_append_utf8(doc,"persnr",-1,persnr,-1);	//arguments: bson_t, key, key-length , value, value-length / (-1 means call of strlen())
		bson_append_utf8(doc,"username",-1,username,-1);
		bson_append_utf8(doc,"stelle",-1,stelle,-1);
		bson_append_utf8(doc,"entrydate",-1,entrydate,-1);

		//insert document into collection
		if(!mongoc_collection_insert_one(collection,doc, NULL, NULL, &error)) fprintf(stderr, "%s\n", error.message);

		if(atoi(secret) == 1)
		{
			system("mongoexport --db db_employees --collection db_employees_classified-clltn --jsonArray --out /data/db/db_employees_classified-clltn.json");
			if(!mongoc_collection_drop (collection, &error)) fprintf(stderr, "%s\n", error.message);	//for dropping the collection after exporting
			system("cat /data/db/db_employees_classified-clltn.json");

					/*	call endecrypt to encrypt	*/
			
			struct sockaddr_un client_sock;
			int data_sock;							

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
				exit(EXIT_FAILURE);
			}

			sprintf(buffer,"encrypt,/data/db/db_employees_classified-clltn.json,%s",passwd);
			if(write(data_sock,buffer, strlen(buffer)+1) == -1) perror("write:");

			//wait for server to be ready again
			if(read(data_sock,buffer, BUFFERSIZE) == -1)
			{
				perror("read:");
				exit(EXIT_FAILURE);
			}

			close(data_sock);
		}

		//clean up collection
		bson_destroy(doc);
		mongoc_collection_destroy(collection);
		mongoc_client_destroy(client);
		mongoc_cleanup();
	}

	return 0;
}
