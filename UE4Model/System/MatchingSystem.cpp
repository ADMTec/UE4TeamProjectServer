#include "MatchingSystem.hpp"
#include "Server/ZoneServer.hpp"
#include "Packet/PacketGenerator.hpp"
#include "System/ZoneSystem.hpp"


namespace {
    constexpr int64_t GetMatchFullBinary(int64_t max_match_count) {
        int64_t binary = 0;
        for (int64_t i = 0; i < max_match_count; ++i) {
            binary |= (0b1 << i);
        }
        return binary;
    }
}

Match::Match(int64_t match_id, int32_t mapid)
    : state_(State::kRecruiting), mapid_(mapid), match_instance_id_(match_id)
{
    for (int i = 0; i < Match::max_match_count; ++i) {
        match_client_id_queue_[i].second = false;
    }
}

void Match::Join(const std::string& client_key)
{
    std::unique_lock lock(context_guard_);
    if (state_ != State::kRecruiting)
        return;

    uint16_t is_full = 0b0000;
    bool set_value{ false };
    for (int i = 0; i < Match::max_match_count; ++i)
    {
        bool has_value = match_client_id_queue_[i].first.operator bool();
        if (!has_value && set_value == false) {
            match_client_id_queue_[i].first = client_key;
            has_value = set_value = true;
        }
        is_full |= (has_value << i);
    }
    if (is_full == max_flag) {
        state_ = Match::State::kConfirming;
        UE4OutPacket out;
        PacketGenerator::NotifyMatchResult(out);
        out.MakePacketHead();

        // client에 통지
        for (int i = 0; i < Match::max_match_count; ++i)
        {
            auto client = ZoneServer::Instance().GetClient(*match_client_id_queue_[i].first);
            if (client) {
                // match = null 설정
                client->SetMatchWeak(nullptr);
                // 패킷 전송
                client->GetSession()->Send(out, true, false);
            }
        }
    }
}

void Match::Remove(const std::string& client_key)
{
    std::unique_lock lock(context_guard_);

    for (int i = 0; i < Match::max_match_count; ++i)
    {
        bool has_value = match_client_id_queue_[i].first.operator bool();
        if (has_value && match_client_id_queue_[i].first == client_key) {
            match_client_id_queue_[i].first.reset();
            break;
        }
    }
}

void Match::Confirm(const std::string& client_key, bool match_ok)
{
    std::unique_lock lock(context_guard_);
    if (match_ok == false) {
        MatchSystem::Instance().RemoveMatch(match_instance_id_);
        // Packet으로 X 통지
        UE4OutPacket out;
        PacketGenerator::NotifyMatchCanceled(out);
        out.MakePacketHead();
        for (int i = 0; i < Match::max_match_count; ++i)
        {
            if (match_client_id_queue_[i].first.operator bool()) {
                auto client = ZoneServer::Instance().GetClient(*match_client_id_queue_[i].first);
                if (client) {
                    // match = null 설정
                    client->SetMatchWeak(nullptr);
                    // 패킷 전송
                    client->GetSession()->Send(out, true, false);
                }
            }
        }
        return;
    }

    uint16_t is_full = 0b0000;
    for (int i = 0; i < Match::max_match_count; ++i)
    {
        bool has_value = match_client_id_queue_[i].first.operator bool();
        if (has_value) {
            match_client_id_queue_[i].second = true;
        }
        is_full |= (match_client_id_queue_[i].second << i);
    }
    if (is_full == max_flag) {
        state_ = State::kCompleted;
        MatchSystem::Instance().RemoveMatch(match_instance_id_);
        
        // New Map Instance 생성하고 거기에 집어넣기
        auto zone = ZoneSystem::CreateNewInstance(this->mapid_);
        zone->SetState(Zone::State::kActive);

        std::array<std::shared_ptr<Client>, Match::max_match_count> clients;
        for (int i = 0; i < Match::max_match_count; ++i) {
            clients[i] = ZoneServer::Instance().GetClient(*match_client_id_queue_[i].first);
            if (clients[i]) {
                auto chr = clients[i]->GetCharacter();
                if (chr) {
                    std::shared_ptr<Zone> prev_zone = nullptr;
                    {
                        std::lock_guard chr_lock(chr->mutex_);
                        prev_zone = chr->GetZone();
                    }
                    if (prev_zone) {
                        prev_zone->RemoveCharacter(chr->GetObjectId());
                    }
                    zone->SpawnCharacter(chr);
                }
            }
        }
    }
}

int32_t Match::GetMapId() const
{
    std::shared_lock lock(context_guard_);
    return mapid_;
}

int64_t Match::GetMatchInstanceId() const
{
    std::shared_lock lock(context_guard_);
    return match_instance_id_;
}

Match::State Match::GetState() const
{
    std::shared_lock lock(context_guard_);
    return state_;
}

void Match::SetState(Match::State state)
{
    std::unique_lock lock(context_guard_);
    state_ = state;
}

MatchSystem::MatchSystem()
{
}

std::shared_ptr<Match> MatchSystem::GetMatch(int32_t mapid)
{
    std::shared_ptr<Match> match = nullptr;
    std::unique_lock lock(match_guard_);
    auto iter = std::find_if(match_queue_.begin(), match_queue_.end(),
        [mapid](const std::shared_ptr<Match>& match) {
            return mapid == match->GetMapId();
        });
    if (iter == match_queue_.end()) {
        int64_t match_instance_id = match_id_.fetch_add(1);
        match = std::make_shared<Match>(match_instance_id, mapid);
        match_queue_.emplace_back(match);
    } else {
        match = *iter;
    }
    return match;
}

std::shared_ptr<Match> MatchSystem::Find(int64_t match_instance_id)
{
    std::shared_lock lock(match_guard_);
    auto iter = std::find_if(match_queue_.begin(), match_queue_.end(),
        [match_instance_id](const std::shared_ptr<Match>& match) {
            return match_instance_id == match->GetMatchInstanceId();
        });
    if (iter != match_queue_.end()) {
        return *iter;
    }
    return nullptr;
}

void MatchSystem::RemoveMatch(int64_t match_instance_id)
{
    std::unique_lock lock(match_guard_);
    auto iter = std::find_if(match_queue_.begin(), match_queue_.end(),
        [match_instance_id](const std::shared_ptr<Match>& match) {
            return match_instance_id == match->GetMatchInstanceId();
        });
    if (iter != match_queue_.end()) {
        match_queue_.erase(iter);
    }
}

void MatchSystem::Clear()
{
    std::unique_lock lock(match_guard_);
    match_queue_.clear();
}