#include "PsRideEntry.h"
#include "../../../ride/RideEntry.h"

namespace OpenRCT2::PaintScripting
{
    PsRideEntry::PsRideEntry(): _rideEntry(nullptr)
    {
    }

    PsRideEntry::PsRideEntry(const rct_ride_entry& rideEntry)
        : _rideEntry(&rideEntry)
    {
    }

    void PsRideEntry::Register(sol::state& lua)
    {
        auto type = lua.new_usertype<PsRideEntry>("RideEntry", sol::constructors<PsRideEntry(const rct_ride_entry&)>());
        type["GetCarEntry"] = &PsRideEntry::GetCarEntry;
    }

    const rct_ride_entry& PsRideEntry::GetRideEntry()
    {
        return *_rideEntry;
    }

    PsCarEntry PsRideEntry::GetCarEntry(size_t index) const
    {
        return PsCarEntry(_rideEntry->Cars[index]);
    }
}
