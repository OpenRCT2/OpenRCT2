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
        bool respectBanners = false;
        bool includeGhosts = false;
        bool includeQueues = false;
        bool includeWidePaths = false;
    };

    class ScPathNavigator final : public ScBase
    {
    private:
        struct PathNavigatorData
        {
            TileCoordsXYZ position;
            int32_t elementIndex;
            int32_t lastDirection; // -1 if navigator has not moved yet
            PathNavigationOptions options;
        };

    public:
        static JSValue fromElement(
            JSContext* ctx, const CoordsXY& position, int32_t elementIndex, const PathNavigationOptions& options);
        static JSValue fromPosition(JSContext* ctx, const CoordsXYZ& position, const PathNavigationOptions& options);
        void registerClass(JSContext* ctx);
        JSValue create(
            JSContext* ctx, const TileCoordsXYZ& position, int32_t elementIndex, int32_t lastDirection,
            const PathNavigationOptions& options);

    private:
        static void finalize(JSRuntime* rt, JSValue thisVal);
        static PathNavigatorData* getPathNavigatorData(JSValue thisVal);

        static const PathElement* findPathElement(const PathNavigatorData* data);
        static int32_t computePermittedEdges(const PathElement* pathElement, const PathNavigationOptions& options);
        static bool isTraversableNeighbor(const PathElement* pathElement, const PathNavigationOptions& options);

        static JSValue getCurrent(JSContext* ctx, JSValue thisVal);
        static JSValue getEdges(JSContext* ctx, JSValue thisVal);
        static JSValue getPermittedEdges(JSContext* ctx, JSValue thisVal);

        static JSValue getConnectedPaths(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);
        static JSValue moveTo(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);
    };

} // namespace OpenRCT2::Scripting

#endif
