/*****************************************************************************
 * Copyright (c) 2022 OpenRCT2 developers
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
    #include <string>

namespace OpenRCT2::Scripting
{
    class ScTrackSegment;
    extern ScTrackSegment gScTrackSegment;

    class ScTrackSegment final : public ScBase
    {
    private:
        struct TrackSegmentData
        {
            TrackElemType _type;
        };

    public:
        void Register(JSContext* ctx);
        JSValue New(JSContext* ctx, TrackElemType type);

    private:
        static void Finalize(JSRuntime* rt, JSValue thisVal);
        static TrackSegmentData* GetTrackSegmentData(JSValue thisVal);

        static JSValue type_get(JSContext* ctx, JSValue thisVal);
        static JSValue description_get(JSContext* ctx, JSValue thisVal);
        static JSValue beginZ_get(JSContext* ctx, JSValue thisVal);
        static JSValue beginDirection_get(JSContext* ctx, JSValue thisVal);
        static JSValue beginSlope_get(JSContext* ctx, JSValue thisVal);
        static JSValue beginBank_get(JSContext* ctx, JSValue thisVal);
        static JSValue endX_get(JSContext* ctx, JSValue thisVal);
        static JSValue endY_get(JSContext* ctx, JSValue thisVal);
        static JSValue endZ_get(JSContext* ctx, JSValue thisVal);
        static JSValue endDirection_get(JSContext* ctx, JSValue thisVal);
        static JSValue endSlope_get(JSContext* ctx, JSValue thisVal);
        static JSValue endBank_get(JSContext* ctx, JSValue thisVal);
        static JSValue length_get(JSContext* ctx, JSValue thisVal);
        static JSValue elements_get(JSContext* ctx, JSValue thisVal);
        static JSValue getSubpositionLength(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);
        static JSValue getSubpositions(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);
        static JSValue nextCurveElement_get(JSContext* ctx, JSValue thisVal);
        static JSValue previousCurveElement_get(JSContext* ctx, JSValue thisVal);
        static JSValue getMirrorElement(JSContext* ctx, JSValue thisVal);
        static JSValue getAlternativeElement(JSContext* ctx, JSValue thisVal);
        static JSValue getPriceModifier(JSContext* ctx, JSValue thisVal);
        static JSValue getTrackGroup(JSContext* ctx, JSValue thisVal);
        template<TrackElementFlag flag>
        static JSValue getTrackFlag(JSContext* ctx, JSValue thisVal);
        static JSValue getTrackCurvature(JSContext* ctx, JSValue thisVal);
        static JSValue getTrackPitchDirection(JSContext* ctx, JSValue thisVal);
    };

} // namespace OpenRCT2::Scripting

#endif
