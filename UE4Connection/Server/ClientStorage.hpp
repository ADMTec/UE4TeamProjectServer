#pragma once
#include <string>
#include <memory>
#include <shared_mutex>
#include <vector>
#include <unordered_map>
#include "UE4Client.hpp"


class ClientStorage
{
public:
    ClientStorage(const ClientStorage&) = delete;
    void operator=(const ClientStorage&) = delete;
public:
    ClientStorage() = default;
    ~ClientStorage() = default;

    bool Insert(const std::string& uuid, const std::shared_ptr<UE4Client>& client);
    size_t Erase(const std::string& uuid);
    
    std::shared_ptr<UE4Client> Find(const std::string& uuid) const;
    std::vector<std::shared_ptr<UE4Client>> GetCopy() const;
private:
    mutable std::shared_mutex storage_mutex_;
    std::unordered_map<std::string, std::shared_ptr<UE4Client>> storage_;
};