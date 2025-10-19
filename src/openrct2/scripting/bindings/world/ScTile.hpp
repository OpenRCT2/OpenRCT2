/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "ScTileElement.hpp"

    #include <cstdio>
    #include <cstring>
    #include <memory>
    #include <utility>
    #include <vector>

namespace OpenRCT2::Scripting
{
    class ScTile;
    extern ScTile gScTile;

    class ScTile : public ScBase
    {
    private:
        static JSValue x_get(JSContext* ctx, JSValue thisValue);

        static JSValue y_get(JSContext* ctx, JSValue thisValue);

        static JSValue numElements_get(JSContext* ctx, JSValue thisValue);

        static JSValue elements_get(JSContext* ctx, JSValue thisValue);

        static JSValue data_get(JSContext* ctx, JSValue thisValue);
        static JSValue data_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue getElement(JSContext* ctx, JSValue thisValue, int argc, JSValue* argv);
        static JSValue insertElement(JSContext* ctx, JSValue thisValue, int argc, JSValue* argv);

        static JSValue removeElement(JSContext* ctx, JSValue thisValue, int argc, JSValue* argv);

        static CoordsXY GetCoordinates(JSValue thisValue);
        static TileElement* GetFirstElement(JSValue thisValue);

        static uint32_t GetNumElements(const TileElement* first);

    public:
        JSValue New(JSContext* ctx, CoordsXY& coords);
        void Register(JSContext* ctx);

    private:
        static void Finalize(JSRuntime* rt, JSValue thisValue);
    };
} // namespace OpenRCT2::Scripting

#endif
