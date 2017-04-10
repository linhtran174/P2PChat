#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

int error(char *message);

//Provide STUN Client API:
int getPublicIpAddress(char * returnIp, unsigned short * returnPort)
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
	* (short *)(&STUNRequest[2]) = htons(0x0000);    // msg_length
	* (int *)(&STUNRequest[4])   = htonl(0x2112A442);// magic cookie

	*(int *)(&STUNRequest[8]) = htonl(0x63c7117e);   // transacation ID 
	*(int *)(&STUNRequest[12])= htonl(0x0714278f);
	*(int *)(&STUNRequest[16])= htonl(0x5ded3221);

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
	}

	return 0;
}

int error(char *message){
	printf("ERROR: %s\n", message);
	return -1;
}
