#include "LoginServer.hpp"
#include <iostream>


int main()
{
    try {
        auto& server = LoginServer::Instance();
        server.Initialize();
        server.Run();
        server.Stop();
    } catch (const std::exception & e) {
        std::cout << e.what() << std::endl;
    }
    return 0;
}