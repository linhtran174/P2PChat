#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

// #include "socketHelper.c"
#include "nameIpMap.c"

int main(){
	mapPut("Linh Tran", "1.2.3.4");
	mapPut("Thinh Tran", "12.25.63.56");
	mapPut("Phung Mai Minh", "123.456.789.10");
	printf("%s \n", mapGet("Linh Tran"));
	printf("%s \n", mapGet("Phung Mai Minh"));
	printf("%s \n", mapGet("Thinh Tran"));
// 
	pthread_exit(NULL);
}
