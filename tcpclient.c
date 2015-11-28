#include "tcpclient.h"

/*int main(){
	printf("%s\n", getGPS());
	printf("%s\n", getdGPS());
	printf("%s\n", getLasers());
	printf("%s\n", move(4));
	printf("%s\n", turn(90));
	printf("%s\n", stop());
	printf("%s\n", getImage(600, 500));
}*/

void openSocket(){
    //Create a socket for a TCP connection
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    //Build server address
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family      = AF_INET;
    struct hostent *thehost = gethostbyname(servIP);
    servAddr.sin_addr.s_addr = *((unsigned long *) thehost->h_addr_list[0]);
}

char* getResponse(){
	char* buffer = malloc(RCVBUFSIZE);

    //Receive data from the server until it is no longer sent
    int msgCount = 1;
    int totalBytesReceived = 0;
    int bytesReceived;
    while((bytesReceived = recv(sock, buffer + totalBytesReceived, RCVBUFSIZE, 0)) > 0)
   	{
   		totalBytesReceived += bytesReceived;
   		buffer = realloc(buffer, totalBytesReceived + RCVBUFSIZE);
        msgCount++;
        printf("%d\n", totalBytesReceived);
    }

    buffer = realloc(buffer, totalBytesReceived + 1);
    buffer[totalBytesReceived] = '\0';

    char* beginning = strstr(buffer, "\r\n\r\n");
    beginning += 4;

    return beginning;
}

void sendRequest(char* request){
	//Connect to the server
    if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
        DieWithError("connect() failed");

    int requestStringLen = strlen(request);

    //Send request to the server
    if (send(sock, request, requestStringLen, 0) != requestStringLen)
        DieWithError("send() sent a different number of bytes than expected");
}

char* getImage(){
	openSocket();

	//Set server port
    servAddr.sin_port = htons(8081);

    //Set up the request string to the server
    char* requestString = "GET /snapshot?topic=/robot_9/image?width=600?height=500 HTTP/1.1\r\nUser-Agent: Wget/1.14 (darwin12.2.1)\r\nAccept: */*\r\nHost: 169.55.155.236\r\nConnection: Keep-Alive\r\n\r\n";

    sendRequest(requestString);
    char* response = getResponse();

    close(sock);

	return response;
}

char* getGPS(){
	openSocket();

	//Set server port
    servAddr.sin_port = htons(8082);

    //Set up the request string to the server
    char* requestString = "GET /state?id=5winnow HTTP/1.1\r\nUser-Agent: Wget/1.14 (darwin12.2.1)\r\nAccept: */*\r\nHost: 169.55.155.236\r\nConnection: Keep-Alive\r\n\r\n";

    sendRequest(requestString);
    char* response = getResponse();
    close(sock);

	return response;
}

char* getdGPS(){
	openSocket();

	//Set server port
    servAddr.sin_port = htons(8084);

    //Set up the request string to the server
    char* requestString = "GET /state?id=5winnow HTTP/1.1\r\nUser-Agent: Wget/1.14 (darwin12.2.1)\r\nAccept: */*\r\nHost: 169.55.155.236\r\nConnection: Keep-Alive\r\n\r\n";

    sendRequest(requestString);
    char* response = getResponse();

    close(sock);

	return response;
}

char* getLasers(){
	openSocket();

	//Set server port
    servAddr.sin_port = htons(8083);

    //Set up the request string to the server
    char* requestString = "GET /state?id=5winnow HTTP/1.1\r\nUser-Agent: Wget/1.14 (darwin12.2.1)\r\nAccept: */*\r\nHost: 169.55.155.236\r\nConnection: Keep-Alive\r\n\r\n";

    sendRequest(requestString);
    char* response = getResponse();
    
    close(sock);

	return response;
}

char* move(int speed){
	openSocket();

	//Set server port
    servAddr.sin_port = htons(8082);

    //Set up the request string to the server
    char* requestHolder = "GET /state?id=5winnow&lx=%d HTTP/1.1\r\nUser-Agent: Wget/1.14 (darwin12.2.1)\r\nAccept: */*\r\nHost: 169.55.155.236\r\nConnection: Keep-Alive\r\n\r\n";
    char* requestString = malloc(sizeof(char) * (strlen(requestHolder) + log10(speed)));

    sprintf(requestString, requestHolder, speed);

    sendRequest(requestString);
    char* response = getResponse();
    
    free(requestString);
    close(sock);

	return response;
}

char* turn(int degrees){
	openSocket();

	//Set server port
    servAddr.sin_port = htons(8082);

    //Set up the request string to the server
    char* requestHolder = "GET /state?id=5winnow&az=%d HTTP/1.1\r\nUser-Agent: Wget/1.14 (darwin12.2.1)\r\nAccept: */*\r\nHost: 169.55.155.236\r\nConnection: Keep-Alive\r\n\r\n";
    char* requestString = malloc(sizeof(char) * (strlen(requestHolder) + log10(degrees)));

    sprintf(requestString, requestHolder, degrees);

    sendRequest(requestString);
    char* response = getResponse();
    
    free(requestString);
    close(sock);

	return response;
}

char* stop(){
	openSocket();

	//Set server port
    servAddr.sin_port = htons(8082);

    //Set up the request string to the server
    char* requestString = "GET /state?id=5winnow&lx=0 HTTP/1.1\r\nUser-Agent: Wget/1.14 (darwin12.2.1)\r\nAccept: */*\r\nHost: 169.55.155.236\r\nConnection: Keep-Alive\r\n\r\n";

    sendRequest(requestString);
    char* response = getResponse();
    
    close(sock);

	return response;
}

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}