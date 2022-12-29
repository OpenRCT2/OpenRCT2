
#include "PsCoordsXY.h"

namespace OpenRCT2::PaintScripting
{
    PsCoordsXY::PsCoordsXY()
    {
    }

    PsCoordsXY::PsCoordsXY(const CoordsXY& coords): _coords(coords)
    {
    }

    void PsCoordsXY::Register(sol::state& lua)
    {
        auto paintType = lua.new_usertype<PsCoordsXY>(
            "CoordsXY", sol::constructors<PsCoordsXY(), PsCoordsXY(const CoordsXY&)>());
    }

    const CoordsXY& PsCoordsXY::GetCoords() const
    {
        return _coords;
    }
}
