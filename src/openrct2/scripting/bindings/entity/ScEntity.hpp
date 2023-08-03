/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../../../Context.h"
#    include "../../../common.h"
#    include "../../../entity/EntityList.h"
#    include "../../../entity/EntityRegistry.h"
#    include "../../../entity/Peep.h"
#    include "../../../util/Util.h"
#    include "../../Duktape.hpp"
#    include "../../ScriptEngine.h"

#    include <algorithm>
#    include <string_view>
#    include <unordered_map>

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
                if (targetApiVersion <= API_VERSION_33_PEEP_DEPRECATION
                    && (entity->Type == EntityType::Guest || entity->Type == EntityType::Staff))
                {
                    return "peep";
                }
                return GetEntityTypeName(entity->Type);
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
                    case EntityType::Vehicle:
                        duk_error(ctx, DUK_ERR_ERROR, "Removing a vehicle is currently unsupported.");
                        break;
                    case EntityType::Guest:
                    case EntityType::Staff:
                    {
                        auto peep = entity->As<Peep>();
                        // We can't remove a single peep from a ride at the moment as this can cause complications with the
                        // vehicle car having an unsupported peep capacity.
                        if (peep == nullptr || peep->State == PeepState::OnRide || peep->State == PeepState::EnteringRide)
                        {
                            duk_error(ctx, DUK_ERR_ERROR, "Removing a peep that is on a ride is currently unsupported.");
                        }
                        else
                        {
                            peep->Remove();
                        }
                        break;
                    }
                    case EntityType::SteamParticle:
                    case EntityType::MoneyEffect:
                    case EntityType::CrashedVehicleParticle:
                    case EntityType::ExplosionCloud:
                    case EntityType::CrashSplash:
                    case EntityType::ExplosionFlare:
                    case EntityType::JumpingFountain:
                    case EntityType::Balloon:
                    case EntityType::Duck:
                    case EntityType::Litter:
                        EntityRemove(entity);
                        break;
                    case EntityType::Null:
                        break;
                    default:
                        break;
                }
            }
        }

        EntityBase* GetEntity() const
        {
            return ::GetEntity(_id);
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
