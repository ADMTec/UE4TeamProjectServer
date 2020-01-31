#pragma once
#include <cstdint>
#include <string>
#include <vector>


class EquipTableData
{
public:
    EquipTableData() = default;
    ~EquipTableData() = default;
public:
    static constexpr const char* file_name_ = "EquipTable.csv";
    void Read(const std::vector<std::string>& input);
public:
    int32_t GetID() const;
    void SetID(int32_t value);
    const std::string& GetName() const;
    void SetName(const std::string& value);
    int32_t GetIconID() const;
    void SetIconID(int32_t value);
    int32_t GetMaleMeshID() const;
    void SetMaleMeshID(int32_t value);
    int32_t GetFemaleMeshID() const;
    void SetFemaleMeshID(int32_t value);
    int32_t GetType() const;
    void SetType(int32_t value);
    int32_t GetSubType() const;
    void SetSubType(int32_t value);
    int32_t GetRequiredLevel() const;
    void SetRequiredLevel(int32_t value);
    int32_t GetATK() const;
    void SetATK(int32_t value);
    int32_t GetDEF() const;
    void SetDEF(int32_t value);
    int32_t GetSTR() const;
    void SetSTR(int32_t value);
    int32_t GetDEX() const;
    void SetDEX(int32_t value);
    int32_t GetINT() const;
    void SetINT(int32_t value);
    int32_t GetRequiredSTR() const;
    void SetRequiredSTR(int32_t value);
    int32_t GetRequiredDEX() const;
    void SetRequiredDEX(int32_t value);
    int32_t GetRequiredINT() const;
    void SetRequiredINT(int32_t value);
    const std::string& GetToolTip() const;
    void SetToolTip(const std::string& value);
private:
    int32_t ID;
    std::string Name;
    int32_t IconID;
    int32_t MaleMeshID;
    int32_t FemaleMeshID;
    int32_t Type;
    int32_t SubType;
    int32_t RequiredLevel;
    int32_t ATK;
    int32_t DEF;
    int32_t STR;
    int32_t DEX;
    int32_t INT;
    int32_t RequiredSTR;
    int32_t RequiredDEX;
    int32_t RequiredINT;
    std::string ToolTip;
};
