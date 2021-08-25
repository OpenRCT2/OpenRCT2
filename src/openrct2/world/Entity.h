/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "EntityBase.h"

constexpr uint16_t MAX_ENTITIES = 10000;

SpriteBase* try_get_sprite(size_t spriteIndex);
SpriteBase* get_sprite(size_t sprite_idx);

template<typename T = SpriteBase> T* GetEntity(size_t sprite_idx)
{
    auto spr = get_sprite(sprite_idx);
    return spr != nullptr ? spr->As<T>() : nullptr;
}

template<typename T = SpriteBase> T* TryGetEntity(size_t sprite_idx)
{
    auto spr = try_get_sprite(sprite_idx);
    return spr != nullptr ? spr->As<T>() : nullptr;
}

SpriteBase* CreateEntity(EntityType type);
template<typename T> T* CreateEntity()
{
    return static_cast<T*>(CreateEntity(T::cEntityType));
}

// Use only with imports that must happen at a specified index
SpriteBase* CreateEntityAt(const uint16_t index, const EntityType type);
// Use only with imports that must happen at a specified index
template<typename T> T* CreateEntityAt(const uint16_t index)
{
    return static_cast<T*>(CreateEntityAt(index, T::cEntityType));
}
