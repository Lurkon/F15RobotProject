#include "tcpclient.h"

char* servIP = "169.55.155.236";
char* robotID = "robot_9";
int sock;
int sockOpen = 0;
struct sockaddr_in servAddr;

int main(){
	char* gps = getGPS();
    int size = *(int*)gps;
    gps += 4;
    int i;

    for(i = 0; i<size; i++){
        printf("%c", *gps);
        gps++;
    }
    printf("\n");


    char* dgps = getdGPS();
    size = *(int*)dgps;
    dgps += 4;

    for(i = 0; i<size; i++){
        printf("%c", *dgps);
        dgps++;
    }
    printf("\n");
	
    
    char* lasers = getLasers();
    size = *(int*)lasers;
    lasers += 4;

    for(i = 0; i<size; i++){
        printf("%c", *lasers);
        lasers++;
    }
    printf("\n");


    char* turns = turn(30);
    size = *(int*)turns;
    turns += 4;

    for(i = 0; i<size; i++){
        printf("%c", *turns);
        turns++;
    }
    printf("\n");
	
    
    char* moves = move(4);
    size = *(int*)moves;
    moves += 4;

    for(i = 0; i<size; i++){
        printf("%c", *moves);
        moves++;
    }
    printf("\n");


    char* stops = stop();
    size = *(int*)stops;
    stops += 4;

    for(i = 0; i<size; i++){
        printf("%c", *stops);
        stops++;
    }
    printf("\n");


    FILE* fp = fopen("test.jpg", "w+");

    char* image = getImage();
    size = *(int*)image;
    image += 4;

    for(i = 0; i<size; i++){
        fprintf(fp, "%c", *image);
        image++;
    }
    printf("\n");
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
    }

    char* beginning = strstr(buffer, "\r\n\r\n");
    *(int*) beginning = totalBytesReceived - (int)(beginning-buffer) - 4;

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
    char* requestHolder = "GET /twist?id=5winnow&lx=%d HTTP/1.1\r\nUser-Agent: Wget/1.14 (darwin12.2.1)\r\nAccept: */*\r\nHost: 169.55.155.236\r\nConnection: Keep-Alive\r\n\r\n";
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
    char* requestHolder = "GET /twist?id=5winnow&az=%d HTTP/1.1\r\nUser-Agent: Wget/1.14 (darwin12.2.1)\r\nAccept: */*\r\nHost: 169.55.155.236\r\nConnection: Keep-Alive\r\n\r\n";
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
    char* requestString = "GET /twist?id=5winnow&lx=0 HTTP/1.1\r\nUser-Agent: Wget/1.14 (darwin12.2.1)\r\nAccept: */*\r\nHost: 169.55.155.236\r\nConnection: Keep-Alive\r\n\r\n";

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
