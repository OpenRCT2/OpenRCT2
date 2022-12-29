#include "PsPaintSession.h"

#include "../../../drawing/ImageId.hpp"
#include "../../Paint.h"
#include "PsImageId.h"
#include "PsCoordsXY.h"
#include "PsVehicle.h"
#include "../../../ride/Vehicle.h"

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

        _mapPosition = PsCoordsXY(_paintSession->MapPosition);
        _currentRotation = _paintSession->CurrentRotation;
    }

    PaintSession& PsPaintSession::GetPaintSession()
    {
        return *_paintSession;
    }

    void PsPaintSession::Register(sol::state& lua)
    {
        auto type = lua.new_usertype<PsPaintSession>(
            "PaintSession", sol::constructors<PsPaintSession(), PsPaintSession(PaintSession&)>());
        type["GetTrackColour"] = &PsPaintSession::GetTrackColour;
        type["MapPosition"] = &PsPaintSession::_mapPosition;
        type["CurrentRotation"] = &PsPaintSession::_currentRotation;
        type["SetAsActiveEntity"] = &PsPaintSession::SetAsActiveEntity;
    }

    PsImageId& PsPaintSession::GetTrackColour(uint8_t index)
    {
        return _trackColours[index];
    }

    void PsPaintSession::SetAsActiveEntity(PsVehicle& vehicle)
    {
        auto vehicleStruct = vehicle.GetVehicle();
        _paintSession->InteractionType = ViewportInteractionItem::Entity;
        _paintSession->CurrentlyDrawnEntity = &vehicleStruct;
    }

    /*std::vector<PsImageId> PsPaintSession::GetTrackColours() const
    {
        return _trackColours;
    }*/
} // namespace OpenRCT2::PaintScripting
