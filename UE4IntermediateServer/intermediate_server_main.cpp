#include "IntermediateServer.hpp"
#include <iostream>


int main()
{
    try {
        IntermediateServer::Instance().Initialize();
        IntermediateServer::Instance().Run();
        IntermediateServer::Instance().Stop();
    } catch (const std::exception & e) {
        std::cout << e.what() << std::endl;
    }
    return 0;
}