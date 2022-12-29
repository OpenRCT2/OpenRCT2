#pragma once

#include "../../drawing/ImageId.hpp"
#include "../../world/Location.hpp"
#include "bindings/PsImageId.h"
#include "bindings/PsPaintSession.h"
#include "bindings/PsPaint.h"

#include <memory>
#include <mutex>
#include <sol/sol.hpp>
#include <vector>

struct Ride;
class PaintObject;
struct PaintSession;
struct TrackElement;
namespace OpenRCT2::PaintScripting
{
    class PsPaintSession;
    class PaintScriptEngine
    {
    public:
        PaintScriptEngine();

        void Initialize();

        void CallScript(PaintObject& paintObject);

        sol::load_result LoadScript(const std::string& script);

        void SetPaintSession(PaintSession& session);
        void SetRide(Ride& session);
        void SetTrackSequence(uint8_t trackSequence);
        void SetDirection(uint8_t direction);
        void SetHeight(int32_t height);
        void SetTrackElement(const TrackElement& trackElement);
    private:
        // it seems we need a mutex for multithreading
        std::mutex _mutex;
        sol::state _lua;

        PsPaint _paint;
    };

} // namespace OpenRCT2::PaintScripting
