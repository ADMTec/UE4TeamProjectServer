#include "ImplUE4Client.hpp"


ImplUE4Client::ImplUE4Client(const std::shared_ptr<NioSession>& session)
    : uuid_(__UUID::Generate()), session_(session)
{

}

ImplUE4Client::~ImplUE4Client()
{
}

void ImplUE4Client::Close()
{
    if (session_) {
        session_->Close();
    }
}

const __UUID& ImplUE4Client::GetUUID() const
{
    return uuid_;
}

std::shared_ptr<NioSession> ImplUE4Client::GetSession() const
{
    return session_;
}

void ImplUE4Client::SetContextUUID(uint32_t key, const __UUID& uuid)
{
    std::unique_lock lock(context_guard_);
    context_.insert(std::make_pair(key, uuid));
}
std::optional<__UUID> ImplUE4Client::GetContextUUID(uint32_t key) const
{
    std::shared_lock lock(context_guard_);
    auto iter = context_.find(key);
    if (iter != context_.end()) {
        return iter->second;
    }
    return std::nullopt;
}