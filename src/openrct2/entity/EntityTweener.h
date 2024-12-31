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

#include <vector>

class EntityTweener
{
    std::vector<EntityBase*> Entities;
    std::vector<CoordsXYZ> PrePos;
    std::vector<CoordsXYZ> PostPos;

private:
    void PopulateEntities();
    void AddEntity(EntityBase* entity);

public:
    static EntityTweener& Get();

    void PreTick();
    void PostTick();
    void RemoveEntity(EntityBase* entity);
    void Tween(float alpha);
    void Restore();
    void Reset();
};
