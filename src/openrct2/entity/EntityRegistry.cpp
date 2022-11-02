/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "EntityRegistry.h"

#include "../Game.h"
#include "../core/Algorithm.hpp"
#include "../core/ChecksumStream.h"
#include "../core/Crypt.h"
#include "../core/DataSerialiser.h"
#include "../core/Guard.hpp"
#include "../core/MemoryStream.h"
#include "../entity/Peep.h"
#include "../entity/Staff.h"
#include "../interface/Viewport.h"
#include "../peep/RideUseSystem.h"
#include "../profiling/Profiling.h"
#include "../ride/Vehicle.h"
#include "../scenario/Scenario.h"
#include "Balloon.h"
#include "Duck.h"
#include "EntityTweener.h"
#include "Fountain.h"
#include "MoneyEffect.h"
#include "Particle.h"

#include <algorithm>
#include <cmath>
#include <iterator>
#include <numeric>
#include <vector>

union Entity
{
    uint8_t pad_00[0x200];
    EntityBase base;
    Entity()
        : pad_00()
    {
    }
};

static Entity _entities[MAX_ENTITIES]{};
static std::array<std::list<EntityId>, EnumValue(EntityType::Count)> gEntityLists;
static std::vector<EntityId> _freeIdList;

static bool _entityFlashingList[MAX_ENTITIES];

constexpr const uint32_t SPATIAL_INDEX_SIZE = (MAXIMUM_MAP_SIZE_TECHNICAL * MAXIMUM_MAP_SIZE_TECHNICAL) + 1;
constexpr const uint32_t SPATIAL_INDEX_LOCATION_NULL = SPATIAL_INDEX_SIZE - 1;

static std::array<std::vector<EntityId>, SPATIAL_INDEX_SIZE> gEntitySpatialIndex;

static void FreeEntity(EntityBase& entity);

static constexpr size_t GetSpatialIndexOffset(const CoordsXY& loc)
{
    if (loc.IsNull())
        return SPATIAL_INDEX_LOCATION_NULL;

    // NOTE: The input coordinate is rotated and can have negative components.
    const auto tileX = std::abs(loc.x) / COORDS_XY_STEP;
    const auto tileY = std::abs(loc.y) / COORDS_XY_STEP;

    if (tileX >= MAXIMUM_MAP_SIZE_TECHNICAL || tileY >= MAXIMUM_MAP_SIZE_TECHNICAL)
        return SPATIAL_INDEX_LOCATION_NULL;

    return tileX * MAXIMUM_MAP_SIZE_TECHNICAL + tileY;
}

constexpr bool EntityTypeIsMiscEntity(const EntityType type)
{
    switch (type)
    {
        case EntityType::SteamParticle:
        case EntityType::MoneyEffect:
        case EntityType::CrashedVehicleParticle:
        case EntityType::ExplosionCloud:
        case EntityType::CrashSplash:
        case EntityType::ExplosionFlare:
        case EntityType::JumpingFountain:
        case EntityType::Balloon:
        case EntityType::Duck:
            return true;
        default:
            return false;
    }
}

uint16_t GetEntityListCount(EntityType type)
{
    return static_cast<uint16_t>(gEntityLists[EnumValue(type)].size());
}

uint16_t GetNumFreeEntities()
{
    return static_cast<uint16_t>(_freeIdList.size());
}

std::string EntitiesChecksum::ToString() const
{
    std::string result;

    result.reserve(raw.size() * 2);
    for (auto b : raw)
    {
        char buf[3];
        snprintf(buf, 3, "%02x", static_cast<int32_t>(b));
        result.append(buf);
    }

    return result;
}

EntityBase* TryGetEntity(EntityId entityIndex)
{
    const auto idx = entityIndex.ToUnderlying();
    return idx >= MAX_ENTITIES ? nullptr : &_entities[idx].base;
}

EntityBase* GetEntity(EntityId entityIndex)
{
    if (entityIndex.IsNull())
    {
        return nullptr;
    }
    openrct2_assert(entityIndex.ToUnderlying() < MAX_ENTITIES, "Tried getting entity %u", entityIndex.ToUnderlying());
    return TryGetEntity(entityIndex);
}

const std::vector<EntityId>& GetEntityTileList(const CoordsXY& spritePos)
{
    return gEntitySpatialIndex[GetSpatialIndexOffset(spritePos)];
}

static void ResetEntityLists()
{
    for (auto& list : gEntityLists)
    {
        list.clear();
    }
}

static void ResetFreeIds()
{
    _freeIdList.clear();
    _freeIdList.resize(MAX_ENTITIES);

    // List needs to be back to front to simplify removing
    auto nextId = 0;
    std::for_each(std::rbegin(_freeIdList), std::rend(_freeIdList), [&](auto& elem) {
        elem = EntityId::FromUnderlying(nextId);
        nextId++;
    });
}

const std::list<EntityId>& GetEntityList(const EntityType id)
{
    return gEntityLists[EnumValue(id)];
}

/**
 *
 *  rct2: 0x0069EB13
 */
void ResetAllEntities()
{
    gSavedAge = 0;

    // Free all associated Entity pointers prior to zeroing memory
    for (int32_t i = 0; i < MAX_ENTITIES; ++i)
    {
        auto* spr = GetEntity(EntityId::FromUnderlying(i));
        if (spr == nullptr)
        {
            continue;
        }
        FreeEntity(*spr);
    }

    std::fill(std::begin(_entities), std::end(_entities), Entity());
    OpenRCT2::RideUse::GetHistory().Clear();
    OpenRCT2::RideUse::GetTypeHistory().Clear();
    for (int32_t i = 0; i < MAX_ENTITIES; ++i)
    {
        auto* spr = GetEntity(EntityId::FromUnderlying(i));
        if (spr == nullptr)
        {
            continue;
        }
        spr->Type = EntityType::Null;
        spr->sprite_index = EntityId::FromUnderlying(i);

        _entityFlashingList[i] = false;
    }
    ResetEntityLists();
    ResetFreeIds();
    ResetEntitySpatialIndices();
}

static void EntitySpatialInsert(EntityBase* entity, const CoordsXY& newLoc);

/**
 *
 *  rct2: 0x0069EBE4
 * This function looks as though it sets some sort of order for sprites.
 * Sprites can share their position if this is the case.
 */
void ResetEntitySpatialIndices()
{
    for (auto& vec : gEntitySpatialIndex)
    {
        vec.clear();
    }
    for (EntityId::UnderlyingType i = 0; i < MAX_ENTITIES; i++)
    {
        auto* spr = GetEntity(EntityId::FromUnderlying(i));
        if (spr != nullptr && spr->Type != EntityType::Null)
        {
            EntitySpatialInsert(spr, { spr->x, spr->y });
        }
    }
}

#ifndef DISABLE_NETWORK

template<typename T> void NetworkSerialseEntityType(DataSerialiser& ds)
{
    for (auto* ent : EntityList<T>())
    {
        ent->Serialise(ds);
    }
}

template<typename... T> void NetworkSerialiseEntityTypes(DataSerialiser& ds)
{
    (NetworkSerialseEntityType<T>(ds), ...);
}

EntitiesChecksum GetAllEntitiesChecksum()
{
    EntitiesChecksum checksum{};

    OpenRCT2::ChecksumStream ms(checksum.raw);
    DataSerialiser ds(true, ms);
    NetworkSerialiseEntityTypes<Guest, Staff, Vehicle, Litter>(ds);

    return checksum;
}
#else

EntitiesChecksum GetAllEntitiesChecksum()
{
    return EntitiesChecksum{};
}

#endif // DISABLE_NETWORK

static void EntityReset(EntityBase* entity)
{
    // Need to retain how the sprite is linked in lists
    auto entityIndex = entity->sprite_index;
    _entityFlashingList[entityIndex.ToUnderlying()] = false;

    Entity* spr = reinterpret_cast<Entity*>(entity);
    *spr = Entity();

    entity->sprite_index = entityIndex;
    entity->Type = EntityType::Null;
}

static constexpr uint16_t MAX_MISC_SPRITES = 300;
static void AddToEntityList(EntityBase* entity)
{
    auto& list = gEntityLists[EnumValue(entity->Type)];
    // Entity list must be in sprite_index order to prevent desync issues
    list.insert(std::lower_bound(std::begin(list), std::end(list), entity->sprite_index), entity->sprite_index);
}

static void AddToFreeList(EntityId index)
{
    // Free list must be in reverse sprite_index order to prevent desync issues
    _freeIdList.insert(std::upper_bound(std::rbegin(_freeIdList), std::rend(_freeIdList), index).base(), index);
}

static void RemoveFromEntityList(EntityBase* entity)
{
    auto& list = gEntityLists[EnumValue(entity->Type)];
    auto ptr = binary_find(std::begin(list), std::end(list), entity->sprite_index);
    if (ptr != std::end(list))
    {
        list.erase(ptr);
    }
}

uint16_t GetMiscEntityCount()
{
    uint16_t count = 0;
    for (auto id : { EntityType::SteamParticle, EntityType::MoneyEffect, EntityType::CrashedVehicleParticle,
                     EntityType::ExplosionCloud, EntityType::CrashSplash, EntityType::ExplosionFlare,
                     EntityType::JumpingFountain, EntityType::Balloon, EntityType::Duck })
    {
        count += GetEntityListCount(id);
    }
    return count;
}

static void PrepareNewEntity(EntityBase* base, const EntityType type)
{
    // Need to reset all sprite data, as the uninitialised values
    // may contain garbage and cause a desync later on.
    EntityReset(base);

    base->Type = type;
    AddToEntityList(base);

    base->x = LOCATION_NULL;
    base->y = LOCATION_NULL;
    base->z = 0;
    base->sprite_width = 0x10;
    base->sprite_height_negative = 0x14;
    base->sprite_height_positive = 0x8;
    base->SpriteRect = {};

    EntitySpatialInsert(base, { LOCATION_NULL, 0 });
}

EntityBase* CreateEntity(EntityType type)
{
    if (_freeIdList.size() == 0)
    {
        // No free sprites.
        return nullptr;
    }

    if (EntityTypeIsMiscEntity(type))
    {
        // Misc sprites are commonly used for effects, if there are less than MAX_MISC_SPRITES
        // free it will fail to keep slots for more relevant sprites.
        // Also there can't be more than MAX_MISC_SPRITES sprites in this list.
        uint16_t miscSlotsRemaining = MAX_MISC_SPRITES - GetMiscEntityCount();
        if (miscSlotsRemaining >= _freeIdList.size())
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

    PrepareNewEntity(entity, type);

    return entity;
}

EntityBase* CreateEntityAt(const EntityId index, const EntityType type)
{
    auto id = binary_find(std::rbegin(_freeIdList), std::rend(_freeIdList), index);
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

    PrepareNewEntity(entity, type);
    return entity;
}

template<typename T> void MiscUpdateAllType()
{
    for (auto misc : EntityList<T>())
    {
        misc->Update();
    }
}

template<typename... T> void MiscUpdateAllTypes()
{
    (MiscUpdateAllType<T>(), ...);
}

/**
 *
 *  rct2: 0x00672AA4
 */
void UpdateAllMiscEntities()
{
    PROFILED_FUNCTION();

    MiscUpdateAllTypes<
        SteamParticle, MoneyEffect, VehicleCrashParticle, ExplosionCloud, CrashSplashParticle, ExplosionFlare, JumpingFountain,
        Balloon, Duck>();
}

// Performs a search to ensure that insert keeps next_in_quadrant in sprite_index order
static void EntitySpatialInsert(EntityBase* entity, const CoordsXY& newLoc)
{
    size_t newIndex = GetSpatialIndexOffset(newLoc);
    auto& spatialVector = gEntitySpatialIndex[newIndex];
    auto index = std::lower_bound(std::begin(spatialVector), std::end(spatialVector), entity->sprite_index);
    spatialVector.insert(index, entity->sprite_index);
}

static void EntitySpatialRemove(EntityBase* entity)
{
    size_t currentIndex = GetSpatialIndexOffset({ entity->x, entity->y });
    auto& spatialVector = gEntitySpatialIndex[currentIndex];
    auto index = binary_find(std::begin(spatialVector), std::end(spatialVector), entity->sprite_index);
    if (index != std::end(spatialVector))
    {
        spatialVector.erase(index, index + 1);
    }
    else
    {
        log_warning("Bad sprite spatial index. Rebuilding the spatial index...");
        ResetEntitySpatialIndices();
    }
}

static void EntitySpatialMove(EntityBase* entity, const CoordsXY& newLoc)
{
    size_t newIndex = GetSpatialIndexOffset(newLoc);
    size_t currentIndex = GetSpatialIndexOffset({ entity->x, entity->y });
    if (newIndex == currentIndex)
        return;

    EntitySpatialRemove(entity);
    EntitySpatialInsert(entity, newLoc);
}

void EntityBase::MoveTo(const CoordsXYZ& newLocation)
{
    if (x != LOCATION_NULL)
    {
        // Invalidate old position.
        Invalidate();
    }

    auto loc = newLocation;
    if (!MapIsLocationValid(loc))
    {
        loc.x = LOCATION_NULL;
    }

    EntitySpatialMove(this, loc);

    if (loc.x == LOCATION_NULL)
    {
        x = loc.x;
        y = loc.y;
        z = loc.z;
    }
    else
    {
        EntitySetCoordinates(loc, this);
        Invalidate(); // Invalidate new position.
    }
}

void EntitySetCoordinates(const CoordsXYZ& entityPos, EntityBase* entity)
{
    auto screenCoords = Translate3DTo2DWithZ(get_current_rotation(), entityPos);

    entity->SpriteRect = ScreenRect(
        screenCoords - ScreenCoordsXY{ entity->sprite_width, entity->sprite_height_negative },
        screenCoords + ScreenCoordsXY{ entity->sprite_width, entity->sprite_height_positive });
    entity->SetLocation(entityPos);
}

/**
 * Frees any dynamically attached memory to the entity, such as peep name.
 */
static void FreeEntity(EntityBase& entity)
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
        OpenRCT2::RideUse::GetHistory().RemoveHandle(guest->sprite_index);
        OpenRCT2::RideUse::GetTypeHistory().RemoveHandle(guest->sprite_index);
    }
}

/**
 *
 *  rct2: 0x0069EDB6
 */
void EntityRemove(EntityBase* entity)
{
    FreeEntity(*entity);

    EntityTweener::Get().RemoveEntity(entity);
    RemoveFromEntityList(entity); // remove from existing list
    AddToFreeList(entity->sprite_index);

    EntitySpatialRemove(entity);
    EntityReset(entity);
}

/**
 * Loops through all floating entities and removes them.
 * Returns the amount of removed objects as feedback.
 */
uint16_t RemoveFloatingEntities()
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

void EntitySetFlashing(EntityBase* entity, bool flashing)
{
    assert(entity->sprite_index.ToUnderlying() < MAX_ENTITIES);
    _entityFlashingList[entity->sprite_index.ToUnderlying()] = flashing;
}

bool EntityGetFlashing(EntityBase* entity)
{
    assert(entity->sprite_index.ToUnderlying() < MAX_ENTITIES);
    return _entityFlashingList[entity->sprite_index.ToUnderlying()];
}
