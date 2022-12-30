#pragma once

#include <sol/sol.hpp>
#include <memory>
#include "PsStationObject.h"
#include "PsRideEntry.h"
#include "PsVehicle.h"

struct Ride;
namespace OpenRCT2::PaintScripting
{
    class PsRide
    {
    public:
        PsRide(Ride& ride);

        static void Register(sol::state& lua);

        Ride& GetRide();

    private:
        Ride* _ride;
        PsStationObject GetStationObject();
        PsRideEntry GetRideEntry();
        uint32_t GetLifecycleFlags();
        PsVehicle GetVehicle(size_t index);

        uint8_t GetVehicleBodyColour(size_t colourIndex);
        uint8_t GetVehicleTrimColour(size_t colourIndex);
    };
}
