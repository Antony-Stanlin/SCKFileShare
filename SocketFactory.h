#pragma once

#include "SocketType.h"
#include "FileTransfer.h"


#include <memory>


class SocketFactory
{

public:

    static std::shared_ptr<FileTransfer> GetSocket(SocketType socketType);
};