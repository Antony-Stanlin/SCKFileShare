#include "Server.h"

#include <iostream>
#include <cstdint>


void Server::SetOutputPath(std::string outputPath){

    this->outputPath = outputPath;
}

int Server::Setup(){

    int result = PrepareSocket();

    if (result != 0)
        return result;

    if (!File::CreateDirectory(outputPath))
        return 1;


    result = Bind();

    if (result != 0)
        return result;

    result = Listen();

    if (result != 0)
        return result;

    std::cout << "Server started.\n";

    std::cout << "Waiting for client...\n";

    result = Accept(clientSocket);


    if (result != 0)
        return result;

    std::cout << "Client connected successfully.\n";

    result = ReceiveFile();

    CloseClientSocket();

    return result;
}

int Server::ReceiveFile(){

    if (!ReceiveFileName()){

        std::cout << "Failed to receive file name.\n";

        return 1;
    }

    receiverThread.Start(&Server::Receiver, this);

    writerThread.Start(&Server::Writer,this);

    receiverThread.Join();
    writerThread.Join();

    return 0;
}

bool Server::ReceiveFileName(){

    uint32_t networkLength = 0;

    if (!ReceiveAll(
        clientSocket,
        reinterpret_cast<char*>(&networkLength),
        sizeof(networkLength)
    ))
    {

        return false;
    }

    uint32_t nameLength = ntohl(networkLength);

    if (nameLength == 0 || nameLength > 1024){

        std::cout << "valid file name length.\n";

        return false;
    }

    std::vector<char> nameBuffer(nameLength);

    if (!ReceiveAll(
        clientSocket,
        nameBuffer.data(),
        static_cast<int>(nameLength)
    ))
    {
        return false;
    }

    fileName.assign(nameBuffer.begin(),nameBuffer.end());

    outputFileName = outputPath + "\\received_" + fileName;

    std::cout << "Receiving file: " << outputFileName << "\n";

    return true;
}


bool Server::ReceiveBlock(std::vector<char>& block){

    uint32_t networkSize = 0;

    if (!ReceiveAll(
        clientSocket,
        reinterpret_cast<char*>(&networkSize),
        sizeof(networkSize)
    ))
    {

        return false;
    }

    uint32_t blockSize = ntohl(networkSize);

    if (blockSize == 0){

        block.clear();

        return true;
    }

    if (blockSize > BLOCK_SIZE){

        std::cout << "Invalid block size: "
            << blockSize
            << "\n";

        return false;
    }

    block.resize(blockSize);

    if (!ReceiveAll(
        clientSocket,
        block.data(),
        static_cast<int>(blockSize)
    ))
    {
        return false;
    }

    std::cout << "Receiver: received "
        << block.size()
        << " bytes.\n";

    return true;
}


void Server::Receiver()
{
    while (true){

        std::vector<char> block;

        if (!ReceiveBlock(block)){

            std::cout << "Receiver: failed to receive block.\n";

            break;
        }

        // Empty block means end of file

        if (block.empty())
            break;


        fileQueue.Push(block);

        std::cout << "Receiver: added "
            << block.size()
            << " bytes to queue.\n";
    }

    fileQueue.SetFinished();

    std::cout << "Receiver: finished receiving file.\n";
}

void Server::Writer()
{

    File file;

    if (!file.OpenForWrite(outputFileName)){

        fileQueue.SetFinished();

        return;
    }

    std::vector<char> block;

    while (fileQueue.Pop(block)){

        DWORD bytesWritten = 0;

        bool result = file.Write(
                block.data(),
                static_cast<DWORD>(block.size()),
                bytesWritten
            );


        if (!result){
            std::cout << "Writer: failed to write block.\n";

            break;
        }

        if (bytesWritten != block.size()){

            std::cout<< "Writer: failed to write complete block.\n";

            break;
        }

        std::cout << "Writer: wrote "
            << block.size()
            << " bytes.\n";
    }

    file.Close();

    std::cout
        << "Writer: file writing completed.\n";
}

void Server::CloseClientSocket()
{

    CloseSocket(clientSocket);
}