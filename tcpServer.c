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

//Below the structs for return messages are defined
struct cLengthMessage {
	char TML;
	char RequestID;
	char Operation;
	char length;
};
struct disvowUpcaseMessage {
	char TML;
	char RequestID;
	char ReturnString[256];
};

//char * toUpperCase(char inputArray[]) {
//	int i = 0;
//	while(inputArray[i])
//   {
//      putchar (toupper(inputArray[i]));
//      i++;
//   }
//   return inputArray[];
//}

int main(int argc, char *argv[])
{
	int sockfd, portNum, newsockfd, socketReadErrorFlag;
	struct sockaddr_in serv_addr;
	struct sockaddr cli_addr;
	//socklen_t clientLength;
	socklen_t clientLength;
	char buffer[256];
	//char msgStr[];


    if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
	// Create a socket
	// socket(int domain, int type, int protocol)
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
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
		sizeof(serv_addr)) < 0) {
			error("ERROR on binding\n");
			exit(1);
		}

    // This listen() call tells the socket to listen to the incoming connections.
    // The listen() function places all incoming connection into a backlog queue
    // until accept() call accepts the connection.
    // Here, we set the maximum size for the backlog queue to 5.
    listen(sockfd,5);

    // The accept() call actually accepts an incoming connection
    //clientLength = sizeof(cli_addr);
	printf("Listened and got length\n");

    // This accept() function will write the connecting client's address info
    // into the the address structure and the size of that structure is clientLength.
    // The accept() returns a new socket file descriptor for the accepted connection.
    // So, the original socket file descriptor can continue to be used
    // for accepting new connections while the new socker file descriptor is used for
    // communicating with the connected client.
    //newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clientLength);
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clientLength);
    if (newsockfd < 0)  {
        error("ERROR on accept");
		exit(1);
	}
	else {
		printf("No error past accept() and newsockfd > 0");
	}

	//printf("server: got connection from %s port %d\n",
    //        inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));





	while(1) {
		bzero(buffer,256);

		socketReadErrorFlag = read(newsockfd,buffer,255);
		if (socketReadErrorFlag < 0)
			error("ERROR reading from socket");
		printf("Here is the message: %s\n",buffer);
		printf("Here is the message: %x\n",buffer);
		uint8_t TML = *(uint8_t *)(buffer);
		uint8_t requestID = (uint8_t)(buffer[1]);
		uint8_t operation = (uint8_t) (buffer[2]);
		int varLen = TML - 3;
		char recvdVar[varLen];
		//= buffer[3:(TML -1)];
		memcpy(recvdVar, &buffer[3], varLen);
		printf("recvdVar: %s\n", recvdVar);
		printf("TML: %i\n", TML);
		printf("RequestID: %i\n", requestID);
		printf("Operation: %i\n", operation);
		//bzero(buffer,256);

		//char upperString[recvdVar]; = strupr(recvdVar); // String that is turned to upper case
		char upperString[varLen]; // String that is turned to upper case
		//memcpy(upperString, recvdVar, varLen);
		int i = 0;
		while((recvdVar[i]))
		{
			upperString[i] = toupper(recvdVar[i]);
			//memcpy(&upperString[i],toupper(upperString[i]) , 1);
			i++;
		}
		//upperString = strupr(recvdVar);
		if(operation == 3) {	//this is where UPPERCase gets off
			bzero(buffer,256);
			printf("inside operation 3 \n");
			buffer[0] = (char)(varLen + 2);
			buffer[1] = (char)(requestID);
			memcpy(&buffer[2], upperString, varLen);
			printf("upperString: %s \n", upperString);
			send(newsockfd, buffer, TML, 0);
			printf("sent buffer: %s \n", buffer);
			printf("sent buffer: %x \n", buffer);

			bzero(buffer,256);
		}

		if(operation == 2) //this is for Disemvoweling
		{
			bzero(buffer,256);
			printf("inside operation 2 \n");
			char line[150];
			char temp[150];
			printf("Enter a line of string: ");
			scanf("%[^\]", line);
			for(i=0; line[i]!='\0'; ++i)
			{
        if(line[i]!='a' || line[i]!='e' || line[i]!='i' ||
           line[i]!='o' || line[i]!='u' || line[i]!='A' ||
           line[i]!='E' || line[i]!='I' || line[i]!='O' ||
           line[i]!='U')
					 {
						 temp[i] = line[i];
					 }
      {
			send(newsockfd, buffer, TML, 0);
		}

		if(operation == 1){ //this is for cLength
			bzero(buffer,256);
			printf("inside operation 1 \n");
			char line[150];
			int consonants = 0;
			printf("Enter a line of string: ");
			scanf("%[^\]", line);

			for(i=0; line[i]!='\0'; ++i)
			{
				if((line[i]>='a'&& line[i]<='z') || (line[i]>='A'&& line[i]<='Z'))
				{
					++consonants;
				}
			}
			printf("\nConsonants: %d",consonants);

		// switch(operation){
		//
		// 	case 1:	// Number of consonants
		// 	case 3:
		// 		int cCount = 0;
		// 		//for(int i = 0; i < varLen; i++){
		// 		//	if(recvdVar[i] == '')
		// 		//}
		// 		int i = 0;
		// 		while(recvdVar[i])
		// 		{
		// 			putchar(toupper(inputArray[i]));
		// 			i++;
		// 		}
		// 		break;
		// 	case 2:
		//
		// 		break;
		// 	default:
		// 		printf("Operation not recognized");
		//
		// }
		bzero(buffer,256);
		printf("Please type a message in response:\n");
		//fgets(buffer, 256, stdin);
		//msgStr = "Connection Established!";
		strcpy(buffer, "Connection Established!") ;
		close(sockfd);
		// This send() function sends the 13 bytes of the string to the new socket
		//send(newsockfd, "Hello, world!\n", 13, 0);
		send(newsockfd, buffer, sizeof(buffer), 0);
		printf("1 to exit: \n");
		int exitcode;
		scanf("%d", &exitcode);
		if(exitcode == 1) {
			close(newsockfd);
			exit(1);
		}
	}




    close(newsockfd);


    //printf("Hello world!\n");
    return 0;
}
