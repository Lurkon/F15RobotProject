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
	if (argc != 4)
	{
		printf("Usage: ./udpclient.c \"serverName\" \"serverPort\"
			\"numberOfSides\"");
		return 0;
	}

	signal (SIGINT, clientCNTCCode);
	serverName = argv[1];
	sscanf(argv[2],"%d",&serverPort);
	servPort = serverPort;
	sscanf(argv[3],"%d",&numSides);
}

void draw (int n) {
	//int i
	//for (i=0; i<n; i++
		//send picture
		//write picture
		//send move n
		//wait 1
		//send stop
		//send GPS
		//write GPS
		//send turn 180(n-2)
		//wait 1
		//send stop
}
