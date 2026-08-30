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
#include "../GameState.h"
#include "../core/Algorithm.hpp"
#include "../core/ChecksumStream.h"
#include "../core/DataSerialiser.h"
#include "../core/Guard.hpp"
#include "../core/String.hpp"
#include "../entity/EntityList.h"
#include "../entity/Staff.h"
#include "../interface/Viewport.h"
#include "../peep/RideUseSystem.h"
#include "../profiling/Profiling.h"
#include "../ride/Vehicle.h"
#include "../world/Map.h"
#include "Balloon.h"
#include "Duck.h"
#include "EntityTweener.h"
#include "JumpingFountain.h"
#include "MoneyEffect.h"
#include "Particle.h"

#include <cassert>
#include <iterator>
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
        return entity.spatialIndex & ~kSpatialIndexDirtyMask;
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
    uint16_t EntityRegistry::getEntityListCount(EntityType type)
    {
        return static_cast<uint16_t>(gEntityLists[EnumValue(type)].size());
    }

    // TODO: make part of EntityList unit?
    uint16_t EntityRegistry::getNumFreeEntities()
    {
        return static_cast<uint16_t>(_freeIdList.size());
    }

    std::string EntitiesChecksum::toString() const
    {
        return String::StringFromHex(raw);
    }

    EntityBase* EntityRegistry::tryGetEntity(EntityId entityIndex)
    {
        const auto idx = entityIndex.ToUnderlying();
        return idx >= kMaxEntities ? nullptr : &entities[idx].base;
    }

    EntityBase* EntityRegistry::getEntity(EntityId entityIndex)
    {
        if (entityIndex.IsNull())
        {
            return nullptr;
        }
        Guard::Assert(entityIndex.ToUnderlying() < kMaxEntities, "Tried getting entity %u", entityIndex.ToUnderlying());
        return tryGetEntity(entityIndex);
    }

    const std::vector<EntityId>& EntityRegistry::getEntityTileList(const CoordsXY& spritePos)
    {
        return gEntitySpatialIndex[ComputeSpatialIndex(spritePos)];
    }

    void EntityRegistry::resetEntityLists()
    {
        for (auto& list : gEntityLists)
        {
            list.clear();
        }
    }

    void EntityRegistry::resetFreeIds()
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

    const std::list<EntityId>& EntityRegistry::getEntityList(const EntityType id)
    {
        return gEntityLists[EnumValue(id)];
    }

    /**
     *
     *  rct2: 0x0069EB13
     */
    void EntityRegistry::resetAllEntities()
    {
        // Free all associated Entity pointers prior to zeroing memory
        for (int32_t i = 0; i < kMaxEntities; ++i)
        {
            auto* spr = getEntity(EntityId::FromUnderlying(i));
            if (spr == nullptr)
            {
                continue;
            }
            freeEntity(*spr);
        }

        std::fill(std::begin(entities), std::end(entities), Entity_t());
        RideUse::GetHistory().Clear();
        RideUse::GetTypeHistory().Clear();
        for (int32_t i = 0; i < kMaxEntities; ++i)
        {
            auto* spr = getEntity(EntityId::FromUnderlying(i));
            if (spr == nullptr)
            {
                continue;
            }
            spr->type = EntityType::null;
            spr->id = EntityId::FromUnderlying(i);

            _entityFlashingList[i] = false;
        }
        resetEntityLists();
        resetFreeIds();
        resetEntitySpatialIndices();
    }

    /**
     *
     *  rct2: 0x0069EBE4
     * This function looks as though it sets some sort of order for sprites.
     * Sprites can share their position if this is the case.
     */
    void EntityRegistry::resetEntitySpatialIndices()
    {
        for (auto& vec : gEntitySpatialIndex)
        {
            vec.clear();
        }
        for (EntityId::UnderlyingType i = 0; i < kMaxEntities; i++)
        {
            auto* entity = getEntity(EntityId::FromUnderlying(i));
            if (entity != nullptr && entity->type != EntityType::null)
            {
                entitySpatialInsert(*entity, { entity->x, entity->y });
            }
        }
    }

#ifndef DISABLE_NETWORK
    EntitiesChecksum EntityRegistry::getAllEntitiesChecksum()
    {
        EntitiesChecksum checksum{};

        ChecksumStream ms(checksum.raw);
        DataSerialiser ds(true, ms);
        networkSerialiseEntityTypes<Guest, Staff, Vehicle, Litter>(ds);

        return checksum;
    }
#else
    EntitiesChecksum EntityRegistry::getAllEntitiesChecksum()
    {
        return EntitiesChecksum{};
    }
#endif // DISABLE_NETWORK

    void EntityRegistry::entityReset(EntityBase& entity)
    {
        // Need to retain how the sprite is linked in lists
        auto entityIndex = entity.id;
        _entityFlashingList[entityIndex.ToUnderlying()] = false;

        Entity_t* tempEntity = reinterpret_cast<Entity_t*>(&entity);
        *tempEntity = Entity_t();

        entity.id = entityIndex;
        entity.type = EntityType::null;
    }

    void EntityRegistry::addToEntityList(EntityBase& entity)
    {
        auto& list = gEntityLists[EnumValue(entity.type)];

        // Entity list is sorted by id to prevent desyncs.
        Algorithm::sortedInsert(list, entity.id);
    }

    void EntityRegistry::addToFreeList(EntityId index)
    {
        // Free list must be in reverse sprite_index order to prevent desync issues
        _freeIdList.insert(std::upper_bound(std::rbegin(_freeIdList), std::rend(_freeIdList), index).base(), index);
    }

    void EntityRegistry::removeFromEntityList(EntityBase& entity)
    {
        auto& list = gEntityLists[EnumValue(entity.type)];
        auto ptr = Algorithm::binaryFind(std::begin(list), std::end(list), entity.id);
        if (ptr != std::end(list))
        {
            list.erase(ptr);
        }
    }

    uint16_t EntityRegistry::getMiscEntityCount()
    {
        uint16_t count = 0;
        for (auto id : { EntityType::steamParticle, EntityType::moneyEffect, EntityType::crashedVehicleParticle,
                         EntityType::explosionCloud, EntityType::crashSplash, EntityType::explosionFlare,
                         EntityType::jumpingFountain, EntityType::balloon, EntityType::duck })
        {
            count += getEntityListCount(id);
        }
        return count;
    }

    void EntityRegistry::prepareNewEntity(EntityBase& base, const EntityType type)
    {
        // Need to reset all sprite data, as the uninitialised values
        // may contain garbage and cause a desync later on.
        entityReset(base);

        base.type = type;
        addToEntityList(base);

        base.x = kLocationNull;
        base.y = kLocationNull;
        base.z = 0;
        base.spriteData.width = 0x10;
        base.spriteData.heightMin = 0x14;
        base.spriteData.heightMax = 0x8;
        base.spriteData.spriteRect = {};
        base.spatialIndex = kInvalidSpatialIndex;

        entitySpatialInsert(base, { kLocationNull, 0 });
    }

    EntityBase* EntityRegistry::createEntity(EntityType type)
    {
        if (_freeIdList.empty())
        {
            // No free sprites.
            return nullptr;
        }

        if (EntityTypeIsMiscEntity(type))
        {
            // Misc sprites are commonly used for effects, give other entity types higher priority.
            if (getMiscEntityCount() >= kMaxMiscEntities)
            {
                return nullptr;
            }

            // If there are less than kMaxMiscEntities free slots, ensure other entities can be created.
            if (_freeIdList.size() < kMaxMiscEntities)
            {
                return nullptr;
            }
        }

        auto* entity = getEntity(_freeIdList.back());
        if (entity == nullptr)
        {
            return nullptr;
        }
        _freeIdList.pop_back();

        prepareNewEntity(*entity, type);

        return entity;
    }

    EntityBase* EntityRegistry::createEntityAt(const EntityId index, const EntityType type)
    {
        auto id = Algorithm::binaryFind(std::rbegin(_freeIdList), std::rend(_freeIdList), index);
        if (id == std::rend(_freeIdList))
        {
            return nullptr;
        }

        auto* entity = getEntity(index);
        if (entity == nullptr)
        {
            return nullptr;
        }

        _freeIdList.erase(std::next(id).base());

        prepareNewEntity(*entity, type);
        return entity;
    }

    /**
     *
     *  rct2: 0x00672AA4
     */
    void EntityRegistry::updateAllMiscEntities()
    {
        PROFILED_FUNCTION();

        miscUpdateAllTypes<
            SteamParticle, MoneyEffect, VehicleCrashParticle, ExplosionCloud, CrashSplashParticle, ExplosionFlare,
            JumpingFountain, Balloon, Duck>();
    }

    void EntityRegistry::updateMoneyEffect()
    {
        miscUpdateAllTypes<MoneyEffect>();
    }

    // Performs a search to ensure that insert keeps next_in_quadrant in sprite_index order
    void EntityRegistry::entitySpatialInsert(EntityBase& entity, const CoordsXY& newLoc)
    {
        const auto newIndex = ComputeSpatialIndex(newLoc);

        auto& spatialVector = gEntitySpatialIndex[newIndex];

        Algorithm::sortedInsert(spatialVector, entity.id);

        entity.spatialIndex = newIndex;
    }

    void EntityRegistry::entitySpatialRemove(EntityBase& entity)
    {
        const auto currentIndex = GetSpatialIndex(entity);

        auto& spatialVector = gEntitySpatialIndex[currentIndex];
        auto index = Algorithm::binaryFind(std::begin(spatialVector), std::end(spatialVector), entity.id);
        if (index != std::end(spatialVector))
        {
            spatialVector.erase(index, index + 1);
        }
        else
        {
            LOG_WARNING("Bad sprite spatial index. Rebuilding the spatial index...");
            resetEntitySpatialIndices();
        }

        entity.spatialIndex = kInvalidSpatialIndex;
    }

    void EntityRegistry::updateEntitySpatialIndex(EntityBase& entity)
    {
        if (entity.spatialIndex & kSpatialIndexDirtyMask)
        {
            if (entity.spatialIndex != kInvalidSpatialIndex)
            {
                entitySpatialRemove(entity);
            }
            entitySpatialInsert(entity, { entity.x, entity.y });
        }
    }

    void EntityRegistry::updateEntitiesSpatialIndex()
    {
        for (auto& entityList : gEntityLists)
        {
            for (auto& entityId : entityList)
            {
                auto* entity = tryGetEntity(entityId);
                if (entity != nullptr && entity->type != EntityType::null)
                {
                    updateEntitySpatialIndex(*entity);
                }
            }
        }
    }

    /**
     * Frees any dynamically attached memory to the entity, such as peep name.
     */
    void EntityRegistry::freeEntity(EntityBase& entity)
    {
        auto* guest = entity.as<Guest>();
        auto* staff = entity.as<Staff>();
        if (staff != nullptr)
        {
            staff->setName({});
            staff->clearPatrolArea();
        }
        else if (guest != nullptr)
        {
            guest->setName({});
            guest->guestNextInQueue = EntityId::GetNull();

            RideUse::GetHistory().RemoveHandle(guest->id);
            RideUse::GetTypeHistory().RemoveHandle(guest->id);
        }
    }

    /**
     *
     *  rct2: 0x0069EDB6
     */
    void EntityRegistry::entityRemove(EntityBase* entity)
    {
        freeEntity(*entity);

        EntityTweener::get().removeEntity(entity);
        removeFromEntityList(*entity); // remove from existing list
        addToFreeList(entity->id);

        entitySpatialRemove(*entity);
        entityReset(*entity);
    }

    /**
     * Loops through all floating entities and removes them.
     * Returns the amount of removed objects as feedback.
     */
    uint16_t EntityRegistry::removeFloatingEntities()
    {
        uint16_t removed = 0;
        for (auto* balloon : EntityList<Balloon>())
        {
            entityRemove(balloon);
            removed++;
        }
        for (auto* duck : EntityList<Duck>())
        {
            if (duck->isFlying())
            {
                entityRemove(duck);
                removed++;
            }
        }
        for (auto* money : EntityList<MoneyEffect>())
        {
            entityRemove(money);
            removed++;
        }
        return removed;
    }

    void EntityRegistry::entitySetFlashing(EntityBase* entity, bool flashing)
    {
        assert(entity->id.ToUnderlying() < kMaxEntities);
        _entityFlashingList[entity->id.ToUnderlying()] = flashing;
    }

    bool EntityRegistry::entityGetFlashing(EntityBase* entity)
    {
        assert(entity->id.ToUnderlying() < kMaxEntities);
        return _entityFlashingList[entity->id.ToUnderlying()];
    }
} // namespace OpenRCT2

using namespace OpenRCT2;

CoordsXYZ EntityBase::getLocation() const
{
    return { x, y, z };
}

void EntityBase::setLocation(const CoordsXYZ& newLocation)
{
    if (getLocation() == newLocation)
    {
        // No change, this can happen quite often when the entity is interpolated.
        return;
    }

    x = newLocation.x;
    y = newLocation.y;
    z = newLocation.z;

    if (spatialIndex & kSpatialIndexDirtyMask)
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

    spatialIndex |= kSpatialIndexDirtyMask;
}

static void EntitySetCoordinates(const CoordsXYZ& entityPos, EntityBase* entity)
{
    auto screenCoords = Translate3DTo2DWithZ(GetCurrentRotation(), entityPos);

    entity->spriteData.spriteRect = ScreenRect(
        screenCoords - ScreenCoordsXY{ entity->spriteData.width, entity->spriteData.heightMin },
        screenCoords + ScreenCoordsXY{ entity->spriteData.width, entity->spriteData.heightMax });
    entity->setLocation(entityPos);
}

void EntityBase::moveTo(const CoordsXYZ& newLocation)
{
    if (x != kLocationNull)
    {
        // Invalidate old position.
        invalidate();
    }

    auto loc = newLocation;
    if (!MapIsLocationValid(loc))
    {
        loc.x = kLocationNull;
    }

    if (loc.x == kLocationNull)
    {
        setLocation(loc);
    }
    else
    {
        EntitySetCoordinates(loc, this);
        invalidate(); // Invalidate new position.
    }
}

void EntityBase::moveToAndUpdateSpatialIndex(const CoordsXYZ& newLocation)
{
    moveTo(newLocation);

    // TODO: pass as param instead of relying on global game state
    auto& gameState = getGameState();

    gameState.entities.updateEntitySpatialIndex(*this);
}
