#pragma once

#include "../../../world/Location.hpp"
#include "../../Boundbox.h"
#include "PsCoordsXYZ.h"

#include <sol/sol.hpp>

namespace OpenRCT2::PaintScripting
{
    class PsBoundBoxXYZ
    {
    public:
        PsBoundBoxXYZ();
        PsBoundBoxXYZ(const PsCoordsXYZ& offset, const PsCoordsXYZ& size);

        static void Register(sol::state& lua);

        const BoundBoxXYZ& GetBoundBoxXYZ() const;

    private:
        BoundBoxXYZ _boundBox;
    };
} // namespace OpenRCT2::PaintScripting
