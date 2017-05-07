#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#define SOC_SIZE sizeof(struct sockaddr_in)

struct sockaddr_in createSocketAddr(char *ipAddr, short port);
int createAndBind(short port);
void sendToAddr(
	int localSoc, void* buffer,
	size_t len, struct sockaddr_in *addr
);
void receiveFrom(int socket, void *buffer, size_t len, struct sockaddr_in *senderAddr);

int createAndBind(short port){
	int socketId = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in localSoc = createSocketAddr("localhost", port);
	bind(socketId, (struct sockaddr *)&localSoc, sizeof(localSoc));
	return socketId;
}

struct sockaddr_in createSocketAddr(char *ipAddr, short port){
	struct sockaddr_in sockAddr;
    bzero(&sockAddr, sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(port);

	if(strcmp("localhost", ipAddr) != 0){
		inet_pton(AF_INET, ipAddr, &sockAddr.sin_addr);
	}
	return sockAddr;
}

void sendToAddr(
	int localSoc, void* buffer,
	size_t len, struct sockaddr_in *addr){
	
	int sendResult;
	sendResult = 
	printf("localSoc ID: %d\n", localSoc);
	sendto(localSoc, buffer, len,
    0, (struct sockaddr *)addr, SOC_SIZE);
    if(sendResult == -1){
    	printf("sendto error! errno: %d\n", errno);
    }
}

void receiveFrom(int socket, void *buffer, 
	size_t len, struct sockaddr_in *senderAddr){

	int sockLen = (senderAddr == NULL) ? 0 : SOC_SIZE;
	static int recResult;
	recResult = 
	recvfrom(socket, buffer, len, 0, (struct sockaddr*)senderAddr, &sockLen);
	if(recResult == -1){
		printf("recvfrom error! \n");
	}
}