#pragma once

#include "../../../scripting/Duktape.hpp"
#include <memory>

struct rct_ride_entry;
namespace OpenRCT2::PaintScripting
{
    class PsRideEntry
    {
    public:
        PsRideEntry();
        PsRideEntry(const rct_ride_entry& rideEntry);

        static void Register(duk_context* context);

        const rct_ride_entry& GetRideEntry() const;
    private:
        const rct_ride_entry* _rideEntry;
    };
}
