#pragma once
#include <string>
#include <thread>

class Client {
    int m_Socket;
    int m_Port;
    const char* m_ServerIP;

    std::thread* m_ListeningThread;
public:
    Client(const char* ip, int port);
    ~Client();    

    std::string recieveString();
    void recieveRaw(void* data, int& length);

    void processServer();
};