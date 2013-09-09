#pragma once

#include <functional>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "SocketServer.h"

class LightsCore
{
public:
    void Run();

private:
    SocketServer m_socketServer;
};
