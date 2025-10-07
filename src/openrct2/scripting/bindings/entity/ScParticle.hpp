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

    #include "../../../entity/Particle.h"
    #include "../../../world/Location.hpp"
    #include "ScEntity.hpp"

    #include <optional>

namespace OpenRCT2::Scripting
{
    class ScCrashedVehicleParticle final : public ScEntity
    {
    public:
        static JSValue New(JSContext* ctx, EntityId entityId);

    private:
        static void AddFuncs(JSContext* ctx, JSValue obj);
        static VehicleCrashParticle* GetCrashedVehicleParticle(JSValue thisVal);

        static JSValue colours_get(JSContext* ctx, JSValue thisVal);
        static JSValue colours_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue acceleration_get(JSContext* ctx, JSValue thisVal);
        static JSValue acceleration_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue velocity_get(JSContext* ctx, JSValue thisVal);
        static JSValue velocity_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue frame_get(JSContext* ctx, JSValue thisVal);
        static JSValue frame_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue crashedSpriteBase_get(JSContext* ctx, JSValue thisVal);
        static JSValue crashedSpriteBase_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue timeToLive_get(JSContext* ctx, JSValue thisVal);
        static JSValue timeToLive_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue Launch(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);
    };
} // namespace OpenRCT2::Scripting

#endif
