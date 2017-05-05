#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

int registerNewUser(const char* name, char * returnIp, unsigned short * returnPort);
int keepAlive(int socket); //keep UDP socket alive

int main(){
	char a[10];
	short b[20];
	registerNewUser("Linh Tran", a, b);
}

//////////////////////CODE////////////////////
int error(char *message);
int registerNewUser(const char* name, char * returnIp, unsigned short * returnPort)
{
	//create local socket with port 12345
	int localSoc = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in localAddr;
    bzero(&localAddr, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(12345);
	bind(localSoc, (struct sockaddr *)&localAddr, sizeof(localAddr));

	//create server socket
	struct sockaddr_in serverAddr;
	char *stun_server_ip = "64.233.188.127"; //stun.l.google.com
	unsigned short stun_server_port = 19302;
    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, stun_server_ip, &serverAddr.sin_addr);
    serverAddr.sin_port = htons(stun_server_port);

    //create STUNRequest
    unsigned char STUNRequest[20];
	* (short *)(&STUNRequest[0]) = htons(0x0001);    // stun_method
	* (short *)(&STUNRequest[2]) = htons(0x00020);    // msg_length
	* (int *)(&STUNRequest[4])   = htonl(0x2112A442);// magic cookie

	*(int *)(&STUNRequest[8]) = htonl(0x63c7117e);   // transacation ID 
	*(int *)(&STUNRequest[12])= htonl(0x0714278f);
	*(int *)(&STUNRequest[16])= htonl(0x5ded3221);



	//register a name
	int i = 0;
	while(name[i] != 0){
		STUNRequest[22+i] = name[i];
		i++;
	}
	
    //send request
    short requestResult =
    sendto(localSoc, STUNRequest, sizeof(STUNRequest),
    0, (struct sockaddr *)&serverAddr, sizeof(serverAddr)); // send UDP
	if (requestResult == -1) return error("cannot send STUN request");

	//wait 100ms for the response
	usleep(1000 * 100);

	//read response
	unsigned char buf[300]; //buffer
    requestResult = recvfrom(localSoc, buf, 300, 0, NULL, 0); // recv UDP
    if (requestResult == -1) return error("cannot receive any response");
    printf("OK\n"); getchar();
	// parse response
	short STUNSuccess = *(short *)(&buf[0]) == htons(0x0101);
	short attr_type = htons(*(short *)(&buf[20]));
	short attr_length = htons(*(short *)(&buf[22]));
	if (STUNSuccess)
	{
		printf("STUN binding resp: success !\n");
		if (attr_type == 0x0020)
		{
			// parse : port, IP 
			*returnPort = (buf[26] * 256 + buf[27]) ^ 0x2112;
			sprintf(returnIp,"%d.%d.%d.%d",buf[28]^0x21,buf[29]^0x12,buf[30]^0xA4,buf[31]^0x42);
		}
		keepAlive(localSoc);
	}

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

void* sendKeepAlive(void *_socket){
	int socket = *(int *)_socket;
	printf("OK\n"); getchar();
	//create dump server address
	struct sockaddr_in dumpServer;
    bzero(&dumpServer, sizeof(dumpServer));
    dumpServer.sin_family = AF_INET;
    inet_pton(AF_INET, "45.119.81.200", &dumpServer.sin_addr);
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

int keepAlive(int socket){
	pthread_t thread;
	int createSuccess = pthread_create( &thread, NULL, &sendKeepAlive, (void *)&socket);
	if(createSuccess)
		printf("Error - pthread_create() return code: %d\n", createSuccess);
}


int error(char *message){
	printf("ERROR: %s\n", message);
	return -1;
}
