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
    class ScPathNavigator;
    extern ScPathNavigator gScPathNavigator;

    struct PathNavigationOptions
    {
        bool RespectBanners = false;
        bool IncludeGhosts = false;
        bool IncludeQueues = false;
        bool IncludeWidePaths = false;
    };

    class ScPathNavigator final : public ScBase
    {
    private:
        struct PathNavigatorData
        {
            TileCoordsXYZ _position;
            int32_t _elementIndex;
            int32_t _lastDirection; // -1 if navigator has not moved yet
            PathNavigationOptions _options;
        };

    public:
        static JSValue FromElement(
            JSContext* ctx, const CoordsXY& position, int32_t elementIndex, const PathNavigationOptions& options);
        static JSValue FromPosition(JSContext* ctx, const CoordsXYZ& position, const PathNavigationOptions& options);
        void Register(JSContext* ctx);
        JSValue New(
            JSContext* ctx, const TileCoordsXYZ& position, int32_t elementIndex, int32_t lastDirection,
            const PathNavigationOptions& options);

    private:
        static void Finalize(JSRuntime* rt, JSValue thisVal);
        static PathNavigatorData* GetPathNavigatorData(JSValue thisVal);

        static const PathElement* FindPathElement(const PathNavigatorData* data);
        static int32_t GetPermittedEdges(const PathElement* pathElement, const PathNavigationOptions& options);
        static bool IsTraversableNeighbor(const PathElement* pathElement, const PathNavigationOptions& options);

        static JSValue current_get(JSContext* ctx, JSValue thisVal);
        static JSValue edges_get(JSContext* ctx, JSValue thisVal);
        static JSValue permittedEdges_get(JSContext* ctx, JSValue thisVal);

        static JSValue getConnectedPaths(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);
        static JSValue moveTo(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);
    };

} // namespace OpenRCT2::Scripting

#endif
