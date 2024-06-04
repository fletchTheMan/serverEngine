#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "client.h"

int createSocket(int isIPv6);
int createSocketV4();
int createSocketV6();

int main(){
	int isIPv6 = 1;
	int socketID = 0;
	socketID = createSocket(isIPv6);
	if(listen(socketID, 10) == -1){
		printf("Unable to listen on the socket");
		close(socketID);
		exit(-1);
	}
	printf("Listening on the socket\n");
	while(1){
		pthread_t thread;
		int clientID = -1;
		clientID = accept(socketID, 0, 0);
		if(clientID == -1){
			continue;
		}
		printf("Accepted the client on the socket\n");
		/*
		pthread_create(&thread, NULL, handle_client, (void *)&clientID);
		pthread_detach(thread);
		*/
		handle_client((void*)&clientID);
	}
	close(socketID);
	return 0;
}

int createSocket(int isIPv6){
	if(isIPv6){
		return createSocketV6();
	}
	else{
		return createSocketV4();
	}
}

int createSocketV4(){
	int socketID;
	struct sockaddr_in hostAddr;

	socketID = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if(socketID < 0){
		printf("Unable to create socket exiting now\n");
		exit(-1);
	}
	
	hostAddr.sin_family = AF_INET;
	hostAddr.sin_port = htons(8080);
	hostAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(socketID, (struct sockaddr *)&hostAddr, sizeof(hostAddr)) < 0){
		printf("Unable to bind socket exiting now\n");
		shutdown(socketID, 2);
		exit(-1);
	}

	return socketID;
}

int createSocketV6(){
	int socketID;
	struct sockaddr_in6 hostAddr;
	
	socketID = socket(AF_INET6, SOCK_STREAM, 0);
	if(socketID < 0){
		printf("Unable to create socket exiting now\n");
		exit(-1);
	}
	
	hostAddr.sin6_family = AF_INET6;
	hostAddr.sin6_addr = in6addr_any;
	hostAddr.sin6_port = htons(8080);

	if(bind(socketID, (struct sockaddr *)&hostAddr, sizeof(hostAddr)) < 0){
		printf("Unable to bind socket exiting now\n");
		shutdown(socketID, 2);
		exit(-1);
	}

	return socketID;
}
