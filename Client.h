#pragma once

#include "FileTransfer.h"
#include "File.h"
#include "Thread.h"

#include <string>
#include <vector>


class Client : public FileTransfer
{

private:

    std::string serverIP;

    std::string filePath;


    Thread readerThread;

    Thread senderThread;


    int SendFile();

    bool SendFileName();

    bool SendBlock(
        const std::vector<char>& block
    );

    bool SendEndMarker();


    void Reader();

    void Sender();


    std::string GetFileName();


public:

    void SetServerIP(
        std::string serverIP
    );


    void SetFilePath(
        std::string filePath
    );


    int Setup() override;

};