#include "Scene.h"

#include "../Context.h"

using namespace OpenRCT2;

Scene::Scene(IContext& context)
    : _context(context)
{
}

IContext& Scene::GetContext()
{
    return _context;
}

GameState* Scene::GetGameState()
{
    return _context.GetGameState();
}

void Scene::FinishScene()
{
    if (_nextScene != nullptr)
    {
        _context.SetActiveScene(_nextScene);
        _nextScene = nullptr;
    }
}

void Scene::SetCompletionScene(IScene* scene)
{
    _nextScene = scene;
}
