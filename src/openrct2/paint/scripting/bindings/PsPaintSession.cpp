#include "PsPaintSession.h"

#include "../../../drawing/ImageId.hpp"
#include "../../Paint.h"
#include "PsImageId.h"

#include <sol/sol.hpp>

namespace OpenRCT2::PaintScripting
{
    PsPaintSession::PsPaintSession()
        : _paintSession(nullptr)
    {
    }

    PsPaintSession::PsPaintSession(PaintSession& paintSession)
        : _paintSession(&paintSession)
    {
        for (const auto& colour : _paintSession->TrackColours)
            _trackColours.push_back(PsImageId(colour));
    }

    void PsPaintSession::Update(PaintSession& paintSession)
    {
        _paintSession = &paintSession;
        for (const auto& colour : _paintSession->TrackColours)
            _trackColours.push_back(PsImageId(colour));
    }

    PaintSession& PsPaintSession::GetPaintSession()
    {
        return *_paintSession;
    }

    void PsPaintSession::Register(sol::state& lua)
    {
        auto paintType = lua.new_usertype<PsPaintSession>(
            "PaintSession", sol::constructors<PsPaintSession(), PsPaintSession(PaintSession&)>());
        paintType["GetTrackColour"] = &PsPaintSession::GetTrackColour;
    }

    PsImageId& PsPaintSession::GetTrackColour(uint8_t index)
    {
        return _trackColours[index];
    }

    /*std::vector<PsImageId> PsPaintSession::GetTrackColours() const
    {
        return _trackColours;
    }*/
} // namespace OpenRCT2::PaintScripting
