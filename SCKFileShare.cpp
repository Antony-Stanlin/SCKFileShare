#include "SocketFactory.h"
#include "SocketType.h"

#include "Client.h"
#include "Server.h"

#include <iostream>
#include <memory>
#include <string>


int main(int argc,char* argv[]){

    if (argc < 2){

        std::cout << "Usage:\n\n"
            << "Server:\n"
            << "SCKFileShare.exe server <output_folder>\n\n"
            << "Client:\n"
            << "SCKFileShare.exe client <server_ip> <file_path>\n";

        return 1;
    }

    std::string mode = argv[1];

    if (mode == "server"){
        if (argc != 3){

            std::cout << "Invalid arguments.\n\n"
                << "Usage:\n"
                << "SCKFileShare.exe server <output_folder>\n";

            return 1;
        }

        std::string outputPath = argv[2];

        std::shared_ptr<FileTransfer> fileTransfer = SocketFactory::GetSocket(SocketType::SERVER);

        if (fileTransfer == nullptr){
            std::cout << "Failed to create server.\n";

            return 1;
        }

        Server* server = static_cast<Server*>(fileTransfer.get());


        server->SetOutputPath(outputPath);

        int result = server->Setup();

        if (result != 0){
            std::cout << "Server failed.\n";

            return 1;
        }
    }

    else if (mode == "client"){

        if (argc != 4)
        {
            std::cout
                << "Invalid arguments.\n\n"

                << "Usage:\n"
                << "SCKFileShare.exe client <server_ip> <file_path>\n";

            return 1;
        }

        std::string serverIP = argv[2];

        std::string filePath = argv[3];

        std::shared_ptr<FileTransfer> fileTransfer = SocketFactory::GetSocket(SocketType::CLIENT);

        if (fileTransfer == nullptr){

            std::cout << "Failed to create client.\n";

            return 1;
        }

        Client* client = static_cast<Client*>(fileTransfer.get());

        client->SetServerIP(serverIP);

        client->SetFilePath(filePath);

        int result = client->Setup();

        if (result != 0){

            std::cout << "Client failed.\n";

            return 1;
        }
    }

    else
    {
        std::cout << "Invalid mode.\n"
            << "Use 'server' or 'client'.\n";

        return 1;
    }

    return 0;
}