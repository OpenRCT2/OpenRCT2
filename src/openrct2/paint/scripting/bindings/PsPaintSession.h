#pragma once

#include <sol/sol.hpp>
#include <memory>
#include "PsImageId.h"
#include "PsCoordsXY.h"

struct PaintSession;
namespace OpenRCT2::PaintScripting
{
    class PsVehicle;
    class PsPaintSession
    {
    public:
        PsPaintSession();
        PsPaintSession(PaintSession& paintSession);

        static void Register(sol::state& lua);

        PaintSession& GetPaintSession();
    private:
        PaintSession* _paintSession;
        std::vector<PsImageId> _trackColours;
        PsCoordsXY _mapPosition;
        uint8_t _currentRotation;

        PsImageId& GetTrackColour(uint8_t index);

        void SetAsActiveEntity(PsVehicle& vehicle);
    };
}
