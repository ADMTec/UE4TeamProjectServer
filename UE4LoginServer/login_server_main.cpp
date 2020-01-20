#include "LoginServer.hpp"

int main()
{
    auto& server = LoginServer::Instance();

    server.Initialize();
    server.Run();
    server.Stop();
    return 0;
}