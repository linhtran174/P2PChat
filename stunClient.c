#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>


//Provide STUN Client API:
int stun_xor_addr(int localSoc, char * return_ip, unsigned short * return_port)
{
	//create local socket with port 12345
	int localSoc = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in localAddr;
    bzero(&localAddr, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(12345);
	bind(sockfd, (struct sockaddr *)&localAddr, sizeof(localAddr));


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
    sendto(localSoc, STUNRequest, sizeof(STUNRequest), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr)); // send UDP
	if (requestResult == -1)
	{
		printf("sendto error\n");
		return -1;
	}

	//wait 100ms for the response
	usleep(1000 * 100);

	//read response
	unsigned char buf[300]; //buffer
    requestResult = recvfrom(localSoc, buf, 300, 0, NULL, 0); // recv UDP
    if (requestResult == -1)
	{
	    printf("recvfrom error\n");
	    return -2;
	}

	short attr_type;
	short attr_length;
	short STUNSuccess = *(short *)(&buf[0]) == htons(0x0101);
	if (STUNSuccess)
	{
		printf("STUN binding resp: success !\n");

		// parse 
    	attr_type = htons(*(short *)(&buf[i]));
		attr_length = htons(*(short *)(&buf[i+2]));
		if (attr_type == 0x0020)
		{
			// parse : port, IP 
			port = (buf[26] * 256 + buf[27]) ^ 0x2112;

			// return 
			*return_port = port;
			sprintf(return_ip,"%d.%d.%d.%d",buf[28]^0x21,buf[29]^0x12,buf[30]^0xA4,buf[31]^0x42);

			break;
		}
	}

	// do not close the socket !!!
	return 0;
}