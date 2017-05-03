#include <stdio.h>
#include <stdlib.h>

int main(){
	//create local socket with STUN port 3478 
	int localSoc = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in localAddr;
    bzero(&localAddr, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(3478);
	bind(localSoc, (struct sockaddr *)&localAddr, sizeof(localAddr));


	//
	struct sockaddr_in userAddr;
	//userAddr.sin_port
	//userAddr.sin_addr.s_addr
	
	socklen_t addrLength = sizeof(userAddr);
	while(1){

		unsigned char buf[300]; //buffer
    	request = recvfrom(localSoc, buf, 300, 0, (struct sockaddr *)&userAddr, &addrLength); // recv UDP

    	short stun_method_code = *(short *)(&buf[0]);

    	if(stun_method_code == 1){
    		//create STUN response
		    unsigned char STUNResponse[40];
			* (short *)(&STUNResponse[0]) = htons(0x0101);    // stun_success
			* (short *)(&STUNResponse[20])= htons(0x0020); 	//attr_type
			
			//xor-mapped port/IP - xor with magic cookie: 0x2112A442
			* (short*)(&STUNResponse[26]) = htons(userAddr.sin_port ^ 0x2112);
			* (long *)(&STUNResponse[28]) = htonl(userAddr.sin_addr.s_addr ^ 0x2112A442);

			//register-name success code
			* (short *)(&STUNResponse[32])

			sendto(localSoc, STUNResponse, sizeof(STUNResponse), 0,
               (struct sockaddr *)&userAddr, sizeof(userAddr));
    	}



	}
}