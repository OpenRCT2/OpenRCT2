/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Theme.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/interface/Window.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/GameState.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/drawing/Text.h>
#include <openrct2/ui/WindowManager.h>

using namespace OpenRCT2::Drawing;

namespace OpenRCT2::Ui::Windows
{
    static constexpr ScreenSize kWindowSize = { 300, 16 };

    static constexpr Widget kEditorBottomToolbarWidgets[] = {
        makeWidget({ 0, 0 }, { 0, 0 }, WidgetType::placeholder, WindowColour::primary),
    };

    class EditorStatusLine final : public Window
    {
    private:
        static constexpr StringId kEditorStepNames[] = {
            STR_EDITOR_STEP_OBJECT_SELECTION,       // Editor::Step::objectSelection
            STR_EDITOR_STEP_LANDSCAPE_EDITOR,       // Editor::Step::landscapeEditor
            STR_EDITOR_STEP_INVENTIONS_LIST_SET_UP, // Editor::Step::inventionsListSetUp
            STR_EDITOR_STEP_OPTIONS_SELECTION,      // Editor::Step::optionsSelection
            STR_EDITOR_STEP_OBJECTIVE_SELECTION,    // Editor::Step::objectiveSelection
            STR_EDITOR_STEP_SCENARIO_DETAILS,       // Editor::Step::scenarioDetails
            STR_EDITOR_STEP_SAVE_SCENARIO,          // Editor::Step::saveScenario
            STR_EDITOR_STEP_ROLLERCOASTER_DESIGNER, // Editor::Step::rollerCoasterDesigner
            STR_EDITOR_STEP_TRACK_DESIGNS_MANAGER,  // Editor::Step::designsManager
        };

    public:
        void onOpen() override
        {
            setWidgets(kEditorBottomToolbarWidgets);
        }

        void onPrepareDraw() override
        {
            ColourSchemeUpdateByClass(
                this,
                gLegacyScene == LegacyScene::scenarioEditor ? WindowClass::editorScenarioBottomToolbar
                                                            : WindowClass::editorTrackBottomToolbar);
        }

        void onDraw(RenderTarget& rt) override
        {
            auto colour = colours[2].withFlag(ColourFlag::translucent, false).withFlag(ColourFlag::withOutline, true);
            auto stringId = kEditorStepNames[EnumValue(getGameState().editorStep)];
            drawText(rt, windowPos + ScreenCoordsXY{ width / 2, height - 12 }, stringId, { colour, TextAlignment::centre });
        }
    };

    WindowBase* editorStatusLineOpen()
    {
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->Create<EditorStatusLine>(
            WindowClass::editorStatusLine,
            { (ContextGetWidth() - kWindowSize.width) / 2, ContextGetHeight() - kWindowSize.height }, kWindowSize,
            { WindowFlag::stickToFront, WindowFlag::transparent, WindowFlag::noBackground, WindowFlag::noTitleBar });

        return window;
    }
} // namespace OpenRCT2::Ui::Windows
