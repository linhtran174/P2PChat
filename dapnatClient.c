#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "threadHelper.c"
#include "socketHelper.c"
#include "dapnatUser.c"

//stun.l.google.com:19302
#define STUN_G "64.233.188.127"
//xseed.tech
#define XSEED "45.119.81.200"


int registerNewUser(const char* name);
int parseUserList(const char *messageBuffer, int numOfUser);
void* keepAliveService(void* socket); //keep port open on router
void stunService(Socket me, char *returnIp, unsigned short *returnPort);

Socket me, server;

void stringAppend(char *parent, char *child){
	int count = strlen(parent);
	strcpy(&parent[count], child);
}


int main(){
	//create local socket with port 12345
	me = newSocket("localhost", "12345");
	server = newSocket("127.0.0.1", "3478");

	char publicAddr[16];
	unsigned short openedPort;
	stunService(me, publicAddr, &openedPort);

	printf("DAPNAT Client - %s:%d\n", publicAddr, openedPort);
	printf("Please enter your name (A-Z | 1-9 | less than 100 characters): ");

	char name[1000];
	fgets(name, 1000, stdin);

	registerNewUser(name);

	// receive message loop
	char messageBuffer[1000];
	memset(messageBuffer, 1, 1000);
	messageBuffer[999] = 0;

	char messageCode[4];
	while(1){
		receive(me, server, messageBuffer);
		//do something with messageBuffer

		strncpy(messageCode, messageBuffer, 3);
		if(strcmp(messageCode, "LST") == 0){
			printf("processing LST\n");

			//parse numOfUser
			int numOfUser;
			sscanf(&messageBuffer[4], "%d", &numOfUser);

			parseUserList(&messageBuffer[6], numOfUser);
		}
		else if(strcmp(messageCode, "NEW") == 0){
			printf("processing NEW\n");

		}

		memset(messageBuffer, 1, 1000);
		messageBuffer[999] = 0;
	}

	pthread_exit(NULL);
}

//////////////////////CODE////////////////////
int error(char *message);
int registerNewUser(const char* name)
{
	sendTo(me, server, "REG_Linh Tran");
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

void parseUserList(string ){

}