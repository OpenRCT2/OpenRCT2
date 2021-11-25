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
