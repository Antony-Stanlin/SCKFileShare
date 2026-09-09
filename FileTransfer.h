#pragma once

#include "Socket.h"
#include "Queue.h"


class FileTransfer : public Socket
{

protected:

    static constexpr size_t BLOCK_SIZE = 7 * 1024;

    Queue fileQueue;


public:

    FileTransfer();

    virtual ~FileTransfer();

};