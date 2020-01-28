#include "EquipTableData.hpp"
#include <boost/lexical_cast.hpp>


void EquipTableData::Read(const std::vector<std::string>& input)
{
    ID = boost::lexical_cast<int32_t>(input[0]);
    Name = input[1];
    IconID = boost::lexical_cast<int32_t>(input[2]);
    MaleMeshID = boost::lexical_cast<int32_t>(input[3]);
    FemaleMeshID = boost::lexical_cast<int32_t>(input[4]);
    Type = boost::lexical_cast<int32_t>(input[5]);
    SubType = boost::lexical_cast<int32_t>(input[6]);
    RequiredLevel = boost::lexical_cast<int32_t>(input[7]);
    ATK = boost::lexical_cast<int32_t>(input[8]);
    DEF = boost::lexical_cast<int32_t>(input[9]);
    STR = boost::lexical_cast<int32_t>(input[10]);
    DEX = boost::lexical_cast<int32_t>(input[11]);
    INT = boost::lexical_cast<int32_t>(input[12]);
    RequiredSTR = boost::lexical_cast<int32_t>(input[13]);
    RequiredDEX = boost::lexical_cast<int32_t>(input[14]);
    RequiredINT = boost::lexical_cast<int32_t>(input[15]);
    ToolTip = input[16];
}


int32_t EquipTableData::GetID() const {
    return ID;
}

void EquipTableData::SetID(int32_t value) {
    ID = value;
}

const std::string& EquipTableData::GetName() const {
    return Name;
}

void EquipTableData::SetName(const std::string& value) {
    Name = value;
}

int32_t EquipTableData::GetIconID() const {
    return IconID;
}

void EquipTableData::SetIconID(int32_t value) {
    IconID = value;
}

int32_t EquipTableData::GetMaleMeshID() const {
    return MaleMeshID;
}

void EquipTableData::SetMaleMeshID(int32_t value) {
    MaleMeshID = value;
}

int32_t EquipTableData::GetFemaleMeshID() const {
    return FemaleMeshID;
}

void EquipTableData::SetFemaleMeshID(int32_t value) {
    FemaleMeshID = value;
}

int32_t EquipTableData::GetType() const {
    return Type;
}

void EquipTableData::SetType(int32_t value) {
    Type = value;
}

int32_t EquipTableData::GetSubType() const {
    return SubType;
}

void EquipTableData::SetSubType(int32_t value) {
    SubType = value;
}

int32_t EquipTableData::GetRequiredLevel() const {
    return RequiredLevel;
}

void EquipTableData::SetRequiredLevel(int32_t value) {
    RequiredLevel = value;
}

int32_t EquipTableData::GetATK() const {
    return ATK;
}

void EquipTableData::SetATK(int32_t value) {
    ATK = value;
}

int32_t EquipTableData::GetDEF() const {
    return DEF;
}

void EquipTableData::SetDEF(int32_t value) {
    DEF = value;
}

int32_t EquipTableData::GetSTR() const {
    return STR;
}

void EquipTableData::SetSTR(int32_t value) {
    STR = value;
}

int32_t EquipTableData::GetDEX() const {
    return DEX;
}

void EquipTableData::SetDEX(int32_t value) {
    DEX = value;
}

int32_t EquipTableData::GetINT() const {
    return INT;
}

void EquipTableData::SetINT(int32_t value) {
    INT = value;
}

int32_t EquipTableData::GetRequiredSTR() const {
    return RequiredSTR;
}

void EquipTableData::SetRequiredSTR(int32_t value) {
    RequiredSTR = value;
}

int32_t EquipTableData::GetRequiredDEX() const {
    return RequiredDEX;
}

void EquipTableData::SetRequiredDEX(int32_t value) {
    RequiredDEX = value;
}

int32_t EquipTableData::GetRequiredINT() const {
    return RequiredINT;
}

void EquipTableData::SetRequiredINT(int32_t value) {
    RequiredINT = value;
}

const std::string& EquipTableData::GetToolTip() const {
    return ToolTip;
}

void EquipTableData::SetToolTip(const std::string& value) {
    ToolTip = value;
}
