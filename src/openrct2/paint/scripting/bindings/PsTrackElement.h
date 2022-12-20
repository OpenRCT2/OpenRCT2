#pragma once

#include "../../../scripting/Duktape.hpp"

#include <memory>

struct TrackElement;
namespace OpenRCT2::PaintScripting
{
    class PsTrackElement
    {
    public:
        PsTrackElement(TrackElement& trackElement);
        PsTrackElement();

        void Update(const TrackElement& trackElement);
        static void Register(duk_context* context);

        const TrackElement& GetTrackElement() const;
    private:
        const TrackElement* _trackElement;
    };
} // namespace OpenRCT2::PaintScripting
