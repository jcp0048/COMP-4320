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

struct NormalMsg {
	char message[512]; // Just a message.
}__attribute__((__packed__));


struct UdpMsg {
	//char magicNumber[4]; // The magicNumber in ASCII.
	uint32_t magicNumber; // The magicNumber 0x4A6F7921.
	//uint8_t Ph;	// The most significant byte of port.
	//uint8_t Pl;	// The least significant byte of port.
	uint16_t tcpPort;	// Here we'll just use this unsigned 16 bit since we are just having to extract it from the server side, and not this side.
	uint8_t GID;	// The ID of the group.
}__attribute__((__packed__));

struct UdpReply { // For when maybe told to wait, maybe error msgs.
	//char magicNumber[4]; // The magicNumber in ASCII.
	uint32_t magicNumber; // The magicNumber 0x4A6F7921.
	uint8_t GID;	// The GID of the server.
	uint8_t Ph;	// The most significant byte of port.
	uint8_t Pl;	// The least significant byte of port.

}__attribute__((__packed__));

struct UdpWaitReply { // For when told to wait.
	//char magicNumber[4]; // The magicNumber in ASCII.
	uint32_t magicNumber; // The magicNumber 0x4A6F7921.
	uint8_t GID;	// The GID of the server.
	uint16_t waitPort;	// Port we are told to wait at.

}__attribute__((__packed__));

struct UdpInvite { // For when given chat partner info.
	//char magicNumber[4]; // The magicNumber in ASCII.
	uint32_t magicNumber; // The magicNumber 0x4A6F7921.
	struct in_addr waitingIP; // IP addr of waiting client.
	uint16_t waitingPort;	// Port of client who is waiting to chat.
	uint8_t GID;	// The GID of the server.
	
}__attribute__((__packed__));

int main(int argc, char *argv[])
{
	if (argc < 4) {
		//fprintf(stderr,"ERROR, no port provided\n");
		printf("please give these 3 arguments: ServerName ServerPort MyPort\n");
		exit(1);
	}
	
	short serverPort = atoi(argv[2]);
	short tcpPort = atoi(argv[3]);
	uint8_t GID = 21;
	
	// 0x4A6F7921 == 1248819489 == Joy!
	
	
	
	//char *serverHost = argv[1];
	char serverHost[512];
	//memcpy(serverHost, argv[1], sizeof(&argv[1]));
	//printf("sizeof(argv[1])%i\n", sizeof(&argv[1]));
	printf("serverPort is %s\n", argv[1]);
	//printf("could not obtain address of %s\n", serverHost);
	
	// Data members
	int sockfd, portNum, newsockfd, socketReadErrorFlag, recvlen;
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t clientLength = sizeof(cli_addr);
	char buffer[512]; // Probably have to change this.
	//int pKeyC; // This user's key with the Authenticator
	//int decrypC; // The decryption key needed to decrypt C messages. (public key?)
	//
	//double Ks; // The session key.
	//// Messages to B
	//struct AtoBInitMsg initMsgToB;
	//struct AnBConfirmMsg confirmMsg;
	
	portNum = 10115; // Need to change.
	
	//fprintf(stderr, "could not obtain address of %s\n", serverHost);
	
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");
	else 
		printf("Socket seem to create sucessfully~\n");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	struct hostent *hp;     /* host information */
	struct sockaddr_in servaddr;    /* server address */
	//char *my_messsage = "this is a test message";
	//string my_message = "this is a test message";
	
	/* fill in the server's address and data */
	memset((char*)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(portNum);
	
	/* look up the address of the server given its name */
	hp = gethostbyname(argv[1]);
	if (!hp) {
		fprintf(stderr, "could not obtain address of %s\n", argv[1]);
		//std::cout << stderr << "could not obtain address of %s\n" << cathyHost;
		//return 0;
	}
	
	/* put the host's address into the server address structure */
	memcpy((void *)&servaddr.sin_addr, hp->h_addr_list[0], hp->h_length);

	//struct AtoCMsg msgToC; // Message to Cathy
	//msgToC.aID = 1;
	//msgToC.bID = 2;
	//msgToC.r1 = (rand() % 32) + 1;
	//
	//cout << "Create and send 1st request msg to Cathy" << endl;
	//cout << "msgToC.aID: " << (unsigned int)msgToC.aID << "\n";
	//cout << "msgToC.bID: " << (unsigned int)msgToC.bID << "\n";
	//cout << "msgToC.r1: " << (unsigned int)msgToC.r1 << "\n";
	
	struct UdpMsg msg2Server; // Message to send to server.
	/**
	Format to UDP Server: 
	
	0x4A6F7921 Ph Pl GID
	
	magicNumber = 0x4A6F7921
	Ph is the most significant byte of port
	Pl is the least significant byte of port
	GID = 1 byte of GID
	**/
	//char portNumBuffer[5]; // We'll just use a length of 5 since we will pretend we can only have 5 digit port numbers.
	//memcpy(portNumBuffer, argv[4], 5); // We will copy 5 values as chars from the 4th argument, which is suppose to be the TCP port number.
	//char portNumBuffer
	//memcpy(msg2Server.magicNumber, "Joy!", 4);
	msg2Server.magicNumber = htonl((uint32_t)(0x4A6F7921));
	msg2Server.tcpPort = htons((uint16_t)(tcpPort));	// Important here that we use the network byte order.
	msg2Server.GID = GID;
	
	// Here... Since in big Endian, MSB is stored in the smallest address and LSB is stored in the last, I guess that means in a port number 12345, the MSB will be 1, and LSB will be 5. 
	// But I honestly don't understand what he means here because the port number is only a byte... 
	
	
	struct NormalMsg msg; // A normal message.
	
	
	memcpy(msg.message, "Hello Server!", 13);
	//msg.message = "Hello Server!";
	
	/* send a message to the server */
	//if (sendto(sockfd, my_message.c_str(), strlen(my_message.c_str()), 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
	//if (sendto(sockfd, (void *) &msg, 13, 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
	if (sendto(sockfd, (void *) &msg2Server, 7, 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("sendto failed");
		return 0;
	}
	
	struct UdpReply waitOrError;
	struct UdpWaitReply UdpWaitInfo;
	struct UdpInvite chatInstr;
	
	listen(sockfd,5);
	clientLength = sizeof(cli_addr);
	recvlen = recvfrom(sockfd, buffer, 512, 0, (struct sockaddr *)&cli_addr, &clientLength);
	//recvlen = recvfrom(sockfd, &waitOrError, 7, 0, (struct sockaddr *)&cli_addr, &clientLength);
	if (recvlen > 0) {
		printf("recvlen %i\n", recvlen);
		//int recvlen2 = recvfrom(sockfd, &waitOrError, 7, 0, (struct sockaddr *)&cli_addr, &clientLength);
		if (recvlen == 7) { // This is either an error or a wait request
			memcpy(&waitOrError, buffer, 7);
			printf("waitOrError.magicNumber %x\n", waitOrError.magicNumber);
			printf("waitOrError.Ph: %i\n", waitOrError.Ph);
			if(waitOrError.Ph == 0) { // This means this is an ERROR message.
				printf("There was an error in our request. \n");
				printf("Error Code(waitOrError.Pl) is: %x\n", waitOrError.Pl);
			}
			else { // We received a message to tell us to wait.
				printf("The request was successful, we are told to wait for a partner. \n");
				memcpy(&UdpWaitInfo, buffer, 7);
				printf("UdpWaitInfo.magicNumber %x\n", UdpWaitInfo.magicNumber);
				printf("UdpWaitInfo.GID %i\n", UdpWaitInfo.GID);
				printf("UdpWaitInfo.waitPort %i\n", UdpWaitInfo.waitPort);
				printf("UdpWaitInfo.waitPort non-Network byte order %i\n", ntohs((uint16_t)(UdpWaitInfo.waitPort)));
			}
			printf("Does waitOrError.magicNumber == Joy! ?\n");
			printf("waitOrError.magicNumber: %x\n", waitOrError.magicNumber);
			//printf("&waitOrError.magicNumber[0]: %s\n", &waitOrError.magicNumber[0]);
			//printf("&waitOrError.magicNumber[1]: %s\n", &waitOrError.magicNumber[1]);
			//printf("&waitOrError.magicNumber[2]: %s\n", &waitOrError.magicNumber[2]);
			//printf("&waitOrError.magicNumber[3]: %s\n", &waitOrError.magicNumber[3]);
			
		}
		else if (recvlen > 7) { // This is an invite ?
			printf("Recieved message longer than 7 bytes, suspect is a chat invite. \n");
			memcpy(&chatInstr, buffer, 11);
			printf("chatInstr.magicNumber %x\n", chatInstr.magicNumber);
			//printf("chatInstr.waitingIP %s\n", chatInstr.waitingIP);
			printf("chatInstr.waitingIP non-Network byte order %s\n", inet_ntoa(chatInstr.waitingIP));
			printf("chatInstr.waitingPort %i\n", chatInstr.waitingPort);
			printf("chatInstr.waitingPort non-Network byte order %i\n", ntohs((uint16_t)(chatInstr.waitingPort)));
			printf("chatInstr.GID %i\n", chatInstr.GID);
		}
		else {
			printf("There are errors with the response that came back. Length is shorter than 7. \n");
		}
				
		
		//printf("recvlen2 %i\n", recvlen2);
	}
	//listen(sockfd,5); // Listen for Cathy's reply
	//		
	//clientLength = sizeof(cli_addr);
	//
	////int recvfrom(int socket, void *restrict buffer, size_t length, int flags, struct sockaddr *restrict src_addr, socklen_t *restrict *src_len)
	//struct CtoAMsg replyFromC;
	//
	//recvlen = recvfrom(sockfd, &replyFromC, 384, 0, (struct sockaddr *)&cli_addr, &clientLength);
	//if (recvlen > 0) {
	//	
	//	cout << "Now we check the reply from Cathy" << endl;
	//	cout << "replyFromC.enAID:" << (double)replyFromC.enAID << endl;
	//	cout << "replyFromC.enBID:" << (double)replyFromC.enBID << endl;
	//	cout << "replyFromC.enR1:" << (double)replyFromC.enR1 << endl;
	//	cout << "replyFromC.ks:" << (double)replyFromC.ks << endl;
	//	cout << "replyFromC.enAIDbc:" << (double)replyFromC.enAIDbc << endl;
	//	cout << "replyFromC.enKsbc:" << (double)replyFromC.enKsbc << endl;
	//	
	//	
	//	
	//	// Decrypt Reply
	//	cout << "replyFromC.enAID Decrypted:" << deRSA((double)replyFromC.enAID, pKeyC, decrypC) << endl;
	//	cout << "replyFromC.enBID Decrypted:" << deRSA((double)replyFromC.enBID, pKeyC, decrypC) << endl;
	//	cout << "replyFromC.enR1 Decrypted:" << deRSA((double)replyFromC.enR1, pKeyC, decrypC) << endl;
	//	Ks = deRSA((double)replyFromC.ks, pKeyC, decrypC);
	//	cout << "replyFromC.ks Decrypted:" << Ks << endl;
	//	initMsgToB.enAIDbc = deRSA((double)replyFromC.enAIDbc, pKeyC, decrypC);
	//	initMsgToB.enKsbc = deRSA((double)replyFromC.enKsbc, pKeyC, decrypC);
	//	cout << "replyFromC.enAIDbc Decrypted:" << initMsgToB.enAIDbc << endl;
	//	cout << "replyFromC.enKsbc Decrypted:" << initMsgToB.enKsbc << endl;
	//	
	//}
	close(sockfd);
}