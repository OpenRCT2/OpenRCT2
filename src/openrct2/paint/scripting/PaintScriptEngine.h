#pragma once

#include "../../scripting/Duktape.hpp"
#include "../../scripting/ScriptEngine.h"

#include <memory>
#include <vector>

struct Ride;
class PaintObject;
namespace OpenRCT2::PaintScripting
{
    class PaintScriptEngine
    {
    public:
        PaintScriptEngine();

        void Initialize();

        uint32_t AddScript(const std::string& scriptContent);

        void CallScript(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height, const TrackElement& trackElement,
            PaintObject* paintObject);

    private:
        OpenRCT2::Scripting::DukContext _context;
        uint32_t _scriptsCount;

        /*the stack is organized like this:
        *
        * Script functions
        *
        * 
        * 
        */
    };

} // namespace OpenRCT2::PaintScripting
