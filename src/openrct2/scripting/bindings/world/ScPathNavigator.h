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

    #include "../../ScriptEngine.h"

    #include <cstdint>

namespace OpenRCT2::Scripting
{
    class ScPathNavigator;
    extern ScPathNavigator gScPathNavigator;

    class ScPathNavigator final : public ScBase
    {
    private:
        struct PathNavigatorData
        {
            TileCoordsXYZ _position;
        };

    public:
        static JSValue FromElement(JSContext* ctx, const CoordsXY& position, int32_t elementIndex);
        static JSValue FromPosition(JSContext* ctx, const CoordsXYZ& position);
        void Register(JSContext* ctx);
        JSValue New(JSContext* ctx, const TileCoordsXYZ& position);

    private:
        static void Finalize(JSRuntime* rt, JSValue thisVal);
        static PathNavigatorData* GetPathNavigatorData(JSValue thisVal);

        static const PathElement* FindPathElement(const TileCoordsXYZ& pos);
        static int32_t GetPermittedEdges(const PathElement* pathElement);

        static JSValue position_get(JSContext* ctx, JSValue thisVal);
        static JSValue edges_get(JSContext* ctx, JSValue thisVal);
        static JSValue permittedEdges_get(JSContext* ctx, JSValue thisVal);
        static JSValue isSloped_get(JSContext* ctx, JSValue thisVal);
        static JSValue slopeDirection_get(JSContext* ctx, JSValue thisVal);
        static JSValue isQueue_get(JSContext* ctx, JSValue thisVal);
        static JSValue isWide_get(JSContext* ctx, JSValue thisVal);
        static JSValue ride_get(JSContext* ctx, JSValue thisVal);
        static JSValue station_get(JSContext* ctx, JSValue thisVal);

        static JSValue getConnectedPaths(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);
        static JSValue moveTo(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);
    };

} // namespace OpenRCT2::Scripting

#endif
