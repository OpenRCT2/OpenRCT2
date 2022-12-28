#pragma once

#include <sol/sol.hpp>
#include <memory>
#include "PsImageId.h"

struct PaintSession;
namespace OpenRCT2::PaintScripting
{
    class PsPaintSession
    {
    public:
        PsPaintSession();
        PsPaintSession(PaintSession& paintSession);
        void Update(PaintSession& paintSession);

        static void Register(sol::state& lua);

        PaintSession& GetPaintSession();
    private:
        PaintSession* _paintSession;
        std::vector<PsImageId> _trackColours;
        PsImageId& GetTrackColour(uint8_t index);
    };
}
