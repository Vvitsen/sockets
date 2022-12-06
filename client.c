#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int main(int argc, char* argv[]){

	if (argc < 3){
		fprintf(stderr, "usage %s hostname port\n", argv[0]);
		exit(0);
	}
	
	struct hostent* server; //pointer to a structure of type hostent <netdb.h>
							// it defines a host computer on the internet
	server = gethostbyname(argv[1]); //takes a name and returns a pointer to a hostent
	if (server == NULL){
		fprintf(stderr, "ERROR, not such host \n");
		exit(0);
	}
	
	int port = atoi(argv[2]);
	
	struct sockaddr_in serv_addr; // is a structure, containing an internet address (<netinet/in.h>)
	bzero((char*)&serv_addr, sizeof(serv_addr)); //sets all the buffer values to zero
	serv_addr.sin_family = AF_INET; // always set to AF_INET
	bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length); // IP address of the host
																						// h_addr = name of the host
	serv_addr.sin_port = htons(port); // converts int port to network byte order
		
	int sockfd = socket(AF_INET, SOCK_STREAM, 0); //creates a new socket. AF_INET = Inernet domain
												//						SOCK_STREAM =stream socket type
												//						0 = most appropriate protocol
	if (sockfd < 0){
		perror("ERROR opening socket");
		exit(0);
	}
	
	if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0 ){ //establishs a connection to the server
		perror("ERROR connecting");
		exit(0);
	}
	printf("please, enter the message: \n");
	
	char buffer[256]; // to contain read characters
	bzero(buffer, 256); // set all the buffer values to zero
	fgets(buffer, 255, stdin); //reads the message from stdin
	if (write(sockfd, buffer, strlen(buffer)) < 0){ // writes the message to the socket
		perror("ERROR writing to socket");
		exit(0);
	}
	
	bzero(buffer, 256);// set all the buffer values to zero again
	if (read(sockfd, buffer, 255) < 0){ // reads the reply from the socket
		perror("ERROR reading from socket");
		exit(0);
	}
	printf("message: %s/n", buffer);
	
	close(sockfd);
	return 0;
}