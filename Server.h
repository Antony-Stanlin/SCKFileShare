#pragma once

#include "FileTransfer.h"
#include "File.h"
#include "Thread.h"


#include <string>
#include <vector>


class Server : public FileTransfer{

private:

    SOCKET clientSocket = INVALID_SOCKET;

    std::string outputPath;

    std::string fileName;

    std::string outputFileName;

    Thread receiverThread;

    Thread writerThread;

    int ReceiveFile();

    bool ReceiveFileName();

    bool ReceiveBlock(std::vector<char>& block);

    void Receiver();

    void Writer();

    void CloseClientSocket();

public:

    void SetOutputPath(std::string outputPath);
    int Setup() override;
};