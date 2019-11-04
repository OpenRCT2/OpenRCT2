#include "Scene.h"

#include "../Context.h"

using namespace OpenRCT2;

Scene::Scene(IContext& context)
    : _context(context)
{
}

GameState* Scene::GetGameState()
{
    return _context.GetGameState();
}
