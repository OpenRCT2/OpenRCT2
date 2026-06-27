/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../core/StringTypes.h"
#include "../Scene.h"

enum class ModalResult : int8_t;

namespace OpenRCT2
{
    class EditorScene : public Scene
    {
    public:
        using Scene::Scene;

        void Load() override;
        void Tick() override;
        void Stop() override;

        void ConvertSaveToScenario();
        bool LoadLandscape(const utf8* path);

        static void OpenEditorWindows();
        static void OpenWindowsForCurrentStep();
        static void FinaliseMainView();
        static void resetMainViewport();

    protected:
        void SetAllLandOwned();

    private:
        void AfterLoadCleanup();
        void clearMapForEditing();
        void clearFinances();
    };
} // namespace OpenRCT2
