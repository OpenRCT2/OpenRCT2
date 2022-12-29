#pragma once

#include <sol/sol.hpp>
#include "../../../object/StationObject.h"

namespace OpenRCT2::PaintScripting
{
    class PsStationObject
    {
    public:
        PsStationObject(const StationObject& stationObject);

        static void Register(sol::state& lua);

        const StationObject& GetStationObject() const;
    private:
        const StationObject& _stationObject;
    };
}
