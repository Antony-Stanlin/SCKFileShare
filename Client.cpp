#include "Client.h"

#include <iostream>
#include <cstdint>


int Client::setup() {

	int result = prepareSocket();
	if (result == 1)
		return result;

	std::string serverIP = getServerIP();

	result = connectToServer(serverIP);

	if (result == 1)
		return result;

	std::cout<< "Server connected successfully.\n";

	return sendFile();
}

int Client::connectToServer(std::string serverIP) {
	sockaddr_in serverAddress{};
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(PORT);

	int result = inet_pton(
		AF_INET,
		serverIP.c_str(),
		&serverAddress.sin_addr
	);

	if (result < 1) {
		std::cout<< "Invalid IP Address.\n";
		return 1;
	}

	result = connect(
		tcpSocket,
		reinterpret_cast<sockaddr*>(&serverAddress),
		sizeof(serverAddress)
	);

	if (result == SOCKET_ERROR) {

		std::cout<< "Failed to connect server. Error: "
			<< WSAGetLastError()
			<< "\n";
		return 1;
	}

	return 0;
}

std::string Client::getServerIP() {

	std::string ip;

	std::cout<< "Enter the server IP : ";
	std::cin >> ip;

	return ip;
}

std::string Client::getFileName() {

	std::string name;

	std::cout<< "Enter the file name : ";
	std::cin >> name;

	return name;
}


int Client::sendFile() {

	fileName = getFileName();

	std::ifstream file(fileName, std::ios::binary);

	if (!file) {
		std::cout<< "Failed to open file.\n";
		return 1;
	}

	file.close();

	if (!sendFileName()) {

		std::cout<< "Failed to send file name.\n";
		return 1;
	}

	std::thread readerThread(&Client::reader, this);

	std::thread senderThread(&Client::sender, this);

	readerThread.join();
	senderThread.join();

	std::cout << "File transfer completed.\n";

	return 0;
}

bool Client::sendFileName() {

	uint32_t nameLength = static_cast<uint32_t>(fileName.size());

	uint32_t networkLength = htonl(nameLength);


	// Send file name length

	if (!sendAll(
		reinterpret_cast<char*>(&networkLength),
		sizeof(networkLength)
	)) {
		return false;
	}


	// Send file name

	if (!sendAll(
		fileName.data(),
		static_cast<int>(fileName.size())
	)) {

		return false;
	}


	std::cout << "File name sent: "
		<< fileName
		<< "\n";

	return true;
}

void Client::reader() {

	std::ifstream file(
		fileName,
		std::ios::binary
	);


	if (!file) {

		std::cout<< "Reader: failed to open file.\n";

		{
			std::lock_guard<std::mutex> lock(queueMutex);

			readingFinished = true;
		}

		queueCV.notify_one();

		return;
	}

	while (true) {

		std::vector<char> buffer(BLOCK_SIZE);

		file.read(
			buffer.data(),
			static_cast<std::streamsize>(BLOCK_SIZE)
		);

		std::streamsize bytesRead = file.gcount();

		if (bytesRead <= 0)
			break;

		buffer.resize(static_cast<size_t>(bytesRead));

		{
			std::lock_guard<std::mutex> lock(queueMutex);

			fileQueue.push(buffer);
		}

		std::cout << "Reader: added "
			<< buffer.size()
			<< " bytes to queue.\n";

		queueCV.notify_one();
	}

	file.close();

	{
		std::lock_guard<std::mutex> lock(queueMutex);

		readingFinished = true;
	}

	queueCV.notify_one();

	std::cout << "Reader: finished reading file.\n";
}

void Client::sender() {

	while (true) {

		std::vector<char> block;

		{
			std::unique_lock<std::mutex> lock(queueMutex);

			queueCV.wait(
				lock,
				[this]() {
					return !fileQueue.empty() || readingFinished;
				}
			);

			if (fileQueue.empty() && readingFinished)
				break;

			block = fileQueue.front();

			fileQueue.pop();
		}

		queueCV.notify_one();

		if (!sendBlock(block)) {

			std::cout<< "Sender: failed to send block.\n";

			return;
		}
	}

	if (!sendEndMarker()) {

		std::cout << "Sender: failed to send end marker.\n";

		return;
	}

	std::cout << "Sender: finished sending file.\n";
}

bool Client::sendBlock(std::vector<char>& block) {

	uint32_t blockSize = static_cast<uint32_t>(block.size());

	uint32_t networkSize = htonl(blockSize);

	// Send block size

	if (!sendAll(
		reinterpret_cast<char*>(&networkSize),
		sizeof(networkSize)
	)) {

		return false;
	}


	// Send complete block

	if (!sendAll(
		block.data(),
		static_cast<int>(block.size())
	)) {

		return false;
	}

	std::cout << "Sender: sent block "
		<< block.size()
		<< " bytes.\n";

	return true;
}

bool Client::sendEndMarker() {

	uint32_t endMarker = 0;


	if (!sendAll(
		reinterpret_cast<char*>(&endMarker),
		sizeof(endMarker)
	)) {

		return false;
	}

	std::cout << "Sender: end marker sent.\n";

	return true;
}

bool Client::sendAll(const char* data,int size) {

	int totalSent = 0;

	while (totalSent < size) {

		int sent = send(
			tcpSocket,
			data + totalSent,
			size - totalSent,
			0
		);

		if (sent == SOCKET_ERROR) {

			std::cout << "Send failed. Error: "
				<< WSAGetLastError()
				<< "\n";

			return false;
		}

		if (sent == 0)
			return false;

		totalSent += sent;
	}
	return true;
}