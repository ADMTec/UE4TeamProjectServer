#include "LobbyServer.hpp"
#include <iostream>


int main()
{
    try {
        LobbyServer::Instance().Initialize();
        LobbyServer::Instance().Run();
        LobbyServer::Instance().Stop();
    } catch (const std::exception & e) {
        std::cout << e.what() << std::endl;
    }
    return 0;
}