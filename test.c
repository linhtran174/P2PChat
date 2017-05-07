#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include "socketHelper.c"

int main(){

	int localSoc = createAndBind(12345);
	printf("localSoc: %d\n", localSoc);
	localSoc = *(int *)((void *)&localSoc);
	printf("localSoc: %d\n", localSoc);

	pthread_exit(NULL);
}
