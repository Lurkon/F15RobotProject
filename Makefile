make:	udpserver.cpp tcpclient.c
	gcc -g -Wall -c -o tcpclient.o tcpclient.c
	g++ -g -Wall -c -o udpserver.o udpserver.cpp
	g++ -g -Wall -o server udpserver.o tcpclient.o
	gcc -g -Wall -o client udpclient.c
	rm *.o
