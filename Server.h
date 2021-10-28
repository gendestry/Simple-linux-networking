#pragma once
#include <string>
#include <thread>
#include <vector>

struct Connection {
    int socket;
    std::thread* th;
    char* ip;
    bool alive = true;
};

class Server {
private:
    // Packet stuff
    int m_Socket;
    int m_Port;
    int m_MaxConnQueued = 5;

    // Logging
    bool m_LogToFile = false;
    std::string m_LogFilePath = "server.log";

    // Connections
    std::vector<Connection> m_Connections;
    std::thread* m_ListeningThread;

public:
    Server(int port);
    ~Server();

    void Log(std::string txt);

    void logToFile(bool logtofile);
    void logFilePath(std::string path);

    void sendString(int socket, std::string msg);
    void sendRaw(int socket, void* data, int len);

private:
    std::string getCurrentTime();

    void listenForClients();
    void processClient(int index);
};