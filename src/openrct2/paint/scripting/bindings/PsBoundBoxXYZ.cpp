#include "PsBoundBoxXYZ.h"

namespace OpenRCT2::PaintScripting
{
    PsBoundBoxXYZ::PsBoundBoxXYZ()
    {
    }

    PsBoundBoxXYZ::PsBoundBoxXYZ(const PsCoordsXYZ& offset, const PsCoordsXYZ& size)
        : _boundBox(offset.GetCoords(), size.GetCoords())
    {
    }

    void PsBoundBoxXYZ::Register(sol::state& lua)
    {
        auto type = lua.new_usertype<PsBoundBoxXYZ>(
            "BoundBoxXYZ", sol::constructors<PsBoundBoxXYZ(), PsBoundBoxXYZ(const PsCoordsXYZ&, const PsCoordsXYZ&)>());
    }

    const BoundBoxXYZ& PsBoundBoxXYZ::GetBoundBoxXYZ() const
    {
        return _boundBox;
    }
}
