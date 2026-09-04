#pragma once

#include "Socket.h"

#include <condition_variable>
#include <fstream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>


class Server : public Socket {

private:

	static constexpr int PORT = 5000;
	static constexpr size_t BLOCK_SIZE = 7 * 1024;
	SOCKET clientSocket = INVALID_SOCKET;
	std::string outputFileName;

	std::queue<std::vector<char>> fileQueue;
	std::mutex queueMutex;
	std::condition_variable queueCV;

	bool receivingFinished = false;

	int bindPort();
	int listenForClient();
	int acceptClient();

	void setOutputPath();
	int receiveFile();
	bool receiveFileName();
	bool receiveBlock(std::vector<char>& block);
	bool receiveEndMarker();
	bool receiveAll(char* data, int size);
	
	void receiver();
	void writer();

	void closeClientSocket();


public:

	int setup() override;
};