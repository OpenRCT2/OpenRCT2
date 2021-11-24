/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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

#pragma pack(push, 1)
/**
 * Entity structure.
 * size: 0x0200
 */
union Entity
{
    uint8_t pad_00[0x200];
    EntityBase base;
    // Provide a constructor as EntityBase is not trivially constructible
    Entity()
        : pad_00()
    {
    }
};
assert_struct_size(Entity, 0x200);

struct EntitiesChecksum
{
    std::array<std::byte, 20> raw;

    std::string ToString() const;
};

#pragma pack(pop)

void ResetAllEntities();
void ResetEntitySpatialIndices();
void UpdateAllMiscEntities();
void EntitySetCoordinates(const CoordsXYZ& entityPos, EntityBase* entity);
void EntityRemove(EntityBase* entity);
uint16_t RemoveFloatingEntities();

EntitiesChecksum GetAllEntitiesChecksum();

void EntitySetFlashing(EntityBase* entity, bool flashing);
bool EntityGetFlashing(EntityBase* entity);
