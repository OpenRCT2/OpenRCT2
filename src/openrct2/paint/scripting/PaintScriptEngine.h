#pragma once

#include "../../scripting/Duktape.hpp"
#include "../../scripting/ScriptEngine.h"

#include <memory>
#include <vector>
#include <mutex>

struct Ride;
class PaintObject;
namespace OpenRCT2::PaintScripting
{
    class PaintScriptEngine
    {
    public:
        PaintScriptEngine();

        void Initialize();

        void CallScript(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height, const TrackElement& trackElement,
            PaintObject* paintObject);

    private:
        //it seems we need a mutex for multithreading
        std::mutex _mutex;
    };

} // namespace OpenRCT2::PaintScripting
