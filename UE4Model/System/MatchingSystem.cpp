#include "MatchingSystem.hpp"
#include "Server/ZoneServer.hpp"
#include "Packet/PacketGenerator.hpp"
#include "System/ZoneSystem.hpp"
#include <sstream>


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

    auto iter = std::find_if(match_client_id_queue_.begin(), match_client_id_queue_.end(),
        [](const std::pair<opt_client_key, bool>& pair) {
            return !pair.first.operator bool();
        });
    if (iter != match_client_id_queue_.end()) {
        iter->first = client_key;
    }

    // 매칭 큐 꽉 참
    iter = std::find_if(match_client_id_queue_.begin(), match_client_id_queue_.end(),
        [](const std::pair<opt_client_key, bool>& pair) {
            return pair.first.operator bool() == false;
        });
    if (iter == match_client_id_queue_.end()) {
        state_ = Match::State::kConfirming;
        UE4OutPacket out;
        PacketGenerator::NotifyMatchResult(out);
        out.MakePacketHead();

        std::for_each(match_client_id_queue_.begin(), match_client_id_queue_.end(),
            [&out](const std::pair<opt_client_key, bool>& pair) {
                auto client = ZoneServer::Instance().GetClient(*pair.first);
                if (client) {
                    client->GetSession()->Send(out, true, false);
                }
            });
    }
}

void Match::Remove(const std::string& client_key)
{
    std::unique_lock lock(context_guard_);

    auto iter = std::find_if(match_client_id_queue_.begin(), match_client_id_queue_.end(),
        [client_key](const std::pair<opt_client_key, bool>& pair) {
            if (pair.first) {
                return pair.first == client_key;
            }
            return false;
        });
    if (iter != match_client_id_queue_.end()) {
        iter->first.reset();
        iter->second = false;
    }

    iter = std::find_if(match_client_id_queue_.begin(), match_client_id_queue_.end(),
        [](const std::pair<opt_client_key, bool>& pair) {
            return pair.first.operator bool() == true;
        });
    if (iter == match_client_id_queue_.end()) {
        MatchSystem::Instance().RemoveMatch(this->match_instance_id_);
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
        std::for_each(match_client_id_queue_.begin(), match_client_id_queue_.end(),
            [&out](const std::pair<opt_client_key, bool>& pair) {
                if (pair.first) {
                    auto client = ZoneServer::Instance().GetClient(*pair.first);
                    if (client) {
                        // match = null 설정
                        client->SetMatchWeak(nullptr);
                        // 패킷 전송
                        client->GetSession()->Send(out, true, false);
                    }
                }
            });
        return;
    }

    auto iter = std::find_if(match_client_id_queue_.begin(), match_client_id_queue_.end(),
        [client_key](const std::pair<opt_client_key, bool>& pair) {
            return pair.first == client_key;
        });
    if (iter != match_client_id_queue_.end()) {
        iter->second = true;
    }

    iter = std::find_if(match_client_id_queue_.begin(), match_client_id_queue_.end(),
        [](const std::pair<opt_client_key, bool>& pair) {
            return pair.second == false;
        });
    if (iter == match_client_id_queue_.end()) {
        state_ = State::kCompleted;
        MatchSystem::Instance().RemoveMatch(match_instance_id_);
        
        // New Map Instance 생성하고 거기에 집어넣기
        auto zone = ZoneSystem::CreateNewInstance(this->mapid_);
        zone->SetState(Zone::State::kActive);

        std::for_each(match_client_id_queue_.begin(), match_client_id_queue_.end(),
            [&zone](const std::pair<opt_client_key, bool>& pair) {
                auto client = ZoneServer::Instance().GetClient(*pair.first);
                if (client) {
                    auto chr = client->GetCharacter();
                    std::shared_ptr<Zone> prev = nullptr;
                    int64_t oid = -1;
                    {
                        std::lock_guard lock(chr->mutex_);
                        oid = chr->GetObjectId();
                        prev = chr->GetZoneFromWeak();
                        chr->SetZone(nullptr);
                    }
                    if (prev) {
                        prev->RemoveCharacter(oid);
                    }
                    zone->SpawnCharacter(chr);
                }
            });
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

std::string Match::GetDebugString() const
{
    std::unique_lock lock(context_guard_);
    std::stringstream ss;
    ss << "-----------------------------------------------" << '\n';
    ss << " Debug State [Match] Instance ID: " << match_instance_id_  << '\n';
    ss << " Mapid: " << mapid_ << '\n';
    ss << " State: ";
    switch (state_) {
        case State::kRecruiting: ss << "kRecruiting"; break;
        case State::kConfirming: ss << "kConfirming"; break;
        case State::kCompleted: ss << "kCompleted"; break;
    }
    ss << '\n';
    ss << "-----------------------------------------------" << '\n';
    return ss.str();
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

std::shared_ptr<Match> MatchSystem::CreateOrFind(int32_t mapid)
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
            return match_instance_id == match->match_instance_id_;
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

std::string MatchSystem::GetDebugString() const
{
    std::vector<std::shared_ptr<Match>> copy;
    {
        std::shared_lock lock(match_guard_);
        copy = match_queue_;
    }
    std::unique_lock lock(match_guard_);
    std::stringstream ss;
    ss << "-----------------------------------------------" << '\n';
    ss << "-----------------------------------------------" << '\n';
    ss << " Debug State [MatchSystem]" << '\n';
    if (copy.empty()) {
        ss << " NONE" << '\n';
    } else {
        for (const auto& match : copy) {
            ss << match->GetDebugString();
        }
    }
    ss << "-----------------------------------------------" << '\n';
    ss << "-----------------------------------------------" << '\n';
    return ss.str();
}
