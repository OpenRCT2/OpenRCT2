#include "PsVehicle.h"
#include "../../../ride/Vehicle.h"

namespace OpenRCT2::PaintScripting
{
    PsVehicle::PsVehicle(): _vehicle(nullptr)
    {
    }

    PsVehicle::PsVehicle(const Vehicle& vehicle)
        : _vehicle(&vehicle)
    {
    }

    void PsVehicle::Register(sol::state& lua)
    {
        auto type = lua.new_usertype<PsVehicle>("Vehicle", sol::constructors<PsVehicle(const Vehicle&)>());
    }

    const Vehicle& PsVehicle::GetVehicle() const
    {
        return *_vehicle;
    }
}
