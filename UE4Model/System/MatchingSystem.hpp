#pragma once
#include "UE4DevelopmentLibrary/Utility/TSingleton.hpp"
#include <shared_mutex>
#include <unordered_map>
#include "Server/Alias.hpp"


class MatchingSystem : public TSingleton<MatchingSystem>
{
    friend class TSingleton<MatchingSystem>;
    MatchingSystem();
public:

private:
    std::shared_mutex wait_user_guard_;
    std::vector<std::weak_ptr<Client>> wait_user_;
};