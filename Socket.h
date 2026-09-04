#pragma once

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")


class Socket {

protected:
	WSAData wsa{};
	SOCKET tcpSocket = INVALID_SOCKET;
	int prepareSocket();
	void closeAndCleanup(SOCKET* socket1);
	void closeAndCleanup(SOCKET* socket1, SOCKET* socket2);
	int checkForError(int result, std::string message);
	int checkForError(SOCKET* socket1, std::string message);
	int checkForError(SOCKET* socket1, SOCKET* socket2, std::string message);
	int checkForError(SOCKET* socket1, int result, std::string message);
public:
	virtual ~Socket();
	virtual int setup() = 0;
private:
	int initializeSocket();
	int createSocket();
};