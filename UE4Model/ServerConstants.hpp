#pragma once
#include "UE4DevelopmentLibrary/Utility.hpp"
#include <string>

struct ServerConstants : public TSingleton<const ServerConstants>
{
private:
    friend class TSingleton<const ServerConstants>;
    ServerConstants() = default;
public:
    void Initialize();

public:
    const char* data_root_dir = "Data";
    const char* map_data_root = "Map";
    const char* csv_data_root = "Csv";
    std::wstring odbc_name;
    std::wstring db_id;
    std::wstring db_pw;
    std::string ip;
    uint16_t port;
    int32_t max_connection;
    int32_t num_io_worker_;
    std::string intermediate_server_ip;
    uint16_t intermediate_server_port;
};

#define ServerConstant ServerConstants::Instance()