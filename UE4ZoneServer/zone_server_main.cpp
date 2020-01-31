#include "UE4Model/ExportZoneServer.hpp"
#include <stdexcept>
#include <iostream>


int main()
{
    try {
        ExportZoneServer server;
        server.Initialize();
        server.Run();
        server.Stop();
    } catch (const std::exception& e) {
        std::cout << e.what();
    }
    return 0;
}