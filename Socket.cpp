#include "Socket.h"


Socket::Socket(){
}

Socket::~Socket(){
    CloseSocket(tcpSocket);

    WSACleanup();
}

void Socket::CloseSocket(SOCKET& socket){

    if (socket != INVALID_SOCKET){
        closesocket(socket);

        socket = INVALID_SOCKET;
    }
}

int Socket::InitializeSocket(){

    int result = WSAStartup(MAKEWORD(2, 2), &wsa);

    if (result != 0){
        std::cout << "WinSockAPI failed with error code: "
            << result
            << "\n";

        return 1;
    }

    return 0;
}


int Socket::CreateSocket(){

    tcpSocket = socket(
        AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP
    );

    if (tcpSocket == INVALID_SOCKET){

        std::cout << "Failed to create socket. Error: "
            << WSAGetLastError()
            << "\n";

        return 1;
    }


    return 0;
}

int Socket::PrepareSocket(){

    int result = InitializeSocket();

    if (result != 0)
        return result;

    return CreateSocket();
}


int Socket::Connect(std::string serverIP){

    sockaddr_in serverAddress{};

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);

    int result = inet_pton(
        AF_INET,
        serverIP.c_str(),
        &serverAddress.sin_addr
    );

    if (result != 1){

        std::cout << "Invalid IP Address.\n";

        return 1;
    }

    result = connect(
        tcpSocket,
        reinterpret_cast<sockaddr*>(&serverAddress),
        sizeof(serverAddress)
    );

    if (result == SOCKET_ERROR){

        std::cout << "Failed to connect server. Error: "
            << WSAGetLastError()
            << "\n";

        return 1;
    }

    return 0;
}


int Socket::Bind(){

    sockaddr_in serverAddress{};

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(PORT);


    int result = bind(
        tcpSocket,
        reinterpret_cast<sockaddr*>(&serverAddress),
        sizeof(serverAddress)
    );

    if (result == SOCKET_ERROR){

        std::cout << "Bind failed. Error: "
            << WSAGetLastError()
            << "\n";

        return 1;
    }
    return 0;
}

int Socket::Listen(){

    int result = listen(
        tcpSocket,
        SOMAXCONN
    );


    if (result == SOCKET_ERROR)
    {

        std::cout
            << "Listen failed. Error: "
            << WSAGetLastError()
            << "\n";

        return 1;
    }


    return 0;
}


int Socket::Accept(
    SOCKET& clientSocket)
{

    sockaddr_in clientAddress{};


    int clientAddressSize =
        sizeof(clientAddress);


    clientSocket = accept(
        tcpSocket,
        reinterpret_cast<sockaddr*>(
            &clientAddress
            ),
        &clientAddressSize
    );


    if (clientSocket == INVALID_SOCKET)
    {

        std::cout
            << "Accept failed. Error: "
            << WSAGetLastError()
            << "\n";

        return 1;
    }


    return 0;
}


bool Socket::SendAll(
    SOCKET socket,
    const char* data,
    int size)
{

    int totalSent = 0;


    while (totalSent < size)
    {

        int result = send(
            socket,
            data + totalSent,
            size - totalSent,
            0
        );


        if (result == SOCKET_ERROR)
        {

            std::cout
                << "Send failed. Error: "
                << WSAGetLastError()
                << "\n";

            return false;
        }


        if (result == 0)
        {
            return false;
        }


        totalSent += result;
    }


    return true;
}


bool Socket::ReceiveAll(
    SOCKET socket,
    char* data,
    int size)
{

    int totalReceived = 0;


    while (totalReceived < size)
    {

        int result = recv(
            socket,
            data + totalReceived,
            size - totalReceived,
            0
        );


        if (result == SOCKET_ERROR)
        {

            std::cout
                << "Receive failed. Error: "
                << WSAGetLastError()
                << "\n";

            return false;
        }


        if (result == 0)
        {

            std::cout
                << "Connection closed by peer.\n";

            return false;
        }


        totalReceived += result;
    }


    return true;
}