#include "PsCoordsXY.h"

namespace OpenRCT2::PaintScripting
{
    PsCoordsXY::PsCoordsXY()
    {
    }

    PsCoordsXY::PsCoordsXY(const CoordsXY& coords)
    {
        _coords = coords;
    }

    const CoordsXY& PsCoordsXY::GetCoords() const
    {
        return _coords;
    }

    void PsCoordsXY::Register(duk_context* context)
    {
        dukglue_register_constructor<PsCoordsXY>(context, "CoordsXY");
    }
}
