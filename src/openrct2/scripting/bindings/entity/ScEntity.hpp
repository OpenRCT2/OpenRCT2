/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
#    include "../../../peep/Peep.h"
#    include "../../../peep/Staff.h"
#    include "../../../util/Util.h"
#    include "../../../world/EntityList.h"
#    include "../../../world/Litter.h"
#    include "../../../world/Sprite.h"
#    include "../../Duktape.hpp"
#    include "../../ScriptEngine.h"
#    include "../ride/ScRide.hpp"

#    include <algorithm>
#    include <string_view>
#    include <unordered_map>

namespace OpenRCT2::Scripting
{
    class ScEntity
    {
    protected:
        uint16_t _id = SPRITE_INDEX_NULL;

    public:
        ScEntity(uint16_t id)
            : _id(id)
        {
        }

    private:
        int32_t id_get() const
        {
            auto entity = GetEntity();
            return entity != nullptr ? entity->sprite_index : 0;
        }

        std::string type_get() const
        {
            const auto targetApiVersion = GetTargetAPIVersion();

            auto entity = GetEntity();
            if (entity != nullptr)
            {
                switch (entity->Type)
                {
                    case EntityType::Vehicle:
                        return "car";
                    case EntityType::Guest:
                        if (targetApiVersion <= API_VERSION_33_PEEP_DEPRECATION)
                            return "peep";
                        else
                            return "guest";
                    case EntityType::Staff:
                        if (targetApiVersion <= API_VERSION_33_PEEP_DEPRECATION)
                            return "peep";
                        else
                            return "staff";
                    case EntityType::SteamParticle:
                        return "steam_particle";
                    case EntityType::MoneyEffect:
                        return "money_effect";
                    case EntityType::CrashedVehicleParticle:
                        return "crashed_vehicle_particle";
                    case EntityType::ExplosionCloud:
                        return "explosion_cloud";
                    case EntityType::CrashSplash:
                        return "crash_splash";
                    case EntityType::ExplosionFlare:
                        return "explosion_flare";
                    case EntityType::Balloon:
                        return "balloon";
                    case EntityType::Duck:
                        return "duck";
                    case EntityType::JumpingFountain:
                        return "jumping_fountain";
                    case EntityType::Litter:
                        return "litter";
                    case EntityType::Null:
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
                        sprite_remove(entity);
                        break;
                    case EntityType::Null:
                        break;
                    default:
                        break;
                }
            }
        }

        SpriteBase* GetEntity() const
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

    static const DukEnumMap<Litter::Type> LitterTypeMap({
        { "vomit", Litter::Type::Vomit },
        { "vomit_alt", Litter::Type::VomitAlt },
        { "empty_can", Litter::Type::EmptyCan },
        { "rubbish", Litter::Type::Rubbish },
        { "burger_box", Litter::Type::BurgerBox },
        { "empty_cup", Litter::Type::EmptyCup },
        { "empty_box", Litter::Type::EmptyBox },
        { "empty_bottle", Litter::Type::EmptyBottle },
        { "empty_bowl_red", Litter::Type::EmptyBowlRed },
        { "empty_drink_carton", Litter::Type::EmptyDrinkCarton },
        { "empty_juice_cup", Litter::Type::EmptyJuiceCup },
        { "empty_bowl_blue", Litter::Type::EmptyBowlBlue },
    });

    class ScLitter : public ScEntity
    {
    public:
        ScLitter(uint16_t Id)
            : ScEntity(Id)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_set_base_class<ScEntity, ScLitter>(ctx);
            dukglue_register_property(ctx, &ScLitter::litterType_get, &ScLitter::litterType_set, "litterType");
            dukglue_register_property(ctx, &ScLitter::creationTick_get, nullptr, "creationTick");
        }

    private:
        Litter* GetLitter() const
        {
            return ::GetEntity<Litter>(_id);
        }

        std::string litterType_get() const
        {
            auto* litter = GetLitter();
            auto it = LitterTypeMap.find(litter->SubType);
            if (it == LitterTypeMap.end())
                return "";
            return std::string{ it->first };
        }

        void litterType_set(const std::string& litterType)
        {
            ThrowIfGameStateNotMutable();

            auto it = LitterTypeMap.find(litterType);
            if (it == LitterTypeMap.end())
                return;
            auto* litter = GetLitter();
            litter->SubType = it->second;
        }

        uint32_t creationTick_get() const
        {
            auto* litter = GetLitter();
            if (litter == nullptr)
                return 0;
            return litter->creationTick;
        }
    };

} // namespace OpenRCT2::Scripting

#endif
