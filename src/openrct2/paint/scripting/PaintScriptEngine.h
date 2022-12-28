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

        void CallScript(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
            const TrackElement& trackElement, PaintObject* paintObject);

    private:
        // it seems we need a mutex for multithreading
        std::mutex _mutex;
        sol::state _lua;

        PsPaint _paint;
    };

} // namespace OpenRCT2::PaintScripting
