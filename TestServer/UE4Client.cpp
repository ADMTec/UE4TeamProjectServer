#include "UE4Client.hpp"

UE4Client::UE4Client(const std::shared_ptr<NioSession>& session)
    : session_(session), uuid_(__UUID::Generate())
{
}

void UE4Client::Close()
{
    if (session_) {
        session_->Close();
        session_.reset();
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