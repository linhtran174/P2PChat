#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

//stun.l.google.com:19302
#define STUN_G "64.233.188.127"
//xseed.tech
#define XSEED "45.119.81.200"
#define LOCALHOST "127.0.0.1"


int registerNewUser(const char* name, const char* ipAddress, short* port);
void keepAlive(int socket); //keep UDP socket alive (port remains constants)
void stunService(int localSoc, char *returnIp, unsigned short *returnPort);


int main(){
	char a[10];
	short b[20];

	//create local socket with port 12345
	int localSoc = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in localAddr;
    bzero(&localAddr, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(12345);
    // inet_pton(AF_INET, "127.0.0.1", &localAddr.sin_addr);
	bind(localSoc, (struct sockaddr *)&localAddr, sizeof(localAddr));


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

	//create server socket
	struct sockaddr_in serverAddr;
	char *stun_server_ip = LOCALHOST;
	unsigned short stun_server_port = 3478;
    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, stun_server_ip, &serverAddr.sin_addr);
    serverAddr.sin_port = htons(stun_server_port);

    //create STUNRequest
    unsigned char STUNRequest[20];
	* (short *)(&STUNRequest[0]) = htons(0x0001);    // stun_method
	* (short *)(&STUNRequest[2]) = htons(0x0000);    // msg_length
	* (int *)(&STUNRequest[4])   = htonl(0x2112A442);// magic cookie

	*(int *)(&STUNRequest[8]) = htonl(0x63c7117e);   // transacation ID 
	*(int *)(&STUNRequest[12])= htonl(0x0714278f);
	*(int *)(&STUNRequest[16])= htonl(0x5ded3221);

    //send request
    short requestResult =
    sendto(localSoc, STUNRequest, sizeof(STUNRequest),
    0, (struct sockaddr *)&serverAddr, sizeof(serverAddr)); // send UDP
	if (requestResult == -1){
		error("cannot send STUN request");
		return;
	}

	//read response
	unsigned char buf[300]; //buffer
    requestResult = recvfrom(localSoc, buf, 300, 0, NULL, 0); // recv UDP
    if (requestResult == -1){
    	error("cannot receive any response");
    	return;	
    }

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
		keepAlive(localSoc);
	}
}

void* sendKeepAlive(void *_socket){
	int socket = *(int *)_socket;

	//create dump server address
	struct sockaddr_in dumpServer;
    bzero(&dumpServer, sizeof(dumpServer));
    dumpServer.sin_family = AF_INET;
    inet_pton(AF_INET, XSEED, &dumpServer.sin_addr);
    dumpServer.sin_port = htons(80);	

	while(1){
		printf("sending keep-alive\n");
		char nothing[] = "keepAlive";
		sendto(socket, nothing, 9,
    	0, (struct sockaddr *)&dumpServer, sizeof(dumpServer));
		sleep(10);
	}
	pthread_exit(NULL);
}

void keepAlive(int socket){
	pthread_t thread;
	int createSuccess = pthread_create( &thread, NULL, &sendKeepAlive, (void *)&socket);
	if(createSuccess)
		printf("Error - pthread_create() return code: %d\n", createSuccess);
}

int error(char *message){
	printf("ERROR: %s\n", message);
	return -1;
}
