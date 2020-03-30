#include "PartySystem.hpp"
#include <algorithm>
#include <atomic>
#include "Server/ZoneServer.hpp"


std::shared_mutex Party::System::party_array_garud_;
std::vector<std::shared_ptr<Party>> Party::System::party_array_;


std::optional<int64_t> Party::System::CreateParty(const std::shared_ptr<Client>& client)
{
    int64_t party_id = Party::System::GetNewPartyId();
    auto party = std::make_shared<Party>(party_id, client);
    party->party_id_ = party_id;
    party->party_leader_uuid_ = client->GetUUID();
    party->party_user_.emplace_back(client);

    std::unique_lock lock(party_array_garud_);
    party_array_.emplace_back(party);
    return party_id;
}

void Party::System::DisbandParty(int64_t party_id, int64_t chr_cid)
{
    // leader_check

}

bool Party::System::JoinParty(int64_t party_id, const std::shared_ptr<Client>& client)
{
    std::shared_ptr<Party> party = FindPartyFromPartyId(party_id);
    if (!party) {
        return false;
    }
    std::unique_lock lock(party->party_guard_);
    if (party->party_user_.size() == 0) {
        return false;
    }
    // 기존 Party 유저들에게 BroadCast
    // 새로 들어온 유저에게 Party 정보 주기
    party->party_user_.push_back(client);
    return true;
}

bool Party::System::LeaveParty(int64_t party_id, const std::string& uuid)
{
    std::shared_ptr<Party> party = FindPartyFromPartyId(party_id);
    if (!party) {
        return false;
    }
    std::unique_lock lock(party->party_guard_);
    auto iter = std::find_if(party->party_user_.begin(), party->party_user_.end(),
        [uuid](const std::shared_ptr<Client>& client) {
            return client->GetUUID().ToString() == uuid;
        });
    if (iter == party->party_user_.end()) {
        return false;
    }
    auto client = *iter;
    party->party_user_.erase(iter);
    // 남아있는 유저들에게 탈퇴 패킷
    // client 에는 탈퇴 OK 알림
    return true;
}

std::shared_ptr<Party> Party::System::FindPartyFromPartyId(int64_t party_id)
{
    std::shared_lock lock(party_array_garud_);
    auto iter = std::find_if(party_array_.begin(), party_array_.end(),
        [party_id](const std::shared_ptr<Party>& party) {
            return party->GetPartyId() == party_id;
        });
    if (iter != party_array_.end()) {
        return *iter;
    }
    return nullptr;
}

int64_t Party::System::GetNewPartyId()
{
    static std::atomic<int64_t> party_id_;
    return party_id_.fetch_add(1);
}

// ----------------------------------------------------------------------------------------
Party::Party(int64_t party_id, const std::shared_ptr<Client>& client)
    : party_id_(party_id)
{
    party_leader_uuid_ = client->GetUUID();
}

Party::Party()
    : party_id_(-1)
{
}

void Party::BraodCast(UE4OutPacket& out)
{
    std::shared_lock lock(party_guard_);
    for (const auto& user : party_user_) {
        user->GetSession()->Send(out, true ,false);
    }
}

void Party::BraodCast(UE4OutPacket& out, const __UUID& except_client_uuid)
{
    std::shared_lock lock(party_guard_);
    for (const auto& user : party_user_) {
        if (user->GetUUID() != except_client_uuid) {
            user->GetSession()->Send(out, true, false);
        }
    }
}

int64_t Party::GetPartyId() const
{
    return party_id_;
}
