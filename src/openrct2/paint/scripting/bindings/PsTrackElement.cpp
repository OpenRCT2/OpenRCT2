
#include "PsTrackElement.h"

namespace OpenRCT2::PaintScripting
{
    PsTrackElement::PsTrackElement(): _element(nullptr)
    {
    }

    PsTrackElement::PsTrackElement(const TrackElement& element)
        : _element(&element)
    {
    }

    void PsTrackElement::Register(sol::state& lua)
    {
        auto type = lua.new_usertype<PsTrackElement>(
            "CoordsXY", sol::constructors<PsTrackElement(), PsTrackElement(const TrackElement&)>());
    }

    const TrackElement& PsTrackElement::GetTrackElement() const
    {
        return *_element;
    }
}
