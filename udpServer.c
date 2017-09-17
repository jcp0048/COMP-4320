#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	int sockfd, portNum, newsockfd, socketReadErrorFlag, recvlen;
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t clientLength;
	char buffer[256];
	
    if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
	// Create a socket
	// socket(int domain, int type, int protocol)
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");

	// clear address structure
    bzero((char *) &serv_addr, sizeof(serv_addr));
	
	portNum = atoi(argv[1]);

	// setup host addr structure for us in bind call
	// Server byte order
	serv_addr.sin_family = AF_INET;
	
	// Convert short integer value for port must be converted into network byte order
	serv_addr.sin_port = htons(portNum);
	
	// automatically be filled with current host's IP address
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
    // bind(int fd, struct sockaddr *local_addr, socklen_t addr_length)
    // bind() passes file descriptor, the address structure, 
    // and the length of the address structure
    // This bind() call will bind  the socket to the current IP address on port, portNum
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
		sizeof(serv_addr)) < 0) 
			error("ERROR on binding\n");

    // This listen() call tells the socket to listen to the incoming connections.
    // The listen() function places all incoming connection into a backlog queue
    // until accept() call accepts the connection.
    // Here, we set the maximum size for the backlog queue to 5.
    listen(sockfd,5);			
			
    // The accept() call actually accepts an incoming connection
    clientLength = sizeof(cli_addr);
	 
	//int recvfrom(int socket, void *restrict buffer, size_t length, int flags, struct sockaddr *restrict src_addr, socklen_t *restrict *src_len)
	recvlen = recvfrom(sockfd, buffer, 256, 0, (struct sockaddr *)&cli_addr, &clientLength);
	if (recvlen > 0) {
        buffer[recvlen] = 0;
        printf("received message: \"%s\"\n", buffer);
					
		// Sets return message 
		printf("Please type a message in response:\n");
		fgets(buffer, 256, stdin);
						
		sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&cli_addr, clientLength);
						
	}
	 
    close(sockfd);
    return 0;
}
