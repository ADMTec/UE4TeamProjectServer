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
    void Initialize(int32_t db_cid);
public:
    virtual void Write(OutputStream& out) const override;
    virtual void Read(InputStream& input) override;
public:
    int32_t GetRacial() const;
    void SetRacial(int32_t value);
    int32_t GetGender() const;
    void SetGender(int32_t value);
    float GetHP() const;
    void SetHP(float value);
    float GetMaxHP() const;
    void SetMaxHP(float value);
    float GetStamina() const;
    void SetStamina(float value);
    float GetMaxStamina() const;
    void SetMaxStamina(float value);
    float GetAttack() const;
    void SetAttack(float value);
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
    int32_t GetLuk() const;
    void SetLuk(int32_t value);
private:
    Character& base_;

    int32_t racial = 0;
    int32_t gender_ = 0;
    float hp_ = 0.0f;
    float max_hp = 0.0f;
    float stamina_ = 0.0f;
    float max_stamina = 0.0f;
    float attack_ = 0.0f;
    float attackrange_ = 0.0f;
    float attackspeed_ = 0.0f;
    float speed_ = 0.0f;
    int32_t faceid_ = 0;
    int32_t hair_id_ = 0;
    int32_t job_ = 0;
    int64_t lv_ = 0;
    int32_t str_ = 0;
    int32_t dex_ = 0;
    int32_t intel_ = 0;
    int32_t luk_ = 0;
};