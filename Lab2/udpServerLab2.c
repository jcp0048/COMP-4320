#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<netdb.h>

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

/**
Incoming Msg format:

|Magic# 0x4A6F7921| TML | GID | Checksum | RequestID | Length1 | Hostname 1 | ... | Ln | Hn |
| 	 4 bytes	  |  2  |  1  |     1    |      1    |    1    |  variable  | ... | 1  |var |
**/

/** 
The first we need to parse the first 5 elements of the incoming request.
These are everything but the Hostnames(Hn) and Hostname lengths (Ln).
**/
struct requestHeader {
	uint32_t magicNumber;
	uint16_t TML;
	uint8_t GID;
	uint8_t checksum;
	uint8_t requestID;
}__attribute__((__packed__));

struct requestContent {
		char hostInfo[65537];
		char ipArray[512];
		int reqVarCount;
}__attribute__((__packed__));


/**
	TODO: Need to make the server continuously run instead of ending after one request.
	(But I think for writing/testing purpose, this should be left until the end.)
**/

int main(int argc, char *argv[])
{
	int sockfd, portNum, newsockfd, socketReadErrorFlag, recvlen;
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t clientLength = sizeof(cli_addr);
	char buffer[65536]; // Max size is 2^16
	
    if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
	// Create a socket
	// socket(int domain, int type, int protocol)
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");
	else 
		printf("Socket seem to create sucessfully~\n");
	
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
			
    clientLength = sizeof(cli_addr);
	 
	//int recvfrom(int socket, void *restrict buffer, size_t length, int flags, struct sockaddr *restrict src_addr, socklen_t *restrict *src_len)
	recvlen = recvfrom(sockfd, buffer, 65536, 0, (struct sockaddr *)&cli_addr, &clientLength);
	if (recvlen > 0) {
        
		buffer[recvlen] = 0;
        printf("received message: \"%s\"\n", buffer);
					
		struct requestHeader inRequest; //This is the message from the client
		inRequest.magicNumber = ntohl(*(uint32_t*)(buffer));
		inRequest.TML = ntohs(*(uint16_t*)(buffer + 4));
		inRequest.GID = (uint8_t)(buffer[6]);
		inRequest.checksum = (uint8_t)(buffer[7]);
		inRequest.requestID = (uint8_t)(buffer[8]);

		/**
			TODO: Create code to check validity of incoming magic number.
			If invalid, should jump to error path.
		**/

		printf("magicNumber: %lX\n", inRequest.magicNumber); 
		printf("magicNumber: %i\n", inRequest.magicNumber); 
		printf("TML: %i\n", inRequest.TML); 
		printf("RequestID: %i\n", inRequest.requestID);
		printf("GID: %i\n", inRequest.GID);
		printf("checksum: %i\n", inRequest.checksum);		
	
		/**
			TODO: Need logic/code to check the message using the recieved checksum.
			First add all the bytes(except the checksum byte) in binary addition,
			then add the checksum byte to that result you get. 
			You should then get 1111 1111, to which the 1's compliment is 0000 0000.
			
			If bad, go to error path.
		**/
	
		/**	PUT ME BACK! FIX TML LENGTH BACK. 
			Currently the TML is spoofed for testing(The max number). 
		**/
		//int reqContentLen = inRequest.TML - 9; //Length of the contents, everything but the header
		int reqContentLen = recvlen - 9; //Can delete me, or not, dont matter.
		
		/**
			TODO: We also need to check the length of the request. Not very clear on what that means, but my guess is that the length of the request must be what it says in its TML. 
			(To accomplish this, I'd compare the value recvlen against the TML variable.)
		**/
		printf("reqContentLen: %i\n", reqContentLen);

		struct requestContent inContent;

		// Copies the remaining bytes into another array
		memcpy(inContent.hostInfo, &buffer[9], reqContentLen);
		
		int urlCopyItor = 0; // Used to iterate the array
		inContent.reqVarCount = 0; // Set the number of processed vars to 0.
		while(urlCopyItor < reqContentLen) 
		{
			//run thur each and add the length in urlCopyItor
			char urlString[512];
			char urlLength;
			urlLength = inContent.hostInfo[urlCopyItor]; // Set it first to the length of the var that follows
			urlCopyItor = urlCopyItor + 1; //For the one byte var length
			memcpy(urlString, &inContent.hostInfo[urlCopyItor], (uint8_t)urlLength);
			urlCopyItor = urlCopyItor + (uint8_t)urlLength;
			printf("urlCopyItor: %i\n", urlCopyItor);
			printf("urlLength: %i\n", (uint8_t)urlLength);
			printf("urlString: %s\n", urlString);
						
			// Get and store the IP address for the requested host/URL
			struct hostent *url2IP = gethostbyname(urlString);
			printf("Host Name->%s\n", url2IP->h_name);
			printf("IP ADDRESS->%s\n",inet_ntoa(*(struct in_addr *)url2IP->h_name) );
			int ipInsertPlace = 4 * inContent.reqVarCount;
			inContent.ipArray[ipInsertPlace] = inet_ntoa(*(struct in_addr *)url2IP->h_name);
			
			inContent.reqVarCount = inContent.reqVarCount + 1; // Increase our count of number of host/ip addresses.
			
			bzero(urlString,512);
		}

		/**
			TODO: Need to make the fail response and unhappy path now. :(
		**/
		
		struct successResponse {
			uint32_t magicNumber;
			//unsigned long magicNumber;
			uint16_t TML;
			uint8_t GID;
			uint8_t checksum;
			uint8_t requestID;
			char IPs[(inContent.reqVarCount * 4)];
		}__attribute__((__packed__));
		
		struct successResponse returnMsg;
		returnMsg.magicNumber = htonl(1248819489); // The magic number
		returnMsg.TML = htons(9 + (inContent.reqVarCount * 4));
		returnMsg.GID = inRequest.GID;
		returnMsg.checksum = inRequest.checksum;
		returnMsg.requestID = inRequest.requestID;
		
		memcpy(returnMsg.IPs, &inContent.ipArray[0], inContent.reqVarCount);
		
		sendto(sockfd, (void *) &returnMsg, returnMsg.TML, 0, (struct sockaddr *)&cli_addr, clientLength);
		
	}
	 
    close(sockfd);
    return 0;
}