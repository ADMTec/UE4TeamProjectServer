#pragma once
#include "ZoneObject.hpp"

class PawnObject : public ZoneObject
{
public:
    PawnObject(const PawnObject& rhs);
    void operator=(const PawnObject& rhs);
public:
    PawnObject(ZoneObject::Type template_id, oid_t object_id = -1);
    virtual ~PawnObject();
public:
    float GetHP() const;
    float GetMaxHP() const;
    float GetAttackMin() const;
    float GetAttackMax() const;
    float GetAttackRange() const;
    float GetAttackSpeed() const;
    float GetDefence() const;
    float GetSpeed() const;
protected:
    void SetHP(float value);
    void SetMaxHP(float value);
    void SetAttackMin(float value);
    void SetAttackMax(float value);
    void SetAttackRange(float value);
    void SetAttackSpeed(float value);
    void SetDefence(float value);
    void SetSpeed(float value);
public:
    virtual void Write(OutputStream& output) const override;
    virtual void Read(InputStream& input) override;
private:
    float hp_ = 0.0f;
    float max_hp_ = 0.0f;
    float attack_min_ = 0.0f;
    float attack_max_ = 0.0f;
    float attack_range_ = 0.0f;
    float attack_speed_ = 0.0f;
    float defence_ = 0.0f;
    float speed_ = 0.0f;
};