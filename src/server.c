#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include "client.h"

int createSocket(int isIPv6);

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
		int clientID = -1;
		clientID = accept(socketID, 0, 0);
		if(clientID == -1){
			printf("clientID is -1 exiting now");
			return -1;
		}
		handle_client(clientID);
	}
	close(socketID);
	return 0;
}

int createSocket(int isIPv6){
	struct addrinfo *result, *temp;
	struct addrinfo hints;
	int socketID = 0;
	memset(&hints, 0, sizeof(hints));
	/* Mostly taken form the man 3 getaddrinfo 
	 * used getaddrinfo to work with both ipv4 and ipv6*/
	if(isIPv6){
		hints.ai_family = AF_INET6;
	}
	else{
		hints.ai_family = AF_INET;
	}
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;
	/* This is same as INADDR_ANY or IN6ADDR_ANY_INIT */
	hints.ai_flags = AI_PASSIVE;
	/* Do not need the rest of it so set all of it to null */
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	if(getaddrinfo(NULL, "8080", &hints, &result) != 0){
		printf("Cannot get the host address info and so cannot create the socket\n");
		exit(-1);
	}

	temp = result;
	while(temp != NULL){
		socketID = socket(temp->ai_family, temp->ai_socktype, 
				temp->ai_protocol);
		if(socketID == -1){
			continue;
		}
		else if(bind(socketID, (struct sockaddr *)temp->ai_addr, temp->ai_addrlen) == 0){
			
			break;
		}
		else{
			shutdown(socketID, 2);
		}
		temp = temp -> ai_next;
	}
	
	freeaddrinfo(result);
	if(temp == NULL){
		printf("Failed to bind socket exiting now");
		exit(-1);
	}
	else{
		/* Print the address for testing purposes */
		char str[INET6_ADDRSTRLEN];
		if (temp->ai_addr->sa_family == AF_INET) {
			struct sockaddr_in *p = (struct sockaddr_in *)temp->ai_addr;
			printf("%s\n", inet_ntop(AF_INET, &p->sin_addr, str, sizeof(str)));
		} else if (temp->ai_addr->sa_family == AF_INET6) {
			struct sockaddr_in6 *p = (struct sockaddr_in6 *)temp->ai_addr;
			printf("%s\n", inet_ntop(AF_INET6, &p->sin6_addr, str, sizeof(str)));
		}
		printf("Make and bound socket\n");
		return socketID;
	}
}
