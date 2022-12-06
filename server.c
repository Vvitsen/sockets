#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>



int main(int argc, char* argv[]){

	if (argc < 2){
		fprintf(stderr, "ERROR, no port provided \n");
	}
	int port = atoi(argv[1]);



	struct sockaddr_in serv_addr; // is a structure, containing an internet address (<netinet/in.h>)
	bzero((char*)&serv_addr, sizeof(serv_addr)); //sets all the buffer values to zero
	serv_addr.sin_family = AF_INET; // always set to AF_INET
	serv_addr.sin_addr.s_addr = INADDR_ANY; // IP address of the host
	serv_addr.sin_port = htons(port); // conwert to network byte order

	int sockfd = socket(AF_INET, SOCK_STREAM, 0); //creates a new socket. AF_INET = Inernet domain
											//						SOCK_STREAM =stream socket type
											//						0 = most appropriate protocol
	if (sockfd < 0){
		perror("ERROR opening socket");
		exit(1);
	}

	if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0 ){
		perror("ERROR on biding");
		exit(1);
	}

	listen(sockfd, 5);// listen on the socket for connection

	struct sockaddr_in cli_addr;
	socklen_t clien = sizeof(cli_addr);
	int newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clien); //creates a new connected socket
	if (newsockfd < 0){
		perror("ERROR on accept");
		exit(1);
	}

	char buffer[256]; // to contain read characters
	bzero(buffer, 256);

	if (read(newsockfd,buffer, 255) < 0){
		perror("ERROR reading from socket");
		exit(1);
	}
	printf("message: %s/n", buffer);

	if (write(newsockfd, "I got your message\n", 20) < 0){
		perror("ERROR writing to socket");
		exit(1);
	}

	close(newsockfd);
	close(sockfd);
	return 0;
}
