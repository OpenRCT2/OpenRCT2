#pragma once

#include <sol/sol.hpp>
#include <memory>
#include "PsStationObject.h"

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
    };
}
