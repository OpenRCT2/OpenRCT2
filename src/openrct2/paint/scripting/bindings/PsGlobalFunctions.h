#pragma once

#include "../../../scripting/Duktape.hpp"
#include "../../../world/Location.hpp"

namespace OpenRCT2::PaintScripting
{
    class PsPaintSession;
    class PsImageId;
    namespace PsGlobalFunctions
    {
        void Register(duk_context* context);
        void WoodenASupportsPaintSetup(
            PsPaintSession* session, int32_t supportType, int32_t special, int32_t height, PsImageId* imageId);
        bool IsEqual(int32_t val1, int32_t val2);
        bool TestSession(PsPaintSession* session);
    }; // namespace PsGlobalFunctions
} // namespace OpenRCT2::PaintScripting
