#include "Zone.hpp"
#include "Private/ZoneImpl.hpp"


Zone::Zone(const Zone&)
{

}

void Zone::operator=(const Zone&)
{

}

Zone::Zone() : impl_(nullptr)
{
    impl_ = new ZoneImpl();
}

Zone::Zone(ZoneImpl* impl)
    : impl_(impl)
{
}

Zone::~Zone()
{
    delete impl_;
}

void Zone::SpawnPlayer(const std::shared_ptr<class Character>& chr) 
{
    impl_->SpawnPlayer(chr);
}

void Zone::AddMonster(Monster& mob, Location location, Rotation rotation) 
{
    impl_->AddMonster(mob, location, rotation);
}

void Zone::SpawnMonster(Monster& mob, Location location, Rotation rotation) 
{
    impl_->SpawnMonster(mob, location, rotation);
}

void Zone::AddNPC(int32_t npc, Location location, Rotation rotation) 
{
    impl_->AddNPC(npc, location, rotation);
}

void Zone::SpawnNpc(int32_t npc, Location location, Rotation rotation) {

    impl_->SpawnNPC(npc, location, rotation);
}

void Zone::AttackMonster(const Character& chr, int64_t mob_obj_id) {

    impl_->AttackMonster(chr, mob_obj_id);
}

void Zone::BroadCast(class NioOutPacket& outpacket) {
    impl_->BroadCast(outpacket);
}
void Zone::BroadCast(class NioOutPacket& outpacket, int64_t except_chr_oid) {
    impl_->BroadCast(outpacket, except_chr_oid);
}

int64_t Zone::GetInstanceId() const {
    return impl_->GetInstanceId();
}

void Zone::SetInstanceId(int64_t id) {
    impl_->SetInstanceId(id);
}

int32_t Zone::GetMapId() const {
    return impl_->GetMapId();
}

void Zone::SetMapId(int32_t map_id) {
    impl_->SetMapId(map_id);
}

Zone::Type Zone::GetType() const {
    return impl_->GetType();
}

void Zone::SetType(Zone::Type type) {
    impl_->SetType(type);
}

void Zone::SetPlayerSpawn(Location spawn)
{
    impl_->SetPlayerSpawn(spawn);
}

int64_t Zone::GetNewObjectId() {
    return impl_->GetNewObjectId();
}