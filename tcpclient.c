#include "tcpclient.h"

int main(){
	printf("%s", getImage("robot_9", 600, 500));
}

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

char* getImage(char* topic, int width, int height){
	openSocket();

	//Set server port
    servAddr.sin_port = htons(8081);

    //Set up the request string to the server
    char* requestHolder = "GET /snapshot?topic=/%s/image?width=%d?height=%d HTTP/1.1\r\nUser-Agent: Wget/1.14 (darwin12.2.1)\r\nAccept: */*\r\nHost: 169.55.155.236\r\nConnection: Keep-Alive\r\n\r\n";
    char* requestString = malloc(sizeof(char) * (strlen(requestHolder) + strlen(topic) + log10(width) + log10(height)));

    sprintf(requestString, requestHolder, topic, width, height);

    //Connect to the server
    if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
        DieWithError("connect() failed");

    int requestStringLen = strlen(requestString);

    //Send request to the server
    if (send(sock, requestString, requestStringLen, 0) != requestStringLen)
        DieWithError("send() sent a different number of bytes than expected");

    char* response = getResponse();

    free(requestString);

	return response;
}

char* getGPS(){
	openSocket();

	return NULL;
}

char* getdGPS(){
	openSocket();

	return NULL;
}

char* getLasers(){
	openSocket();

	return NULL;
}

char* move(int speed){
	openSocket();

	return NULL;
}

char* turn(int degrees){
	openSocket();

	return NULL;
}

char* stop(){
	openSocket();

	return NULL;
}

char* sendReceiveRobot(char* message, int length){
	openSocket();

	return NULL;
}

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}