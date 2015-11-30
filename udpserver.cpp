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
#define HEADSIZE1 28
#define HEADSIZE2 28
#define MAXPENDING 5

using namespace std;

//struct classProtocol *claProto;
//struct nineProtocol *ourProto;
int myPass;
bool quit=0;

void sendData(char *data, unsigned int *buffer)
{
   int i=0;
   while (data[i]!=EOF)
   {
      //cout << data[i];
      i++;
   }
   cout << endl << i << endl;
}

void interpret1(unsigned int *buffer)
{
   for (int i=0;i<75;i++)
      buffer[i]=htonl(buffer[i]);
   if (buffer[1]==0 && buffer[2]==0)
   {
      buffer[1]=myPass;
      cout << "Connect\n";
   }
   
   else if (buffer[1]==myPass)
   {
      switch (buffer[2])
      {
         case 2:
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
            sendData(move(buffer[3]), buffer);
            break;
         case 64:
            sendData(turn(buffer[3]), buffer);
            break;
         case 128:
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
   int clntSock, servSock, servPort, robotNum;
   unsigned int clntLen;
   struct sockaddr_in servAddr, clntAddr;

   servPort=8080; //fill from -p
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
      unsigned int buffer[BUFSIZE], recvSize;
      bool is_class;
      clntLen=sizeof(clntAddr);
      
      if ((recvSize = recvfrom(servSock, buffer, BUFSIZE, 0, (struct sockaddr *) &clntAddr, &clntLen))<0)
      {
         cerr << "recv() failed 1\n";
         close(clntSock);
         exit(1);
      }

      if (buffer[0]==0)
         is_class=1;
      else
         is_class=0;
         
      interpret1(buffer); //updates buffer with the response
      
      if (sendto(servSock, buffer, BUFSIZE, 0, 
      (struct sockaddr *) &clntAddr, sizeof(clntAddr)) != BUFSIZE)
      {
         cerr << "send sent a different number of bytes\n";
         close(clntSock);
         exit(1);
      }
      
      if (is_class)
      {
         while (!quit)
         {
            if ((recvSize = recvfrom(servSock, buffer, BUFSIZE, 0, (struct sockaddr *) &clntAddr, &clntLen))<0)
            {
               cerr << "recv() failed 1\n";
               close(clntSock);
               exit(1);
            }
            
            interpret1(buffer); //updates buffer with the response
            
            if (sendto(servSock, buffer, BUFSIZE, 0, 
               (struct sockaddr *) &clntAddr, sizeof(clntAddr)) != BUFSIZE)
               {
                  cerr << "send sent a different number of bytes\n";
                  close(clntSock);
                  exit(1);
               }
         }
         quit=0;
      }
      
      else
      {/*
         if ((recvSize=recv(clntSock,buffer,BUFSIZE,0))<0)
         {
            cerr << "recv() failed\n";
            close(clntSock);
            exit(1);
         }*/
         
         if ((recvSize = recvfrom(servSock, buffer, BUFSIZE, 0, (struct sockaddr *) &clntAddr, &clntLen))<0)
         {
            cerr << "recv() failed\n";
            close(clntSock);
            exit(1);
         }
         
         unsigned char *buff=new unsigned char[BUFSIZE-HEADSIZE2];
         int j=0;
         for (int i=HEADSIZE2; i<BUFSIZE; i+=4)
         {
            buff[j] = (htonl(buffer[i/4]) >> 24) & 0xFF;
            buff[j+1] = (htonl(buffer[i/4]) >> 16) & 0xFF;
            buff[j+2] = (htonl(buffer[i/4]) >> 8) & 0xFF;
            buff[j+3] = htonl(buffer[i/4]) & 0xFF;
            j+=4;
         }

         for (int i=0; i<htonl(buffer[6]); i+=2)
         {
            interpret2(buff[i],buff[i+1],buffer);
            /*
            if (send(clntSock,buffer,BUFSIZE,0)!=BUFSIZE)
            {
               cerr << "send sent a different number of bytes\n";
               close(clntSock);
               exit(1);
            }*/
            
            if (sendto(servSock, buffer, BUFSIZE, 0, 
             (struct sockaddr *) &clntAddr, sizeof(clntAddr)) != BUFSIZE)
             {
               cerr << "send sent a different number of bytes\n";
               close(clntSock);
               exit(1);
             }
         }
      }
      quit=0;
      close(clntSock);
   }
}
