#include "PsPaintSession.h"
#include "../../Paint.h"

namespace OpenRCT2::PaintScripting
{
    PsPaintSession::PsPaintSession()
    {
        _paintSession = nullptr;
    }

    void PsPaintSession::Register(duk_context* context)
    {
        dukglue_register_constructor<PsPaintSession>(context, "PaintSession");
        dukglue_register_method(context, &PsPaintSession::getTrackColour, "GetTrackColour");
    }

    void PsPaintSession::Update(PaintSession& paintSession)
    {
        _paintSession = &paintSession;

        for (size_t i = 0; i < 4; i++)
        {
            _trackColours[i] = std::make_shared<PsImageId>(_paintSession->TrackColours[i]);
        }
    }

    PaintSession* PsPaintSession::GetSession()
    {
        return _paintSession;
    }

    std::shared_ptr<PsImageId> PsPaintSession::getTrackColour(size_t index) const
    {
        return _trackColours[index];
    }
}
