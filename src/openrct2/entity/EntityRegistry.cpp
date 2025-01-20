/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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

#include <cassert>
#include <cmath>
#include <iterator>
#include <numeric>
#include <vector>

using namespace OpenRCT2;

static std::array<std::list<EntityId>, EnumValue(EntityType::Count)> gEntityLists;
static std::vector<EntityId> _freeIdList;

static bool _entityFlashingList[kMaxEntities];

static constexpr const uint32_t kSpatialIndexSize = (kMaximumMapSizeTechnical * kMaximumMapSizeTechnical) + 1;
static constexpr uint32_t kSpatialIndexNullBucket = kSpatialIndexSize - 1;

static constexpr uint32_t kInvalidSpatialIndex = 0xFFFFFFFFu;
static constexpr uint32_t kSpatialIndexDirtyMask = 1u << 31;

static std::array<std::vector<EntityId>, kSpatialIndexSize> gEntitySpatialIndex;

static void FreeEntity(EntityBase& entity);

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

static constexpr uint32_t GetSpatialIndex(EntityBase* entity)
{
    return entity->SpatialIndex & ~kSpatialIndexDirtyMask;
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
    return String::StringFromHex(raw);
}

EntityBase* TryGetEntity(EntityId entityIndex)
{
    auto& gameState = GetGameState();
    const auto idx = entityIndex.ToUnderlying();
    return idx >= kMaxEntities ? nullptr : &gameState.Entities[idx].base;
}

EntityBase* GetEntity(EntityId entityIndex)
{
    if (entityIndex.IsNull())
    {
        return nullptr;
    }
    Guard::Assert(entityIndex.ToUnderlying() < kMaxEntities, "Tried getting entity %u", entityIndex.ToUnderlying());
    return TryGetEntity(entityIndex);
}

const std::vector<EntityId>& GetEntityTileList(const CoordsXY& spritePos)
{
    return gEntitySpatialIndex[ComputeSpatialIndex(spritePos)];
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
    _freeIdList.resize(kMaxEntities);

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

    auto& gameState = GetGameState();
    std::fill(std::begin(gameState.Entities), std::end(gameState.Entities), Entity_t());
    OpenRCT2::RideUse::GetHistory().Clear();
    OpenRCT2::RideUse::GetTypeHistory().Clear();
    for (int32_t i = 0; i < kMaxEntities; ++i)
    {
        auto* spr = GetEntity(EntityId::FromUnderlying(i));
        if (spr == nullptr)
        {
            continue;
        }
        spr->Type = EntityType::Null;
        spr->Id = EntityId::FromUnderlying(i);

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
    for (EntityId::UnderlyingType i = 0; i < kMaxEntities; i++)
    {
        auto* entity = GetEntity(EntityId::FromUnderlying(i));
        if (entity != nullptr && entity->Type != EntityType::Null)
        {
            EntitySpatialInsert(entity, { entity->x, entity->y });
        }
    }
}

#ifndef DISABLE_NETWORK

template<typename T>
void NetworkSerialseEntityType(DataSerialiser& ds)
{
    for (auto* ent : EntityList<T>())
    {
        ent->Serialise(ds);
    }
}

template<typename... T>
void NetworkSerialiseEntityTypes(DataSerialiser& ds)
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
    auto entityIndex = entity->Id;
    _entityFlashingList[entityIndex.ToUnderlying()] = false;

    Entity_t* tempEntity = reinterpret_cast<Entity_t*>(entity);
    *tempEntity = Entity_t();

    entity->Id = entityIndex;
    entity->Type = EntityType::Null;
}

static constexpr uint16_t kMaxMiscEntities = 1600;

static void AddToEntityList(EntityBase* entity)
{
    auto& list = gEntityLists[EnumValue(entity->Type)];
    // Entity list must be in sprite_index order to prevent desync issues
    list.insert(std::lower_bound(std::begin(list), std::end(list), entity->Id), entity->Id);
}

static void AddToFreeList(EntityId index)
{
    // Free list must be in reverse sprite_index order to prevent desync issues
    _freeIdList.insert(std::upper_bound(std::rbegin(_freeIdList), std::rend(_freeIdList), index).base(), index);
}

static void RemoveFromEntityList(EntityBase* entity)
{
    auto& list = gEntityLists[EnumValue(entity->Type)];
    auto ptr = BinaryFind(std::begin(list), std::end(list), entity->Id);
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

    base->x = kLocationNull;
    base->y = kLocationNull;
    base->z = 0;
    base->SpriteData.Width = 0x10;
    base->SpriteData.HeightMin = 0x14;
    base->SpriteData.HeightMax = 0x8;
    base->SpriteData.SpriteRect = {};
    base->SpatialIndex = kInvalidSpatialIndex;

    EntitySpatialInsert(base, { kLocationNull, 0 });
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

    PrepareNewEntity(entity, type);

    return entity;
}

EntityBase* CreateEntityAt(const EntityId index, const EntityType type)
{
    auto id = BinaryFind(std::rbegin(_freeIdList), std::rend(_freeIdList), index);
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

template<typename T>
void MiscUpdateAllType()
{
    for (auto misc : EntityList<T>())
    {
        misc->Update();
    }
}

template<typename... T>
void MiscUpdateAllTypes()
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

void UpdateMoneyEffect()
{
    MiscUpdateAllTypes<MoneyEffect>();
}

// Performs a search to ensure that insert keeps next_in_quadrant in sprite_index order
static void EntitySpatialInsert(EntityBase* entity, const CoordsXY& newLoc)
{
    const auto newIndex = ComputeSpatialIndex(newLoc);

    auto& spatialVector = gEntitySpatialIndex[newIndex];
    auto index = std::lower_bound(std::begin(spatialVector), std::end(spatialVector), entity->Id);
    spatialVector.insert(index, entity->Id);

    entity->SpatialIndex = newIndex;
}

static void EntitySpatialRemove(EntityBase* entity)
{
    const auto currentIndex = GetSpatialIndex(entity);

    auto& spatialVector = gEntitySpatialIndex[currentIndex];
    auto index = BinaryFind(std::begin(spatialVector), std::end(spatialVector), entity->Id);
    if (index != std::end(spatialVector))
    {
        spatialVector.erase(index, index + 1);
    }
    else
    {
        LOG_WARNING("Bad sprite spatial index. Rebuilding the spatial index...");
        ResetEntitySpatialIndices();
    }

    entity->SpatialIndex = kInvalidSpatialIndex;
}

void UpdateEntitiesSpatialIndex()
{
    for (auto& entityList : gEntityLists)
    {
        for (auto& entityId : entityList)
        {
            auto* entity = GetEntity(entityId);
            if (entity == nullptr || entity->Type == EntityType::Null)
                continue;

            if (entity->SpatialIndex & kSpatialIndexDirtyMask)
            {
                if (entity->SpatialIndex != kInvalidSpatialIndex)
                {
                    EntitySpatialRemove(entity);
                }
                EntitySpatialInsert(entity, { entity->x, entity->y });
            }
        }
    }
}

CoordsXYZ EntityBase::GetLocation() const
{
    return { x, y, z };
}

void EntityBase::SetLocation(const CoordsXYZ& newLocation)
{
    x = newLocation.x;
    y = newLocation.y;
    z = newLocation.z;
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
        guest->GuestNextInQueue = EntityId::GetNull();

        OpenRCT2::RideUse::GetHistory().RemoveHandle(guest->Id);
        OpenRCT2::RideUse::GetTypeHistory().RemoveHandle(guest->Id);
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
    AddToFreeList(entity->Id);

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
    assert(entity->Id.ToUnderlying() < kMaxEntities);
    _entityFlashingList[entity->Id.ToUnderlying()] = flashing;
}

bool EntityGetFlashing(EntityBase* entity)
{
    assert(entity->Id.ToUnderlying() < kMaxEntities);
    return _entityFlashingList[entity->Id.ToUnderlying()];
}
