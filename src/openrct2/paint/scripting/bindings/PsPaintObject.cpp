#include "PsPaintObject.h"
#include "../../../world/TileElement.h"
#include "../../../ride/Ride.h"

namespace OpenRCT2::PaintScripting
{
    PsPaintObject::PsPaintObject()
    {
    }

    int32_t PsPaintObject::getHeight() const
    {
        return _height;
    }

    uint8_t PsPaintObject::getDirection() const
    {
        return _direction;
    }

    uint8_t PsPaintObject::getTrackSequence() const
    {
        return _trackSequence;
    }

    std::shared_ptr<PsPaintSession> PsPaintObject::getPaintSession() const
    {
        return std::make_shared<PsPaintSession>(_paintSession);
    }

    void PsPaintObject::Register(duk_context* context)
    {
        PsPaintSession::Register(context);

        dukglue_register_global(context, &PsPaintObject::PaintObject, "PaintObject");
        dukglue_register_property(context, &PsPaintObject::getHeight, nullptr, "Height");
        dukglue_register_property(context, &PsPaintObject::getDirection, nullptr, "Direction");
        dukglue_register_property(context, &PsPaintObject::getTrackSequence, nullptr, "TrackSequence");
        dukglue_register_property(context, &PsPaintObject::getPaintSession, nullptr, "Session");
    }

    void PsPaintObject::Update(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height, const TrackElement& trackElement)
    {
        _direction = direction;
        _height = height;
        _trackSequence = trackSequence;
        _paintSession.Update(session);
    }

    PsPaintObject PsPaintObject::PaintObject = PsPaintObject();
    int32_t PsPaintObject::_height = 0;
    uint8_t PsPaintObject::_direction = 0;
    uint8_t PsPaintObject::_trackSequence = 0;
    PsPaintSession PsPaintObject::_paintSession;
}
