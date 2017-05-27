#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include "socketHelper.c"
#include "dapnatUser.c"
void printUser(User user);

int main(){
	char messageCode[4];
	strncpy(messageCode, "LST 0 aaskld", 3);
	messageCode[3] = 0;
  	printf("%d", strcmp(messageCode, "LST")==0);
	
	pthread_exit(NULL);
}

void printUser(User user){
	printf("User: %s at %s:%s\n", user->name, user->soc->ip, user->soc->port);
}
