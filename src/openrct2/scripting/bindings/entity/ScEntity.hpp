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

    #include "../../../Context.h"
    #include "../../../GameState.h"
    #include "../../../entity/EntityRegistry.h"
    #include "../../../entity/Peep.h"
    #include "../../Duktape.hpp"
    #include "../../ScriptEngine.h"

    #include <string_view>
    #include <unordered_map>

namespace OpenRCT2::Scripting
{
    class ScEntity
    {
    protected:
        EntityId _id{ EntityId::GetNull() };

    public:
        ScEntity(EntityId id)
            : _id(id)
        {
        }

    private:
        DukValue id_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();

            auto entity = GetEntity();
            if (entity == nullptr)
                return ToDuk(ctx, nullptr);

            return ToDuk(ctx, entity->Id.ToUnderlying());
        }

        std::string type_get() const
        {
            const auto targetApiVersion = GetTargetAPIVersion();

            auto entity = GetEntity();
            if (entity != nullptr)
            {
                switch (entity->Type)
                {
                    case EntityType::vehicle:
                        return "car";
                    case EntityType::guest:
                        if (targetApiVersion <= kApiVersionPeepDeprecation)
                            return "peep";
                        return "guest";
                    case EntityType::staff:
                        if (targetApiVersion <= kApiVersionPeepDeprecation)
                            return "peep";
                        return "staff";
                    case EntityType::steamParticle:
                        return "steam_particle";
                    case EntityType::moneyEffect:
                        return "money_effect";
                    case EntityType::crashedVehicleParticle:
                        return "crashed_vehicle_particle";
                    case EntityType::explosionCloud:
                        return "explosion_cloud";
                    case EntityType::crashSplash:
                        return "crash_splash";
                    case EntityType::explosionFlare:
                        return "explosion_flare";
                    case EntityType::balloon:
                        return "balloon";
                    case EntityType::duck:
                        return "duck";
                    case EntityType::jumpingFountain:
                        return "jumping_fountain";
                    case EntityType::litter:
                        return "litter";
                    case EntityType::null:
                        return "unknown";
                    default:
                        break;
                }
            }
            return "unknown";
        }

        // x getter and setter
        int32_t x_get() const
        {
            auto entity = GetEntity();
            return entity != nullptr ? entity->x : 0;
        }
        void x_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto entity = GetEntity();
            if (entity != nullptr)
            {
                entity->MoveTo({ value, entity->y, entity->z });
            }
        }

        // y getter and setter
        int32_t y_get() const
        {
            auto entity = GetEntity();
            return entity != nullptr ? entity->y : 0;
        }
        void y_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto entity = GetEntity();
            if (entity != nullptr)
            {
                entity->MoveTo({ entity->x, value, entity->z });
            }
        }

        // z getter and setter
        int16_t z_get() const
        {
            auto entity = GetEntity();
            return entity != nullptr ? entity->z : 0;
        }
        void z_set(int16_t value)
        {
            ThrowIfGameStateNotMutable();
            auto entity = GetEntity();
            if (entity != nullptr)
            {
                entity->MoveTo({ entity->x, entity->y, value });
            }
        }

        void remove()
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto entity = GetEntity();
            if (entity != nullptr)
            {
                entity->Invalidate();
                switch (entity->Type)
                {
                    case EntityType::vehicle:
                        duk_error(ctx, DUK_ERR_ERROR, "Removing a vehicle is currently unsupported.");
                        break;
                    case EntityType::guest:
                    case EntityType::staff:
                    {
                        auto peep = entity->As<Peep>();
                        // We can't remove a single peep from a ride at the moment as this can cause complications with the
                        // vehicle car having an unsupported peep capacity.
                        if (peep == nullptr || peep->State == PeepState::onRide || peep->State == PeepState::enteringRide)
                        {
                            duk_error(ctx, DUK_ERR_ERROR, "Removing a peep that is on a ride is currently unsupported.");
                        }
                        else
                        {
                            peep->Remove();
                        }
                        break;
                    }
                    case EntityType::steamParticle:
                    case EntityType::moneyEffect:
                    case EntityType::crashedVehicleParticle:
                    case EntityType::explosionCloud:
                    case EntityType::crashSplash:
                    case EntityType::explosionFlare:
                    case EntityType::jumpingFountain:
                    case EntityType::balloon:
                    case EntityType::duck:
                    case EntityType::litter:
                        getGameState().entities.EntityRemove(entity);
                        break;
                    case EntityType::null:
                        break;
                    default:
                        break;
                }
            }
        }

        EntityBase* GetEntity() const
        {
            return getGameState().entities.GetEntity(_id);
        }

    public:
        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScEntity::id_get, nullptr, "id");
            dukglue_register_property(ctx, &ScEntity::type_get, nullptr, "type");
            dukglue_register_property(ctx, &ScEntity::x_get, &ScEntity::x_set, "x");
            dukglue_register_property(ctx, &ScEntity::y_get, &ScEntity::y_set, "y");
            dukglue_register_property(ctx, &ScEntity::z_get, &ScEntity::z_set, "z");
            dukglue_register_method(ctx, &ScEntity::remove, "remove");
        }
    };

} // namespace OpenRCT2::Scripting

#endif
