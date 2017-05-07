#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include "threadHelper.c"

void *stunService(void *socket);

int main(){
	//create local socket with STUN port 3478 
	int localSoc = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in localAddr;
    bzero(&localAddr, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(3478);
	bind(localSoc, (struct sockaddr *)&localAddr, sizeof(localAddr));

	runThread(&stunService, (void*) &localSoc);


	pthread_exit(NULL);
}


void *stunService(void *_localSoc){
	int localSoc = *((int *)_localSoc);
	struct sockaddr_in userAddr;
	//userAddr.sin_port
	//userAddr.sin_addr.s_addr
	
	int addrLength = sizeof(userAddr);
	unsigned char buf[300]; //buffer
	while(1){
    	recvfrom(localSoc, buf, 300, 0, (struct sockaddr *)&userAddr, &addrLength);
    	short stun_method_code = ntohs(*(short *)(&buf[0]));

    	if(stun_method_code == 1){
    		printf("received STUN request\n");
    		printf("userAddr: %d:%d\n", userAddr.sin_addr.s_addr, userAddr.sin_port);
    		
    		//create STUN response
		    unsigned char STUNResponse[40];
			* (short *)(&STUNResponse[0]) = htons(0x0101);  // stun_success
			* (short *)(&STUNResponse[20])= htons(0x0020); 	//attr_type
			
			//xor-mapped port/IP - xor with magic cookie: 0x2112A442
			* (short*)(&STUNResponse[26]) = userAddr.sin_port ^ htons(0x2112);
			* (long *)(&STUNResponse[28]) = userAddr.sin_addr.s_addr ^ htonl(0x2112A442);

			sendto(localSoc, STUNResponse, sizeof(STUNResponse), 0,
               (struct sockaddr *)&userAddr, sizeof(userAddr));
    	}
	}	
}