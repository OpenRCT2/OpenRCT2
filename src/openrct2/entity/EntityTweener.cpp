/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#include "EntityTweener.h"

#include "../entity/Guest.h"
#include "../entity/Staff.h"
#include "../interface/Viewport.h"
#include "../interface/WindowTypes.h"
#include "../ride/Vehicle.h"
#include "EntityList.h"

#include <algorithm>
#include <cmath>

namespace OpenRCT2
{
    static inline ViewportList GetUnzoomedViewports() noexcept
    {
        ViewportList viewports;
        WindowVisitEach([&](WindowBase* w) {
            if (auto* vp = WindowGetViewport(w); vp != nullptr)
            {
                if (!vp->isVisible)
                {
                    // Ignore viewports that are not visible.
                    return;
                }
                if (vp->zoom > ZoomLevel{ 0 })
                {
                    // Ignore viewports that are zoomed out, interpolation wouldn't have much of an effect
                    // due to the loss of detail.
                    return;
                }
                viewports.push_back(vp);
            }
        });
        return viewports;
    }

    static inline bool IsEntityVisible(const ViewportList& vpList, const EntityBase* entity) noexcept
    {
        const auto worldLoc = entity->getLocation();

        for (const auto* vp : vpList)
        {
            const auto screenPos = Translate3DTo2DWithZ(vp->rotation, worldLoc);
            if (vp->Contains(screenPos))
            {
                // Entity is visible in at least one viewport, tween.
                return true;
            }
        }

        return false;
    }

    void EntityTweener::addEntity(const ViewportList& vpList, EntityBase* entity)
    {
        if (!IsEntityVisible(vpList, entity))
        {
            return;
        }

        entities.push_back(entity);
        prePos.emplace_back(entity->getLocation());
    }

    void EntityTweener::populateEntities()
    {
        const auto vpList = GetUnzoomedViewports();
        if (vpList.empty())
        {
            // No viewports that fit the criteria, bail.
            return;
        }

        for (auto ent : EntityList<Guest>())
        {
            addEntity(vpList, ent);
        }
        for (auto ent : EntityList<Staff>())
        {
            addEntity(vpList, ent);
        }
        for (auto ent : EntityList<Vehicle>())
        {
            addEntity(vpList, ent);
        }
    }

    void EntityTweener::preTick()
    {
        restore();
        reset();
        populateEntities();
    }

    void EntityTweener::postTick()
    {
        for (auto* ent : entities)
        {
            if (ent == nullptr)
            {
                // Sprite was removed, add a dummy position to keep the index aligned.
                postPos.emplace_back(0, 0, 0);
            }
            else
            {
                postPos.emplace_back(ent->getLocation());
            }
        }
    }

    static bool CanTweenEntity(EntityBase* ent)
    {
        if (ent->is<Guest>() || ent->is<Staff>() || ent->is<Vehicle>())
            return true;
        return false;
    }

    void EntityTweener::removeEntity(EntityBase* entity)
    {
        if (!CanTweenEntity(entity))
        {
            // Only peeps and vehicles are tweened, bail if type is incorrect.
            return;
        }

        auto it = std::find(entities.begin(), entities.end(), entity);
        if (it != entities.end())
            *it = nullptr;
    }

    void EntityTweener::tween(float alpha)
    {
        const float inv = (1.0f - alpha);
        for (size_t i = 0; i < entities.size(); ++i)
        {
            auto* ent = entities[i];
            if (ent == nullptr)
                continue;

            auto& posA = prePos[i];
            auto& posB = postPos[i];

            if (posA == posB)
                continue;

            ent->moveTo(
                { static_cast<int32_t>(std::round(posB.x * alpha + posA.x * inv)),
                  static_cast<int32_t>(std::round(posB.y * alpha + posA.y * inv)),
                  static_cast<int32_t>(std::round(posB.z * alpha + posA.z * inv)) });
        }
    }

    void EntityTweener::restore()
    {
        for (size_t i = 0; i < entities.size(); ++i)
        {
            auto* ent = entities[i];
            if (ent == nullptr || prePos[i] == postPos[i])
                continue;

            ent->moveTo(postPos[i]);
        }
    }

    void EntityTweener::reset()
    {
        entities.clear();
        prePos.clear();
        postPos.clear();
    }

    static EntityTweener tweener;

    EntityTweener& EntityTweener::get()
    {
        return tweener;
    }

} // namespace OpenRCT2
