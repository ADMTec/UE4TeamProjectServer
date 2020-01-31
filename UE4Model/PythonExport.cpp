#include "Model/Character/Character.hpp"
#include "Model/Zone.hpp"
#include "boost/python.hpp"

BOOST_PYTHON_MODULE(UE4Model)
{
    FVector& (ZoneObject::*NonConstGetLocation)(void) = &ZoneObject::GetLocation;
    FVector& (ZoneObject::*NonConstGetRotation)(void) = &ZoneObject::GetRotation;

    using namespace boost::python;
    class_<FVector>("FVector")
        .def_readwrite("x", &FVector::x)
        .def_readwrite("y", &FVector::y)
        .def_readwrite("z", &FVector::z);
    class_<ZoneObject>("ZoneObject", boost::python::no_init)
        .add_property("type", &ZoneObject::GetType)
        .add_property("template_id", &ZoneObject::GetTemplateId, &ZoneObject::SetTemplateId)
        .add_property("object_id", &ZoneObject::GetObjectId, &ZoneObject::SetObjectId)
        .def("location", NonConstGetLocation, return_value_policy<reference_existing_object>())
        .def("rotation", NonConstGetRotation, return_value_policy<reference_existing_object>());
    class_<PawnObject, bases<ZoneObject>>("PawnObject", boost::python::no_init)
        .add_property("hp", &PawnObject::GetHP, &PawnObject::SetHP)
        .add_property("max_hp", &PawnObject::GetMaxHP, &PawnObject::SetMaxHP)
        .add_property("attack_min", &PawnObject::GetAttackMin, &PawnObject::SetAttackMin)
        .add_property("Attack_max", &PawnObject::GetAttackMax, &PawnObject::SetAttackMax)
        .add_property("Attack_range", &PawnObject::GetAttackRange, &PawnObject::SetAttackRange)
        .add_property("Attack_speed", &PawnObject::GetAttackSpeed, &PawnObject::SetAttackSpeed)
        .add_property("defence", &PawnObject::GetDefence, &PawnObject::SetDefence)
        .add_property("speed", &PawnObject::GetSpeed, &PawnObject::SetSpeed);
    class_<Character, bases<PawnObject>, boost::noncopyable>("Character", boost::python::no_init)
        .add_property("level", &Character::GetLevel)
        .add_property("job", &Character::GetJob)
        .add_property("str", &Character::GetStr)
        .add_property("dex", &Character::GetDex)
        .add_property("intel", &Character::GetIntel)
        .add_property("gold", &Character::GetGold)
        .add_property("stamina", &Character::GetStamina)
        .add_property("max_stamina", &Character::GetMaxStamina)
        .def("equipment", &Character::GetEquipment, return_value_policy<reference_existing_object>());
    class_<Equipment>("Equipment")
        .add_property("total_add_atk", &Equipment::GetTotalATK)
        .add_property("total_add_str", &Equipment::GetTotalAddStr)
        .add_property("total_add_dex", &Equipment::GetTotalAddDex)
        .add_property("total_add_intel", &Equipment::GetTotalAddIntel);
    class_<Zone, boost::noncopyable>("Zone")
        .def("SpawnMonster", &Zone::SpawnMonster);
}