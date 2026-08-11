/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Scene.h"

#include <memory>

namespace OpenRCT2::Drawing
{
    struct RenderTarget;
}

namespace OpenRCT2
{
    class IntroSceneImpl;

    class IntroScene final : public Scene
    {
    public:
        using Scene::Scene;

        IntroScene(IContext& context);

        void Load() override;
        void Tick() override;
        void Stop() override;

    private:
        std::shared_ptr<IntroSceneImpl> _impl;
    };

    bool IntroIsPlaying();
    void IntroDraw(Drawing::RenderTarget& rt);
} // namespace OpenRCT2
