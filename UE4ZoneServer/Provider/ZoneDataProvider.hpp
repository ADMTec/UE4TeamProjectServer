#pragma once
#include "UE4DevelopmentLibrary/Utility.hpp"
#include "Data/ZoneData.hpp"
#include <unordered_map>
#include <optional>


class ZoneDataProvider : public TSingleton<const ZoneDataProvider>
{
    friend class TSingleton<const ZoneDataProvider>;
    ZoneDataProvider();
public:
    void Initialize();

    std::optional<const ZoneData* const> GetData(int64_t mapid) const;
private:
    std::unordered_map<int64_t, ZoneData> zone_data_;
};

