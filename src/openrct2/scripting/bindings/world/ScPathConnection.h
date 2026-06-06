/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

    #include "../../../world/Location.hpp"
    #include "../../ScriptEngine.h"

    #include <cstdint>

namespace OpenRCT2::Scripting
{
    class ScPathConnection;
    extern ScPathConnection gScPathConnection;

    class ScPathConnection final : public ScBase
    {
    private:
        struct PathConnectionData
        {
            TileCoordsXYZ position;
            int32_t elementIndex;
            int32_t direction; // -1 if none (e.g. starting tile)
        };

    public:
        void registerClass(JSContext* ctx);
        JSValue create(JSContext* ctx, const TileCoordsXYZ& position, int32_t elementIndex, int32_t direction);

    private:
        static void finalize(JSRuntime* rt, JSValue thisVal);
        static PathConnectionData* getData(JSValue thisVal);
        static const PathElement* findPathElement(const PathConnectionData* data);

        static JSValue getPosition(JSContext* ctx, JSValue thisVal);
        static JSValue getDirection(JSContext* ctx, JSValue thisVal);
        static JSValue getElementIndex(JSContext* ctx, JSValue thisVal);
        static JSValue isSloped(JSContext* ctx, JSValue thisVal);
        static JSValue getSlopeDirection(JSContext* ctx, JSValue thisVal);
        static JSValue isQueue(JSContext* ctx, JSValue thisVal);
        static JSValue isWide(JSContext* ctx, JSValue thisVal);
        static JSValue getRide(JSContext* ctx, JSValue thisVal);
        static JSValue getStation(JSContext* ctx, JSValue thisVal);
    };

} // namespace OpenRCT2::Scripting

#endif
