#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctime>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <cstring>
#include <stdio.h>
#include "tcpclient.h"

#define BUFSIZE 300
#define HEADSIZE 28
#define MAXPENDING 5

using namespace std;

//struct classProtocol *claProto;
//struct nineProtocol *ourProto;
unsigned int myPass;
int servSock, servPort;
sockaddr_in servAddr, cliAddr;
bool quit=0;

void sendData(char *data, unsigned int *buffer)
{
   int size=((int *) data)[0]; //total data to send
   data+=4;
   int buff_size=BUFSIZE-HEADSIZE;
   int send=size/buff_size;
   int start=0;
   if (size%buff_size!=0)
      send++;
   buffer[5]=size;
   
   for (int j=1;j<=send;j++)
   {
      buffer[4]=start;
      int i;
      for (i=start;i<j*buff_size;i++)
      {
         if (i<size)
         {
            *(char *) ((char *)buffer+i-start+HEADSIZE)=data[i];
         }
      }
      
      if (i>size)
         buffer[6]=size-(j-1)*buff_size;
      else
         buffer[6]=buff_size;
         
      //cout << buffer[6] << endl;
      /*
      for (i=0;i<7;i++)
      {
         cout << buffer[i] << endl;//cout << (int) ((char *)buffer)[i] << endl;
      }
      */
      if ((int) sendto(servSock, buffer, BUFSIZE, 0, 
      (struct sockaddr *) &cliAddr, sizeof(cliAddr)) != BUFSIZE)
      {
         perror("Send failed\n");
         cerr << "send sent a different number of bytes x\n";
         close(servSock);
         exit(1);
      }
      cout << "offset: " << buffer[4] << " total: " << buffer[5] << " payload: " << buffer[6] << endl;
      start+=BUFSIZE-HEADSIZE;
   }
   //free(data);
}

void interpret1(unsigned int *buffer)
{
   //cout << buffer[2] << "|" << htonl(buffer[2]) << endl;
   //for (int i=0;i<75;i++)
      buffer[1]=htonl(buffer[1]);
   if (buffer[1]==0 && buffer[2]==0)
   {
      buffer[1]=myPass;
      cout << "Connect\n";
   }
   
   else if (buffer[1]==myPass)
   {
      //cout << "request: " << buffer[2] << endl;
      switch (buffer[2])
      {
         case 2:
            cout << "image\n";
            sendData(getImage(), buffer);
            break;
         case 4:
            cout << "GPS\n";
            sendData(getGPS(), buffer);
            break;
         case 8:
            cout << "dGPS\n";
            sendData(getdGPS(), buffer);
            break;
         case 16:
            sendData(getLasers(), buffer);
            break;
         case 32:
            cout << "MOVE BITCH\n";
            sendData(move(buffer[3]), buffer);
            break;
         case 64:
            cout << "Turn\n";
            sendData(turn(buffer[3]), buffer);
            break;
         case 128:
            cout << "Sleep\n";
            sleep(buffer[3]);
            sendData(stop(), buffer);
            break;
         case 255:
            myPass=rand();
            quit=1;
            //cout << "Quit\n";
            //reply with ack?
            break;
      }
   }
   for (int i=0;i<75;i++)
      buffer[i]=ntohl(buffer[i]);
}

void interpret2(unsigned char command, unsigned char data, unsigned int *buffer)
{
	//cout << (int) command << "|" << (int) data << endl;
   switch (command)
   {
      case 1:
         myPass=rand();
         quit=1;
         //cout << "Quit\n";
         //reply with ack?
         break;
      case 2:
         cout << "Image: \n";
         //cout << (int *) getImage();
         sendData(getImage(), buffer);
         break;
      case 4:
         sendData(getGPS(), buffer);
         break;
      case 8:
         sendData(getdGPS(), buffer);
         break;
      case 16:
         sendData(getLasers(), buffer);
         break;
      case 32:
         cout << "Move: ";
         sendData(move(data), buffer);
         break;
      case 64:
         cout << "Turn: ";
         sendData(turn(data), buffer);
         break;
      case 128:
         sleep(data);
         cout << "Stop: ";
         sendData(stop(), buffer);
         break;/*
      case 255:
         getRequests(buffer);
         break;*/
   }
}

int main(int argc, char *argv[])
{
   srand(time(NULL));
   int robotNum;
   unsigned int clntLen;
   //struct sockaddr_in servAddr;
   char *servIP, *robotID;
   
   if (argc<9)
   {
      cerr << "./server -h <hostname-of-robot> -i <robot-id> -n <robot-number> -p <port>" << endl;
      exit(1);
   }
   
   //port
   if (strcmp(argv[1],"-p")==0)
   {
      servPort=atoi(argv[2]);
   }
   else if (strcmp(argv[3],"-p")==0)
   {
      servPort=atoi(argv[4]);
   }
   else if (strcmp(argv[5],"-p")==0)
   {
      servPort=atoi(argv[6]);
   }
   else if (strcmp(argv[7],"-p")==0)
   {
      servPort=atoi(argv[8]);
   }
   
   //IP
   if (strcmp(argv[1],"-h")==0)
   {
      servIP=argv[2];
   }
   else if (strcmp(argv[3],"-h")==0)
   {
      servIP=argv[4];
   }
   else if (strcmp(argv[5],"-h")==0)
   {
      servIP=argv[6];
   }
   else if (strcmp(argv[7],"-h")==0)
   {
      servIP=argv[8];
   }
   //robotID
   if (strcmp(argv[1],"-i")==0)
   {
      robotID=argv[2];
   }
   else if (strcmp(argv[3],"-i")==0)
   {
      robotID=argv[4];
   }
   else if (strcmp(argv[5],"-i")==0)
   {
      robotID=argv[6];
   }
   else if (strcmp(argv[7],"-i")==0)
   {
      robotID=argv[8];
   }
   //robotNum
   if (strcmp(argv[1],"-n")==0)
   {
      robotNum=atoi(argv[2]);
   }
   else if (strcmp(argv[3],"-n")==0)
   {
      robotNum=atoi(argv[4]);
   }
   else if (strcmp(argv[5],"-n")==0)
   {
      robotNum=atoi(argv[6]);
   }
   else if (strcmp(argv[7],"-n")==0)
   {
      robotNum=atoi(argv[8]);
   }
   
   setVar(servIP, robotID, robotNum);
  
   //commands(argv);

   myPass=rand();
   if ((servSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
   {
      fprintf(stderr,"socket failed\n");
      exit(1);
   }
   
   const int trueValue = 1;
   setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &trueValue, sizeof(trueValue));
   #ifdef __APPLE__   // MacOS/X requires an additional call also
   setsockopt(servSock, SOL_SOCKET, SO_REUSEPORT, &trueValue, sizeof(trueValue));
   #endif
   
   memset(&servAddr,0,sizeof(servAddr));
   servAddr.sin_family=AF_INET;
   servAddr.sin_addr.s_addr=htonl(INADDR_ANY);
   servAddr.sin_port=htons(servPort);
   
   if (bind(servSock,(struct sockaddr *) &servAddr,sizeof(servAddr))<0)
   {
      cerr << "bind() failed\n";
      exit(1);
   }
   
   while (1)
   {
      cout << "New Client\n";
      unsigned int buffer[BUFSIZE/sizeof(unsigned int)], recvSize;
      bool is_class;
      clntLen=sizeof(cliAddr);

      if ((recvSize = recvfrom(servSock, buffer, BUFSIZE, 0, (struct sockaddr *) &cliAddr, &clntLen)<0))
      {
         cerr << "recv() failed 1\n";
         close(servSock);
         exit(1);
      }

      if (buffer[0]==0)
         is_class=1;
      else
         is_class=0;
         
      interpret1(buffer); //updates buffer with the password

      if (sendto(servSock, buffer, BUFSIZE, 0, 
      (struct sockaddr *) &cliAddr, sizeof(cliAddr)) != BUFSIZE)
      {
         cerr << "send sent a different number of bytes z\n";
         close(servSock);
         exit(1);
      }
      
      if (is_class)
      {
         while (!quit)
         {
            if ((recvSize = recvfrom(servSock, buffer, BUFSIZE, 0, (struct sockaddr *) &cliAddr, &clntLen))<0)
            {
               cerr << "recv() failed 1\n";
               close(servSock);
               exit(1);
            }
            
            interpret1(buffer); //updates buffer with the response
         }
         quit=0;
      }
      
      else
      {
         if ((recvSize = recvfrom(servSock, buffer, BUFSIZE, 0, (struct sockaddr *) &cliAddr, &clntLen))<0)
         {
            cerr << "recv() failed\n";
            close(servSock);
            exit(1);
         }
         
         unsigned char *buff=new unsigned char[BUFSIZE-HEADSIZE];
         int j=0;
         for (int i=HEADSIZE; i<BUFSIZE; i+=4)
         {
            buff[j] = (htonl(buffer[i/4]) >> 24) & 0xFF;
            buff[j+1] = (htonl(buffer[i/4]) >> 16) & 0xFF;
            buff[j+2] = (htonl(buffer[i/4]) >> 8) & 0xFF;
            buff[j+3] = htonl(buffer[i/4]) & 0xFF;
            j+=4;
         }

         for (int i=0; i<(int)htonl(buffer[6]); i+=2)
         {
            interpret2(buff[i],buff[i+1],buffer);
         }
      }
      quit=0;
      //close(servSock);
   }
}
