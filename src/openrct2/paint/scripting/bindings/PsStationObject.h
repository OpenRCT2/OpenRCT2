#pragma once

#include "../../../scripting/Duktape.hpp"

class StationObject;
namespace OpenRCT2::PaintScripting
{
    class PsStationObject
    {

    public:
        static void Register(duk_context* context);
        PsStationObject(const StationObject* stationObject);

        const StationObject* GetStationObject();
    private:
        const StationObject* _stationObject;
        uint32_t getFlags() const;
    };
}
