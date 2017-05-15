#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

// #include "socketHelper.c"
#include "dapnatUser.c"
void printUser(User user);

int main(){

	mapPut("Linh Tran", 
		newUser("Linh Tran",
			newSocket("123.456.789.10", "1234")
		)
	);

	mapPut("Thinh Tran",
		newUser("Thinh Tran",
			newSocket("12.34.56.78", "2345")
		)
	);

	mapPut("Phung Mai Minh",
		newUser("Phung Mai Minh", 
			newSocket("34.56.78.910", "3456")
		)
	);

	User linh = mapGet("Linh Tran");
	User phung = mapGet("Phung Mai Minh");
	User thinh = mapGet("Thinh Tran");

	printUser(linh);
	printUser(phung);
	printUser(thinh);
	pthread_exit(NULL);
}

void printUser(User user){
	printf("User: %s at %s:%s\n", user->name, user->soc->ip, user->soc->port);
}
