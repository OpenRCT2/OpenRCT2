#include "PsCoordsXYZ.h"

namespace OpenRCT2::PaintScripting
{
    PsCoordsXYZ::PsCoordsXYZ()
    {
    }

    PsCoordsXYZ::PsCoordsXYZ(int x, int y, int z)
        : _coords(x, y, z)
    {
    }

    void PsCoordsXYZ::Register(sol::state& lua)
    {
        auto type = lua.new_usertype<PsCoordsXYZ>("CoordsXYZ", sol::constructors<PsCoordsXYZ(), PsCoordsXYZ(int, int, int)>());
    }

    const CoordsXYZ& PsCoordsXYZ::GetCoords() const
    {
        return _coords;
    }
}
