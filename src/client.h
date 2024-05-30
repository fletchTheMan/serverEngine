#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <stdio.h>
#include <string.h>

#define SIZE 1000

/* This handles all of the client logic that is nessiary for the program */
enum Response_Type {
	TEXT,
	HTML,
	CSS,
	PNG,
	ICO
};

int http_response_file(int clientID, int fd, enum Response_Type httpType);

int http_response_header(int clientID, enum Response_Type httpType, int responseLength);

int http_response_text(int clientID, char reponse[], int responseLength);

int http_response_html(int clientID, int htmlfd);

int http_response_css(int clientID, int cssfd);

void *handle_client(int clientID);
