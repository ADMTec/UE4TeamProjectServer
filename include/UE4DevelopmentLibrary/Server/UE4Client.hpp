#pragma once
#include "../Utility.hpp"
#include "../Nio.hpp"
#include <any>
#include <string>
#include <optional>
#include <shared_mutex>
#include <unordered_map>


template<typename Session>
class UE4Client
{
public:
    UE4Client(const UE4Client&) = delete;
    void operator=(const UE4Client&) = delete;
public:
    explicit UE4Client(const std::shared_ptr<Session>& session)
        : state_(-1), uuid_(__UUID::Generate()), session_(session)
    {
    }

    ~UE4Client()
    {
    }

    void Close()
    {
        if (session_) {
            session_->Close();
        }
    }

    void SetState(int state)
    {
        std::unique_lock lock(context_guard_);
        state_ = state;
    }

    int GetState() const
    {
        std::shared_lock lock(context_guard_);
        return state_;
    }

    const __UUID& GetUUID() const
    {
        std::shared_lock lock(context_guard_);
        return uuid_;
    }

    int32_t GetAccid() const
    {
        std::shared_lock lock(context_guard_);
        return accid_;
    }

    void SetAccid(int32_t value)
    {
        std::unique_lock lock(context_guard_);
        accid_ = value;
    }

    int32_t GetCid() const
    {
        std::shared_lock lock(context_guard_);
        return cid_;
    }

    void SetCid(int32_t value)
    {
        std::unique_lock lock(context_guard_);
        cid_ = value;
    }

    void SetAccount(const std::string& account)
    {
        std::unique_lock lock(context_guard_);
        account_ = account;
    }

    std::string GetAccount() const
    {
        std::shared_lock lock(context_guard_);
        return account_;
    }

    std::shared_ptr<Session> GetSession() const
    {
        std::shared_lock lock(context_guard_);
        return session_;
    }
private:
    mutable std::shared_mutex context_guard_;
    int state_;
    __UUID uuid_;
    int32_t accid_;
    int32_t cid_;
    std::string account_;
    std::shared_ptr<Session> session_;
};