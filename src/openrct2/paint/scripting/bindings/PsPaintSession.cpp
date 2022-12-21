#include "PsPaintSession.h"
#include "PsImageId.h"
#include "PsCoordsXY.h"
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
        dukglue_register_property(context, &PsPaintSession::getTrackColours, nullptr, "TrackColours");
        dukglue_register_property(context, &PsPaintSession::getMapPosition, nullptr, "MapPosition");
        dukglue_register_property(context, &PsPaintSession::getCurrentRotation, nullptr, "CurrentRotation");
    }

    void PsPaintSession::Update(PaintSession& paintSession)
    {
        _paintSession = &paintSession;
    }

    PaintSession* PsPaintSession::GetSession()
    {
        return _paintSession;
    }

    std::vector<std::shared_ptr<PsImageId>> PsPaintSession::getTrackColours() const
    {
        std::vector<std::shared_ptr<PsImageId>> result;
        result.resize(4);
        for (size_t i = 0; i < 4; i++)
            result[i] = std::make_shared<PsImageId>(_paintSession->TrackColours[i]);
        return result;
    }

    std::shared_ptr<PsCoordsXY> PsPaintSession::getMapPosition() const
    {
        return std::make_shared<PsCoordsXY>(_paintSession->MapPosition);
    }

    uint8_t PsPaintSession::getCurrentRotation() const
    {
        return _paintSession->CurrentRotation;
    }
}
