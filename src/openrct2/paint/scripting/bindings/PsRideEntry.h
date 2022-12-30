#pragma once

#include <sol/sol.hpp>
#include "PsCarEntry.h"

struct rct_ride_entry;
namespace OpenRCT2::PaintScripting
{
    class PsRideEntry
    {
    public:
        PsRideEntry();
        PsRideEntry(const rct_ride_entry& rideEntry);

        static void Register(sol::state& lua);

        const rct_ride_entry& GetRideEntry();
    private:
        const rct_ride_entry* _rideEntry;
        PsCarEntry GetCarEntry(size_t index) const;
    };
}
