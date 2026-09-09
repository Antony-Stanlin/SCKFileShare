#include "Client.h"

#include <iostream>
#include <cstdint>


void Client::SetServerIP(std::string serverIP){
    this->serverIP = serverIP;
}


void Client::SetFilePath(std::string filePath){
    this->filePath = filePath;
}


int Client::Setup(){

    int result = PrepareSocket();

    if (result != 0)
        return result;

    result = Connect(serverIP);

    if (result != 0)
        return result;

    std::cout << "Server connected successfully.\n";

   return SendFile();
}

int Client::SendFile(){

    File file;

    if (!file.OpenForRead(filePath))
        return 1;

    file.Close();

    if (!SendFileName()){

        std::cout << "Failed to send file name.\n";
        return 1;
    }

    readerThread.Start(&Client::Reader, this);

    senderThread.Start(&Client::Sender, this);

    readerThread.Join();
    senderThread.Join();

    std::cout << "File transfer completed.\n";

    return 0;
}

bool Client::SendFileName(){

    std::string fileName = GetFileName();

    uint32_t nameLength = static_cast<uint32_t>(fileName.size());

    uint32_t networkLength = htonl(nameLength);

    if (!SendAll(
        tcpSocket,
        reinterpret_cast<const char*>(&networkLength),
        sizeof(networkLength)
    ))
    {
        return false;
    }

    if (!SendAll(
        tcpSocket,
        fileName.data(),
        static_cast<int>(fileName.size())
    )){
        return false;
    }

    std::cout << "File name sent: "
        << fileName
        << "\n";

    return true;
}

bool Client::SendBlock( const std::vector<char>& block){

    uint32_t blockSize = static_cast<uint32_t>(block.size());

    uint32_t networkSize = htonl(blockSize);

    if (!SendAll(
        tcpSocket,
        reinterpret_cast<const char*>(&networkSize),
        sizeof(networkSize)
    ))
    {

        return false;
    }

    if (!SendAll(
        tcpSocket,
        block.data(),
        static_cast<int>(
            block.size()
            )
    ))
    {
        return false;
    }

    std::cout << "Sender: sent block "
        << block.size()
        << " bytes.\n";

    return true;
}

bool Client::SendEndMarker(){

    uint32_t endMarker = 0;

    uint32_t networkMarker = htonl(endMarker);

    if (!SendAll(
        tcpSocket,
        reinterpret_cast<const char*>(&networkMarker),
        sizeof(networkMarker)
    ))
    {
        return false;
    }

    std::cout << "Sender: end marker sent.\n";

    return true;
}

void Client::Reader(){
    File file;

    if (!file.OpenForRead(filePath)){

        fileQueue.SetFinished();

        return;
    }

    while (true){

        std::vector<char> buffer(BLOCK_SIZE);

        DWORD bytesRead = 0;

        bool result = file.Read(
            buffer.data(),
            static_cast<DWORD>(BLOCK_SIZE),
            bytesRead
        );

        if (!result)
            break;

        if (bytesRead == 0)
            break;

        buffer.resize(static_cast<size_t>(bytesRead));

        fileQueue.Push(buffer);

        std::cout << "Reader: added "
            << buffer.size()
            << " bytes to queue.\n";
    }

    file.Close();

    fileQueue.SetFinished();

    std::cout << "Reader: finished reading file.\n";
}

void Client::Sender(){

    std::vector<char> block;

    while (fileQueue.Pop(block)){

        if (!SendBlock(block)){

            std::cout<< "Sender: failed to send block.\n";

            return;
        }
    }

    if (!SendEndMarker()){

        std::cout << "Sender: failed to send end marker.\n";

        return;
    }

    std::cout << "Sender: finished sending file.\n";
}

std::string Client::GetFileName(){

    size_t position = filePath.find_last_of("\\/");

    if (position == std::string::npos){

        return filePath;
    }

    return filePath.substr(position + 1);
}