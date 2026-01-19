/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "EntityRegistry.h"

#include "../Diagnostic.h"
#include "../Game.h"
#include "../GameState.h"
#include "../core/Algorithm.hpp"
#include "../core/ChecksumStream.h"
#include "../core/Crypt.h"
#include "../core/DataSerialiser.h"
#include "../core/Guard.hpp"
#include "../core/MemoryStream.h"
#include "../core/String.hpp"
#include "../entity/EntityList.h"
#include "../entity/Peep.h"
#include "../entity/Staff.h"
#include "../interface/Viewport.h"
#include "../peep/RideUseSystem.h"
#include "../profiling/Profiling.h"
#include "../ride/Vehicle.h"
#include "../world/Map.h"
#include "Balloon.h"
#include "Duck.h"
#include "EntityTweener.h"
#include "Fountain.h"
#include "MoneyEffect.h"
#include "Particle.h"

#include <cassert>
#include <cmath>
#include <iterator>
#include <numeric>
#include <vector>

namespace OpenRCT2
{
    using namespace OpenRCT2::Core;

    static constexpr uint32_t ComputeSpatialIndex(const CoordsXY& loc)
    {
        if (loc.IsNull())
            return kSpatialIndexNullBucket;

        // NOTE: The input coordinate is rotated and can have negative components.
        const auto tileX = std::abs(loc.x) / kCoordsXYStep;
        const auto tileY = std::abs(loc.y) / kCoordsXYStep;

        if (tileX >= kMaximumMapSizeTechnical || tileY >= kMaximumMapSizeTechnical)
            return kSpatialIndexNullBucket;

        return tileX * kMaximumMapSizeTechnical + tileY;
    }

    static constexpr uint32_t GetSpatialIndex(EntityBase& entity)
    {
        return entity.SpatialIndex & ~kSpatialIndexDirtyMask;
    }

    static constexpr bool EntityTypeIsMiscEntity(const EntityType type)
    {
        switch (type)
        {
            case EntityType::steamParticle:
            case EntityType::moneyEffect:
            case EntityType::crashedVehicleParticle:
            case EntityType::explosionCloud:
            case EntityType::crashSplash:
            case EntityType::explosionFlare:
            case EntityType::jumpingFountain:
            case EntityType::balloon:
            case EntityType::duck:
                return true;
            default:
                return false;
        }
    }

    // TODO: make part of EntityList unit?
    uint16_t EntityRegistry::GetEntityListCount(EntityType type)
    {
        return static_cast<uint16_t>(gEntityLists[EnumValue(type)].size());
    }

    // TODO: make part of EntityList unit?
    uint16_t EntityRegistry::GetNumFreeEntities()
    {
        return static_cast<uint16_t>(_freeIdList.size());
    }

    std::string EntitiesChecksum::ToString() const
    {
        return String::StringFromHex(raw);
    }

    EntityBase* EntityRegistry::TryGetEntity(EntityId entityIndex)
    {
        const auto idx = entityIndex.ToUnderlying();
        return idx >= kMaxEntities ? nullptr : &entities[idx].base;
    }

    EntityBase* EntityRegistry::GetEntity(EntityId entityIndex)
    {
        if (entityIndex.IsNull())
        {
            return nullptr;
        }
        Guard::Assert(entityIndex.ToUnderlying() < kMaxEntities, "Tried getting entity %u", entityIndex.ToUnderlying());
        return TryGetEntity(entityIndex);
    }

    const std::vector<EntityId>& EntityRegistry::GetEntityTileList(const CoordsXY& spritePos)
    {
        return gEntitySpatialIndex[ComputeSpatialIndex(spritePos)];
    }

    void EntityRegistry::ResetEntityLists()
    {
        for (auto& list : gEntityLists)
        {
            list.clear();
        }
    }

    void EntityRegistry::ResetFreeIds()
    {
        _freeIdList.clear();
        _freeIdList.resize(kMaxEntities);

        // List needs to be back to front to simplify removing
        auto nextId = 0;
        std::for_each(std::rbegin(_freeIdList), std::rend(_freeIdList), [&](auto& elem) {
            elem = EntityId::FromUnderlying(nextId);
            nextId++;
        });
    }

    const std::list<EntityId>& EntityRegistry::GetEntityList(const EntityType id)
    {
        return gEntityLists[EnumValue(id)];
    }

    /**
     *
     *  rct2: 0x0069EB13
     */
    void EntityRegistry::ResetAllEntities()
    {
        // Free all associated Entity pointers prior to zeroing memory
        for (int32_t i = 0; i < kMaxEntities; ++i)
        {
            auto* spr = GetEntity(EntityId::FromUnderlying(i));
            if (spr == nullptr)
            {
                continue;
            }
            FreeEntity(*spr);
        }

        std::fill(std::begin(entities), std::end(entities), Entity_t());
        RideUse::GetHistory().Clear();
        RideUse::GetTypeHistory().Clear();
        for (int32_t i = 0; i < kMaxEntities; ++i)
        {
            auto* spr = GetEntity(EntityId::FromUnderlying(i));
            if (spr == nullptr)
            {
                continue;
            }
            spr->Type = EntityType::null;
            spr->Id = EntityId::FromUnderlying(i);

            _entityFlashingList[i] = false;
        }
        ResetEntityLists();
        ResetFreeIds();
        ResetEntitySpatialIndices();
    }

    /**
     *
     *  rct2: 0x0069EBE4
     * This function looks as though it sets some sort of order for sprites.
     * Sprites can share their position if this is the case.
     */
    void EntityRegistry::ResetEntitySpatialIndices()
    {
        for (auto& vec : gEntitySpatialIndex)
        {
            vec.clear();
        }
        for (EntityId::UnderlyingType i = 0; i < kMaxEntities; i++)
        {
            auto* entity = GetEntity(EntityId::FromUnderlying(i));
            if (entity != nullptr && entity->Type != EntityType::null)
            {
                EntitySpatialInsert(*entity, { entity->x, entity->y });
            }
        }
    }

#ifndef DISABLE_NETWORK
    EntitiesChecksum EntityRegistry::GetAllEntitiesChecksum()
    {
        EntitiesChecksum checksum{};

        ChecksumStream ms(checksum.raw);
        DataSerialiser ds(true, ms);
        NetworkSerialiseEntityTypes<Guest, Staff, Vehicle, Litter>(ds);

        return checksum;
    }
#else
    EntitiesChecksum EntityRegistry::GetAllEntitiesChecksum()
    {
        return EntitiesChecksum{};
    }
#endif // DISABLE_NETWORK

    void EntityRegistry::EntityReset(EntityBase& entity)
    {
        // Need to retain how the sprite is linked in lists
        auto entityIndex = entity.Id;
        _entityFlashingList[entityIndex.ToUnderlying()] = false;

        Entity_t* tempEntity = reinterpret_cast<Entity_t*>(&entity);
        *tempEntity = Entity_t();

        entity.Id = entityIndex;
        entity.Type = EntityType::null;
    }

    void EntityRegistry::AddToEntityList(EntityBase& entity)
    {
        auto& list = gEntityLists[EnumValue(entity.Type)];

        // Entity list is sorted by Id to prevent desyncs.
        Algorithm::sortedInsert(list, entity.Id);
    }

    void EntityRegistry::AddToFreeList(EntityId index)
    {
        // Free list must be in reverse sprite_index order to prevent desync issues
        _freeIdList.insert(std::upper_bound(std::rbegin(_freeIdList), std::rend(_freeIdList), index).base(), index);
    }

    void EntityRegistry::RemoveFromEntityList(EntityBase& entity)
    {
        auto& list = gEntityLists[EnumValue(entity.Type)];
        auto ptr = Algorithm::binaryFind(std::begin(list), std::end(list), entity.Id);
        if (ptr != std::end(list))
        {
            list.erase(ptr);
        }
    }

    uint16_t EntityRegistry::GetMiscEntityCount()
    {
        uint16_t count = 0;
        for (auto id : { EntityType::steamParticle, EntityType::moneyEffect, EntityType::crashedVehicleParticle,
                         EntityType::explosionCloud, EntityType::crashSplash, EntityType::explosionFlare,
                         EntityType::jumpingFountain, EntityType::balloon, EntityType::duck })
        {
            count += GetEntityListCount(id);
        }
        return count;
    }

    void EntityRegistry::PrepareNewEntity(EntityBase& base, const EntityType type)
    {
        // Need to reset all sprite data, as the uninitialised values
        // may contain garbage and cause a desync later on.
        EntityReset(base);

        base.Type = type;
        AddToEntityList(base);

        base.x = kLocationNull;
        base.y = kLocationNull;
        base.z = 0;
        base.SpriteData.Width = 0x10;
        base.SpriteData.HeightMin = 0x14;
        base.SpriteData.HeightMax = 0x8;
        base.SpriteData.SpriteRect = {};
        base.SpatialIndex = kInvalidSpatialIndex;

        EntitySpatialInsert(base, { kLocationNull, 0 });
    }

    EntityBase* EntityRegistry::CreateEntity(EntityType type)
    {
        if (_freeIdList.size() == 0)
        {
            // No free sprites.
            return nullptr;
        }

        if (EntityTypeIsMiscEntity(type))
        {
            // Misc sprites are commonly used for effects, give other entity types higher priority.
            if (GetMiscEntityCount() >= kMaxMiscEntities)
            {
                return nullptr;
            }

            // If there are less than kMaxMiscEntities free slots, ensure other entities can be created.
            if (_freeIdList.size() < kMaxMiscEntities)
            {
                return nullptr;
            }
        }

        auto* entity = GetEntity(_freeIdList.back());
        if (entity == nullptr)
        {
            return nullptr;
        }
        _freeIdList.pop_back();

        PrepareNewEntity(*entity, type);

        return entity;
    }

    EntityBase* EntityRegistry::CreateEntityAt(const EntityId index, const EntityType type)
    {
        auto id = Algorithm::binaryFind(std::rbegin(_freeIdList), std::rend(_freeIdList), index);
        if (id == std::rend(_freeIdList))
        {
            return nullptr;
        }

        auto* entity = GetEntity(index);
        if (entity == nullptr)
        {
            return nullptr;
        }

        _freeIdList.erase(std::next(id).base());

        PrepareNewEntity(*entity, type);
        return entity;
    }

    /**
     *
     *  rct2: 0x00672AA4
     */
    void EntityRegistry::UpdateAllMiscEntities()
    {
        PROFILED_FUNCTION();

        MiscUpdateAllTypes<
            SteamParticle, MoneyEffect, VehicleCrashParticle, ExplosionCloud, CrashSplashParticle, ExplosionFlare,
            JumpingFountain, Balloon, Duck>();
    }

    void EntityRegistry::UpdateMoneyEffect()
    {
        MiscUpdateAllTypes<MoneyEffect>();
    }

    // Performs a search to ensure that insert keeps next_in_quadrant in sprite_index order
    void EntityRegistry::EntitySpatialInsert(EntityBase& entity, const CoordsXY& newLoc)
    {
        const auto newIndex = ComputeSpatialIndex(newLoc);

        auto& spatialVector = gEntitySpatialIndex[newIndex];

        Algorithm::sortedInsert(spatialVector, entity.Id);

        entity.SpatialIndex = newIndex;
    }

    void EntityRegistry::EntitySpatialRemove(EntityBase& entity)
    {
        const auto currentIndex = GetSpatialIndex(entity);

        auto& spatialVector = gEntitySpatialIndex[currentIndex];
        auto index = Algorithm::binaryFind(std::begin(spatialVector), std::end(spatialVector), entity.Id);
        if (index != std::end(spatialVector))
        {
            spatialVector.erase(index, index + 1);
        }
        else
        {
            LOG_WARNING("Bad sprite spatial index. Rebuilding the spatial index...");
            ResetEntitySpatialIndices();
        }

        entity.SpatialIndex = kInvalidSpatialIndex;
    }

    void EntityRegistry::UpdateEntitySpatialIndex(EntityBase& entity)
    {
        if (entity.SpatialIndex & kSpatialIndexDirtyMask)
        {
            if (entity.SpatialIndex != kInvalidSpatialIndex)
            {
                EntitySpatialRemove(entity);
            }
            EntitySpatialInsert(entity, { entity.x, entity.y });
        }
    }

    void EntityRegistry::UpdateEntitiesSpatialIndex()
    {
        for (auto& entityList : gEntityLists)
        {
            for (auto& entityId : entityList)
            {
                auto* entity = TryGetEntity(entityId);
                if (entity != nullptr && entity->Type != EntityType::null)
                {
                    UpdateEntitySpatialIndex(*entity);
                }
            }
        }
    }

    /**
     * Frees any dynamically attached memory to the entity, such as peep name.
     */
    void EntityRegistry::FreeEntity(EntityBase& entity)
    {
        auto* guest = entity.As<Guest>();
        auto* staff = entity.As<Staff>();
        if (staff != nullptr)
        {
            staff->SetName({});
            staff->ClearPatrolArea();
        }
        else if (guest != nullptr)
        {
            guest->SetName({});
            guest->GuestNextInQueue = EntityId::GetNull();

            RideUse::GetHistory().RemoveHandle(guest->Id);
            RideUse::GetTypeHistory().RemoveHandle(guest->Id);
        }
    }

    /**
     *
     *  rct2: 0x0069EDB6
     */
    void EntityRegistry::EntityRemove(EntityBase* entity)
    {
        FreeEntity(*entity);

        EntityTweener::Get().RemoveEntity(entity);
        RemoveFromEntityList(*entity); // remove from existing list
        AddToFreeList(entity->Id);

        EntitySpatialRemove(*entity);
        EntityReset(*entity);
    }

    /**
     * Loops through all floating entities and removes them.
     * Returns the amount of removed objects as feedback.
     */
    uint16_t EntityRegistry::RemoveFloatingEntities()
    {
        uint16_t removed = 0;
        for (auto* balloon : EntityList<Balloon>())
        {
            EntityRemove(balloon);
            removed++;
        }
        for (auto* duck : EntityList<Duck>())
        {
            if (duck->IsFlying())
            {
                EntityRemove(duck);
                removed++;
            }
        }
        for (auto* money : EntityList<MoneyEffect>())
        {
            EntityRemove(money);
            removed++;
        }
        return removed;
    }

    void EntityRegistry::EntitySetFlashing(EntityBase* entity, bool flashing)
    {
        assert(entity->Id.ToUnderlying() < kMaxEntities);
        _entityFlashingList[entity->Id.ToUnderlying()] = flashing;
    }

    bool EntityRegistry::EntityGetFlashing(EntityBase* entity)
    {
        assert(entity->Id.ToUnderlying() < kMaxEntities);
        return _entityFlashingList[entity->Id.ToUnderlying()];
    }
} // namespace OpenRCT2

using namespace OpenRCT2;

CoordsXYZ EntityBase::GetLocation() const
{
    return { x, y, z };
}

void EntityBase::SetLocation(const CoordsXYZ& newLocation)
{
    if (GetLocation() == newLocation)
    {
        // No change, this can happen quite often when the entity is interpolated.
        return;
    }

    x = newLocation.x;
    y = newLocation.y;
    z = newLocation.z;

    if (SpatialIndex & kSpatialIndexDirtyMask)
    {
        // Already marked as dirty.
        return;
    }

    const auto newSpatialIndex = ComputeSpatialIndex({ x, y });
    if (newSpatialIndex == GetSpatialIndex(*this))
    {
        // Avoid marking it dirty when we don't leave the current tile.
        return;
    }

    SpatialIndex |= kSpatialIndexDirtyMask;
}

static void EntitySetCoordinates(const CoordsXYZ& entityPos, EntityBase* entity)
{
    auto screenCoords = Translate3DTo2DWithZ(GetCurrentRotation(), entityPos);

    entity->SpriteData.SpriteRect = ScreenRect(
        screenCoords - ScreenCoordsXY{ entity->SpriteData.Width, entity->SpriteData.HeightMin },
        screenCoords + ScreenCoordsXY{ entity->SpriteData.Width, entity->SpriteData.HeightMax });
    entity->SetLocation(entityPos);
}

void EntityBase::MoveTo(const CoordsXYZ& newLocation)
{
    if (x != kLocationNull)
    {
        // Invalidate old position.
        Invalidate();
    }

    auto loc = newLocation;
    if (!MapIsLocationValid(loc))
    {
        loc.x = kLocationNull;
    }

    if (loc.x == kLocationNull)
    {
        SetLocation(loc);
    }
    else
    {
        EntitySetCoordinates(loc, this);
        Invalidate(); // Invalidate new position.
    }
}

void EntityBase::MoveToAndUpdateSpatialIndex(const CoordsXYZ& newLocation)
{
    MoveTo(newLocation);

    // TODO: pass as param instead of relying on global game state
    auto& gameState = getGameState();

    gameState.entities.UpdateEntitySpatialIndex(*this);
}
