#pragma once

#include <iostream>

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <string>

#pragma comment(lib, "ws2_32.lib")


class Socket
{

protected:

    WSAData wsa{};

    SOCKET tcpSocket = INVALID_SOCKET;

    static constexpr int PORT = 5000;


    int PrepareSocket();

    int Connect(std::string serverIP);

    int Bind();

    int Listen();

    int Accept(SOCKET& clientSocket);

    bool SendAll(SOCKET socket, const char* data, int size);

    bool ReceiveAll(SOCKET socket,char* data,int size);

    void CloseSocket(SOCKET& socket);

public:

    Socket();

    virtual ~Socket();

    virtual int Setup() = 0;


private:

    int InitializeSocket();

    int CreateSocket();

};