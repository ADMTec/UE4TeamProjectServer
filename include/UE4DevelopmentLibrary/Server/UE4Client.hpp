#pragma once
#include "../Utility.hpp"
#include "../Nio.hpp"
#include <string>
#include <optional>
#include <shared_mutex>
#include <unordered_map>


class UE4Client
{
public:
    UE4Client(const UE4Client&) = delete;
    void operator=(const UE4Client&) = delete;
public:
    explicit UE4Client(const std::shared_ptr<NioSession>& session);
    ~UE4Client();

    void Close();
    const __UUID& GetUUID() const;
    std::shared_ptr<NioSession> GetSession() const;

    void SetContextUUID(uint32_t key, const __UUID& uuid);
    std::optional<__UUID> GetContextUUID(uint32_t key) const;
private:
    __UUID uuid_;
    std::shared_ptr<NioSession> session_;
    mutable std::shared_mutex context_guard_;
    std::unordered_map<int32_t, __UUID> context_;
};