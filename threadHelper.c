#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

int runThread(void *(*start_routine)(void *), void *arg);

//return threadId
int runThread(void *(*start_routine)(void *), void *arg){
	static pthread_t threadId;
	int createSuccess = 
	pthread_create( &threadId, NULL, start_routine, arg);
	if(createSuccess)
		printf("Error - pthread_create() return code: %d\n", createSuccess);

	return threadId;
}