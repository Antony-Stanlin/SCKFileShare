#include "Socket.h"


Socket::~Socket() {

	if (tcpSocket != INVALID_SOCKET)
		closesocket(tcpSocket);

	WSACleanup();
}

void Socket::closeAndCleanup(SOCKET* socket1) {

	if (*socket1 != INVALID_SOCKET)
		closesocket(*socket1);

	WSACleanup();
}

void Socket::closeAndCleanup(SOCKET* socket1, SOCKET* socket2) {

	if (*socket1 != INVALID_SOCKET)
		closesocket(*socket1);

	if (*socket2 != INVALID_SOCKET)
		closesocket(*socket2);

	WSACleanup();
}

int Socket::checkForError(int result, std::string message) {

	if (result != 0) {

		std::cout << message
			<< result
			<< "\n";

		return 1;
	}

	return 0;
}

int Socket::checkForError(SOCKET* socket1, std::string message) {

	if (*socket1 == INVALID_SOCKET) {

		std::cout << message
			<< WSAGetLastError()
			<< "\n";

		closeAndCleanup(socket1);

		return 1;
	}

	return 0;
}

int Socket::checkForError(SOCKET* socket1, SOCKET* socket2, std::string message) {

	if (*socket2 == INVALID_SOCKET) {

		std::cout << message
			<< WSAGetLastError()
			<< "\n";

		closeAndCleanup(socket1, socket2);

		return 1;
	}

	return 0;
}

int Socket::checkForError(SOCKET* socket1, int result, std::string message) {

	if (result == SOCKET_ERROR) {

		std::cout << message
			<< WSAGetLastError()
			<< "\n";

		closeAndCleanup(socket1);

		return 1;
	}

	return 0;
}

int Socket::initializeSocket() {

	int result = WSAStartup(
		MAKEWORD(2, 2),
		&wsa
	);

	return checkForError(
		result,
		"WinSockAPI failed with error code: "
	);
}

int Socket::createSocket() {

	tcpSocket = socket(
		AF_INET,
		SOCK_STREAM,
		IPPROTO_TCP
	);

	return checkForError(
		&tcpSocket,
		"Failed to create socket. Error: "
	);
}

int Socket::prepareSocket() {

	int result = initializeSocket();

	if (result == 1)
		return result;

	return createSocket();
}