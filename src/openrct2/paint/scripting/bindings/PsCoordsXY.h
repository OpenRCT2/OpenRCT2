#pragma once

#include <sol/sol.hpp>
#include "../../../world/Location.hpp"

namespace OpenRCT2::PaintScripting
{
    class PsCoordsXY
    {
    public:
        PsCoordsXY();
        PsCoordsXY(const CoordsXY& coords);

        static void Register(sol::state& lua);

        const CoordsXY& GetCoords() const;
    private:
        CoordsXY _coords;
    };
}
