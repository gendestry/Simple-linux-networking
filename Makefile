all: cmain smain

cmain: Client.o cmain.o
	gcc cmain.o Client.o -o cmain

smain: Server.o smain.o
	gcc smain.o Server.o -o smain

smain.o: cmain.cpp
	gcc -c cmain.cpp -o cmain.o

smain.o: smain.cpp
	gcc -c smain.cpp -o smain.o

Client.o: Client.h Client.cpp
	gcc -c Client.cpp -o Client.o

Server.o: Server.h Server.cpp
	gcc -c Server.cpp -o Server.o

clean:
	rm *.o