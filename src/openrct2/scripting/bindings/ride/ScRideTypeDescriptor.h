/*****************************************************************************
 * Copyright (c) 2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

    #include "../../../ride/RideTypes.h"
    #include "../../ScriptEngine.h"

    #include <cstdint>

namespace OpenRCT2::Scripting
{
    class ScRideTypeDescriptor;
    extern ScRideTypeDescriptor gScRideTypeDescriptor;

    class ScRideTypeDescriptor final : public ScBase
    {
    private:
        struct RideTypeDescriptorData
        {
            ride_type_t _rideType;
        };

    public:
        void Register(JSContext* ctx);
        JSValue New(JSContext* ctx, ride_type_t rideType);

    private:
        static void Finalize(JSRuntime* rt, JSValue thisVal);
        static RideTypeDescriptorData* GetRideTypeDescriptorData(JSValue thisVal);

        static JSValue rideType_get(JSContext* ctx, JSValue thisVal);
        static JSValue name_get(JSContext* ctx, JSValue thisVal);
        static JSValue category_get(JSContext* ctx, JSValue thisVal);
        static JSValue startTrackSegment_get(JSContext* ctx, JSValue thisVal);
        static JSValue standardTrackSegments_get(JSContext* ctx, JSValue thisVal);
        static JSValue extraTrackSegments_get(JSContext* ctx, JSValue thisVal);
        static JSValue standardTrackGroups_get(JSContext* ctx, JSValue thisVal);
        static JSValue extraTrackGroups_get(JSContext* ctx, JSValue thisVal);
    };

} // namespace OpenRCT2::Scripting

#endif
