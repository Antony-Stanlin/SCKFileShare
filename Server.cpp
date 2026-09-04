#include "Server.h"
#include <iostream>
#include <cstdint>

int Server::setup() {

	int result = prepareSocket();
	if (result != 0)
		return result;

	result = bindPort();
	if (result != 0)
		return result;

	result = listenForClient();
	if (result != 0)
		return result;

	std::cout<< "Server started.\n";
	std::cout<< "Waiting for client...\n";

	result = acceptClient();
	if (result != 0)
		return result;
	std::cout<< "Client connected successfully.\n";

	setOutputPath();

	result = receiveFile();
	closeClientSocket();
	return result;
}

int Server::bindPort() {
	sockaddr_in serverAddress{};

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(PORT);

	int result = bind(
		tcpSocket,
		reinterpret_cast<sockaddr*>(&serverAddress),
		sizeof(serverAddress)
	);

	if (result == SOCKET_ERROR) {
		std::cout<< "Bind failed. Error: "
			<< WSAGetLastError()
			<< "\n";
		return 1;
	}
	return 0;
}

int Server::listenForClient() {
	int result = listen(
		tcpSocket,
		SOMAXCONN
	);

	if (result == SOCKET_ERROR) {

		std::cout<< "Listen failed. Error: "
			<< WSAGetLastError()
			<< "\n";
		return 1;
	}
	return 0;
}

int Server::acceptClient() {

	sockaddr_in clientAddress{};
	int clientAddressSize = sizeof(clientAddress);

	clientSocket = accept(
		tcpSocket,
		reinterpret_cast<sockaddr*>(&clientAddress),
		&clientAddressSize
	);

	if (clientSocket == INVALID_SOCKET) {
		std::cout<< "Accept failed. Error: "
			<< WSAGetLastError()
			<< "\n";
		return 1;
	}
	return 0;
}

int Server::receiveFile() {

	if (!receiveFileName()) {
		std::cout<< "Failed to receive file name.\n";
		return 1;
	}

	std::thread receiverThread(&Server::receiver, this);

	std::thread writerThread(&Server::writer, this);

	receiverThread.join();
	writerThread.join();

	std::cout<< "File received successfully.\n";

	return 0;
}


bool Server::receiveFileName() {
	uint32_t networkLength = 0;

	if (!receiveAll(reinterpret_cast<char*>(&networkLength),sizeof(networkLength)))
		return false;

	uint32_t nameLength = ntohl(networkLength);

	if (nameLength == 0 || nameLength > 1024) {
		std::cout<< "Invalid file name length.\n";
		return false;
	}

	std::vector<char> nameBuffer(nameLength);

	if (!receiveAll(nameBuffer.data(),static_cast<int>(nameLength)))
		return false;

	std::string fileName(nameBuffer.begin(),nameBuffer.end());

	return true;
}


void Server::receiver() {

	while (true) {

		std::vector<char> block;

		if (!receiveBlock(block)) {
			std::cout<< "Receiver: failed to receive block.\n";

			{
				std::lock_guard<std::mutex> lock(queueMutex);
				receivingFinished = true;
			}

			queueCV.notify_one();
			return;
		}

		if (block.empty())
			break;

		{
			std::lock_guard<std::mutex> lock(queueMutex);
			fileQueue.push(block);
		}

		std::cout<< "Receiver: added "
			<< block.size()
			<< " bytes to queue.\n";
		queueCV.notify_one();
	}

	receiveEndMarker();

	{
		std::lock_guard<std::mutex> lock(queueMutex);
		receivingFinished = true;
	}
	queueCV.notify_one();

	std::cout<< "Receiver: finished receiving file.\n";
}

bool Server::receiveBlock(std::vector<char>& block) {

	uint32_t networkSize = 0;

	if (!receiveAll(reinterpret_cast<char*>(&networkSize),sizeof(networkSize)))
		return false;

	uint32_t blockSize = ntohl(networkSize);

	if (blockSize == 0) {
		block.clear();
		return true;
	}

	if (blockSize > BLOCK_SIZE) {

		std::cout<< "Invalid block size: "
			<< blockSize
			<< "\n";
		return false;
	}

	block.resize(blockSize);

	if (!receiveAll(block.data(),static_cast<int>(blockSize)))
		return false;

	std::cout<< "Receiver: received "
		<< block.size()
		<< " bytes.\n";
	return true;
}

bool Server::receiveEndMarker() {
	std::cout<< "Receiver: end marker received.\n";
	return true;
}

bool Server::receiveAll(char* data,int size) {

	int totalReceived = 0;

	while (totalReceived < size) {

		int received = recv(
			clientSocket,
			data + totalReceived,
			size - totalReceived,
			0
		);

		if (received == SOCKET_ERROR) {

			std::cout<< "Receive failed. Error: "
				<< WSAGetLastError()
				<< "\n";

			return false;
		}

		if (received == 0) {

			std::cout << "Client disconnected.\n";

			return false;
		}

		totalReceived += received;
	}

	return true;
}

void Server::writer() {

	std::ofstream file(
		outputFileName,
		std::ios::binary
	);

	if (!file) {

		std::cout << "Writer: failed to create file.\n";

		return;
	}

	while (true) {

		std::vector<char> block;

		{
			std::unique_lock<std::mutex> lock(queueMutex);

			queueCV.wait(
				lock,
				[this]() {

					return !fileQueue.empty() || receivingFinished;
				}
			);

			if (fileQueue.empty() &&
				receivingFinished) {

				break;
			}


			block = fileQueue.front();

			fileQueue.pop();
		}


		queueCV.notify_one();


		file.write(
			block.data(),
			static_cast<std::streamsize>(
				block.size()
				)
		);


		if (!file) {

			std::cout << "Writer: failed to write block.\n";

			break;
		}


		std::cout << "Writer: wrote "
			<< block.size()
			<< " bytes.\n";
	}


	file.close();

	std::cout << "Writer: file writing completed.\n";
}

void Server::closeClientSocket() {

	if (clientSocket != INVALID_SOCKET) {

		closesocket(clientSocket);

		clientSocket = INVALID_SOCKET;
	}
}

void Server::setOutputPath() {

	std::string path;
	
	std::cout << "Enter the output folder : ";

	std::cin >> path;

	if (!path.empty() && path.back() == '\\')
		path.pop_back();

	outputFileName = path;

	std::cout << "Output file: "
		<< outputFileName
		<< "\n";

}