#pragma once
#include "UE4DevelopmentLibrary/Utility.hpp"
#include <unordered_map>
#include <shared_mutex>
#include <optional>
#include <vector>
#include <memory>
#include "Server/Alias.hpp"

class Party
{
public:
    class System
    {
    public:
        // return party key
        static std::optional<int64_t> CreateParty(const std::shared_ptr<Client>& client);
        static void DisbandParty(int64_t party_id, int64_t chr_cid);
        static bool JoinParty(int64_t party_id, const std::shared_ptr<Client>& client);
        static bool LeaveParty(int64_t party_id, const std::string& uuid);
    private:
        static std::shared_ptr<Party> FindPartyFromPartyId(int64_t party_id);
        static int64_t GetNewPartyId();
    private:
        static std::shared_mutex party_array_garud_;
        static std::vector<std::shared_ptr<Party>> party_array_;
    };
public:
    Party(int64_t party_id, const std::shared_ptr<Client>& client);
    Party();
    void BraodCast(class UE4OutPacket& out);
    void BraodCast(class UE4OutPacket& out, const __UUID& except_client_uuid);
    int64_t GetPartyId() const;
private:
    std::shared_mutex party_guard_;
    int64_t party_id_;
    std::optional<__UUID> party_leader_uuid_;
    std::vector<std::shared_ptr<Client>> party_user_;
};