/* Group 9: Mitchell Devenport, Nathan Jones,
 *	Richard Orr, and Sameer Singh
 * This program takes three inputs:
 *	server name: The name of the robot proxy
 *	server port: The port on which the proxy resides
 *	N: The number of sides of the polygon to be drawn by the robot.
 * Due Date: 12/2/2015
 * Sekou Remy's class
 */
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <time.h>
#include "protocol.h"

//function declarations
int cmdLine(int, char **, char **, int *, int *, int *);
char *getResponse();
char *getImage();
char *getGPS();
char *getdGPS();
char *getLasers();
char *move(int speed);
char *turn(int degrees);
char *stop();
void DieWithError(char *errorMessage);
void openSocket(char **, unsigned short);
int connectNine();
int connectClass();
void draw();
void writeData();

//buffer size
#define MAX 300

//Which protocol are we using?
int protocol = 9; //default is group protocol
int password = 0;
struct classProtocol cProto;

//Files written
int written = 0;

//socket data
int sock;
int sockOpen = 0;
struct sockaddr_in servAddr;
struct sockaddr_in fromAddr;
unsigned int fromSize;

/* The plan is to have three major parts to this program:
 *	connection: connect to the proxy
 *	draw: draw the polygon
 *	write: writes responses to file
*/

int main (int argc, char **argv)
{
	//initiate stuff
	char *serverName;
	int serverPort;
	int numSides;
	int sideLength;
	unsigned short servPort;

	//check command line parameters
	if(cmdLine(argc, argv, &serverName, &serverPort, &numSides,
			&sideLength)!=0)
	{
		fprintf(stderr,"Usage: ./udpclient.c -h <hostname> -p "
			"<port> -n <number of sides (4<=n<=8> -l <length "
			"of sides>\n");
		return 0;
	}

	servPort=serverPort;
	if (serverPort==-1)
	{
		printf("Invalid Server Port.\n");
		return 0;
	}


	//This is now actual connection things
	openSocket(&serverName, servPort);

	//make connection using group 9 protocol
	if(connectNine()==0)
	{
		//good connection
	}
	else
	{
		if(connectClass()!=0)
			DieWithError("Could not connect!\n");
	}
	return 0;
}

//connects to the proxy, returns 0 if successful, -1 else
int connectNine()
{return -1;}

//connects to the proxy, returns 0 if successful, -1 else
int connectClass()
{
	cProto.protocol=0;
	cProto.password=0;
	cProto.cliRequest=0;
	sendto(sock,&cProto,MAX,0,(struct sockaddr *) 
		&servAddr,sizeof(servAddr));
	return -1;
}

//create a socket for a udp connection to the server
void openSocket(char **servIP, unsigned short servPort)
{
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP))<0)
		DieWithError("socket() failed\n");

	//build the server address
        memset(&servAddr, 0, sizeof(servAddr));
        servAddr.sin_family=AF_INET;
        servAddr.sin_addr.s_addr=inet_addr(*servIP);
        servAddr.sin_port=htons(servPort);
        if(servAddr.sin_addr.s_addr==-1)
        {
                struct hostent *thehost=gethostbyname(*servIP);
                servAddr.sin_addr.s_addr=*((unsigned long *)
                        thehost->h_addr_list[0]);
        }

	//set timeout
        struct timeval timeout;               
        timeout.tv_sec=5;
        timeout.tv_usec=0;
        if (setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout,
                        sizeof(timeout))<0)
                DieWithError("setsockopt send failed\n");
}

void draw()
{
	// for (i=0; i<numSides; i++)
		//send picture
		//write picture
		//send move l
		//wait 1
		//send stop
		//send GPS
		//write GPS
		//send turn equation with n
		//wait 1
		//send stop
}

//cmdLine reads command line arguments into the proper variables.
//returns -1 if there is an error
//returns 0 otherwise
int cmdLine(int argc, char **argv, char **name, int *port, int *n, int *l)
{
	int i;
	int s=0;
	//name - name of the server
	for (i=0; i<argc; i++)
	{
		if (strcmp(argv[i],"-h")==0)
		{
			if (i++ < argc)
			{
				s =1;
				*name = argv[i];
				i=argc;
			}
			else
			{
				//out of bounds
				return -1;
			}
		}
	}
	if (s==0)
		return -1;
	s=0;
	//port - port of the server
        for (i=0; i<argc; i++)
        {
                if (strcmp(argv[i],"-p")==0)
                {
                        if (i++ < argc)
                        {
				s=1;
                                *port = atoi(argv[i]);
                                i=argc;
                        }   
                        else
                        {
                                //out of bounds
                                return -1;
                        }
                }
        }
	if (s==0)
		return -1;
	s=0;
	//n - number of sides
        for (i=0; i<argc; i++)
        {
                if (strcmp(argv[i],"-n")==0)
                {
                        if (i++ < argc)
                        {
				s=1;
                                *n = atoi(argv[i]);
                                i=argc;
                        }   
                        else
                        {
                                //out of bounds
                                return -1;
                        }
                }
        }
	if(s==0)
		return -1;
	s=0;

	//l - length of sides
        for (i=0; i<argc; i++)
        {
                if (strcmp(argv[i],"-l")==0)
                {
                        if (i++ < argc)
                        {
				s=1;
                                *l = atoi(argv[i]);
                                i=argc;
                        }   
                        else
                        {
                                //out of bounds
                                return -1;
                        }
                }
        }
	if (s==0)
		return -1;
	return 0;
}

void DieWithError(char *errorMessage)
{
	perror(errorMessage);
	exit(1);
}
