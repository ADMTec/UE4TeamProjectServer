#include "IntermediateServer.hpp"

int main()
{
    IntermediateServer::Instance().Initialize();
    IntermediateServer::Instance().Run();
    IntermediateServer::Instance().Stop();
    return 0;
}