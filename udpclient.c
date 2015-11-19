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

//function declarations
void clientCNTCCode();
void draw();

//abort signal
int abortsignal = 0;

//buffer size
#define MAX 300

//Which protocol are we using?
int protocol = 0;

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

	//UDP Declarations
	int sock;
	struct sockaddr_in servAddr;
	struct sockaddr_in fromAddr;
	struct hostent *thehost;
	unsigned short servPort;
	unsigned in fromSize;
	char buffer[MAX];
	int respStringLen;

	//check command line parameters
	if (argc != 5)
	{
		printf("Usage: ./udpclient.c \"serverName\" \"serverPort\"
			\"numberOfSides\" \"sideLength\"\n");
		return 0;
	}

	signal (SIGINT, clientCNTCCode);
	serverName = argv[1];
	sscanf(argv[2],"%d",&serverPort);
	servPort = serverPort;
	sscanf(argv[3],"%d",&numSides);
	sscanf(argv[4],"%d",&sideLength);
	if (serverPort == -1)
	{
		printf("Invalid Server Port.\n");
		return 0;
	}
	if (numSides<4||numSides>8)
	{
		printf("Invalid number of sides.\n");
		return 0;
	}
	if (sideLength<1)
	{
		printf("Invalid side length.\n");
	}

	//make connection
	//Try to connect using Group 9 Protocol!
	if()
	{
	}
	else
	{
		//Try to connect using Class Protocol!
	}
}

void draw (int N, int L)
{
	//int i
	//for (i=0; i<N; i++
		//send picture
		//write picture
		//send move L
		//wait 1
		//send stop
		//send GPS
		//write GPS
		//send turn 180(N-2)
		//wait 1
		//send stop
}
