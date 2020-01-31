#pragma once
#include "../Utility.hpp"
#include "../Nio.hpp"
#include <any>
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
    void SetState(int state);
    int GetState() const;
    const __UUID& GetUUID() const;
    int32_t GetAccid() const;
    void SetAccid(int32_t value);
    int32_t GetCid() const;
    void SetCid(int32_t value);

    std::shared_ptr<NioSession> GetSession() const;
    void SetContext(int64_t key, const std::any& value);
    std::optional<std::any> GetContext(int64_t key) const;
private:
    int state_;
    __UUID uuid_;
    int32_t accid_;
    int32_t cid_;
    std::shared_ptr<NioSession> session_;
    mutable std::shared_mutex context_guard_;
    std::unordered_map<int64_t, std::any> context_;
};