#include "UE4Client.hpp"


UE4Client::UE4Client(const std::shared_ptr<NioSession>& session)
    : uuid_(__UUID::Generate()), session_(session)
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

const __UUID& UE4Client::GetUUID() const
{
    return uuid_;
}

std::shared_ptr<NioSession> UE4Client::GetSession() const
{
    return session_;
}

void UE4Client::SetContextUUID(uint32_t key, const __UUID& uuid)
{
    std::unique_lock lock(context_guard_);
    context_.insert(std::make_pair(key, uuid));
}
std::optional<__UUID> UE4Client::GetContextUUID(uint32_t key) const
{
    std::shared_lock lock(context_guard_);
    auto iter = context_.find(key);
    if (iter != context_.end()) {
        return iter->second;
    }
    return std::nullopt;
}