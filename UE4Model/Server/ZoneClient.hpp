#pragma once
#include "UE4DevelopmentLibrary/Utility/UUID.hpp"
#include <shared_mutex>
#include <memory>

class Character;
class ZoneServer;

template<typename session>
class ZoneClient : public std::enable_shared_from_this<ZoneClient<session>>
{
public:
    ZoneClient(const std::shared_ptr<session>& session)
        : state_(-1), uuid_(__UUID::Generate()), session_(session)
    {
    }

    void Close()
    {
        if (session_) {
            session_->Close();
        }
    }
    void SetState(int state) {
        std::unique_lock lock(context_guard_);
        state_ = state;
    }
    int GetState() const {
        std::shared_lock lock(context_guard_);
        return state_;
    }
    const __UUID& GetUUID() const {
        std::shared_lock lock(context_guard_);
        return uuid_;
    }
    int32_t GetAccid() const {
        std::shared_lock lock(context_guard_);
        return accid_;
    }
    void SetAccid(int32_t value) {
        std::unique_lock lock(context_guard_);
        accid_ = value;
    }
    int32_t GetCid() const {
        std::shared_lock lock(context_guard_);
        return cid_;
    }
    void SetCid(int32_t value) {
        std::unique_lock lock(context_guard_);
        cid_ = value;
    }
    void SetAccount(const std::string& account) {
        std::unique_lock lock(context_guard_);
        account_ = account;
    }
    std::string GetAccount() const {
        std::shared_lock lock(context_guard_);
        return account_;
    }
    std::shared_ptr<session> GetSession() const {
        std::shared_lock lock(context_guard_);
        return session_;
    }
    void SetCharacter(const std::shared_ptr<Character>& chr) {
        std::unique_lock lock(context_guard_);
        character_ = chr;
    }
    std::shared_ptr<Character> GetCharacter() const {
        std::shared_lock lock(context_guard_);
        return character_;
    }
private:
    mutable std::shared_mutex context_guard_;
    int state_;
    __UUID uuid_;
    int32_t accid_;
    int32_t cid_;
    std::string account_;
    std::shared_ptr<session> session_;
    std::shared_ptr<Character> character_;
};