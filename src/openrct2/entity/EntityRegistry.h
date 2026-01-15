/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/EnumUtils.hpp"
#include "../world/MapLimits.h"
#include "EntityBase.h"

#include <array>
#include <list>
#include <string>
#include <vector>

namespace OpenRCT2
{
    constexpr uint16_t kMaxEntities = 65535;
    constexpr uint16_t kMaxMiscEntities = 3200;

    constexpr const uint32_t kSpatialIndexSize = (kMaximumMapSizeTechnical * kMaximumMapSizeTechnical) + 1;
    constexpr uint32_t kSpatialIndexNullBucket = kSpatialIndexSize - 1;

    constexpr uint32_t kInvalidSpatialIndex = 0xFFFFFFFFu;
    constexpr uint32_t kSpatialIndexDirtyMask = 1u << 31;

    union Entity_t
    {
        uint8_t Pad00[0x200];
        EntityBase base;
        Entity_t()
            : Pad00()
        {
        }
    };

#pragma pack(push, 1)
    struct EntitiesChecksum
    {
        std::array<std::byte, 20> raw;

        std::string ToString() const;
    };
#pragma pack(pop)

    template<typename T>
    class EntityList;

    class EntityRegistry
    {
    private:
        Entity_t entities[kMaxEntities]{};
        std::array<std::list<EntityId>, EnumValue(EntityType::count)> gEntityLists;
        std::vector<EntityId> _freeIdList;

        bool _entityFlashingList[kMaxEntities];

        std::array<std::vector<EntityId>, kSpatialIndexSize> gEntitySpatialIndex;

    public:
        uint16_t GetEntityListCount(EntityType type);
        uint16_t GetNumFreeEntities();

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

        const std::vector<EntityId>& GetEntityTileList(const CoordsXY& spritePos);

        EntityBase* CreateEntity(EntityType type);

        template<typename T>
        T* CreateEntity()
        {
            return static_cast<T*>(CreateEntity(T::cEntityType));
        }

        // Use only with imports that must happen at a specified index
        EntityBase* CreateEntityAt(EntityId index, EntityType type);
        // Use only with imports that must happen at a specified index
        template<typename T>
        T* CreateEntityAt(EntityId index)
        {
            return static_cast<T*>(CreateEntityAt(index, T::cEntityType));
        }

        const std::list<EntityId>& GetEntityList(EntityType id);
        uint16_t GetMiscEntityCount();

        void ResetAllEntities();
        void ResetEntitySpatialIndices();

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

#endif // DISABLE_NETWORK

        EntitiesChecksum GetAllEntitiesChecksum();

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

        void UpdateAllMiscEntities();
        void UpdateMoneyEffect();
        void EntityRemove(EntityBase* entity);
        uint16_t RemoveFloatingEntities();
        void UpdateEntitiesSpatialIndex();
        void UpdateEntitySpatialIndex(EntityBase& entity);

        void EntitySetFlashing(EntityBase* entity, bool flashing);
        bool EntityGetFlashing(EntityBase* entity);

    private:
        void ResetEntityLists();
        void ResetFreeIds();
        void EntityReset(EntityBase& entity);
        void AddToEntityList(EntityBase& entity);
        void AddToFreeList(EntityId index);
        void RemoveFromEntityList(EntityBase& entity);
        void PrepareNewEntity(EntityBase& base, EntityType type);
        void EntitySpatialInsert(EntityBase& entity, const CoordsXY& newLoc);
        void EntitySpatialRemove(EntityBase& entity);
        void FreeEntity(EntityBase& entity);
    };

} // namespace OpenRCT2
