#include "PsRide.h"

#include "../../../ride/Ride.h"
#include "PsStationObject.h"

namespace OpenRCT2::PaintScripting
{
    PsRide::PsRide(Ride& ride)
        : _ride(&ride)
    {
    }

    void PsRide::Register(sol::state& lua)
    {
        auto type = lua.new_usertype<PsRide>("Ride", sol::constructors<PsRide(Ride&)>());
        type["GetStationObject"] = &PsRide::GetStationObject;
    }

    Ride& PsRide::GetRide()
    {
        return *_ride;
    }

    PsStationObject PsRide::GetStationObject()
    {
        return PsStationObject(*_ride->GetStationObject());
    }
} // namespace OpenRCT2::PaintScripting
