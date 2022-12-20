#pragma once

#include "../../../world/Location.hpp"
#include "../../../scripting/Duktape.hpp"
#include "PsPaintSession.h"
#include "PsRide.h"
#include "PsTrackElement.h"
#include <memory>

struct Ride;
struct TrackElement;
struct PaintSession;

namespace OpenRCT2::PaintScripting
{
    class PsPaintObject
    {
    public:
        PsPaintObject();

        static void Register(duk_context* context);
        static PsPaintObject PaintObject;
        static void Update(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height, const TrackElement& trackElement);

    private:
        static int32_t _height;
        static uint8_t _direction;
        static uint8_t _trackSequence;
        static PsPaintSession _paintSession;
        static PsRide _ride;
        static PsTrackElement _trackElement;

        int32_t getHeight() const;
        uint8_t getDirection() const;
        uint8_t getTrackSequence() const;
        std::shared_ptr<PsPaintSession> getPaintSession() const;
        std::shared_ptr<PsRide> getRide() const;
        std::shared_ptr<PsTrackElement> getTrackElement() const;
        //const Ride& _ride;
        //const TrackElement& _trackElement;
    };
}

