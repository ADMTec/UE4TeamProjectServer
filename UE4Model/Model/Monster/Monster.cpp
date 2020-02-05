#include "Monster.hpp"
#include <sstream>


Monster::Monster(const Monster& rhs)
    : PawnObject(rhs)
{
    state_ = rhs.state_;
}

void Monster::operator=(const Monster& rhs)
{
    PawnObject::operator=(rhs);

    state_ = rhs.state_;
}

Monster::Monster()
    : PawnObject(ZoneObject::Type::kMonster), state_(static_cast<int32_t>(Monster::State::kAlive))
{

}

std::string Monster::GetDebugString() const
{
    auto lo = this->GetLocation();
    std::stringstream ss;
    ss << " Monster OID: " << this->GetObjectId() << ", HP: " << this->GetHP()
        << " Location: [" << lo.x << ", " << lo.y << ", " << lo.z << "]" << '\n';
    return ss.str();
}

int32_t Monster::GetState() const
{
    return state_;
}

void Monster::SetState(int32_t value)
{
    state_ = value;
}
