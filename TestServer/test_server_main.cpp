#include "TestServer.hpp"

int main()
{
    TestServer::Instance().Initialize();
    TestServer::Instance().Run();

    getchar();
    getchar();
    getchar();
    TestServer::Instance().Stop();

    return 0;
}