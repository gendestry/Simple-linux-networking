#include "Server.h"
#include <iostream>
#include <fstream>

#include <string.h> // bzero
#include <ctime>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

Server::Server(int port) {
    m_Port = port;

    m_Socket = socket(AF_INET, SOCK_STREAM, 0);

    if(m_Socket < 0)
        Log("ERR, Socket not created");

    // configure server socket
    struct sockaddr_in serv_addr;

    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    // bind it with the conf
    bind(m_Socket, (sockaddr*) &serv_addr, sizeof(serv_addr));
    listen(m_Socket, m_MaxConnQueued);

    m_ListeningThread = new std::thread(&Server::listenForClients, this);
}

Server::~Server() {
    m_ListeningThread->join();
    delete m_ListeningThread;

    close(m_Socket);
}



// Logging 
void Server::Log(std::string txt) {
    std::cout << getCurrentTime() << txt << std::endl;

    if(m_LogToFile) {
        std::ofstream file;
        file.open(m_LogFilePath, std::ios::app);

        if(!file.is_open()) {
            std::cout << getCurrentTime() << "Could not create file: " << m_LogFilePath << std::endl;
            return;
        }

        file << getCurrentTime() << txt << std::endl;
        file.close();
    }
}

void Server::logToFile(bool logtofile) {
    m_LogToFile = logtofile;
}

void Server::logFilePath(std::string path) {
    m_LogFilePath = path;
}



// Private
std::string Server::getCurrentTime() {
    std::time_t t = std::time(0); // current time
    std::tm* now = std::localtime(&t);

    std::string time;
    time.append("[");
    (now->tm_hour < 10) ? time.append("0" + std::to_string(now->tm_hour)) : time.append(std::to_string(now->tm_hour));
    time.append(":");
    (now->tm_min < 10) ? time.append("0" + std::to_string(now->tm_min)) : time.append(std::to_string(now->tm_min));
    time.append(":");
    (now->tm_sec < 10) ? time.append("0" + std::to_string(now->tm_sec)) : time.append(std::to_string(now->tm_sec));
    time.append("] ");

    return time;
}


// Send & recieve helper
void Server::sendString(int socket, std::string msg) {
    int len = msg.length();

    if(len > 0) {
        write(socket, (void*)&len, sizeof(void*));
        write(socket, (void*)msg.c_str(), len);
    }
}

void Server::sendRaw(int socket, void* data, int len) {
    write(socket, (void*)&len, sizeof(void*));
    write(socket, &data, len);
}


// Listening thread
void Server::listenForClients() {
    Log("Server startup initiated! Listening for connections...");

    // Client info stored here
    struct sockaddr_in cli_addr;
    socklen_t cli_len = sizeof(cli_addr);

    int newSocket;
    while(true) {
        newSocket = accept(m_Socket, (sockaddr*) &cli_addr, &cli_len);

        if(newSocket != 0) { // if connected
            Connection conn;
            conn.socket = newSocket;
            conn.ip = inet_ntoa(cli_addr.sin_addr);

            std::thread* thread = new std::thread(&Server::processClient, this, m_Connections.size());
            conn.th = thread;

            m_Connections.push_back(conn);
        }
    }

    // Cleanup
    for(int i = 0; i < m_Connections.size(); i++) {
        Connection& conn = m_Connections[i];
        close(conn.socket);
        conn.th->join();
        delete conn.th;
    }
}

void Server::processClient(int index) {
    Connection& conn = m_Connections[index];

    Log("Client[" + std::to_string(index) + "] joined with IP: " + std::string(conn.ip) + "!");

    // send motd
    sendString(conn.socket, "Welcome to the server!");

    while(true) {
        
    }

    conn.alive = false;
    close(conn.socket);

    Log("Client[" + std::to_string(index) + "] left!");
}