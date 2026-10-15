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

#include <sfl/static_vector.hpp>
#include <vector>

namespace OpenRCT2
{
    struct EntityBase;
    struct Viewport;

    // TODO: Move this to somewhere else, currently filters also by zoom.
    using ViewportList = sfl::static_vector<Viewport*, kWindowLimitMax>;

    class EntityTweener
    {
        std::vector<EntityBase*> entities;
        std::vector<CoordsXYZ> prePos;
        std::vector<CoordsXYZ> postPos;

    private:
        void populateEntities();
        void addEntity(const ViewportList& vp, EntityBase* entity);

    public:
        static EntityTweener& get();

        void preTick();
        void postTick();
        void removeEntity(EntityBase* entity);
        void tween(float alpha);
        void restore();
        void reset();
    };

} // namespace OpenRCT2
