/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "EntityBase.h"

#include <array>

constexpr uint16_t MAX_ENTITIES = 65535;

EntityBase* GetEntity(EntityId sprite_idx);

template<typename T> T* GetEntity(EntityId sprite_idx)
{
    auto spr = GetEntity(sprite_idx);
    return spr != nullptr ? spr->As<T>() : nullptr;
}

EntityBase* TryGetEntity(EntityId spriteIndex);

template<typename T> T* TryGetEntity(EntityId sprite_idx)
{
    auto spr = TryGetEntity(sprite_idx);
    return spr != nullptr ? spr->As<T>() : nullptr;
}

EntityBase* CreateEntity(EntityType type);

template<typename T> T* CreateEntity()
{
    return static_cast<T*>(CreateEntity(T::cEntityType));
}

// Use only with imports that must happen at a specified index
EntityBase* CreateEntityAt(const EntityId index, const EntityType type);
// Use only with imports that must happen at a specified index
template<typename T> T* CreateEntityAt(const EntityId index)
{
    return static_cast<T*>(CreateEntityAt(index, T::cEntityType));
}

void ResetAllEntities();
void ResetEntitySpatialIndices();
void UpdateAllMiscEntities();
void EntitySetCoordinates(const CoordsXYZ& entityPos, EntityBase* entity);
void EntityRemove(EntityBase* entity);
uint16_t RemoveFloatingEntities();

#pragma pack(push, 1)
struct EntitiesChecksum
{
    std::array<std::byte, 20> raw;

    std::string ToString() const;
};
#pragma pack(pop)
EntitiesChecksum GetAllEntitiesChecksum();

void EntitySetFlashing(EntityBase* entity, bool flashing);
bool EntityGetFlashing(EntityBase* entity);
