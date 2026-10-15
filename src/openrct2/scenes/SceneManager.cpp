/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SceneManager.h"

#include "editor/EditorScene.h"
#include "editor/TrackDesignerScene.h"
#include "editor/TrackManagerScene.h"
#include "game/GameScene.h"
#include "intro/IntroScene.h"
#include "preloader/PreloaderScene.h"
#include "title/TitleScene.h"

namespace OpenRCT2
{
    class SceneManager final : public ISceneManager
    {
    private:
        IScene* _activeScene = nullptr;
        IContext* _sceneContext = nullptr;
        std::unique_ptr<GameScene> _gameScene;
        std::unique_ptr<IntroScene> _introScene;
        std::unique_ptr<PreloaderScene> _preloaderScene;
        std::unique_ptr<EditorScene> _scenarioEditorScene;
        std::unique_ptr<TitleScene> _titleScene;
        std::unique_ptr<TrackDesignerScene> _trackDesignerScene;
        std::unique_ptr<TrackManagerScene> _trackManagerScene;

    public:
        explicit SceneManager(IContext* context)
            : _sceneContext(context)
        {
        }

        IScene* getActiveScene() override
        {
            return _activeScene;
        }

        IScene* getGameScene() override
        {
            if (auto* scene = _gameScene.get())
                return scene;

            _gameScene = std::make_unique<GameScene>(*_sceneContext);
            return _gameScene.get();
        }

        IScene* getIntroScene() override
        {
            if (auto* scene = _introScene.get())
                return scene;

            _introScene = std::make_unique<IntroScene>(*_sceneContext);
            return _introScene.get();
        }

        IScene* getPreloaderScene() override
        {
            if (auto* scene = _preloaderScene.get())
                return scene;

            _preloaderScene = std::make_unique<PreloaderScene>(*_sceneContext);
            return _preloaderScene.get();
        }

        IScene* getScenarioEditorScene() override
        {
            if (auto* scene = _scenarioEditorScene.get())
                return scene;

            _scenarioEditorScene = std::make_unique<EditorScene>(*_sceneContext);
            return _scenarioEditorScene.get();
        }

        IScene* getTitleScene() override
        {
            if (auto* scene = _titleScene.get())
                return scene;

            _titleScene = std::make_unique<TitleScene>(*_sceneContext);
            return _titleScene.get();
        }

        IScene* getTrackDesignerScene() override
        {
            if (auto* scene = _trackDesignerScene.get())
                return scene;

            _trackDesignerScene = std::make_unique<TrackDesignerScene>(*_sceneContext);
            return _trackDesignerScene.get();
        }

        IScene* getTrackManagerScene() override
        {
            if (auto* scene = _trackManagerScene.get())
                return scene;

            _trackManagerScene = std::make_unique<TrackManagerScene>(*_sceneContext);
            return _trackManagerScene.get();
        }

        void setActiveScene(IScene* scene) override
        {
            if (_activeScene == scene)
                return;

            if (_activeScene != nullptr)
                _activeScene->Stop();
            _activeScene = scene;
            if (_activeScene)
                _activeScene->Load();
        }
    };

    std::unique_ptr<ISceneManager> createSceneManager(IContext* context)
    {
        return std::make_unique<SceneManager>(context);
    }
} // namespace OpenRCT2
