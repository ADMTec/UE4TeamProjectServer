#include "UE4Client.hpp"
#include "Private/ImplUE4Client.hpp"


UE4Client::UE4Client(const UE4Client& rhs)
{
    assert(false);
}

void UE4Client::operator=(const UE4Client&)
{
    assert(false);
}

UE4Client::UE4Client(const std::shared_ptr<NioSession>& session)
{
    impl_ = new ImplUE4Client(session);
}

UE4Client::~UE4Client()
{
    delete impl_;
}

void UE4Client::Close()
{
    impl_->Close();
}

const __UUID& UE4Client::GetUUID() const
{
    return impl_->GetUUID();
}

std::shared_ptr<NioSession> UE4Client::GetSession() const
{
    return impl_->GetSession();
}

void UE4Client::SetContextUUID(uint32_t key, const __UUID& uuid)
{
    impl_->SetContextUUID(key, uuid);
}
std::optional<__UUID> UE4Client::GetContextUUID(uint32_t key) const
{
    return impl_->GetContextUUID(key);
}