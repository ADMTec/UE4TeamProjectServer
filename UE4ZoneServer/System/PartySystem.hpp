#pragma once
#include "UE4DevelopmentLibrary/Utility/TSingleton.hpp"
#include <unordered_map>
#include <shared_mutex>
#include <optional>
#include <vector>
#include <memory>


class UE4Client;

class Party
{
public:
    class System : public TSingleton<Party::System>
    {
        friend class TSingleton<Party::System>;
        System();
    public:
        // return party key
        std::optional<int64_t> CreateParty(const std::shared_ptr<UE4Client>& client);
        void DisbandParty(int64_t party_id, int64_t chr_cid);
        bool JoinParty(int64_t party_id, const std::shared_ptr<class UE4Client>& client);
        bool LeaveParty(int64_t party_id, const std::string& uuid);
    private:
        std::shared_ptr<Party> FindPartyFromPartyId(int64_t party_id) const;
        static int64_t GetNewPartyId();
    private:
        std::shared_mutex party_array_garud_;
        std::vector<std::shared_ptr<Party>> party_array_;
    };
    int64_t GetPartyId() const;
private:
    std::shared_mutex party_guard_;
    int64_t party_id_;
    std::string party_leader_uuid_;
    std::vector<std::shared_ptr<UE4Client>> party_user_;
};