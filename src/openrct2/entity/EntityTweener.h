/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../interface/Window.h"
#include "EntityBase.h"

#include <sfl/static_vector.hpp>
#include <vector>

namespace OpenRCT2
{
    struct Viewport;

    // TODO: Move this to somewhere else, currently filters also by zoom.
    using ViewportList = sfl::static_vector<Viewport*, kWindowLimitMax>;

    class EntityTweener
    {
        std::vector<EntityBase*> Entities;
        std::vector<CoordsXYZ> PrePos;
        std::vector<CoordsXYZ> PostPos;

    private:
        void PopulateEntities();
        void AddEntity(const ViewportList& vp, EntityBase* entity);

    public:
        static EntityTweener& Get();

        void PreTick();
        void PostTick();
        void RemoveEntity(EntityBase* entity);
        void Tween(float alpha);
        void Restore();
        void Reset();
    };

} // namespace OpenRCT2
