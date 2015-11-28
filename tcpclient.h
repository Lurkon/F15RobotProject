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

char* servIP = "169.55.155.236";
char* robotID = "robot_9";
int sock;
int sockOpen = 0;
struct sockaddr_in servAddr;

void openSocket();
char* getImage(char* topic, int width, int height);
char* getGPS();
char* getdGPS();
char* getLasers();
char* move(int speed);
char* turn(int degrees);
char* stop();
char* sendToRobot(char* message, int length);
void DieWithError(char* errorMessage);