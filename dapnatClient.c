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
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

int registerNewUser(char* name);
void parseUserList(char *string, int numOfUser);
void* keepAliveService(void* socket); //keep port open on router
void stunService(Socket me, char *returnIp, unsigned short *returnPort);

void* messageProcess(void* arg);

Socket me, server;

void stringAppend(char *parent, char *child){
	int count = strlen(parent);
	strcpy(&parent[count], child);
}


int main(int argc,char *argv[]){
	if(argc < 2){
		printf("Missing argument: <port>\n");
		return;
	}
	char buffer[1000];

	//create local socket with port 12345
	me = newSocket("localhost", argv[1]);
	server = newSocket(XSEED, "3478");

	char publicAddr[16];
	unsigned short openedPort = 0;
	stunService(me, publicAddr, &openedPort);

	runThread(&keepAliveService, (void*)server);

	printf("DAPNAT Client - working on socket %s:%d\n", publicAddr, openedPort);
	printf("Please enter your name (A-Z | 1-9 | less than 50 characters): ");
	
	registerNewUser(buffer);

	runThread(&messageProcess, NULL);

	char peerName[50];
	int ptr = 0;
	Socket peer;

	while(1){
		fgets(buffer, 500, stdin);
		
		ptr = 0;
		while(buffer[ptr] != 0 && ptr <= 50){
			if(buffer[ptr] == ':') break;
			peerName[ptr] = buffer[ptr];
			// printf("%c, ", peerName[ptr]); getchar();
			ptr++;
		}
		peerName[ptr++] = 0;

		if(strcmp(peerName, "all") == 0){
			//broadcasting
		}
		else{
			peer = parseSocket(phonebookGet(peerName));
			if(peer == NULL){
				printf("No such user named %s\n", peerName);
				continue;
			}
			else{
				sendTo(me, peer, &buffer[ptr]);
				free(peer);
			}
			// printf("OK, "); getchar();
			
		}
		// printf("OK, "); getchar();
	}

	pthread_exit(NULL);
}

//////////////////////CODE////////////////////
void processServerMessage(char *messageBuffer){
	
	char messageCode[4];
	strncpy(messageCode, messageBuffer, 3);
	messageCode[3] = 0;
	// printf("messageCode == %s\n", messageCode);
	if(strcmp(messageCode, "LST") == 0){
		int numOfUser;
		sscanf(&messageBuffer[4], "%d", &numOfUser);

		parseUserList(&messageBuffer[6], numOfUser);
	}
	else if(strcmp(messageCode, "NEW") == 0){
		int ptr = 4;
		char name[50];
		char socket[16+6];

		//parseName
		int offset = ptr;
		while(messageBuffer[ptr] != '_'){
			name[ptr-offset] = messageBuffer[ptr];
			ptr++;
		}
		name[ptr-offset] = 0; 
		ptr++;

		//parseSocket
		offset = ptr;
		while(messageBuffer[ptr] != '|'){
			socket[ptr-offset] = messageBuffer[ptr];
			ptr++;
		}
		socket[ptr-offset] = 0;
		ptr++;
		printf(ANSI_COLOR_YELLOW "%s has just joined" ANSI_COLOR_RESET "\n", name);
		// runThread(&keepAliveService, (void *)socket);
		phonebookPut(name, socket);
	}
}

void* messageProcess(void* arg){
	// receive message loop
	Socket sender = newSocket("", "");
	char messageBuffer[1000];
	memset(messageBuffer, 1, 1000);
	messageBuffer[999] = 0;

	int i;
	int isFromServer;

	while(1){
		receive(me, sender, messageBuffer);
		//do something with messageBuffer
		// printf("messageBuffer: %s\n", messageBuffer);

		isFromServer = 
		(strcmp(sender->ip, server->ip) == 0) &&
		(strcmp(sender->port, server->port) == 0);

		if(isFromServer){
			processServerMessage(messageBuffer);
		}
		else{
			// processing messages from peers 
			// printf("messages from peer\n");
			char *socket = serialiseSocket(sender);
			printf(ANSI_COLOR_BLUE"%s:%s"ANSI_COLOR_RESET"\n", phonebookGet(socket), messageBuffer);
			free(socket);
		}

		memset(messageBuffer, 1, 1000);
		messageBuffer[999] = 0;
	}
}

int registerNewUser(char* name)
{
	fgets(name, 50, stdin);

	int end = strlen(name);
	name[end-1] = 0; //remove newline char

	char message[54];
	sprintf(message, "REG %s", name);
	sendTo(me, server, message);
	return 0;
}

void parseUserList(char *string, int numOfUser){
	printf("%s\n", string);

	int i;
	int ptr = 0;
	char name[50];
	char socket[16+6];

	int offset;
	printf(ANSI_COLOR_YELLOW "++++++++++++++++++++\n");
	printf("Online users:\n");
	for(i = 0; i < numOfUser; i++){
		//parseName
		offset = ptr;
		while(string[ptr] != '_'){
			name[ptr-offset] = string[ptr];
			ptr++;
		}
		name[ptr-offset] = 0; 
		ptr++;

		//parseSocket
		offset = ptr;
		while(string[ptr] != '|'){
			socket[ptr-offset] = string[ptr];
			ptr++;
		}
		socket[ptr-offset] = 0;
		ptr++;

		phonebookPut(name, socket);
		// runThread(&keepAliveService, (void *)socket);
		printf("%s\t%s\n", phonebookGet(name), phonebookGet(socket));
	}
	printf("++++++++++++++++++++\n");
	printf("YOU ARE GOOD TO GO!" ANSI_COLOR_RESET "\n\n");
}

void stunService(Socket me, char *returnIp, unsigned short *returnPort){
	int localSoc = me->systemSocketId;
	//create server socket - STUN port 3478
	struct sockaddr_in serverAddr;
	serverAddr = createSocketAddr(XSEED, 3478);

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
		// void *arg = malloc(sizeof(int));
		// *((int*)arg) = localSoc;
		// runThread(&keepAliveService, arg);
	}
}

void* keepAliveService(void *_socket){
	Socket socket = (Socket)_socket;

	char nothing[] = "Xin chao";
	while(1){
    	sendTo(me, socket, nothing);
    	sleep(30);
	}
}
