All: clean server.o client.o

server.o: firstDevice.c
	gcc firstDevice.c -o firstDevice

client.o: secondDevice.c
	gcc secondDevice.c -o secondDevice

clean:
	rm -f client
	rm -f server
