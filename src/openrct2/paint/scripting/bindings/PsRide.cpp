#include "PsRide.h"
#include "PsStationObject.h"
#include "../../../ride/Ride.h"

namespace OpenRCT2::PaintScripting
{
    PsRide::PsRide(Ride& ride)
    {
        _ride = &ride;
    }

    PsRide::PsRide()
    {
        _ride = nullptr;
    }

    void PsRide::Register(duk_context* context)
    {
        dukglue_register_constructor<PsRide>(context, "Ride");
        dukglue_register_method(context, &PsRide::getStationObject, "GetStationObject");
    }

    void PsRide::Update(const Ride& ride)
    {
        _ride = &ride;
    }

    const Ride& PsRide::GetRide() const
    {
        return *_ride;
    }

    std::shared_ptr<PsStationObject> PsRide::getStationObject() const
    {
        return std::make_shared<PsStationObject>(_ride->GetStationObject());
    }
}
