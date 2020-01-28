#pragma once
#include <cstdint>
#include <string>
#include <vector>


class MonsterTableData
{
public:
    MonsterTableData() = default;
    ~MonsterTableData() = default;
public:
    static constexpr const char* file_name_ = "MonsterTable.csv";
    void Read(const std::vector<std::string>& input);
public:
    int32_t GetID() const;
    void SetID(int32_t value);
    const std::string& GetName() const;
    void SetName(const std::string& value);
    int32_t GetMaxHP() const;
    void SetMaxHP(int32_t value);
    int32_t GetATKMin() const;
    void SetATKMin(int32_t value);
    int32_t GetATKMax() const;
    void SetATKMax(int32_t value);
    int32_t GetATKRange() const;
    void SetATKRange(int32_t value);
    int32_t GetATKSpeed() const;
    void SetATKSpeed(int32_t value);
    int32_t GetDefence() const;
    void SetDefence(int32_t value);
    int32_t GetSpeed() const;
    void SetSpeed(int32_t value);
private:
    int32_t ID;
    std::string Name;
    int32_t MaxHP;
    int32_t ATKMin;
    int32_t ATKMax;
    int32_t ATKRange;
    int32_t ATKSpeed;
    int32_t Defence;
    int32_t Speed;
};
