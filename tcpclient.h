#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <math.h>

#define RCVBUFSIZE 32

void openSocket();
char* getResponse();
void sendRequest(char* requestString);
char* getImage();
char* getGPS();
char* getdGPS();
char* getLasers();
char* move(int speed);
char* turn(int degrees);
char* stop();
void DieWithError(char* errorMessage);

#ifdef __cplusplus
}
#endif
