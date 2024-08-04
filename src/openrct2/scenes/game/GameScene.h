/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Scene.h"

namespace OpenRCT2
{
    class GameScene final : public Scene
    {
    public:
        using Scene::Scene;

        void Load() override;
        void Tick() override;
        void Stop() override;
    };
} // namespace OpenRCT2
