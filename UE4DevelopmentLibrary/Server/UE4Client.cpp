#include "UE4Client.hpp"


UE4Client::UE4Client(const std::shared_ptr<NioSession>& session)
    : state_(-1), uuid_(__UUID::Generate()), session_(session)
{

}

UE4Client::~UE4Client()
{
}

void UE4Client::Close()
{
    if (session_) {
        session_->Close();
    }
}

void UE4Client::SetState(int state)
{
    state_ = state;
}

int UE4Client::GetState() const
{
    return state_;
}

const __UUID& UE4Client::GetUUID() const
{
    return uuid_;
}

int32_t UE4Client::GetAccid() const
{
    return accid_;
}

void UE4Client::SetAccid(int32_t value)
{
    accid_ = value;
}

int32_t UE4Client::GetCid() const
{
    return cid_;
}

void UE4Client::SetCid(int32_t value)
{
    cid_ = value;
}

std::shared_ptr<NioSession> UE4Client::GetSession() const
{
    return session_;
}

void UE4Client::SetContext(int64_t key, const std::any& value)
{
    std::unique_lock lock(context_guard_);
    context_[key] = value;
}

std::optional<std::any> UE4Client::GetContext(int64_t key) const
{
    std::shared_lock lock(context_guard_);
    auto iter = context_.find(key);
    if (iter != context_.end()) {
        return iter->second;
    }
    return std::nullopt;
}