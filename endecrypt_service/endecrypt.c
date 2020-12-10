//The program sets up a 256 bit key and a 128 bit IV. This is appropriate for the 256-bit AES encryption that is done in CBC mode

#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/un.h>

#define SOCK_endecrypt_srvc "/tmp/endecryptsrvc.socket"
#define BUFFERSIZE 100

void ErrorHandling(void)
{
	ERR_print_errors_fp(stderr);
	abort();
}

//encrypt function: setting up context - initialising the encryption operation - providing plaintext bytes - finalising encryption operation
int encryption(unsigned char *plaintxt, int len_plaintxt, unsigned char *key, unsigned char *iv, unsigned char *ciphertxt)
{

	int len, len_ciphertxt;
	
	//create and initialise context ctx - EVP_CIPHER object using EVP_aes_256_cbc()
	EVP_CIPHER_CTX *ctx;
	ctx = EVP_CIPHER_CTX_new();

	//init encryption operation with size appropriate key and IV for ciphertext, here: 256-bit AES --> sizeof(IV) = sizeof(block) = 128bit for AES
	if(EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1) ErrorHandling();

	//providing the plaintext and obtaining the number of len bytes of encrypted cipher, can be called numeriously
	if(EVP_EncryptUpdate(ctx, ciphertxt, &len, plaintxt, len_plaintxt) != 1) ErrorHandling();
	len_ciphertxt = len;

	if(EVP_EncryptFinal_ex(ctx, ciphertxt + len, &len) != 1) ErrorHandling();
	len_ciphertxt += len;

	//Cleaning up after playing
	EVP_CIPHER_CTX_free(ctx);

	return len_ciphertxt;
}

//decrypt function: setting up a context - init decryption operation - providing ciphertext to decrypt - finales decryption operation
int decryption(unsigned char *ciphertxt, int len_ciphertxt, unsigned char *key, unsigned char *iv, unsigned char *plaintxt)
{
	int len, len_plaintxt;
	len = 0;

	//create/init context
	EVP_CIPHER_CTX *ctx;
	ctx = EVP_CIPHER_CTX_new();

	unsigned char *tmp_plaintxt;
	tmp_plaintxt = malloc(2*len_ciphertxt);
		
	//disable padding for decryption
	//EVP_CIPHER_CTX_set_padding(ctx, 0);

	//init decryption operation with size appropriate key and IV for ciphertext, here: 256-bit AES --> sizeof(IV) = sizeof(block) = 128bit for AES
	if(EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1) ErrorHandling();
	
	//providing the cyphertext and obtaining the number of len bytes of decrypted cipher, can be called numeriously
	if(EVP_DecryptUpdate(ctx, tmp_plaintxt, &len, ciphertxt, len_ciphertxt) != 1) ErrorHandling();
	len_plaintxt = len;

	if(EVP_DecryptFinal_ex(ctx, tmp_plaintxt + len, &len) != 1)	ErrorHandling();
	len_plaintxt += len;

	memcpy(plaintxt,tmp_plaintxt,len_plaintxt);

	//clean
	EVP_CIPHER_CTX_free(ctx);
	free(tmp_plaintxt);

	return len_plaintxt;
}

void pre_endecrypt_fct(char* x)
{
	char *str_piece[3];

	str_piece[0] = strtok(x,",");
	str_piece[1] = strtok(NULL,",");
	str_piece[2] = strtok(NULL, ",");

	printf("received file %s to %s with password:%s\n", str_piece[1], str_piece[0],str_piece[2]);

	//parse 4 characters as password for encryption/decription
	char passphrase[5] = {0};
	unsigned char *my_key, *my_iv, *plaintxt, *en_or_decryptedtxt;
	unsigned int len_plaintxt, len_endecrtxt;

/*	bool flag = false;
	bool encrypt = false;
	do{

		puts("Please enter your passphrase (four capital letters from A to Z): ");
		scanf("%s", passphrase);
		if(strlen(passphrase) != 4){  puts("The required number of letters is wrong!\n"); }
		else if((int)passphrase[0] < 65 || (int)passphrase[0] > 90) { puts("First letter not as required!\n");}
		else if((int)passphrase[1] < 65 || (int)passphrase[1] > 90) { puts("Second letter not as required!\n");}
		else if((int)passphrase[2] < 65 || (int)passphrase[2] > 90) { puts("Third letter not as required!\n");}
		else if((int)passphrase[3] < 65 || (int)passphrase[3] > 90) { puts("Fourth letter not as required!\n");}
		else{ flag = true;}

	}while(flag != true);
*/	

	//set up 256-bit key and 128-bit IV, convert characters into integers into string and save it as key resp. iv
	my_key = calloc(32,sizeof(unsigned char));
	my_iv = calloc(16,sizeof(unsigned char));
	strcpy(passphrase,str_piece[2]);	

	for(int i=0; i<=3;i++)
	{
		sprintf(((char *)my_key + 8*i),"%d%d%d%d", (int)passphrase[0], (int)passphrase[1], (int)passphrase[2],(int)passphrase[3]);
		sprintf(((char *)my_iv + 4*i),"%d%d", ((int)passphrase[1]-i), ((int)passphrase[3]-i*2));		
	}

	//read in file to encrpyt/decrypt and close afterwards so that file doesn't get corrupted if en/decryption fails
	int fd=open(str_piece[1],O_RDONLY);
	struct stat filestat;
	fstat(fd,&filestat);
	len_plaintxt = (unsigned int) filestat.st_size; 
	plaintxt = malloc(len_plaintxt);				
	read(fd,plaintxt,(size_t)(len_plaintxt*2));
	close(fd);

	plaintxt[len_plaintxt] = '\0' ;
	
	//buffer for the ciphertext, might be longer than plaintext depending on algorithm
	en_or_decryptedtxt = malloc(len_plaintxt*2);
	
	//if( encrypt == true )
	if(strcmp(str_piece[0],"encrypt") == 0)
	{ 
		//encrypting the plaintext
		len_endecrtxt = encryption(plaintxt, len_plaintxt, my_key, my_iv, en_or_decryptedtxt);	
	}
	else
	{	
		//decrypting the ciphertext
		len_endecrtxt = decryption(plaintxt, len_plaintxt, my_key,my_iv, en_or_decryptedtxt);

		//add a null terminator to make text printable
		en_or_decryptedtxt[len_endecrtxt] = '\0';

		//printf("stack of decrypted text: \n");
		//BIO_dump_fp(stdout, (const char *)en_or_decryptedtxt, len_endecrtxt);
	}

	fd = open(str_piece[1], O_WRONLY | O_TRUNC);
	write(fd, en_or_decryptedtxt,len_endecrtxt);
	close(fd);

	free(my_key);
	free(my_iv);
	free(plaintxt);
	free(en_or_decryptedtxt);
}


int main(void)
{

	struct sockaddr_un serv_sock;			//unix socket adress
	int conn_sock, data_sock;
	char buffer[BUFFERSIZE], cbuffer[BUFFERSIZE];

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
	strncpy(serv_sock.sun_path, SOCK_endecrypt_srvc, sizeof(serv_sock.sun_path)-1);

	if(bind(conn_sock, (const struct sockaddr *) &serv_sock, sizeof(struct sockaddr_un)) == -1) 
	{
		system("rm /tmp/endecryptsrvc.socket");
		bind(conn_sock, (const struct sockaddr *) &serv_sock, sizeof(struct sockaddr_un));
	}
	
	//set backlog size to 10
	if( listen(conn_sock, 10) == -1)
	{
		perror("listen:");
		exit(EXIT_FAILURE);
	}

	//loop for handling connections
	while(true){
		//incoming connections
		data_sock = accept(conn_sock, NULL, NULL);
		if(data_sock == -1)
		{
			perror("accept:");
			exit(EXIT_FAILURE);
		}
				
		//wait for incoming data-packet
		if(read(data_sock,buffer, BUFFERSIZE) == -1)
		{
			perror("read nr.1:");
			exit(EXIT_FAILURE);
		}

		buffer[BUFFERSIZE-1] = 0;	//zero-terminate buffer

		pre_endecrypt_fct(buffer);
		sleep(1);
		puts("demanded action done!\n");
		strncat(cbuffer, buffer,strlen(buffer));

		//answer message
		sprintf(buffer, "%s", cbuffer);
		if(write(data_sock, buffer, BUFFERSIZE) == -1)
		{
			perror("write:");
			exit(EXIT_FAILURE);
		}
		
	}

	close(conn_sock);
	
	unlink(SOCK_endecrypt_srvc);

	return 0;
}

