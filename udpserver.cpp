#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctime>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <cstring>

#define BUFSIZE 300
#define HEADSIZE1 28
#define HEADSIZE2 28
#define MAXPENDING 5

using namespace std;

//struct classProtocol *claProto;
//struct nineProtocol *ourProto;
int myPass;
bool quit=0;
void getImage()
{

}
void getGPS()
{

}
void getdGPS()
{

}
void getLasers()
{

}
void move(int)
{

}
void turn(int)
{

}
void stop()
{

}

void interpret1(int *buffer)
{
   if (buffer[1]==0 && buffer[2]==0)
      buffer[1]=myPass;
   
   else if (buffer[1]==myPass)
   {
      switch (buffer[2])
      {
         case 2:
            getImage();
            break;
         case 4:
            getGPS();
            break;
         case 8:
            getdGPS();
            break;
         case 16:
            getLasers();
            break;
         case 32:
            move(buffer[3]);
            break;
         case 64:
            turn(buffer[3]);
            break;
         case 128:
            sleep(buffer[3]);
            stop();
            break;
         case 255:
            myPass=rand();
            quit=1;
            //reply with ack?
            break;
      }
   }
}

void interpret2(int command, int data)
{
   switch (command)
   {
      case 1:
         myPass=rand();
         quit=1;
         //reply with ack?
         break;
      case 2:
         getImage();
         break;
      case 4:
         getGPS();
         break;
      case 8:
         getdGPS();
         break;
      case 16:
         getLasers();
         break;
      case 32:
         move(data);
         break;
      case 64:
         turn(data);
         break;
      case 128:
         sleep(data);
         stop();
         break;/*
      case 255:
         getRequests(buffer);
         break;*/
   }
}

int main(int argc, char *argv[])
{
   int servSock, clntSock, servPort, robotNum;
   unsigned int clntLen;
   struct sockaddr_in servAddr, clntAddr;
   string robotID, robotName;
   
   robotName="169.55.155.236"; //fill from -h?
   robotNum=9; //fill from -n?
   robotID="5winnow"; //fill from -i?
   servPort=0; //fill from -p
   myPass=rand();
   if ((servSock=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
   {
      cerr << "socket() failed\n";
      exit(1);
   }
   
   memset(&servAddr,0,sizeof(servAddr));
   servAddr.sin_family=AF_INET;
   servAddr.sin_addr.s_addr=htonl(INADDR_ANY);
   servAddr.sin_port=htons(servPort);
   
   if (bind(servSock,(struct sockaddr *) &servAddr,sizeof(servAddr))<0)
   {
      cerr << "bind() failed\n";
      exit(1);
   }
   
   if (listen(servSock,MAXPENDING)<0)
   {
      cerr << "listen() failed\n";
      exit(1);
   }
      //design decision, functions for receieve and/or send?
      //DD, global rec/send variables or passed back in a pointer?
   while (1)
   {
      /* ours
      connect
      receive
      respond with pw
      receive once (loop eventually?)
      for loop through commands
         call interpret
         receive answer from interpret
         respond to client */
      int buffer[BUFSIZE], recvSize;
      bool is_class;
      clntLen=sizeof(clntAddr);
      if ((clntSock=accept(servSock,(struct sockaddr *) &clntAddr,&clntLen))<0)
      {
         cerr << "accept() failed\n";
         exit(1);
      }
      
      if ((recvSize=recv(clntSock,buffer,BUFSIZE,0))<0)
      {
         cerr << "recv() failed\n";
         exit(1);
      }

      if (buffer[0]==0)
         is_class=0;
      else
         is_class=1;
         
      interpret1(buffer); //updates buffer with the response
      
      if (send(clntSock,buffer,HEADSIZE1+buffer[6],0)!=HEADSIZE1+buffer[6])
      {
         cerr << "send sent a different number of bytes\n";
         exit(1);
      }
   
      if (is_class)
      {
         while (!quit)
         {
            if ((recvSize=recv(clntSock,buffer,BUFSIZE,0))<0)
            {
               cerr << "recv() failed\n";
               exit(1);
            }

            interpret1(buffer); //updates buffer with the response
            
            if (send(clntSock,buffer,HEADSIZE1+buffer[6],0)!=HEADSIZE1+buffer[6])
            {
               cerr << "send sent a different number of bytes\n";
               exit(1);
            }
         }
      }
      
      else
      {
         if ((recvSize=recv(clntSock,buffer,BUFSIZE,0))<0)
         {
            cerr << "recv() failed\n";
            exit(1);
         }
         
         interpret1(buffer);
         
         for (int i=HEADSIZE2; i<HEADSIZE2+buffer[6]; i=i+2)
         {
            interpret2(buffer[i],buffer[i+1]);
            
            if (send(clntSock,buffer,HEADSIZE1+buffer[6],0)!=HEADSIZE1+buffer[6])
            {
               cerr << "send sent a different number of bytes\n";
               exit(1);
            }
         }
      }
      quit=0;
      close(clntSock);
   }
}
