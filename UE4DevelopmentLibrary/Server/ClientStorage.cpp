#include "ClientStorage.hpp"
#include <algorithm>


bool ClientStorage::Insert(const std::string& uuid, const std::shared_ptr<UE4Client>& client)
{
    std::unique_lock lock(storage_mutex_);
    auto result = storage_.emplace(uuid, client);
    return result.second;
}

size_t ClientStorage::Erase(const std::string& uuid)
{
    std::unique_lock lock(storage_mutex_);
    return storage_.erase(uuid);
}

std::shared_ptr<UE4Client> ClientStorage::Find(const std::string& uuid) const
{
    std::shared_lock lock(storage_mutex_);
    auto iter = storage_.find(uuid);
    if (iter != storage_.end()) {
        return iter->second;
    }
    return nullptr;
}

std::vector<std::shared_ptr<UE4Client>> ClientStorage::GetCopy() const
{
    std::vector<std::shared_ptr<UE4Client>> vec;

    std::shared_lock lock(storage_mutex_);
    vec.reserve(storage_.size());
    for (const auto& element : storage_) {
        vec.emplace_back(element.second);
    }
    return vec;
}