#include "PsVehicle.h"

namespace OpenRCT2::PaintScripting
{
    PsVehicle::PsVehicle()
    {
        _vehicle = nullptr;
    }

    PsVehicle::PsVehicle(const Vehicle& vehicle)
    {
        _vehicle = &vehicle;
    }

    void PsVehicle::Register(duk_context* context)
    {
        dukglue_register_constructor<PsVehicle>(context, "Vehicle");
    }

    const Vehicle& PsVehicle::GetVehicle() const
    {
        return *_vehicle;
    }
}
