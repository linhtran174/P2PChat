#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include "threadHelper.c"
#include "socketHelper.c"

void stunService(char *buf, Socket sender);

Socket me;

int main(){
	//create local socket with STUN port 3478 
	me = newSocket("localhost", 3478);
	// runThread(&stunService, (void*) &localSoc);

	Socket client = newSocket("","");
	char *messageBuffer[500];

	short stun_method_code;
	while(1){
		receive(me, client, messageBuffer);
		stun_method_code = ntohs(*(short *)(&messageBuffer[0]));
		if(stun_method_code == 1){
			stunService(messageBuffer, client);
		}
		// else{
		// 	switch()
		// }
	}

	pthread_exit(NULL);
}


void stunService(char *buf, Socket sender){
	while(1){
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