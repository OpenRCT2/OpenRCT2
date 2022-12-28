#pragma once
#include <sol/sol.hpp>
#include <memory>

namespace OpenRCT2::PaintScripting
{
    class PsPaintSession;
    class PsImageId;
    namespace PsGlobalFunctions
    {
        void Register(sol::state& lua);
    }
}
