#include "client.h"
#include <fcntl.h>

int http_response_header(int clientID, enum Response_Type httpType, int responceLength){
	int sumOfBytes, previousSum;
	char currentResponse[SIZE];
	
	/* init values so not garbage */
	sumOfBytes = 0; previousSum = 0;
	snprintf(currentResponse, SIZE, "HTTP/1.1 200 OK\n");
	sumOfBytes += send(clientID, currentResponse, strlen(currentResponse), 0);
	if(sumOfBytes < previousSum){
		return -1;
	}
	previousSum = sumOfBytes;
	switch (httpType) {
		case TEXT:
			snprintf(currentResponse, SIZE, "Content-Type: text/plain; charset=utf-8\n");
			break;
		case HTML:
			snprintf(currentResponse, SIZE, "Content-Type: text/html; charset=utf-8\n");
			break;
		case CSS:
			snprintf(currentResponse, SIZE, "Content-Type: text/css; charset=utf-8\n");
			break;
		case PNG:
			snprintf(currentResponse, SIZE, "Content-Type: image/png\n"); 
			break;
		case ICO:
			snprintf(currentResponse, SIZE, "Content-Type: image/vnd.microsoft.icon\n");
			break;
	}
	sumOfBytes += send(clientID, currentResponse, strlen(currentResponse), 0);
	if(sumOfBytes < previousSum){
		return -1;
	}
	previousSum = sumOfBytes;
	sumOfBytes += send(clientID, "\n", strlen("\n"), 0);
	if(sumOfBytes < previousSum){
		return -1;
	}
	return sumOfBytes;
}

/* returns bytes send or -1 if error uses the http_response_header function and then sends input text as content */
int http_response_text(int clientID, char response[], int responseLength){
	int sumOfBytes, previousSum;
	sumOfBytes = http_response_header(clientID, TEXT, responseLength);
	if(sumOfBytes == -1){
		return -1;
	}
	previousSum = sumOfBytes;
	sumOfBytes += send(clientID, response, responseLength, 0);
	if(sumOfBytes < previousSum){
		return -1;
	}
	return sumOfBytes;
}

/* This will send the file represented by the file descriptor fd to the client @ file descriptor clientID
 * @param clientID the file descriptor of the client as found by the accept function man 2 accept 
 * @param fd this is the file descriptor of the requested file to be sent to the client make sure that the http_type is 
 * correct for the file
 * @param http_type this is the type of content being sent, it is important that the correct Response_Type is selected
 * so that the http response will be correct, the Response_Type's are TEXT for plain text, HTML for html files and CSS for css files*/
int http_response_file(int clientID, int fd, enum Response_Type http_type){
	struct stat fileInfo;
	int sumOfBytes, previousSum;
	if(fstat(fd, &fileInfo) == -1){
		printf("Error with reading file stats\n");
		return -1;
	}
	sumOfBytes = http_response_header(clientID, http_type, fileInfo.st_size);
	if(sumOfBytes == -1){
		return -1;
	}
	previousSum = sumOfBytes;
	sumOfBytes += sendfile(clientID, fd, NULL, fileInfo.st_size);
	if(sumOfBytes < previousSum){
		return -1;
	}
	return sumOfBytes;
}

int http_response_html(int clientID, int htmlfd){
	return http_response_file(clientID, htmlfd, HTML);
}

int http_response_css(int clientID, int cssfd){
	return http_response_file(clientID, cssfd, CSS);
}

int http_fail_response(){

}

void *handle_client(void *clientID_void){
	char requestType[8], fileRequest[100];
	char clientRequest[1000];
	int fd;
	int clientID = *((int*)clientID_void);
	if(clientID == -1){
		printf("clientID is -1 so client does not exist or some other error\n");
		return NULL;
	}
	printf("Sending data to client now\n");
	
	recv(clientID, clientRequest, SIZE, 0);
	printf("Full request is:%s\n", clientRequest);
	sscanf(clientRequest, "%s %s\n\n", requestType, fileRequest);
	/* Makes sure that the request is a GET request as no other requests are supported yet */
	if(strcmp(requestType, "GET") != 0){
		printf("Resquest of %s, an unsupported action has occured\n", requestType);
		return NULL;
	}
	/* The famous / request is the first request that a browser will make and so needs to be answered */
	if(strcmp(fileRequest, "/") == 0){
		fd = open("index.html", O_RDONLY);
		http_response_html(clientID, fd);
	}
	else if(strcmp(fileRequest, "/favicon.ico") == 0){
		fd = open("favicon.ico", O_RDONLY);
		http_response_file(clientID, fd, ICO);
	}
	else{
		char *fileName;
		fileName = fileRequest + 1;
		fd = open(fileName, O_RDONLY);
		if(fd == -1){
			printf("Failed to get file due to files descriptor error\n");
		}
		else{
			long fileNameLength;
			fileNameLength = strlen(fileName);
			if(fileNameLength < 3){
				printf("File Name does not have an extention\n");
			}
			/* This tests if the last four characters are the given extention */
			else if(fileNameLength - ((long)strstr(fileName, ".css") - (long)fileName) == 4){
				printf("Serving CSS page\n");
				http_response_css(clientID, fd);
				printf("Finished serving CSS page\n");
			}
			else if(fileNameLength - ((long)strstr(fileName, ".html") - (long)fileName) == 5){
				printf("Serving html page\n");
				http_response_html(clientID, fd);
				printf("Finished serving HTML page\n");
			}
			else{
				printf("Failed to get file due to being unable to sense correct formant\n");
			}
		}
	}
	printf("request served\n\n");
	if(fd != -1){
		shutdown(fd, 2);
	}
	shutdown(clientID, 2);
	return (void *)1;
}



