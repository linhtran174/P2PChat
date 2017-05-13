#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "threadHelper.c"
#include "socketHelper.c"

//stun.l.google.com:19302
#define STUN_G "64.233.188.127"
//xseed.tech
#define XSEED "45.119.81.200"


int registerNewUser(const char* name, const char* ipAddress, short* port);
void* keepAliveService(void* socket); //keep port open on router
void stunService(Socket me, char *returnIp, unsigned short *returnPort);

Socket me;

int main(){
	char a[10];
	short b[20];

	//create local socket with port 12345
	me = newSocket("localhost", "12345");

	char publicAddr[16];
	unsigned short openedPort;
	stunService(me, publicAddr, &openedPort);

	printf("DAPNAT Client - %s:%d\n", publicAddr, openedPort);
	printf("Please enter your name (A-Z | 1-9 | less than 100 characters): ");

	char buf[1000];
	fgets(buf, 1000, stdin);

	registerNewUser(buf, publicAddr, &openedPort);



	pthread_exit(NULL);
}

//////////////////////CODE////////////////////
int error(char *message);
int registerNewUser(const char* name, const char* ipAddress, short* port)
{
	// const char *portStr = itoa
	// int messageLength = 
	// char message[strlen(name) + strlen(ipAddress)] = "REG "
	// sendToAddr()

	return 0;
}

void stunService(Socket me, char *returnIp, unsigned short *returnPort){
	int localSoc = me->systemSocketId;
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
		// printf("STUN binding resp: success !\n");
		if (attr_type == 0x0020)
		{
			// parse : port, IP 
			*returnPort = htons(*(short *)&buf[26]) ^ 0x2112;
			sprintf(returnIp,"%d.%d.%d.%d",buf[28]^0x21,buf[29]^0x12,buf[30]^0xA4,buf[31]^0x42);
			printf("Public address: %s:%d\n", returnIp, *returnPort);
		}
		//prepare arg for thread
		void *arg = malloc(sizeof(int));
		*((int*)arg) = localSoc;
		runThread(&keepAliveService, arg);
	}
}

void* keepAliveService(void *_socket ){
	int socket = *(int *)_socket;

	//create dump server address
	struct sockaddr_in dumpServer;
	dumpServer = createSocketAddr("localhost", 3478);

	while(1){
		sleep(10);
		char nothing[] = "keepAlive";
    	sendToAddr(socket, nothing, 10, &dumpServer);
	}
}

int error(char *message){
	printf("ERROR: %s\n", message);
	return -1;
}
