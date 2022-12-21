#pragma once

#include "../../../world/Location.hpp"
#include "../../../scripting/Duktape.hpp"
namespace OpenRCT2::PaintScripting
{
    class PsCoordsXY
    {
    public:
        PsCoordsXY();
        PsCoordsXY(const CoordsXY& coords);
        const CoordsXY& GetCoords() const;

        static void Register(duk_context* context);
    private:
        CoordsXY _coords;
    };
}
