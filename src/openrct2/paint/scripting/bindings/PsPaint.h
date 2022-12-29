#pragma once

#include <sol/sol.hpp>
#include "PsPaintSession.h"

namespace OpenRCT2::PaintScripting
{
    class PsPaint
    {
    public:
        PsPaint();
        PsPaint(PaintSession& session, uint8_t trackSequence, uint8_t direction, int32_t height);
        void Update(PaintSession& session, uint8_t trackSequence, uint8_t direction, int32_t height);
        static void Register(sol::state& lua);

    private:
        PsPaintSession _session;
        uint8_t _trackSequence;
        uint8_t _direction;
        int32_t _height;

        const PsPaintSession& GetPaintSession() const;
        uint8_t GetTrackSequence() const;
        uint8_t GetDirection() const;
        int32_t GetHeight() const;
    };
}
