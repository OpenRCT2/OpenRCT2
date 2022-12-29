#pragma once

#include <sol/sol.hpp>
#include "../../../world/TileElement.h"

struct TrackElement;
namespace OpenRCT2::PaintScripting
{
    class PsTrackElement
    {
    public:
        PsTrackElement();
        PsTrackElement(const TrackElement& element);

        static void Register(sol::state& lua);
        const TrackElement& GetTrackElement() const;
    private:
        const TrackElement* _element;
    };
}
