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
            TileCoordsXYZ _position;
            int32_t _elementIndex;
            int32_t _direction; // -1 if none (e.g. starting tile)
        };

    public:
        void Register(JSContext* ctx);
        JSValue New(JSContext* ctx, const TileCoordsXYZ& position, int32_t elementIndex, int32_t direction);

    private:
        static void Finalize(JSRuntime* rt, JSValue thisVal);
        static PathConnectionData* GetData(JSValue thisVal);
        static const PathElement* FindPathElement(const PathConnectionData* data);

        static JSValue position_get(JSContext* ctx, JSValue thisVal);
        static JSValue direction_get(JSContext* ctx, JSValue thisVal);
        static JSValue elementIndex_get(JSContext* ctx, JSValue thisVal);
        static JSValue isSloped_get(JSContext* ctx, JSValue thisVal);
        static JSValue slopeDirection_get(JSContext* ctx, JSValue thisVal);
        static JSValue isQueue_get(JSContext* ctx, JSValue thisVal);
        static JSValue isWide_get(JSContext* ctx, JSValue thisVal);
        static JSValue ride_get(JSContext* ctx, JSValue thisVal);
        static JSValue station_get(JSContext* ctx, JSValue thisVal);
    };

} // namespace OpenRCT2::Scripting

#endif
