/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#include "EntityTweener.h"

#include "../entity/Guest.h"
#include "../entity/Staff.h"
#include "../ride/Vehicle.h"
#include "EntityList.h"
#include "EntityRegistry.h"

#include <algorithm>
#include <cmath>

void EntityTweener::AddEntity(EntityBase* entity)
{
    Entities.push_back(entity);
    PrePos.emplace_back(entity->GetLocation());
}

void EntityTweener::PopulateEntities()
{
    for (auto ent : EntityList<Guest>())
    {
        AddEntity(ent);
    }
    for (auto ent : EntityList<Staff>())
    {
        AddEntity(ent);
    }
    for (auto ent : EntityList<Vehicle>())
    {
        AddEntity(ent);
    }
}

void EntityTweener::PreTick()
{
    Restore();
    Reset();
    PopulateEntities();
}

void EntityTweener::PostTick()
{
    for (auto* ent : Entities)
    {
        if (ent == nullptr)
        {
            // Sprite was removed, add a dummy position to keep the index aligned.
            PostPos.emplace_back(0, 0, 0);
        }
        else
        {
            PostPos.emplace_back(ent->GetLocation());
        }
    }
}

static bool CanTweenEntity(EntityBase* ent)
{
    if (ent->Is<Guest>() || ent->Is<Staff>() || ent->Is<Vehicle>())
        return true;
    return false;
}

void EntityTweener::RemoveEntity(EntityBase* entity)
{
    if (!CanTweenEntity(entity))
    {
        // Only peeps and vehicles are tweened, bail if type is incorrect.
        return;
    }

    auto it = std::find(Entities.begin(), Entities.end(), entity);
    if (it != Entities.end())
        *it = nullptr;
}

void EntityTweener::Tween(float alpha)
{
    const float inv = (1.0f - alpha);
    for (size_t i = 0; i < Entities.size(); ++i)
    {
        auto* ent = Entities[i];
        if (ent == nullptr)
            continue;

        auto& posA = PrePos[i];
        auto& posB = PostPos[i];

        if (posA == posB)
            continue;

        ent->MoveTo({ static_cast<int32_t>(std::round(posB.x * alpha + posA.x * inv)),
                      static_cast<int32_t>(std::round(posB.y * alpha + posA.y * inv)),
                      static_cast<int32_t>(std::round(posB.z * alpha + posA.z * inv)) });
    }
}

void EntityTweener::Restore()
{
    for (size_t i = 0; i < Entities.size(); ++i)
    {
        auto* ent = Entities[i];
        if (ent == nullptr)
            continue;

        ent->MoveTo(PostPos[i]);
    }
}

void EntityTweener::Reset()
{
    Entities.clear();
    PrePos.clear();
    PostPos.clear();
}

static EntityTweener tweener;

EntityTweener& EntityTweener::Get()
{
    return tweener;
}
