/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ScParticle.hpp"

#include "../ride/ScRide.hpp"

#ifdef ENABLE_SCRIPTING

namespace OpenRCT2::Scripting
{
    static const DukEnumMap<uint8_t> CrashParticleTypeMap({
        { "corner", 0 },
        { "rod", 1 },
        { "wheel", 2 },
        { "panel", 3 },
        { "seat", 4 },
    });

    ScCrashedVehicleParticle::ScCrashedVehicleParticle(EntityId id)
        : ScEntity(id)
    {
    }

    void ScCrashedVehicleParticle::Register(duk_context* ctx)
    {
        dukglue_set_base_class<ScEntity, ScCrashedVehicleParticle>(ctx);
        dukglue_register_property(
            ctx, &ScCrashedVehicleParticle::acceleration_get, &ScCrashedVehicleParticle::acceleration_set, "acceleration");
        dukglue_register_property(
            ctx, &ScCrashedVehicleParticle::velocity_get, &ScCrashedVehicleParticle::velocity_set, "velocity");
        dukglue_register_property(
            ctx, &ScCrashedVehicleParticle::colours_get, &ScCrashedVehicleParticle::colours_set, "colours");
        dukglue_register_property(
            ctx, &ScCrashedVehicleParticle::timeToLive_get, &ScCrashedVehicleParticle::timeToLive_set, "timeToLive");
        dukglue_register_property(
            ctx, &ScCrashedVehicleParticle::crashedSpriteBase_get, &ScCrashedVehicleParticle::crashedSpriteBase_set,
            "crashParticleType");
        dukglue_register_property(ctx, &ScCrashedVehicleParticle::frame_get, &ScCrashedVehicleParticle::frame_set, "frame");
        dukglue_register_method(ctx, &ScCrashedVehicleParticle::Launch, "launch");
    }

    VehicleCrashParticle* ScCrashedVehicleParticle::GetCrashedVehicleParticle() const
    {
        return ::GetEntity<VehicleCrashParticle>(_id);
    }

    void ScCrashedVehicleParticle::frame_set(uint8_t value)
    {
        auto entity = GetCrashedVehicleParticle();
        if (entity != nullptr)
        {
            entity->frame = std::clamp<uint16_t>(value, 0, kCrashedVehicleParticleNumberSprites - 1)
                * kCrashedVehicleParticleFrameToSprite;
        }
    }
    uint8_t ScCrashedVehicleParticle::frame_get() const
    {
        auto entity = GetCrashedVehicleParticle();
        if (entity != nullptr)
        {
            return entity->frame / kCrashedVehicleParticleFrameToSprite;
        }
        return 0;
    }

    void ScCrashedVehicleParticle::crashedSpriteBase_set(const std::string& value)
    {
        auto entity = GetCrashedVehicleParticle();
        if (entity != nullptr)
        {
            entity->crashed_sprite_base = CrashParticleTypeMap[value];
        }
    }
    std::string ScCrashedVehicleParticle::crashedSpriteBase_get() const
    {
        auto entity = GetCrashedVehicleParticle();
        if (entity != nullptr)
        {
            return std::string(CrashParticleTypeMap[entity->crashed_sprite_base]);
        }
        return {};
    }

    void ScCrashedVehicleParticle::timeToLive_set(uint16_t value)
    {
        auto entity = GetCrashedVehicleParticle();
        if (entity != nullptr)
        {
            entity->time_to_live = value;
        }
    }
    uint16_t ScCrashedVehicleParticle::timeToLive_get() const
    {
        auto entity = GetCrashedVehicleParticle();
        if (entity != nullptr)
        {
            return entity->time_to_live;
        }
        return 0;
    }

    void ScCrashedVehicleParticle::velocity_set(const DukValue& value)
    {
        auto entity = GetCrashedVehicleParticle();
        if (entity != nullptr)
        {
            auto velocity = FromDuk<CoordsXYZ>(value);
            entity->velocity_x = velocity.x;
            entity->velocity_y = velocity.y;
            entity->velocity_z = velocity.z;
        }
    }
    DukValue ScCrashedVehicleParticle::velocity_get() const
    {
        auto ctx = GetContext()->GetScriptEngine().GetContext();
        auto entity = GetCrashedVehicleParticle();
        if (entity != nullptr)
        {
            return ToDuk(ctx, CoordsXYZ(entity->velocity_x, entity->velocity_y, entity->velocity_z));
        }
        return {};
    }

    void ScCrashedVehicleParticle::acceleration_set(const DukValue& value)
    {
        auto entity = GetCrashedVehicleParticle();
        if (entity != nullptr)
        {
            auto acceleration = FromDuk<CoordsXYZ>(value);
            entity->acceleration_x = acceleration.x;
            entity->acceleration_y = acceleration.y;
            entity->acceleration_z = acceleration.z;
        }
    }
    DukValue ScCrashedVehicleParticle::acceleration_get() const
    {
        auto ctx = GetContext()->GetScriptEngine().GetContext();
        auto entity = GetCrashedVehicleParticle();
        if (entity != nullptr)
        {
            return ToDuk(ctx, CoordsXYZ(entity->acceleration_x, entity->acceleration_y, entity->acceleration_z));
        }
        return {};
    }

    void ScCrashedVehicleParticle::Launch(const DukValue& value)
    {
        auto entity = GetCrashedVehicleParticle();
        if (entity != nullptr)
        {
            entity->SetSpriteData();
            entity->Launch();

            if (value.type() == DukValue::Type::UNDEFINED)
                return;

            if (value["colours"].type() == DukValue::Type::OBJECT)
            {
                auto coloursInt = FromDuk<VehicleColour>(value["colours"]);
                entity->colour[0] = coloursInt.Body;
                entity->colour[1] = coloursInt.Trim;
            }
            if (value["acceleration"].type() == DukValue::Type::OBJECT)
            {
                auto accelerationXYZ = FromDuk<CoordsXYZ>(value["acceleration"]);
                entity->acceleration_x = accelerationXYZ.x;
                entity->acceleration_y = accelerationXYZ.y;
                entity->acceleration_z = accelerationXYZ.z;
            }
            if (value["velocity"].type() == DukValue::Type::OBJECT)
            {
                auto velocityXYZ = FromDuk<CoordsXYZ>(value["velocity"]);
                entity->velocity_x = velocityXYZ.x;
                entity->velocity_y = velocityXYZ.y;
                entity->velocity_z = velocityXYZ.z;
            }
            if (value["timeToLive"].type() == DukValue::Type::NUMBER)
            {
                entity->time_to_live = value["timeToLive"].as_uint();
            }
            if (value["frame"].type() == DukValue::Type::NUMBER)
            {
                entity->frame = std::clamp<uint16_t>(value["frame"].as_uint(), 0, kCrashedVehicleParticleNumberSprites - 1)
                    * kCrashedVehicleParticleFrameToSprite;
            }
            if (value["crashParticleType"].type() == DukValue::Type::STRING)
            {
                entity->crashed_sprite_base = CrashParticleTypeMap[value["crashParticleType"].as_string()];
            }
        }
    }

    DukValue ScCrashedVehicleParticle::colours_get() const
    {
        auto ctx = GetContext()->GetScriptEngine().GetContext();
        auto entity = GetCrashedVehicleParticle();
        if (entity != nullptr)
        {
            DukObject dukColour(ctx);
            dukColour.Set("body", entity->colour[0]);
            dukColour.Set("trim", entity->colour[1]);
            return dukColour.Take();
        }
        return ToDuk(ctx, nullptr);
    }
    void ScCrashedVehicleParticle::colours_set(const DukValue& value)
    {
        auto entity = GetCrashedVehicleParticle();
        if (entity != nullptr)
        {
            auto colours = FromDuk<VehicleColour>(value);
            entity->colour[0] = colours.Body;
            entity->colour[1] = colours.Trim;
        }
    }
}; // namespace OpenRCT2::Scripting

#endif
