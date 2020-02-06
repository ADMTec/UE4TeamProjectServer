#pragma once
#include "UE4DevelopmentLibrary/Utility/TSingleton.hpp"
#include <shared_mutex>
#include <unordered_map>
#include "Server/Alias.hpp"
#include <optional>
#include <memory>


class Match
{
    static constexpr int max_match_count = 2;
    friend class MatchSystem;
public:
    class System;
    enum class State {
        kRecruiting,
        kConfirming,
        kCompleted,
    };
    using opt_client_key = std::optional<std::string>;
    using opt_confirm = bool;
public:
    Match(int64_t match_id, int32_t mapid);
    
    void Join(const std::string& client_key);
    void Remove(const std::string& client_key);
    void Confirm(const std::string& client_key, bool match_ok);

    State GetState() const;
    void SetState(Match::State state);
    int32_t GetMapId() const;
    int64_t GetMatchInstanceId() const;
    std::string GetDebugString() const;
private:
    mutable std::shared_mutex context_guard_;
    State state_;
    int32_t mapid_;
    int64_t match_instance_id_;
    std::array<std::pair<opt_client_key, bool>, max_match_count> match_client_id_queue_;
};

class MatchSystem : public TSingleton<MatchSystem>
{
    friend class TSingleton<MatchSystem>;
    MatchSystem();
public:
    std::shared_ptr<Match> CreateOrFind(int32_t mapid);
    std::shared_ptr<Match> Find(int64_t match_instance_id);
    void RemoveMatch(int64_t match_instance_id);
    void Clear();
    std::string GetDebugString() const;
private:
    std::atomic<int64_t> match_id_;
    mutable std::shared_mutex match_guard_;
    std::vector<std::shared_ptr<Match>> match_queue_;
};