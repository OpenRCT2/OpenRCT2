#include "PsRideEntry.h"
#include "../../../ride/RideEntry.h"

namespace OpenRCT2::PaintScripting
{
    PsRideEntry::PsRideEntry(): _rideEntry(nullptr)
    {
    }

    PsRideEntry::PsRideEntry(const rct_ride_entry& rideEntry)
    {
        _rideEntry = &rideEntry;
    }

    void PsRideEntry::Register(duk_context* context)
    {
        dukglue_register_constructor<PsRideEntry>(context, "RideEntry");
    }

    const rct_ride_entry& PsRideEntry::GetRideEntry() const
    {
        return *_rideEntry;
    }
}
