#pragma once
#include "UE4DevelopmentLibrary/Stream.hpp"
#include <cstdint>
#include <string>
#include <functional>

class Character;


class CharacterStat : public SerializeInterface
{
public:
    CharacterStat(Character& base);
public:
    void SetDefault(class CharacterTable& data);
    void RecalculateStat();
public:
    virtual void Write(OutputStream& out) const override;
    virtual void Read(InputStream& input) override;
public:
    int32_t GetGender() const;
    void SetGender(int32_t value);
    float GetHP() const;
    void SetHP(float value);
    float GetMaxHP() const;
    void SetMaxHP(float value);
    float GetStamina() const;
    void SetStamina(float value);
    float GetStaminaRecovery() const;
    void SetStaminaRecovery(float value);
    float GetMaxStamina() const;
    void SetMaxStamina(float value);
    float GetAttackMin() const;
    void SetAttackMin(float value);
    float GetAttackMax() const;
    void SetAttackMax(float value);
    float GetAttackRange() const;
    void SetAttackRange(float value);
    float GetAttackSpeed() const;
    void SetAttackSpeed(float value);
    float GetSpeed() const;
    void SetSpeed(float value);
    int32_t GetFaceId() const;
    void SetFaceId(int32_t value);
    int32_t GetHairId() const;
    void SetHairId(int32_t value);
    int32_t GetJob() const;
    void SetJob(int32_t value);
    int64_t GetLevel() const;
    void SetLevel(int64_t value);
    int32_t GetStr() const;
    void SetStr(int32_t value);
    int32_t GetDex() const;
    void SetDex(int32_t value);
    int32_t GetIntel() const;
    void SetIntel(int32_t value);
private:
    Character& base_;


};