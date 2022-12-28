#include "PsPaint.h"

#include "../../Paint.h"
#include "PsPaintSession.h"

namespace OpenRCT2::PaintScripting
{
    PsPaint::PsPaint()
    {
    }

    PsPaint::PsPaint(PaintSession& session, uint8_t trackSequence, uint8_t direction, int32_t height)
        : _trackSequence(trackSequence)
        , _direction(direction)
        , _height(height)
    {
        _session = PsPaintSession(session);
    }

    void PsPaint::Register(sol::state& lua)
    {
        auto type = lua.new_usertype<PsPaint>(
            "PaintObject", sol::constructors<PsPaint(), PsPaint(PaintSession&, uint8_t, uint8_t, int32_t)>());
        type["Direction"] = &PsPaint::_direction;
        type["TrackSequence"] = &PsPaint::_trackSequence;
        type["Height"] = &PsPaint::_height;
        type["Session"] = &PsPaint::_session;
    }

    void PsPaint::Update(PaintSession& session, uint8_t trackSequence, uint8_t direction, int32_t height)
    {
        _trackSequence = trackSequence;
        _direction = direction;
        _height = height;
        _session.Update(session);
    }

    const PsPaintSession& PsPaint::GetPaintSession() const
    {
        return _session;
    }



    uint8_t PsPaint::GetTrackSequence() const
    {
        return _trackSequence;
    }

    uint8_t PsPaint::GetDirection() const
    {
        return _direction;
    }

    int32_t PsPaint::GetHeight() const
    {
        return _height;
    }
} // namespace OpenRCT2::PaintScripting
