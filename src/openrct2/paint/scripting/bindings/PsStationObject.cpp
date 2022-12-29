#include "PsStationObject.h"

namespace OpenRCT2::PaintScripting
{

    PsStationObject::PsStationObject(const StationObject& stationObject)
        : _stationObject(stationObject)
    {
    }

    void PsStationObject::Register(sol::state& lua)
    {
        lua.new_usertype<PsStationObject>(
            "StationObject", sol::constructors<PsStationObject(const StationObject&)>());
    }

    const StationObject& PsStationObject::GetStationObject() const
    {
        return _stationObject;
    }
}
