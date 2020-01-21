#pragma once
#include <cstdint>
#include <string>


class ItemData : public SerializeInterface
{
public:
    ItemData() = default;
    ~ItemData() = default;
public:
    virtual void Write(OutputStream& out) const override;
    virtual void Read(InputStream& input) override;
public:
    int32_t GetId() const;
    void SetId(int32_t value);
    std::string GetName() const;
    void SetName(std::string value);
    float GetAttackMin() const;
    void SetAttackMin(float value);
    float GetAttackMax() const;
    void SetAttackMax(float value);
    float GetDefence() const;
    void SetDefence(float value);
    int32_t GetReqSTR() const;
    void SetReqSTR(int32_t value);
    int32_t GetReqDEX() const;
    void SetReqDEX(int32_t value);
    int32_t GetReqINT() const;
    void SetReqINT(int32_t value);
    int32_t GetReqLUK() const;
    void SetReqLUK(int32_t value);
    int32_t GetSTR() const;
    void SetSTR(int32_t value);
    int32_t GetDEX() const;
    void SetDEX(int32_t value);
    int32_t GetINT() const;
    void SetINT(int32_t value);
    int32_t GetLUK() const;
    void SetLUK(int32_t value);
    float GetMagic() const;
    void SetMagic(float value);
private:
    int32_t nId;
    std::string sName;
    float fAttackMin;
    float fAttackMax;
    float fDefence;
    int32_t nReqSTR;
    int32_t nReqDEX;
    int32_t nReqINT;
    int32_t nReqLUK;
    int32_t nSTR;
    int32_t nDEX;
    int32_t nINT;
    int32_t nLUK;
    float fMagic;
};
