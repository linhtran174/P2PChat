#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

int sendUdp(int argc, char args**){
	//create local socket with port 12345
	int localSoc = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in localAddr;
    bzero(&localAddr, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(12345);
	bind(localSoc, (struct sockaddr *)&localAddr, sizeof(localAddr));

	char message[] = "This is a test message";


	struct sockaddr_in serverAddr;
    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, args[1], &serverAddr.sin_addr);
    serverAddr.sin_port = (short)atoi(args[2]);	

	while(1){
		sleep(3);

		sendto(localSoc, message, sizeof(message),
			0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	}
	return 0;
}

int main(){
	pthread_t thread;
	int createSuccess = pthread_create( &thread, NULL, &sendUdp, (void *)socket);
	if(createSuccess)
		printf("Error - pthread_create() return code: %d\n", createSuccess);

	pthread_exit(NULL);
}
