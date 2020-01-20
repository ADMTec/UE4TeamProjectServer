#pragma once
#include "UE4DevelopmentLibrary/Utility.hpp"
#include "UE4DevelopmentLibrary/Nio.hpp"

class LoginServer : public TSingleton<LoginServer>
{
public:
    ~LoginServer();
    void Initialize();
    void Run();
    void Stop();
    void Exit();
private:
    std::shared_ptr<NioServer> io_server_;
};