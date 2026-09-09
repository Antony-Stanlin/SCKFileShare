#include "SocketFactory.h"

#include "Client.h"
#include "Server.h"


std::shared_ptr<FileTransfer> SocketFactory::GetSocket(SocketType socketType){

    if (socketType == SocketType::CLIENT)
        return std::make_shared<Client>();


    if (socketType == SocketType::SERVER)
        return std::make_shared<Server>();

    return nullptr;
}