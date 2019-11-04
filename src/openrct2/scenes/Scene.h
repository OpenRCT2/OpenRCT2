/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
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
    class GameState;
    interface IContext;

    interface IScene
    {
    public:
        virtual ~IScene() = default;

        virtual void Load() = 0;
        virtual void Update() = 0;
        virtual void Stop() = 0;
    };

    class Scene : public IScene
    {
    public:
        Scene(IContext& context);

        GameState* GetGameState();

    protected:
        IContext& _context;
    };

} // namespace OpenRCT2
