#include "dapnatClient.c"

int main(){
	printf("demo module get IP\n");
	char ip[100];
	unsigned short port = 0;
	char[] name = "Linh Tran";

	registerNewUser(name, ip, &port);
	printf("%s:%d\n", ip, port);


	//important, letting other threads run.
	pthread_exit(NULL);
}