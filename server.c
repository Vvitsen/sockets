#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>
#include <fcntl.h>

void messaging(int fd);

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
	int reuseaddr = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr)); // bind() should allow reuses local addresses

	if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL) | O_NONBLOCK) < 0){  // sets socket to non-blocking
		perror("ERROR on fcntl");
		exit(1);
	}

	if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0 ){
		perror("ERROR on biding");
		exit(1);
	}

	listen(sockfd, 5);// listen on the socket for connection

	fd_set sockfds; // set of sockfd
	int connectlist[5]; // array for connected sockfds
	memset((char*)&connectlist, 0, sizeof(connectlist));
	int highfd = sockfd;
	struct timeval timeout;

	while(1){

		FD_ZERO(&sockfds); // clean set
		FD_SET(sockfd, &sockfds); // adds descriptor sockfd to the set

		int i;

		for (i = 0; i < sizeof(connectlist)/4; i++){ //
			if (connectlist[i] != 0){
				FD_SET(connectlist[i], &sockfds); // adds all descriptors from array to the set
			}
		}

		for (i = 0; i < sizeof(connectlist)/4; i++){ // to find highest fd in the array
			if (connectlist[i] > highfd){
				highfd = connectlist[i];
			}
		}

		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		int ready = select(highfd + 1, &sockfds, (fd_set*)0, (fd_set*)0, &timeout);// return the number of file descriptors
																					// contained in sockfds (readfds)
		/* After select() is returned, sockfds will be cleared of all descriptors except for those that a ready for reading */

		if (ready < 0){
			perror("ERROR in select");
			exit(1);
		}
		if (ready == 0){
			printf(".");
			fflush(stdout);
		}
		else{

			if (FD_ISSET(sockfd, &sockfds)){  // if sockfd is contained in sockfds set
												// search new connection

				int connectfd = accept(sockfd, NULL, NULL); // returns a new connected socket file desciptor
				if (connectfd < 0){
					perror("ERROR on accept");
					exit(1);
				}

				if (fcntl(connectfd, F_SETFL, fcntl(connectfd, F_GETFL) | O_NONBLOCK) < 0){  // sets connected socket to non-blocking
					perror("ERROR on fcntl");
					exit(1);
				}

				for (i = 0; i < sizeof(connectlist)/4; i++){
					if (connectlist[i] == 0){		// if there is vacant place in array
						connectlist[i] = connectfd; // adds new connected socket file descriptor to array
						connectfd = -1;
						printf("\n Connection accepted FD=%d slot=%d", connectlist[i], i);
						break;
					}
				}
			}

			for (i = 0; i < sizeof(connectlist)/4; i++){
				if (FD_ISSET(connectlist[i], &sockfds)){
					messaging(connectlist[i]);
				}
			}

		}
	}

//	struct sockaddr_in cli_addr;
//	socklen_t clien = sizeof(cli_addr);

//	while(1){
//		int newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clien); //creates a new connected socket
//		if (newsockfd < 0){
//			perror("ERROR on accept");
//			exit(1);
//		}
//		int pid = fork();
//		if (pid < 0){
//			perror("ERROR on fork");
//			exit(1);
//		}
//		if (pid == 0){
//			close(sockfd);
//			messaging(newsockfd);
//			exit(1);
//		}
//		else close(newsockfd);
//	}

//	char buffer[256]; // to contain read characters
//	bzero(buffer, 256);
//
//	if (read(newsockfd,buffer, 255) < 0){
//		perror("ERROR reading from socket");
//		exit(1);
//	}
//	printf("message: %s/n", buffer);
//
//	if (write(newsockfd, "I got your message\n", 20) < 0){
//		perror("ERROR writing to socket");
//		exit(1);
//	}

//	close(newsockfd);
	close(sockfd);
	return 0;
}

void messaging(int fd){

	char buffer[256]; // to contain read characters
	bzero(buffer, 256);

	if (read(fd,buffer, 255) < 0){
		perror("ERROR reading from socket");
		exit(1);
	}
	printf(" - %s", buffer);

	if (write(fd, "I got your message\n", 20) < 0){
		perror("ERROR writing to socket");
		exit(1);
	}
}
