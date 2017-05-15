#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include "threadHelper.c"
#include "socketHelper.c"
#include "dapnatUser.c"

void stunService(char *buf, Socket client);
void registerNewUser(char *message, Socket client);

Socket me;
User users[5000];
int userCount = 0;

int main(){
	//create local socket with STUN port 3478 
	me = newSocket("localhost", "3478");
	// runThread(&stunService, (void*) &localSoc);


	Socket client = newSocket("","");
	char messageBuffer[500];
	memset(messageBuffer, 1,500);
	messageBuffer[499] = 0;

	short stun_method_code;
	while(1){
		receive(me, client, messageBuffer);
		
		stun_method_code = ntohs(*(short *)(&messageBuffer[0]));
		if(stun_method_code == 1){
			stunService(messageBuffer, client);
		}
		else{
			registerNewUser(messageBuffer, client);
		}

		memset(messageBuffer, 1,500);		
		messageBuffer[499] = 0;
	}

	// pthread_exit(NULL);
}


int numOfUser = 0;
void stringAppend(char *parent, char *child){
	int count = strlen(parent);
	strcpy(&parent[count], child);
}

void registerNewUser(char *message, Socket client){
	//copy name 
	char name[30];
	int i = 4;	
	while(message[i] != 0){
		name[i-4] = message[i];
		i++;
	}
	name[i-4] = 0;

	User temp = newUser(name, client);
	users[userCount] = temp;
	userCount++;

	
	//gui list
	char buf[500]; buf[0] = 0;
	stringAppend(buf, "LST_");
	sprintf(&buf[4], "%d", userCount - 1);
	stringAppend(buf, "_");

	for(i = 0; i < userCount - 1; i++){
		stringAppend(buf, users[i]->name);
		stringAppend(buf, "_");
		stringAppend(buf, users[i]->soc->ip);
		stringAppend(buf, "_");
		stringAppend(buf, users[i]->soc->port);
		stringAppend(buf, "|");
	}
	sendTo(me, client, buf);
	buf[0] = 0;

	//gui new cho tat ca user
	stringAppend(buf, "NEW");
	stringAppend(buf, "_");
	stringAppend(buf, temp->name);
	stringAppend(buf, "_");
	stringAppend(buf, client->ip);
	stringAppend(buf, "_");
	stringAppend(buf, client->port);

	for(i = 0; i < userCount - 1; i++){
		sendTo(me, users[i]->soc, buf);
	}
}


void stunService(char *buf, Socket client){
	int localSoc = me->systemSocketId;
	struct sockaddr_in clientAddr = createSocketAddr(client->ip, atoi(client->port));

	printf("Received STUN request\n");
	printf("Client: %s:%s\n", client->ip, client->port);
	
	//create STUN response
    unsigned char STUNResponse[40];
	* (short *)(&STUNResponse[0]) = htons(0x0101);  // stun_success
	* (short *)(&STUNResponse[20])= htons(0x0020); 	//attr_type
	
	//xor-mapped port/IP - xor with magic cookie: 0x2112A442
	* (short*)(&STUNResponse[26]) = clientAddr.sin_port ^ htons(0x2112);
	* (long *)(&STUNResponse[28]) = clientAddr.sin_addr.s_addr ^ htonl(0x2112A442);

	sendto(localSoc, STUNResponse, sizeof(STUNResponse), 0,
       (struct sockaddr *)&clientAddr, sizeof(clientAddr));

}