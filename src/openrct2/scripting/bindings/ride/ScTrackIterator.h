/*****************************************************************************
 * Copyright (c) 2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "../../../Identifiers.h"
    #include "../../ScriptEngine.h"

    #include <cstdint>

namespace OpenRCT2::Scripting
{
    class ScTrackIterator;
    extern ScTrackIterator gScTrackIterator;

    class ScTrackIterator final : public ScBase
    {
    private:
        struct TrackIteratorData
        {
            CoordsXYZD _position;
            TrackElemType _type;
        };

    public:
        static JSValue FromElement(JSContext* ctx, const CoordsXY& position, int32_t elementIndex);
        void Register(JSContext* ctx);
        JSValue New(JSContext* ctx, const CoordsXYZD& position, TrackElemType type);

    private:
        static void Finalize(JSRuntime* rt, JSValue thisVal);
        static TrackIteratorData* GetTrackIteratorData(JSValue thisVal);

        static JSValue position_get(JSContext* ctx, JSValue thisVal);
        static JSValue segment_get(JSContext* ctx, JSValue thisVal);
        static JSValue previousPosition_get(JSContext* ctx, JSValue thisVal);
        static JSValue nextPosition_get(JSContext* ctx, JSValue thisVal);

        static JSValue previous(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);
        static JSValue next(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);
    };

} // namespace OpenRCT2::Scripting

#endif
