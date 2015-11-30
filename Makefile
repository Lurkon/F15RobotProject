make:	udpserver.cpp tcpclient.c
	gcc -c -o tcpclient.o tcpclient.c
	g++ -c -o udpserver.o udpserver.cpp
	g++ -o proxy udpserver.o tcpclient.o
	gcc -o client udpclient.c
	rm *.o
