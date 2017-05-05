all:
	gcc -o server -pthread dapnatServer.c
	gcc -o client -pthread dapnatClient.c

server:
	gcc -o server -pthread dapnatServer.c

client:
	gcc -o client -pthread dapnatClient.c

clean:
	rm client
	rm server