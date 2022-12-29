#pragma once

#include "../../drawing/ImageId.hpp"
#include "../../world/Location.hpp"
#include "bindings/PsImageId.h"
#include "bindings/PsPaint.h"
#include "bindings/PsPaintSession.h"

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
        ~PaintScriptEngine();

        void Initialize();

        void CallScript(
            PaintSession& session, int32_t trackSequence, Direction direction, int32_t height, const TrackElement& trackElement,
            Ride& ride, PaintObject& paintObject);

        int LoadScript(const std::string& script);

    private:
        // it seems we need a mutex for multithreading
        std::mutex _mutex;
        sol::state _lua;

        std::vector<sol::protected_function> _scripts;

        void SetPaintSession(PaintSession& session);
        void SetRide(Ride& session);
        void SetTrackSequence(uint8_t trackSequence);
        void SetDirection(uint8_t direction);
        void SetHeight(int32_t height);
        void SetTrackElement(const TrackElement& trackElement);
    };

} // namespace OpenRCT2::PaintScripting
