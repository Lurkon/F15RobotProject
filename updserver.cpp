#include <iostream>
#include <cstblib>
#include <cinttypes>

#define BUFSIZE 300
#define HEADSIZE1 28
#define HEADSIZE2 28

using namespace std;

struct classProtocol *claProto;
struct nineProtocol *ourProto;
string myPass;
void getImage();
void getGPS();
void getdGPS();
void getLasers();
void move(int i);
void turn(int i);
void stop();

int32_t ** interpret(int32_t **buffer)
{
   if (buffer[1]==0 && buffer[2]==0)
   {
      buffer[1]=myPass;
      return buffer;
   }
   
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
            //reply with ack?
            break;
      }
   }
}

int main(int argc, char *argv[])
{
   int servSock, clntSock, servPort, clntLen, robotNum;
   struct sockaddr_in servAddr, clntAddr;
   string robotID, robotName;
   
   robotName="169.55.155.236"; //fill from -h?
   robotNum=9; //fill from -n?
   robotID="5winnow"; //fill from -i?
   servPort=0; //fill from -p
   myPass=rand();
   if ((servSock=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
   {
      fprintf(stderr,"socket() failed\n");
      exit(1);
   }
   
   memset(&servAddr,0,sizeof(servAddr));
   servAddr.sin_family=AF_INET;
   servAddr.sin_addr.s_addr=htonl(INADDR_ANY);
   servAddr.sin_port=htons(servPort);
   
   if (bind(servSock,(struct sockaddr *) &servAddr,sizeof(servAddr))<0)
   {
      fprintf(stderr,"bind() failed\n");
      exit(1);
   }
   
   if (listen(servSock,MAXPENDING)<0)
   {
      fprintf(stderr,"listen() failed\n");
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
      /* class
      connect
      infinite while
         receive command
         call interpret
         receive answer from interpret
         respond to client */
      int32_t buffer[BUFSIZE];
      clntLen=sizeof(clntAddr);
      if ((clntSock=accept(servSock,(struct sockaddr *) &clntAddr,&clntLen))<0)
      {
         fprintf(stderr,"accept() failed\n");
         exit(1);
      }
      
      while (1)
      {
         int recvSize;
         
         if ((recvSize=recv(clntSock,buffer,BUFSIZE,0))<0)
         {
            fprintf(stderr,"recv() failed\n");
            exit(1);
         }

         buffer=interpret(buffer);
         
         if (send(clntSock,buffer,HEADSIZE1+buffer[6],0)!=HEADSIZE1+buffer[6])
         {
            fprintf(stderr,"send sent a different number of bytes\n");
            exit(1);
         }
/*
         while (recvSize>0)
         {
            fwrite(buffer,recvSize,1,fp);
            memset(&buffer[0], 0, sizeof(buffer));
            if ((recvSize=recv(clntSock,buffer,BUFSIZE,0))<0)
            {
               fprintf(stderr,"recv() failed\n");
               exit(1);
            }
         }*/
         close(clntSock);
      }
   }
}
