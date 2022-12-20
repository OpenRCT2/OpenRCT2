#include "PsStationObject.h"
#include "../../../object/StationObject.h"

namespace OpenRCT2::PaintScripting
{
    void PsStationObject::Register(duk_context* context)
    {
        dukglue_register_constructor<PsStationObject, const StationObject*>(context, "StationObject");
        dukglue_register_property(context, &PsStationObject::getFlags, nullptr, "Flags");
    }

    PsStationObject::PsStationObject(const StationObject* stationObject)
    {
        _stationObject = stationObject;
    }

    uint32_t PsStationObject::getFlags() const
    {
        return _stationObject->Flags;
    }

    const StationObject* PsStationObject::GetStationObject()
    {
        return _stationObject;
    }
}
