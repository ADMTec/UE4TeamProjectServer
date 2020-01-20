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
    void GetId() const;
    void SetId(int32_t value);
    void GetName() const;
    void SetName(std::string value);
    void GetAttackMin() const;
    void SetAttackMin(float value);
    void GetAttackMax() const;
    void SetAttackMax(float value);
    void GetDefence() const;
    void SetDefence(float value);
    void GetReqSTR() const;
    void SetReqSTR(int32_t value);
    void GetReqDEX() const;
    void SetReqDEX(int32_t value);
    void GetReqINT() const;
    void SetReqINT(int32_t value);
    void GetReqLUK() const;
    void SetReqLUK(int32_t value);
    void GetSTR() const;
    void SetSTR(int32_t value);
    void GetDEX() const;
    void SetDEX(int32_t value);
    void GetINT() const;
    void SetINT(int32_t value);
    void GetLUK() const;
    void SetLUK(int32_t value);
    void GetMagic() const;
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
