/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

namespace OpenRCT2
{
    struct GameState_t;
    struct IContext;

    struct IScene
    {
    public:
        virtual ~IScene() = default;

        virtual GameState_t& GetGameState() = 0;
        virtual IContext& GetContext() = 0;

        virtual void Load() = 0;
        virtual void Tick() = 0;
        virtual void Stop() = 0;

        virtual IScene* GetCompletionScene() = 0;
        virtual void SetCompletionScene(IScene* scene) = 0;
    };

    class Scene : public IScene
    {
    public:
        Scene(IContext& context);

        GameState_t& GetGameState() override;
        IContext& GetContext() override;

        IScene* GetCompletionScene() override;
        void SetCompletionScene(IScene* scene) override;

    protected:
        void FinishScene();

    protected:
        IContext& _context;
        IScene* _nextScene = nullptr;
    };

} // namespace OpenRCT2
