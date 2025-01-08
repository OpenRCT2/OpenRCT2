/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

    #include "../../../entity/Particle.h"
    #include "../../../world/Location.hpp"
    #include "ScEntity.hpp"

    #include <optional>

namespace OpenRCT2::Scripting
{
    class ScCrashedVehicleParticle : public ScEntity
    {
    public:
        ScCrashedVehicleParticle(EntityId id);

        static void Register(duk_context* ctx);

    private:
        VehicleCrashParticle* GetCrashedVehicleParticle() const;

        DukValue colours_get() const;
        void colours_set(const DukValue& value);

        DukValue acceleration_get() const;
        void acceleration_set(const DukValue& value);

        DukValue velocity_get() const;
        void velocity_set(const DukValue& value);

        uint8_t frame_get() const;
        void frame_set(uint8_t value);

        void crashedSpriteBase_set(const std::string& value);
        std::string crashedSpriteBase_get() const;

        void timeToLive_set(uint16_t value);
        uint16_t timeToLive_get() const;

        void Launch(const DukValue& value);
    };
}; // namespace OpenRCT2::Scripting

#endif
