#pragma once

#include "Socket.h"

#include <fstream>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>
#include <string>


class Client : public Socket {

public:

	int setup() override;


private:

	static constexpr int PORT = 5000;
	static constexpr size_t BLOCK_SIZE = 7 * 1024;


	std::string fileName;
	std::queue<std::vector<char>> fileQueue;
	std::mutex queueMutex;
	std::condition_variable queueCV;

	bool readingFinished = false;

	int connectToServer(std::string serverIP);
	std::string getServerIP();

	std::string getFileName();
	int sendFile();
	bool sendFileName();
	bool sendBlock(std::vector<char>& block);
	bool sendEndMarker();
	bool sendAll(const char* data, int size);

	void reader();
	void sender();
};