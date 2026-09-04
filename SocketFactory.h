#pragma once
#include "SocketType.h"
#include "Socket.h"
#include "Client.h"
#include "Server.h"
#include <memory>


class SocketFactory {
	public:
		static std::shared_ptr<Socket> getSocket(SocketType socketType);
};