#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include "threadHelper.c"
#include "socketHelper.c"

//stun.l.google.com:19302
#define STUN_G "64.233.188.127"
//xseed.tech
#define XSEED "45.119.81.200"
#define LOCALHOST "127.0.0.1"


int registerNewUser(const char* name, const char* ipAddress, short* port);
void* keepAliveService(void* socket); //keep port open on router
void stunService(int localSoc, char *returnIp, unsigned short *returnPort);


int main(){
	char a[10];
	short b[20];

	//create local socket with port 12345
	int localSoc = createAndBind(12345);

	char publicAddr[16];
	short openedPort;
	stunService(localSoc, publicAddr, &openedPort);

	// registerNewUser("Linh Tran", publicAddr, &openedPort);


	pthread_exit(NULL);
}

//////////////////////CODE////////////////////
int error(char *message);
int registerNewUser(const char* name, const char* ipAddress, short* port)
{
	

	return 0;
}

void* messageReceiver(void * _socket){
	int socket = *(int *)_socket;
	char messageBuffer[1000];

	struct sockaddr_in senderAddr;
	socklen_t addrLength = sizeof(senderAddr);

	while(1){
		recvfrom(socket, messageBuffer, 1000, 0, (struct sockaddr *)&senderAddr, &addrLength);
		printf("%s\n", messageBuffer);
	}
}

void stunService(int localSoc, char *returnIp, unsigned short *returnPort){
	//create server socket - STUN port 3478
	struct sockaddr_in serverAddr;
	serverAddr = createSocketAddr("localhost", 3478);

    //create STUNRequest
    unsigned char STUNRequest[20];
	* (short *)(&STUNRequest[0]) = htons(0x0001);    // stun_method
	* (short *)(&STUNRequest[2]) = htons(0x0000);    // msg_length
	* (int *)(&STUNRequest[4])   = htonl(0x2112A442);// magic cookie

	*(int *)(&STUNRequest[8]) = htonl(0x63c7117e);   // transacation ID 
	*(int *)(&STUNRequest[12])= htonl(0x0714278f);
	*(int *)(&STUNRequest[16])= htonl(0x5ded3221);

    //send request
    sendToAddr(localSoc, STUNRequest, sizeof(STUNRequest), &serverAddr); // send UDP

	//read response
	unsigned char buf[300]; //buffer
    receiveFrom(localSoc, buf, 300, NULL); // recv UDP

	// parse response
	short STUNSuccess = (*(short *)(&buf[0])) == htons(0x0101);
	short attr_type = htons(*(short *)(&buf[20]));
	short attr_length = htons(*(short *)(&buf[22]));
	if (STUNSuccess)
	{	
		printf("STUN binding resp: success !\n");
		if (attr_type == 0x0020)
		{
			// parse : port, IP 
			*returnPort = htons(*(short *)&buf[26]) ^ 0x2112;
			sprintf(returnIp,"%d.%d.%d.%d",buf[28]^0x21,buf[29]^0x12,buf[30]^0xA4,buf[31]^0x42);
			printf("Public address: %s:%d\n", returnIp, *returnPort);
		}
		runThread(&keepAliveService, (void*)&localSoc);
	}
}

void* keepAliveService(void *_socket ){
	int socket = *(int *)_socket;

	//create dump server address
	struct sockaddr_in dumpServer;
	dumpServer = createSocketAddr("localhost", 3478);

	while(1){
		char nothing[] = "keepAlive";
    	sendToAddr(socket, nothing, 10, &dumpServer);
		sleep(10);
	}
}

int error(char *message){
	printf("ERROR: %s\n", message);
	return -1;
}
