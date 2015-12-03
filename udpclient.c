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
void getImage();
void getGPS();
void getdGPS();
void writeGPS();
void getLasers();
void move(int speed);
void turn(int degrees);
void stop();
void DieWithError(char *errorMessage);
void openSocket(char **, unsigned short);
int connectNine();
int connectClass();
void disconnectClass();
void draw();
void writeImage();

//buffer size
#define MAX 300

//Which protocol are we using?
int protocol = 9; //default is group protocol
int password = 0;
union Protocol proto;

//Files written
int written = 0;
FILE *image;
FILE *data;

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
	int i, j;

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

	//start the data file
	data=fopen("gpsdata.txt", "w");
	fclose(data);

	//This is now actual connection things
	openSocket(&serverName, servPort);

	//make connection using group 9 protocol
	if(connectNine()==0)
	{
		//good connection
		proto.nine.protocol = 9;
		proto.nine.password = password;
		char *index = (char *)proto.nine.payload;
		for (j=0; j<2; j++)
		{
		for (i=0; i<numSides; i++)
		{
			*index=2;
			index++;
			*index=0;
			index++;
			*index=32;//move
			index++;
			*index=(char) sideLength;
			index++;
			*index=128;//stop
			index++;
			*index=1;
			index++;
			*index=4;//GPS
			index++;
			*index=0;
			index++;
			*index=64;//turn
			index++;
			*index=1;
			index++;
			*index=128;//stop
			index++;
			*index=14/numSides;
			index++;
			*index=8;//dgps
			index++;
			*index=0;
			index++;
		}
		numSides--;
		}
		*index = 2;
		index++;
		*index = 0;
		index++;
		*index =1;
		index++;
		*index=0;
	}
	else
	{
		protocol=0;
		if(connectClass()!=0)
			DieWithError("Could not connect!\n");
		draw (numSides, sideLength);
		draw (numSides-1, sideLength);
		disconnectClass();
	}
	return 0;
}

//connects to the proxy, returns 0 if successful, -1 else
int connectNine()
{
return -1;
}

//connects to the proxy, returns 0 if successful, -1 else
int connectClass()
{
	proto.class.protocol=0;
	proto.class.password=0;
	proto.class.cliRequest=0;
	if(sendto(sock,&proto,MAX,0,(struct sockaddr *) 
		&servAddr,sizeof(servAddr))!=MAX)
		return -1;
	recvfrom(sock, &proto, MAX, 0, (struct sockaddr *)
		&fromAddr, &fromSize);
	if(proto.class.protocol != 0)
		return -1;
	password = proto.class.password;
	if(sendto(sock,&proto,MAX,0,(struct sockaddr *)
		&servAddr,sizeof(servAddr))!=MAX)
		return -1;
	if(proto.class.cliRequest ==0)
		return 0;
	return -1;
}
void disconnectClass()
{
        proto.class.protocol=0;
        proto.class.password=password; 
        proto.class.cliRequest=255;
        if(sendto(sock,&proto,MAX,0,(struct sockaddr *)
                &servAddr,sizeof(servAddr))!=MAX)
          	DieWithError("Could not Quit.\n");

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
        timeout.tv_sec=50;
        timeout.tv_usec=0;
        if (setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout,
                        sizeof(timeout))<0)
                DieWithError("setsockopt send failed\n");
}

void draw(int n, int l)
{
	int i;
	for (i=0; i<n; i++)
	{
		getGPS();
		getImage();
		move(l);
		sleep(1);
		stop();
		getdGPS();
		turn(n);
		usleep(14/n);
		stop();
	}
}

void move(int n)
{
	proto.class.protocol=0;
	proto.class.password=password;
	proto.class.cliRequest=32;
	proto.class.requestData = n;
        proto.class.totalSize=0;
        proto.class.payloadSize=0;
        proto.class.offset=0;
	sendto(sock,&proto,MAX,0,(struct sockaddr *)
           	&servAddr,sizeof(servAddr));
	
       	recvfrom(sock, &proto, MAX, 0, (struct sockaddr *)
               	&fromAddr, &fromSize);
	//write data?
}

void stop()
{
        proto.class.protocol=0;
        proto.class.password=password;
        proto.class.cliRequest=128;
        proto.class.requestData = 0;
        proto.class.totalSize=0;
        proto.class.payloadSize=0;
        proto.class.offset=0;
        sendto(sock,&proto,MAX,0,(struct sockaddr *)
                &servAddr,sizeof(servAddr));
        //get GPS data as response?
        recvfrom(sock, &proto, MAX, 0, (struct sockaddr *)
                &fromAddr, &fromSize);
        //write data?
}

void turn(int n)
{
        proto.class.protocol=0;
        proto.class.password=password;
        proto.class.cliRequest=64;
        proto.class.requestData = 1;
        proto.class.totalSize=0;
        proto.class.payloadSize=0;
        proto.class.offset=0;
        sendto(sock,&proto,MAX,0,(struct sockaddr *)
                &servAddr,sizeof(servAddr));
        //get GPS data as response?
        recvfrom(sock, &proto, MAX, 0, (struct sockaddr *)
                &fromAddr, &fromSize);
        //write data?
}

void getImage()
{
        proto.class.protocol=0;
        proto.class.password=password;
        proto.class.cliRequest=2;  
        proto.class.requestData = 0;
        proto.class.totalSize=0;
        proto.class.payloadSize=0;
        proto.class.offset=0;
        sendto(sock,&proto,MAX,0,(struct sockaddr *)
                &servAddr,sizeof(servAddr));
	//RECEIEVE DATA
        //get GPS data as response?
        recvfrom(sock, &proto, MAX, 0, (struct sockaddr *)
                &fromAddr, &fromSize);
        //write data?
	if(proto.class.totalSize!=0)
		writeImage();

}

void getGPS()
{
        proto.class.protocol=0;
        proto.class.password=password;
        proto.class.cliRequest=4;  
        proto.class.requestData = 0;
	proto.class.totalSize=0;
	proto.class.payloadSize=0;
	proto.class.offset=0;
        sendto(sock,&proto,MAX,0,(struct sockaddr *)
                &servAddr,sizeof(servAddr));

	//RECEIVE DATA
        //get GPS data as response?
        recvfrom(sock, &proto, MAX, 0, (struct sockaddr *)
                &fromAddr, &fromSize);
	perror("fuck\n");
	//write GPS
	if (proto.class.payloadSize!=0)
		writeGPS();
}

void getdGPS()
{
        proto.class.protocol=0;
        proto.class.password=password;
        proto.class.cliRequest=8;  
        proto.class.requestData = 0;
        proto.class.totalSize=0;
        proto.class.payloadSize=0;
        proto.class.offset=0;
        sendto(sock,&proto,MAX,0,(struct sockaddr *)
                &servAddr,sizeof(servAddr));

	//RECEIVE DATA
        //get GPS data as response?
        recvfrom(sock, &proto, MAX, 0, (struct sockaddr *)
                &fromAddr, &fromSize);
        //write data
	if(proto.class.payloadSize!=0)
	{
		writeGPS();
	}
}

void writeGPS() {
	char *start;
	char *index;
	int i;
	int sum=0;

	if (protocol == 0)
	{
		start = malloc(proto.class.totalSize);
		index = start;
		for (i=0; i<proto.class.payloadSize; i++)
		{
			*index = proto.class.payload[i];
			index++;
		}
		sum = proto.class.payloadSize;
		while(sum<proto.class.totalSize) 
		{
		        recvfrom(sock, &proto, MAX, 0, (struct 
				sockaddr *) &fromAddr, &fromSize);
			sum += proto.class.payloadSize;
			for (i = 0; i<proto.class.payloadSize; i++)
			{
				*index = proto.class.payload[i];
				index++;
			}
		}
	data = fopen("gpsdata.txt","a");
	fwrite(start, 1, sum, data);
	free(start);
	fclose(data);
	}
	else
	{
		//write for group protocol
	}
}

void writeImage()
{
        char *start;
        char *index;
        int i;
        int sum=0;
	char *name = 0;

        if (protocol == 0)
        {
                start = malloc(proto.class.totalSize);
                index = start;
                for (i=0; i<proto.class.payloadSize; i++)
                {
                        *index = proto.class.payload[i];
                        index++;
                }
                sum = proto.class.payloadSize;
                while(sum<proto.class.totalSize)
                {
                        recvfrom(sock, &proto, MAX, 0, (struct
                                sockaddr *) &fromAddr, &fromSize);
                        sum += proto.class.payloadSize;
                        for (i = 0; i<proto.class.payloadSize; i++)
                        {
                                *index = proto.class.payload[i];
                                index++;
                        }
                }
	sprintf(name, "picture%d.jpg",written);
        image = fopen(name,"w");
        fwrite(start, 1, sum, image);
        free(start);
	fclose(image);
        }
        else
        {
                //write for group protocol
        }

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
