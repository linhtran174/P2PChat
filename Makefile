all:
	gcc -o server -pthread dapnatServer.c
	gcc -o client -pthread dapnatClient.c

server:
	gcc -o server -pthread dapnatServer.c

client:
	gcc -o client -pthread dapnatClient.c

test:
	gcc -o test -pthread test.c

clean:
	rm client
	rm server