#include "CharacterStat.hpp"


CharacterStat::CharacterStat(Character& base)
    : base_(base)
{
}

void CharacterStat::Initialize(int32_t db_cid)
{
}

void CharacterStat::Write(OutputStream& out) const
{
    out << gender_;
    out << hp_;
    out << max_hp;
    out << stamina_;
    out << max_stamina_;
    out << attack_range_;
    out << attack_speed_;
    out << speed_;
    out << face_id_;
    out << hair_id_;
    out << job_;
    out << lv_;
    out << str_;
    out << dex_;
    out << intel_;
}

void CharacterStat::Read(InputStream& input)
{
    input >> gender_;
    input >> hp_;
    input >> max_hp;
    input >> stamina_;
    input >> max_stamina_;

    input >> attack_range_;
    input >> attack_speed_;
    input >> speed_;
    input >> face_id_;
    input >> hair_id_;
    input >> job_;
    input >> lv_;
    input >> str_;
    input >> dex_;
    input >> intel_;
}

int32_t CharacterStat::GetGender() const {
    return gender_;
}

void CharacterStat::SetGender(int32_t value) {
    gender_ = value;
}

float CharacterStat::GetHP() const {
    return hp_;
}

void CharacterStat::SetHP(float value) {
    hp_ = value;
}

float CharacterStat::GetMaxHP() const {
    return max_hp;
}

void CharacterStat::SetMaxHP(float value) {
    max_hp = value;
}

float CharacterStat::GetStamina() const {
    return stamina_;
}

void CharacterStat::SetStamina(float value) {
    stamina_ = value;
}

float CharacterStat::GetStaminaRecovery() const
{
    return stamina_recovery_;
}

void CharacterStat::SetStaminaRecovery(float value)
{
    stamina_recovery_ = value;
}

float CharacterStat::GetMaxStamina() const {
    return max_stamina_;
}

void CharacterStat::SetMaxStamina(float value) {
    max_stamina_ = value;
}

float CharacterStat::GetAttackMin() const
{
    return attack_min_;
}

void CharacterStat::SetAttackMin(float value)
{
    attack_min_ = value;
}

float CharacterStat::GetAttackMax() const
{
    return attack_max_;
}

void CharacterStat::SetAttackMax(float value)
{
    attack_max_ = value;
}

float CharacterStat::GetAttackRange() const {
    return attack_range_;
}

void CharacterStat::SetAttackRange(float value) {
    attack_range_ = value;
}

float CharacterStat::GetAttackSpeed() const
{
    return attack_speed_;
}

void CharacterStat::SetAttackSpeed(float value)
{
    attack_speed_ = value;
}

float CharacterStat::GetSpeed() const
{
    return speed_;
}

void CharacterStat::SetSpeed(float value)
{
    speed_ = value;
}

int32_t CharacterStat::GetFaceId() const {
    return face_id_;
}

void CharacterStat::SetFaceId(int32_t value) {
    face_id_ = value;
}

int32_t CharacterStat::GetHairId() const {
    return hair_id_;
}

void CharacterStat::SetHairId(int32_t value) {
    hair_id_ = value;
}

int32_t CharacterStat::GetJob() const {
    return job_;
}

void CharacterStat::SetJob(int32_t value) {
    job_ = value;
}

int64_t CharacterStat::GetLevel() const {
    return lv_;
}

void CharacterStat::SetLevel(int64_t value) {
    lv_ = value;
}

int32_t CharacterStat::GetStr() const {
    return str_;
}

void CharacterStat::SetStr(int32_t value) {
    str_ = value;
}

int32_t CharacterStat::GetDex() const {
    return dex_;
}

void CharacterStat::SetDex(int32_t value) {
    dex_ = value;
}

int32_t CharacterStat::GetIntel() const {
    return intel_;
}

void CharacterStat::SetIntel(int32_t value) {
    intel_ = value;
}