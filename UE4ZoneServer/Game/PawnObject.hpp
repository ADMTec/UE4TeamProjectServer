#pragma once
#include "ZoneObject.hpp"

class PawnObject : public ZoneObject
{
public:
    PawnObject(ZoneObject::Template template_id, oid_t object_id = -1);
    virtual ~PawnObject();
public:
    virtual void Write(OutputStream& output) const override;
    virtual void Read(InputStream& input) override;
protected:
    float hp_ = 0.0f;
    float max_hp = 0.0f;
    float attack_min_ = 0.0f;
    float attack_max_ = 0.0f;
    float attack_range_ = 0.0f;
    float attack_speed_ = 0.0f;
    float defence_ = 0.0f;
    float speed_ = 0.0f;
};