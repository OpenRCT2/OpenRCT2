/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "EntityBase.h"

#include <array>
#include <string>

namespace OpenRCT2
{
    union Entity_t
    {
        uint8_t Pad00[0x200];
        EntityBase base;
        Entity_t()
            : Pad00()
        {
        }
    };
} // namespace OpenRCT2

constexpr uint16_t kMaxEntities = 65535;

EntityBase* GetEntity(EntityId entityId);

template<typename T>
T* GetEntity(EntityId entityId)
{
    auto* ent = GetEntity(entityId);
    if (ent == nullptr)
    {
        return nullptr;
    }
    if constexpr (std::is_same_v<T, EntityBase>)
    {
        return ent;
    }
    else
    {
        return ent->As<T>();
    }
}

EntityBase* TryGetEntity(EntityId spriteIndex);

template<typename T>
T* TryGetEntity(EntityId entityId)
{
    auto* ent = TryGetEntity(entityId);
    if (ent == nullptr)
    {
        return nullptr;
    }
    if constexpr (std::is_same_v<T, EntityBase>)
    {
        return ent;
    }
    else
    {
        return ent->As<T>();
    }
}

EntityBase* CreateEntity(EntityType type);

template<typename T>
T* CreateEntity()
{
    return static_cast<T*>(CreateEntity(T::cEntityType));
}

// Use only with imports that must happen at a specified index
EntityBase* CreateEntityAt(const EntityId index, const EntityType type);
// Use only with imports that must happen at a specified index
template<typename T>
T* CreateEntityAt(const EntityId index)
{
    return static_cast<T*>(CreateEntityAt(index, T::cEntityType));
}

void ResetAllEntities();
void ResetEntitySpatialIndices();
void UpdateAllMiscEntities();
void UpdateMoneyEffect();
void EntityRemove(EntityBase* entity);
uint16_t RemoveFloatingEntities();
void UpdateEntitiesSpatialIndex();

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
