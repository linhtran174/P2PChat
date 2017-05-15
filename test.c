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
	User user = newUser("Linh Tran",
		newSocket("123.456.789.10", "1234")
	);

	mapPut("Linh Tran", user);
	User Linh = mapGet("Linh Tran");

	char *linh = serializeUser(Linh);

	Linh = parseUser(linh);
	linh = serializeUser(Linh);
	Linh = parseUser(linh);
	linh = serializeUser(Linh);
	Linh = parseUser(linh);

	printUser(Linh);
	
	pthread_exit(NULL);
}

void printUser(User user){
	printf("User: %s at %s:%s\n", user->name, user->soc->ip, user->soc->port);
}
