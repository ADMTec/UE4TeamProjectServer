#include "Zone.hpp"


void Zone::UpdatePlayerInput()
{
   std::shared_lock lock(object_guard_[ZoneObject::Template::kCharacter]);
    for (const auto& input : input_vec[input_vec_index_]) {
        
    }
}

void Zone::SimulateZone()
{
    while (exit_flag_ == false)
    {
        UpdatePlayerInput();
    }
}
