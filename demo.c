#include "stunClient.c"

int main(){
	printf("demo module get IP");
	char ip[100];
	unsigned short port = 0;
	getPublicIpAddress(ip, &port);
	printf("%s:%d\n", ip, port);
}