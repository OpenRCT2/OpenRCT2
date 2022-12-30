#pragma once

#include <sol/sol.hpp>
#include "../../../world/Location.hpp"

namespace OpenRCT2::PaintScripting
{
    class PsCoordsXYZ
    {
    public:
        PsCoordsXYZ();
        PsCoordsXYZ(int x, int y, int z);

        static void Register(sol::state& lua);

        const CoordsXYZ& GetCoords() const;
    private:
        CoordsXYZ _coords;
    };
}
