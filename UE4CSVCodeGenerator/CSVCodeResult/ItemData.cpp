#include "ItemData.h"


void ItemData::Write(OutputStream& out) const
{
    out << nId;
    out << sName;
    out << fAttackMin;
    out << fAttackMax;
    out << fDefence;
    out << nReqSTR;
    out << nReqDEX;
    out << nReqINT;
    out << nReqLUK;
    out << nSTR;
    out << nDEX;
    out << nINT;
    out << nLUK;
    out << fMagic;
}

void ItemData::Read(InputStream& input)
{
    input >> nId;
    input >> sName;
    input >> fAttackMin;
    input >> fAttackMax;
    input >> fDefence;
    input >> nReqSTR;
    input >> nReqDEX;
    input >> nReqINT;
    input >> nReqLUK;
    input >> nSTR;
    input >> nDEX;
    input >> nINT;
    input >> nLUK;
    input >> fMagic;
}


int32_t ItemData::GetId() const {
    return nId;
}

void ItemData::SetId(int32_t value) {
    nId = value;
}

std::string ItemData::GetName() const {
    return sName;
}

void ItemData::SetName(std::string value) {
    sName = value;
}

float ItemData::GetAttackMin() const {
    return fAttackMin;
}

void ItemData::SetAttackMin(float value) {
    fAttackMin = value;
}

float ItemData::GetAttackMax() const {
    return fAttackMax;
}

void ItemData::SetAttackMax(float value) {
    fAttackMax = value;
}

float ItemData::GetDefence() const {
    return fDefence;
}

void ItemData::SetDefence(float value) {
    fDefence = value;
}

int32_t ItemData::GetReqSTR() const {
    return nReqSTR;
}

void ItemData::SetReqSTR(int32_t value) {
    nReqSTR = value;
}

int32_t ItemData::GetReqDEX() const {
    return nReqDEX;
}

void ItemData::SetReqDEX(int32_t value) {
    nReqDEX = value;
}

int32_t ItemData::GetReqINT() const {
    return nReqINT;
}

void ItemData::SetReqINT(int32_t value) {
    nReqINT = value;
}

int32_t ItemData::GetReqLUK() const {
    return nReqLUK;
}

void ItemData::SetReqLUK(int32_t value) {
    nReqLUK = value;
}

int32_t ItemData::GetSTR() const {
    return nSTR;
}

void ItemData::SetSTR(int32_t value) {
    nSTR = value;
}

int32_t ItemData::GetDEX() const {
    return nDEX;
}

void ItemData::SetDEX(int32_t value) {
    nDEX = value;
}

int32_t ItemData::GetINT() const {
    return nINT;
}

void ItemData::SetINT(int32_t value) {
    nINT = value;
}

int32_t ItemData::GetLUK() const {
    return nLUK;
}

void ItemData::SetLUK(int32_t value) {
    nLUK = value;
}

float ItemData::GetMagic() const {
    return fMagic;
}

void ItemData::SetMagic(float value) {
    fMagic = value;
}
