#include "PsRide.h"

#include "../../../ride/Ride.h"
#include "PsStationObject.h"
#include "../../../entity/EntityRegistry.h"

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
        type["GetRideEntry"] = &PsRide::GetRideEntry;
        type["GetLifecyleFlags"] = &PsRide::GetLifecycleFlags;
        type["GetVehicle"] = &PsRide::GetVehicle;
    }

    Ride& PsRide::GetRide()
    {
        return *_ride;
    }

    PsStationObject PsRide::GetStationObject()
    {
        return PsStationObject(*_ride->GetStationObject());
    }

    PsRideEntry PsRide::GetRideEntry()
    {
        return PsRideEntry(*_ride->GetRideEntry());
    }

    uint32_t PsRide::GetLifecycleFlags()
    {
        return _ride->lifecycle_flags;
    }

    PsVehicle PsRide::GetVehicle(size_t index)
    {
        if (index >= std::size(_ride->vehicles))
            return PsVehicle();
        auto vehicle = GetEntity<Vehicle>(_ride->vehicles[index]);
        return PsVehicle(*vehicle);
    }
} // namespace OpenRCT2::PaintScripting
