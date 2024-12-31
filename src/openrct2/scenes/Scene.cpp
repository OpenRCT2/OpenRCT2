/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Scene.h"

#include "../Context.h"
#include "../GameState.h"

using namespace OpenRCT2;

Scene::Scene(IContext& context)
    : _context(context)
{
}

IContext& Scene::GetContext()
{
    return _context;
}

GameState_t& Scene::GetGameState()
{
    return OpenRCT2::GetGameState();
}

void Scene::FinishScene()
{
    if (_onComplete != nullptr)
    {
        _onComplete();
    }
}

void Scene::SetOnComplete(std::function<void()> onComplete)
{
    _onComplete = onComplete;
}
