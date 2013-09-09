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
    void AttachDebugPrinter(std::function<void(const std::string&)> printFunc);

private:
    std::function<void(const std::string&)> m_debugPrinter;
    SocketServer m_socketServer;
};
