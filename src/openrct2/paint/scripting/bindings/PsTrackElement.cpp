
#include "PsTrackElement.h"

namespace OpenRCT2::PaintScripting
{
    PsTrackElement::PsTrackElement(TrackElement& trackElement): _trackElement(&trackElement)
    {

    }

    PsTrackElement::PsTrackElement()
        : _trackElement(nullptr)
    {
    }

    void PsTrackElement::Update(const TrackElement& trackElement)
    {
        _trackElement = &trackElement;
    }

    const TrackElement& PsTrackElement::GetTrackElement() const
    {
        return *_trackElement;
    }

    void PsTrackElement::Register(duk_context* context)
    {
        dukglue_register_constructor<PsTrackElement>(context, "TrackElement");
    }
}
