#include "MonsterTableData.hpp"
#include <boost/lexical_cast.hpp>


void MonsterTableData::Read(const std::vector<std::string>& input)
{
    ID = boost::lexical_cast<int32_t>(input[0]);
    Name = input[1];
    MaxHP = boost::lexical_cast<int32_t>(input[2]);
    ATKMin = boost::lexical_cast<int32_t>(input[3]);
    ATKMax = boost::lexical_cast<int32_t>(input[4]);
    ATKRange = boost::lexical_cast<int32_t>(input[5]);
    ATKSpeed = boost::lexical_cast<int32_t>(input[6]);
    Defence = boost::lexical_cast<int32_t>(input[7]);
    Speed = boost::lexical_cast<int32_t>(input[8]);
}


int32_t MonsterTableData::GetID() const {
    return ID;
}

void MonsterTableData::SetID(int32_t value) {
    ID = value;
}

const std::string& MonsterTableData::GetName() const {
    return Name;
}

void MonsterTableData::SetName(const std::string& value) {
    Name = value;
}

int32_t MonsterTableData::GetMaxHP() const {
    return MaxHP;
}

void MonsterTableData::SetMaxHP(int32_t value) {
    MaxHP = value;
}

int32_t MonsterTableData::GetATKMin() const {
    return ATKMin;
}

void MonsterTableData::SetATKMin(int32_t value) {
    ATKMin = value;
}

int32_t MonsterTableData::GetATKMax() const {
    return ATKMax;
}

void MonsterTableData::SetATKMax(int32_t value) {
    ATKMax = value;
}

int32_t MonsterTableData::GetATKRange() const {
    return ATKRange;
}

void MonsterTableData::SetATKRange(int32_t value) {
    ATKRange = value;
}

int32_t MonsterTableData::GetATKSpeed() const {
    return ATKSpeed;
}

void MonsterTableData::SetATKSpeed(int32_t value) {
    ATKSpeed = value;
}

int32_t MonsterTableData::GetDefence() const {
    return Defence;
}

void MonsterTableData::SetDefence(int32_t value) {
    Defence = value;
}

int32_t MonsterTableData::GetSpeed() const {
    return Speed;
}

void MonsterTableData::SetSpeed(int32_t value) {
    Speed = value;
}
