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

#include <cstdint>

namespace OpenRCT2::Drawing
{
    struct RenderTarget;
}

namespace OpenRCT2
{
    enum class IntroState : uint8_t
    {
        None,
        PublisherBegin,
        PublisherScroll,
        DeveloperBegin,
        DeveloperScroll,
        LogoFadeIn,
        LogoWait,
        LogoFadeOut,
        Clear = 254,
        Finish = 255,
    };

    class IntroScene final : public Scene
    {
    public:
        using Scene::Scene;

        void Load() override;
        void Tick() override;
        void Stop() override;
    };

    bool IntroIsPlaying();
    void IntroUpdate();
    void IntroDraw(Drawing::RenderTarget& rt);
} // namespace OpenRCT2
