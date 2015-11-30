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
void draw();
void writeData();


//buffer size
#define MAX 300

//Which protocol are we using?
int protocol = 9; //default is group protocol

//Files written
int written = 0;

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
	unsigned int fromSize;

	char *buffer[MAX];
	int respStringLen;
	int password;
	int check;
	int i;
	//header declarations
	//struct nineProtocol *ourProto;
	//struct classProtocol *claProto;

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
	if ((sock=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP))<0)
	{
		printf("socket failed\n");
		return 0;
	}
	struct timeval timeout;
	timeout.tv_sec=5;
	timeout.tv_usec=0;

	//set up the server address
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family=AF_INET;
	servAddr.sin_addr.s_addr=inet_addr(serverName);
	servAddr.sin_port=htons(servPort);
	if(servAddr.sin_addr.s_addr==-1)
	{
		thehost=gethostbyname(serverName);
		servAddr.sin_addr.s_addr=*((unsigned long *)
			thehost->h_addr_list[0]);
	}

	//set timeout
	if (setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout,
			sizeof(timeout))<0)
		fprintf(stderr,"setsockopt send failed\n");
	//make connection using group protocol
	//to be implemented
	if(1)//fail to use the group protocol
	{
		protocol=0; //set to class protocol
		memset(buffer, 0, 28);
	
		if(sendto(sock, buffer, MAX, 0, (struct sockaddr *)
			&servAddr, sizeof(servAddr)) != MAX)
		{
			printf("Wrong number of bytes sent\n");
			return 0;
		}
	}
	respStringLen=recvfrom(sock, buffer, MAX, 0, (struct sockaddr *)
		&fromAddr, &fromSize);
	if (protocol ==0)//do as the romans do
	{
	  //check buffer for data
	  for (i=0; i<13; i=i+4)
	  {
	    check = *buffer[i];
	    check = check<<4;
	    check = check + *buffer[i+1];
	    check = check<<4;
	    check = check + *buffer[i+2];
	    check = check<<4;
	    check = check + *buffer[i+3];
	    if(i==0&&check != protocol)
	      fprintf(stderr,"Incorrect protocol %d\n",check);
	      if(i==2)
		password = check;
              if(i==4&&check != 0)
		fprintf(stderr,"error: %d\n", check);
	    }
	    //now we're cooking with petrol. Assuming I'm not dumb.

	    for (i=0; i<numSides; i++)
	    {
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
	  numSides--;
            for (i=0; i<numSides; i++)
            {
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
	  //send stop
	}
	else //do what we do best
	{
	}

	return 0;
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
