#pragma once

#include <functional>
#include <iostream>
#include <map>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "SocketServer.h"
#include "SocketTester.h"
#include "PatternRunner.h"

class LightsCore
{
public:
    LightsCore();
    ~LightsCore();

    void Run();

private:
    void DispatcherCallback(const SocketCommand& command);

    PatternRunner m_patternRunner;
    SocketTester m_socketTester;
    SocketServer m_socketServer;

    std::map<
        unsigned int, 
        std::function<void(const SocketCommand&)>> m_commandMap;
};


