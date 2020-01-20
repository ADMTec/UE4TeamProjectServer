#pragma once
#include "UE4DevelopmentLibrary/Utility.hpp"
#include "UE4DevelopmentLibrary/Nio.hpp"
#include <string>
#include <optional>
#include <shared_mutex>
#include <unordered_map>


class ImplUE4Client
{
public:
    ImplUE4Client(const ImplUE4Client&) = delete;
    void operator=(const ImplUE4Client&) = delete;
public:
    explicit ImplUE4Client(const std::shared_ptr<NioSession>& session);
    ~ImplUE4Client();

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