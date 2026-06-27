/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <memory>

namespace OpenRCT2
{
    struct IContext;
    struct IScene;

    class ISceneManager
    {
    public:
        virtual ~ISceneManager() = default;

        virtual IScene* getActiveScene() = 0;
        virtual IScene* getEditorScene() = 0;
        virtual IScene* getGameScene() = 0;
        virtual IScene* getIntroScene() = 0;
        virtual IScene* getPreloaderScene() = 0;
        virtual IScene* getTitleScene() = 0;
        virtual IScene* getTrackDesignerScene() = 0;
        virtual IScene* getTrackManagerScene() = 0;

        virtual void setActiveScene(IScene* scene) = 0;
    };

    std::unique_ptr<ISceneManager> createSceneManager(IContext* context);
} // namespace OpenRCT2
