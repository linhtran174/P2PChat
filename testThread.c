#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

void *sendKeepAlive(){
	while(1){
  		printf("Heelow\n"); //must have this \n in-order to see the print result
		sleep(1);
	}  
}

int main(){
	pthread_t thread;
	pthread_create( &thread, NULL, sendKeepAlive, NULL);
	pthread_exit(NULL);
}