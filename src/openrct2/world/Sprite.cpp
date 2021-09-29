/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Sprite.h"

#include "../Game.h"
#include "../core/ChecksumStream.h"
#include "../core/Crypt.h"
#include "../core/DataSerialiser.h"
#include "../core/Guard.hpp"
#include "../core/MemoryStream.h"
#include "../interface/Viewport.h"
#include "../peep/Peep.h"
#include "../peep/RideUseSystem.h"
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

static rct_sprite _spriteList[MAX_ENTITIES];
static std::array<std::list<uint16_t>, EnumValue(EntityType::Count)> gEntityLists;
static std::vector<uint16_t> _freeIdList;

static bool _spriteFlashingList[MAX_ENTITIES];

constexpr const uint32_t SPATIAL_INDEX_SIZE = (MAXIMUM_MAP_SIZE_TECHNICAL * MAXIMUM_MAP_SIZE_TECHNICAL) + 1;
constexpr const uint32_t SPATIAL_INDEX_LOCATION_NULL = SPATIAL_INDEX_SIZE - 1;

static std::array<std::vector<uint16_t>, SPATIAL_INDEX_SIZE> gSpriteSpatialIndex;

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

// Required for GetEntity to return a default
template<> bool EntityBase::Is<EntityBase>() const
{
    return true;
}

template<> bool EntityBase::Is<Litter>() const
{
    return Type == EntityType::Litter;
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

template<> bool EntityBase::Is<MiscEntity>() const
{
    return EntityTypeIsMiscEntity(Type);
}

template<> bool EntityBase::Is<SteamParticle>() const
{
    return Type == EntityType::SteamParticle;
}

template<> bool EntityBase::Is<ExplosionFlare>() const
{
    return Type == EntityType::ExplosionFlare;
}

template<> bool EntityBase::Is<ExplosionCloud>() const
{
    return Type == EntityType::ExplosionCloud;
}

uint16_t GetEntityListCount(EntityType type)
{
    return static_cast<uint16_t>(gEntityLists[EnumValue(type)].size());
}

uint16_t GetNumFreeEntities()
{
    return static_cast<uint16_t>(_freeIdList.size());
}

std::string rct_sprite_checksum::ToString() const
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

EntityBase* try_get_sprite(size_t spriteIndex)
{
    return spriteIndex >= MAX_ENTITIES ? nullptr : &_spriteList[spriteIndex].base;
}

EntityBase* get_sprite(size_t spriteIndex)
{
    if (spriteIndex == SPRITE_INDEX_NULL)
    {
        return nullptr;
    }
    openrct2_assert(spriteIndex < MAX_ENTITIES, "Tried getting sprite %u", spriteIndex);
    return try_get_sprite(spriteIndex);
}

const std::vector<uint16_t>& GetEntityTileList(const CoordsXY& spritePos)
{
    return gSpriteSpatialIndex[GetSpatialIndexOffset(spritePos)];
}

void EntityBase::Invalidate()
{
    if (x == LOCATION_NULL)
        return;

    int32_t maxZoom = 0;
    switch (Type)
    {
        case EntityType::Vehicle:
        case EntityType::Guest:
        case EntityType::Staff:
            maxZoom = 2;
            break;
        case EntityType::CrashedVehicleParticle:
        case EntityType::JumpingFountain:
            maxZoom = 0;
            break;
        case EntityType::Duck:
            maxZoom = 1;
            break;
        case EntityType::SteamParticle:
        case EntityType::MoneyEffect:
        case EntityType::ExplosionCloud:
        case EntityType::CrashSplash:
        case EntityType::ExplosionFlare:
        case EntityType::Balloon:
            maxZoom = 2;
            break;
        case EntityType::Litter:
            maxZoom = 0;
            break;
        default:
            break;
    }

    viewports_invalidate(SpriteRect.GetLeft(), SpriteRect.GetTop(), SpriteRect.GetRight(), SpriteRect.GetBottom(), maxZoom);
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
    std::iota(std::rbegin(_freeIdList), std::rend(_freeIdList), 0);
}

const std::list<uint16_t>& GetEntityList(const EntityType id)
{
    return gEntityLists[EnumValue(id)];
}

/**
 *
 *  rct2: 0x0069EB13
 */
void reset_sprite_list()
{
    gSavedAge = 0;

    // Free all associated Entity pointers prior to zeroing memory
    for (int32_t i = 0; i < MAX_ENTITIES; ++i)
    {
        auto* spr = GetEntity(i);
        if (spr == nullptr)
        {
            continue;
        }
        FreeEntity(*spr);
    }

    std::fill(std::begin(_spriteList), std::end(_spriteList), rct_sprite());
    OpenRCT2::RideUse::GetHistory().Clear();
    OpenRCT2::RideUse::GetTypeHistory().Clear();
    for (int32_t i = 0; i < MAX_ENTITIES; ++i)
    {
        auto* spr = GetEntity(i);
        if (spr == nullptr)
        {
            continue;
        }
        spr->Type = EntityType::Null;
        spr->sprite_index = i;

        _spriteFlashingList[i] = false;
    }
    ResetEntityLists();
    ResetFreeIds();
    reset_sprite_spatial_index();
}

static void SpriteSpatialInsert(EntityBase* sprite, const CoordsXY& newLoc);

/**
 *
 *  rct2: 0x0069EBE4
 * This function looks as though it sets some sort of order for sprites.
 * Sprites can share their position if this is the case.
 */
void reset_sprite_spatial_index()
{
    for (auto& vec : gSpriteSpatialIndex)
    {
        vec.clear();
    }
    for (size_t i = 0; i < MAX_ENTITIES; i++)
    {
        auto* spr = GetEntity(i);
        if (spr != nullptr && spr->Type != EntityType::Null)
        {
            SpriteSpatialInsert(spr, { spr->x, spr->y });
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

rct_sprite_checksum sprite_checksum()
{
    rct_sprite_checksum checksum{};

    OpenRCT2::ChecksumStream ms(checksum.raw);
    DataSerialiser ds(true, ms);
    NetworkSerialiseEntityTypes<Guest, Staff, Vehicle, Litter>(ds);

    return checksum;
}
#else

rct_sprite_checksum sprite_checksum()
{
    return rct_sprite_checksum{};
}

#endif // DISABLE_NETWORK

static void sprite_reset(EntityBase* sprite)
{
    // Need to retain how the sprite is linked in lists
    uint16_t sprite_index = sprite->sprite_index;
    _spriteFlashingList[sprite_index] = false;

    rct_sprite* spr = reinterpret_cast<rct_sprite*>(sprite);
    *spr = rct_sprite();

    sprite->sprite_index = sprite_index;
    sprite->Type = EntityType::Null;
}

static constexpr uint16_t MAX_MISC_SPRITES = 300;
static void AddToEntityList(EntityBase* entity)
{
    auto& list = gEntityLists[EnumValue(entity->Type)];
    // Entity list must be in sprite_index order to prevent desync issues
    list.insert(std::lower_bound(std::begin(list), std::end(list), entity->sprite_index), entity->sprite_index);
}

static void AddToFreeList(uint16_t index)
{
    // Free list must be in reverse sprite_index order to prevent desync issues
    _freeIdList.insert(std::upper_bound(std::rbegin(_freeIdList), std::rend(_freeIdList), index).base(), index);
}

static void RemoveFromEntityList(EntityBase* entity)
{
    auto& list = gEntityLists[EnumValue(entity->Type)];
    auto ptr = std::lower_bound(std::begin(list), std::end(list), entity->sprite_index);
    if (ptr != std::end(list) && *ptr == entity->sprite_index)
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
    sprite_reset(base);

    base->Type = type;
    AddToEntityList(base);

    base->x = LOCATION_NULL;
    base->y = LOCATION_NULL;
    base->z = 0;
    base->sprite_width = 0x10;
    base->sprite_height_negative = 0x14;
    base->sprite_height_positive = 0x8;
    base->SpriteRect = {};

    SpriteSpatialInsert(base, { LOCATION_NULL, 0 });
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

EntityBase* CreateEntityAt(const uint16_t index, const EntityType type)
{
    auto id = std::lower_bound(std::rbegin(_freeIdList), std::rend(_freeIdList), index);
    if (id == std::rend(_freeIdList) || *id != index)
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
void sprite_misc_update_all()
{
    MiscUpdateAllTypes<
        SteamParticle, MoneyEffect, VehicleCrashParticle, ExplosionCloud, CrashSplashParticle, ExplosionFlare, JumpingFountain,
        Balloon, Duck>();
}

// Performs a search to ensure that insert keeps next_in_quadrant in sprite_index order
static void SpriteSpatialInsert(EntityBase* sprite, const CoordsXY& newLoc)
{
    size_t newIndex = GetSpatialIndexOffset(newLoc);
    auto& spatialVector = gSpriteSpatialIndex[newIndex];
    auto index = std::lower_bound(std::begin(spatialVector), std::end(spatialVector), sprite->sprite_index);
    spatialVector.insert(index, sprite->sprite_index);
}

static void SpriteSpatialRemove(EntityBase* sprite)
{
    size_t currentIndex = GetSpatialIndexOffset({ sprite->x, sprite->y });
    auto& spatialVector = gSpriteSpatialIndex[currentIndex];
    auto index = std::lower_bound(std::begin(spatialVector), std::end(spatialVector), sprite->sprite_index);
    if (index != std::end(spatialVector) && *index == sprite->sprite_index)
    {
        spatialVector.erase(index, index + 1);
    }
    else
    {
        log_warning("Bad sprite spatial index. Rebuilding the spatial index...");
        reset_sprite_spatial_index();
    }
}

static void SpriteSpatialMove(EntityBase* sprite, const CoordsXY& newLoc)
{
    size_t newIndex = GetSpatialIndexOffset(newLoc);
    size_t currentIndex = GetSpatialIndexOffset({ sprite->x, sprite->y });
    if (newIndex == currentIndex)
        return;

    SpriteSpatialRemove(sprite);
    SpriteSpatialInsert(sprite, newLoc);
}

void EntityBase::MoveTo(const CoordsXYZ& newLocation)
{
    if (x != LOCATION_NULL)
    {
        // Invalidate old position.
        Invalidate();
    }

    auto loc = newLocation;
    if (!map_is_location_valid(loc))
    {
        loc.x = LOCATION_NULL;
    }

    SpriteSpatialMove(this, loc);

    if (loc.x == LOCATION_NULL)
    {
        x = loc.x;
        y = loc.y;
        z = loc.z;
    }
    else
    {
        sprite_set_coordinates(loc, this);
        Invalidate(); // Invalidate new position.
    }
}

CoordsXYZ EntityBase::GetLocation() const
{
    return { x, y, z };
}

void EntityBase::SetLocation(const CoordsXYZ& newLocation)
{
    x = static_cast<int16_t>(newLocation.x);
    y = static_cast<int16_t>(newLocation.y);
    z = static_cast<int16_t>(newLocation.z);
}

void sprite_set_coordinates(const CoordsXYZ& spritePos, EntityBase* sprite)
{
    auto screenCoords = translate_3d_to_2d_with_z(get_current_rotation(), spritePos);

    sprite->SpriteRect = ScreenRect(
        screenCoords - ScreenCoordsXY{ sprite->sprite_width, sprite->sprite_height_negative },
        screenCoords + ScreenCoordsXY{ sprite->sprite_width, sprite->sprite_height_positive });
    sprite->x = spritePos.x;
    sprite->y = spritePos.y;
    sprite->z = spritePos.z;
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
void sprite_remove(EntityBase* sprite)
{
    FreeEntity(*sprite);

    EntityTweener::Get().RemoveEntity(sprite);
    RemoveFromEntityList(sprite); // remove from existing list
    AddToFreeList(sprite->sprite_index);

    SpriteSpatialRemove(sprite);
    sprite_reset(sprite);
}

/**
 * Loops through all sprites, finds floating objects and removes them.
 * Returns the amount of removed objects as feedback.
 */
uint16_t remove_floating_sprites()
{
    uint16_t removed = 0;
    for (auto* balloon : EntityList<Balloon>())
    {
        sprite_remove(balloon);
        removed++;
    }
    for (auto* duck : EntityList<Duck>())
    {
        if (duck->IsFlying())
        {
            sprite_remove(duck);
            removed++;
        }
    }
    for (auto* money : EntityList<MoneyEffect>())
    {
        sprite_remove(money);
        removed++;
    }
    return removed;
}

void sprite_set_flashing(EntityBase* sprite, bool flashing)
{
    assert(sprite->sprite_index < MAX_ENTITIES);
    _spriteFlashingList[sprite->sprite_index] = flashing;
}

bool sprite_get_flashing(EntityBase* sprite)
{
    assert(sprite->sprite_index < MAX_ENTITIES);
    return _spriteFlashingList[sprite->sprite_index];
}
