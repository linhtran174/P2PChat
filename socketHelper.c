#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#define SOC_SIZE sizeof(struct sockaddr_in)
#define IP_STRING_SIZE 16  //ddd.ddd.ddd.ddd
#define PORT_STRING_SIZE 6 //65535

//////////////////////////////LEVEL 2 API: EASY TO USE SOCKETS///////////////////////////////////////////
typedef struct _dapNatSocket{
	char *ip;
	char *port;
	int systemSocketId;
}*Socket;
Socket newSocket(char *ip, char *port);
void sendTo(Socket me, Socket receiver, char* message);
void receive(Socket me, Socket sender, char* message);






















//////////////////////////////LEVEL 1 API: HELPER FUNCTIONS//////////////////////////////////////////////
struct sockaddr_in createSocketAddr(char *ipAddr, short port);
int createAndBind(short port);
void sendToAddr(int localSoc, void* buffer, size_t len, struct sockaddr_in *addr);
void receiveFrom(int socket, void *buffer, size_t len, struct sockaddr_in *senderAddr);



////////////CODE//////////////////////////////////////////////////////////////////////
Socket newSocket(char *ip, char *port){
	Socket soc = (Socket)malloc(sizeof(struct _dapNatSocket));
	if(!strcmp("localhost", ip)){
		soc->systemSocketId = createAndBind(atoi(port));
	}
	soc->ip = (char *)malloc(IP_STRING_SIZE); 
	strcpy(soc->ip, ip);
	soc->port = (char *)malloc(PORT_STRING_SIZE); 
	strcpy(soc->port, port);

	return soc;
}

void sendTo(Socket me, Socket receiver, char *message){
	// printf("sendTo: \'%s\', length: %d\n", message, (int)strlen(message));
	struct sockaddr_in receiverAddr = createSocketAddr(receiver->ip, atoi(receiver->port));
	sendToAddr(me->systemSocketId, (void*) message, strlen(message) + 1, &receiverAddr);
}

void receive(Socket me, Socket sender, char *message){
	struct sockaddr_in senderAddr;
	receiveFrom(me->systemSocketId, (void*) message, strlen(message), &senderAddr);
	sprintf(sender->port, "%d", ntohs(senderAddr.sin_port));
	inet_ntop(AF_INET, &senderAddr.sin_addr, sender->ip, IP_STRING_SIZE);
	// printf("received message: \'%s\', length: %d\n", message, (int)strlen(message));
}

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
	
	int sendResult = 
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