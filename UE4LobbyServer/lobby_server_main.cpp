#include "LobbyServer.hpp"

int main()
{
    LobbyServer::Instance().Initialize();
    LobbyServer::Instance().Run();
    LobbyServer::Instance().Stop();
    return 0;
}