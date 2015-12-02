make:	udpserver.cpp tcpclient.c
	gcc -Wall -c -o tcpclient.o tcpclient.c
	g++ -Wall -c -o udpserver.o udpserver.cpp
	g++ -Wall -o server udpserver.o tcpclient.o
	gcc -Wall -o client udpclient.c
	rm *.o
