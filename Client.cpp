#include "Client.h"
#include <iostream>

#include <string.h>
#include <stdlib.h>
#include <unistd.h>     // close socket
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

Client::Client(const char* ip, int port) {
    m_Port = port;
    m_ServerIP = ip;

    m_Socket = socket(AF_INET, SOCK_STREAM, 0);
    if(m_Socket < 0)
        std::cout << "Failed to create socket!" << std::endl;

    struct hostent* server;
    server = gethostbyname(ip);

    struct sockaddr_in serv_addr;
    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char*) server->h_addr, (char*) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port);

    if(connect(m_Socket, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "Failed to connect!" << std::endl;
        return;
    }

    m_ListeningThread = new std::thread(&Client::processServer, this);
}

Client::~Client() {
    m_ListeningThread->join();
    delete m_ListeningThread;

    close(m_Socket);
}


void Client::processServer() {
    while(true) {
        std::cout << recieveString() << std::endl;
    }
}


std::string Client::recieveString() {
    int bufflen, res;
    res = read(m_Socket, (void*)&bufflen, sizeof(bufflen));

    if(res == 0)
        return "";

    char buffer[bufflen + 1];
    bzero(buffer, sizeof(buffer));
    res = read(m_Socket, (void*) &buffer, sizeof(buffer));

    if(res == 0)
        return "";

    std::string ret = buffer;
    return ret;
}

void Client::recieveRaw(void* data, int& length) {
    
}