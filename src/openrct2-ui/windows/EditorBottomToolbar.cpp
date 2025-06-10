/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/Theme.h"

#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Editor.h>
#include <openrct2/EditorObjectSelectionSession.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/audio/Audio.h>
#include <openrct2/entity/EntityList.h>
#include <openrct2/management/Research.h>
#include <openrct2/scripting/ScriptEngine.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Scenery.h>
#include <string>

namespace OpenRCT2::Ui::Windows
{
    enum
    {
        WIDX_PREVIOUS_IMAGE,       // 1
        WIDX_PREVIOUS_STEP_BUTTON, // 2
        WIDX_NEXT_IMAGE,           // 4
        WIDX_NEXT_STEP_BUTTON,     // 8
    };

    // clang-format off
    static constexpr Widget kEditorBottomToolbarWidgets[] = {
        makeWidget({  0, 0}, {200, 34}, WidgetType::imgBtn,  WindowColour::primary),
        makeWidget({  2, 2}, {196, 30}, WidgetType::flatBtn, WindowColour::primary),
        makeWidget({440, 0}, {200, 34}, WidgetType::imgBtn,  WindowColour::primary),
        makeWidget({442, 2}, {196, 30}, WidgetType::flatBtn, WindowColour::primary),
    };
    // clang-format on

    class EditorBottomToolbarWindow final : public Window
    {
    private:
        using FuncPtr = void (EditorBottomToolbarWindow::*)() const;

        static constexpr StringId kEditorStepNames[] = {
            STR_EDITOR_STEP_OBJECT_SELECTION,       // EditorStep::ObjectSelection
            STR_EDITOR_STEP_LANDSCAPE_EDITOR,       // EditorStep::LandscapeEditor
            STR_EDITOR_STEP_INVENTIONS_LIST_SET_UP, // EditorStep::InventionsListSetUp
            STR_EDITOR_STEP_OPTIONS_SELECTION,      // EditorStep::OptionsSelection
            STR_EDITOR_STEP_OBJECTIVE_SELECTION,    // EditorStep::ObjectiveSelection
            STR_EDITOR_STEP_SCENARIO_DETAILS,       // EditorStep::ScenarioDetails
            STR_EDITOR_STEP_SAVE_SCENARIO,          // EditorStep::SaveScenario
            STR_EDITOR_STEP_ROLLERCOASTER_DESIGNER, // EditorStep::RollercoasterDesigner
            STR_EDITOR_STEP_TRACK_DESIGNS_MANAGER,  // EditorStep::DesignsManager
        };

    public:
        void OnOpen() override
        {
            SetWidgets(kEditorBottomToolbarWidgets);

            InitScrollWidgets();
            SetAllSceneryItemsInvented();
        }

        bool GameHasEntities() const
        {
            return GetNumFreeEntities() != kMaxEntities || getGameState().park.Flags & PARK_FLAGS_SPRITES_INITIALISED;
        }

        void OnPrepareDraw() override
        {
            ColourSchemeUpdateByClass(
                this,
                gLegacyScene == LegacyScene::scenarioEditor ? WindowClass::EditorScenarioBottomToolbar
                                                            : WindowClass::EditorTrackBottomToolbar);

            uint16_t screenWidth = ContextGetWidth();
            widgets[WIDX_NEXT_IMAGE].left = screenWidth - 200;
            widgets[WIDX_NEXT_IMAGE].right = screenWidth - 1;
            widgets[WIDX_NEXT_STEP_BUTTON].left = screenWidth - 198;
            widgets[WIDX_NEXT_STEP_BUTTON].right = screenWidth - 3;

            widgets[WIDX_PREVIOUS_STEP_BUTTON].type = WidgetType::flatBtn;
            widgets[WIDX_NEXT_STEP_BUTTON].type = WidgetType::flatBtn;
            widgets[WIDX_PREVIOUS_IMAGE].type = WidgetType::imgBtn;
            widgets[WIDX_NEXT_IMAGE].type = WidgetType::imgBtn;

            auto& gameState = getGameState();
            if (gLegacyScene == LegacyScene::trackDesignsManager || gameState.editorStep == EditorStep::SaveScenario)
            {
                HidePreviousStepButton();
                HideNextStepButton();
            }
            else
            {
                if (gameState.editorStep == EditorStep::ObjectSelection
                    || (GameHasEntities() && gameState.editorStep == EditorStep::OptionsSelection))
                {
                    HidePreviousStepButton();
                }
                else if (gameState.editorStep == EditorStep::RollercoasterDesigner)
                {
                    HideNextStepButton();
                }
            }
        }

        void OnDraw(RenderTarget& rt) override
        {
            auto drawPreviousButton = widgets[WIDX_PREVIOUS_STEP_BUTTON].type != WidgetType::empty;
            auto drawNextButton = widgets[WIDX_NEXT_STEP_BUTTON].type != WidgetType::empty;

            if (drawPreviousButton)
                DrawLeftButtonBack(rt);

            if (drawNextButton)
                DrawRightButtonBack(rt);

            DrawWidgets(rt);

            if (drawPreviousButton)
                DrawLeftButton(rt);

            if (drawNextButton)
                DrawRightButton(rt);

            DrawStepText(rt);
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            auto& gameState = getGameState();
            if (widgetIndex == WIDX_PREVIOUS_STEP_BUTTON)
            {
                ((this)->*(kPreviousButtonMouseUp[EnumValue(gameState.editorStep)]))();
            }
            else if (widgetIndex == WIDX_NEXT_STEP_BUTTON)
            {
                ((this)->*(kNextButtonMouseUp[EnumValue(gameState.editorStep)]))();
            }
        }

    private:
        void JumpBackToObjectSelection() const
        {
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->CloseAll();

            getGameState().editorStep = EditorStep::ObjectSelection;
            GfxInvalidateScreen();
        }

        void JumpBackToLandscapeEditor() const
        {
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->CloseAll();

            SetAllSceneryItemsInvented();
            WindowScenerySetDefaultPlacementConfiguration();
            getGameState().editorStep = EditorStep::LandscapeEditor;
            ContextOpenWindow(WindowClass::Map);
            GfxInvalidateScreen();
        }

        void JumpBackToInventionListSetUp() const
        {
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->CloseAll();

            ContextOpenWindow(WindowClass::EditorInventionList);
            getGameState().editorStep = EditorStep::InventionsListSetUp;
            GfxInvalidateScreen();
        }

        void JumpBackToObjectiveSelection() const
        {
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->CloseAll();

            ContextOpenWindow(WindowClass::EditorScenarioOptions);
            getGameState().editorStep = EditorStep::ObjectiveSelection;
            GfxInvalidateScreen();
        }

        void JumpBackToOptionsSelection() const
        {
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->CloseAll();

            ContextOpenWindow(WindowClass::EditorScenarioOptions);
            getGameState().editorStep = EditorStep::OptionsSelection;
            GfxInvalidateScreen();
        }

        void JumpForwardFromObjectSelection() const
        {
            if (!EditorObjectSelectionWindowCheck())
                return;

            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->CloseByClass(WindowClass::EditorObjectSelection);

            FinishObjectSelection();
            if (gLegacyScene == LegacyScene::trackDesigner)
            {
                ContextOpenWindow(WindowClass::ConstructRide);
            }
            else
            {
                ContextOpenWindow(WindowClass::Map);
                ContextOpenWindow(WindowClass::Mapgen);
            }
        }

        void JumpForwardToInventionListSetUp() const
        {
            auto [checksPassed, errorString] = Editor::CheckPark();
            if (checksPassed)
            {
                auto* windowMgr = Ui::GetWindowManager();
                windowMgr->CloseAll();
                ContextOpenWindow(WindowClass::EditorInventionList);
                getGameState().editorStep = EditorStep::InventionsListSetUp;
            }
            else
            {
                ContextShowError(STR_CANT_ADVANCE_TO_NEXT_EDITOR_STAGE, errorString, {});
            }

            GfxInvalidateScreen();
        }

        void JumpForwardToObjectiveSelection() const
        {
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->CloseAll();

            ContextOpenWindow(WindowClass::EditorScenarioOptions);
            getGameState().editorStep = EditorStep::ObjectiveSelection;
            GfxInvalidateScreen();
        }

        void JumpForwardToOptionsSelection() const
        {
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->CloseAll();

            ContextOpenWindow(WindowClass::EditorScenarioOptions);
            getGameState().editorStep = EditorStep::OptionsSelection;
            GfxInvalidateScreen();
        }

        void JumpForwardToScenarioDetails() const
        {
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->CloseAll();

            ContextOpenWindow(WindowClass::EditorScenarioOptions);
            getGameState().editorStep = EditorStep::ScenarioDetails;
            GfxInvalidateScreen();
        }

        static void SaveScenarioCallback(ModalResult result, const utf8* path)
        {
            if (result == ModalResult::ok)
            {
                GameUnloadScripts();
            }
            else
            {
                getGameState().editorStep = EditorStep::ScenarioDetails;
            }
        }

        void JumpForwardToSaveScenario() const
        {
            auto& gameState = getGameState();
            gameState.editorStep = EditorStep::SaveScenario;
            GfxInvalidateScreen();

            const auto savePrepareResult = ScenarioPrepareForSave(gameState);
            if (!savePrepareResult.Successful)
            {
                ContextShowError(STR_UNABLE_TO_SAVE_SCENARIO_FILE, savePrepareResult.Message, {});
                return;
            }

#ifdef ENABLE_SCRIPTING
            // Clear the plugin storage before saving
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.ClearParkStorage();
#endif

            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->CloseAll();

            auto intent = Intent(WindowClass::Loadsave);
            intent.PutEnumExtra<LoadSaveAction>(INTENT_EXTRA_LOADSAVE_ACTION, LoadSaveAction::save);
            intent.PutEnumExtra<LoadSaveType>(INTENT_EXTRA_LOADSAVE_TYPE, LoadSaveType::scenario);
            intent.PutExtra(INTENT_EXTRA_PATH, gameState.scenarioName);
            intent.PutExtra(INTENT_EXTRA_CALLBACK, reinterpret_cast<CloseCallback>(SaveScenarioCallback));
            ContextOpenIntent(&intent);
        }

        void HidePreviousStepButton()
        {
            widgets[WIDX_PREVIOUS_STEP_BUTTON].type = WidgetType::empty;
            widgets[WIDX_PREVIOUS_IMAGE].type = WidgetType::empty;
        }

        void HideNextStepButton()
        {
            widgets[WIDX_NEXT_STEP_BUTTON].type = WidgetType::empty;
            widgets[WIDX_NEXT_IMAGE].type = WidgetType::empty;
        }

        void DrawLeftButtonBack(RenderTarget& rt)
        {
            const auto& previousWidget = widgets[WIDX_PREVIOUS_IMAGE];
            auto leftTop = windowPos + ScreenCoordsXY{ previousWidget.left, previousWidget.top };
            auto rightBottom = windowPos + ScreenCoordsXY{ previousWidget.right, previousWidget.bottom };
            GfxFilterRect(rt, { leftTop, rightBottom }, FilterPaletteID::Palette51);
        }

        void DrawLeftButton(RenderTarget& rt)
        {
            const auto topLeft = windowPos
                + ScreenCoordsXY{ widgets[WIDX_PREVIOUS_IMAGE].left + 1, widgets[WIDX_PREVIOUS_IMAGE].top + 1 };
            const auto bottomRight = windowPos
                + ScreenCoordsXY{ widgets[WIDX_PREVIOUS_IMAGE].right - 1, widgets[WIDX_PREVIOUS_IMAGE].bottom - 1 };
            GfxFillRectInset(rt, { topLeft, bottomRight }, colours[1], INSET_RECT_F_30);

            GfxDrawSprite(
                rt, ImageId(SPR_PREVIOUS),
                windowPos + ScreenCoordsXY{ widgets[WIDX_PREVIOUS_IMAGE].left + 6, widgets[WIDX_PREVIOUS_IMAGE].top + 6 });

            colour_t textColour = colours[1].colour;
            if (gHoverWidget.window_classification == WindowClass::BottomToolbar
                && gHoverWidget.widget_index == WIDX_PREVIOUS_STEP_BUTTON)
            {
                textColour = COLOUR_WHITE;
            }

            int16_t textX = (widgets[WIDX_PREVIOUS_IMAGE].left + 30 + widgets[WIDX_PREVIOUS_IMAGE].right) / 2 + windowPos.x;
            int16_t textY = widgets[WIDX_PREVIOUS_IMAGE].top + 6 + windowPos.y;

            StringId stringId = kEditorStepNames[EnumValue(getGameState().editorStep) - 1];
            if (gLegacyScene == LegacyScene::trackDesigner)
                stringId = STR_EDITOR_STEP_OBJECT_SELECTION;

            DrawTextBasic(rt, { textX, textY }, STR_BACK_TO_PREVIOUS_STEP, {}, { textColour, TextAlignment::CENTRE });
            DrawTextBasic(rt, { textX, textY + 10 }, stringId, {}, { textColour, TextAlignment::CENTRE });
        }

        void DrawRightButtonBack(RenderTarget& rt)
        {
            auto nextWidget = widgets[WIDX_NEXT_IMAGE];
            auto leftTop = windowPos + ScreenCoordsXY{ nextWidget.left, nextWidget.top };
            auto rightBottom = windowPos + ScreenCoordsXY{ nextWidget.right, nextWidget.bottom };
            GfxFilterRect(rt, { leftTop, rightBottom }, FilterPaletteID::Palette51);
        }

        void DrawRightButton(RenderTarget& rt)
        {
            const auto topLeft = windowPos
                + ScreenCoordsXY{ widgets[WIDX_NEXT_IMAGE].left + 1, widgets[WIDX_NEXT_IMAGE].top + 1 };
            const auto bottomRight = windowPos
                + ScreenCoordsXY{ widgets[WIDX_NEXT_IMAGE].right - 1, widgets[WIDX_NEXT_IMAGE].bottom - 1 };
            GfxFillRectInset(rt, { topLeft, bottomRight }, colours[1], INSET_RECT_F_30);

            GfxDrawSprite(
                rt, ImageId(SPR_NEXT),
                windowPos + ScreenCoordsXY{ widgets[WIDX_NEXT_IMAGE].right - 29, widgets[WIDX_NEXT_IMAGE].top + 6 });

            colour_t textColour = colours[1].colour;

            if (gHoverWidget.window_classification == WindowClass::BottomToolbar
                && gHoverWidget.widget_index == WIDX_NEXT_STEP_BUTTON)
            {
                textColour = COLOUR_WHITE;
            }

            int16_t textX = (widgets[WIDX_NEXT_IMAGE].left + widgets[WIDX_NEXT_IMAGE].right - 30) / 2 + windowPos.x;
            int16_t textY = widgets[WIDX_NEXT_IMAGE].top + 6 + windowPos.y;

            StringId stringId = kEditorStepNames[EnumValue(getGameState().editorStep) + 1];
            if (gLegacyScene == LegacyScene::trackDesigner)
                stringId = STR_EDITOR_STEP_ROLLERCOASTER_DESIGNER;

            DrawTextBasic(rt, { textX, textY }, STR_FORWARD_TO_NEXT_STEP, {}, { textColour, TextAlignment::CENTRE });
            DrawTextBasic(rt, { textX, textY + 10 }, stringId, {}, { textColour, TextAlignment::CENTRE });
        }

        void DrawStepText(RenderTarget& rt)
        {
            int16_t stateX = (widgets[WIDX_PREVIOUS_IMAGE].right + widgets[WIDX_NEXT_IMAGE].left) / 2 + windowPos.x;
            int16_t stateY = height - 0x0C + windowPos.y;
            auto colour = colours[2].withFlag(ColourFlag::translucent, false).withFlag(ColourFlag::withOutline, true);
            DrawTextBasic(
                rt, { stateX, stateY }, kEditorStepNames[EnumValue(getGameState().editorStep)], {},
                { colour, TextAlignment::CENTRE });
        }

        static constexpr FuncPtr kPreviousButtonMouseUp[] = {
            /* ObjectSelection       */ nullptr,
            /* LandscapeEditor       */ &EditorBottomToolbarWindow::JumpBackToObjectSelection,
            /* InventionsListSetUp   */ &EditorBottomToolbarWindow::JumpBackToLandscapeEditor,
            /* OptionsSelection      */ &EditorBottomToolbarWindow::JumpBackToInventionListSetUp,
            /* ObjectiveSelection    */ &EditorBottomToolbarWindow::JumpBackToOptionsSelection,
            /* ScenarioDetails       */ &EditorBottomToolbarWindow::JumpBackToObjectiveSelection,
            /* SaveScenario          */ nullptr,
            /* RollercoasterDesigner */ &EditorBottomToolbarWindow::JumpBackToObjectSelection,
            /* DesignsManager        */ nullptr,
        };

        static constexpr FuncPtr kNextButtonMouseUp[] = {
            /* ObjectSelection       */ &EditorBottomToolbarWindow::JumpForwardFromObjectSelection,
            /* LandscapeEditor       */ &EditorBottomToolbarWindow::JumpForwardToInventionListSetUp,
            /* InventionsListSetUp   */ &EditorBottomToolbarWindow::JumpForwardToOptionsSelection,
            /* OptionsSelection      */ &EditorBottomToolbarWindow::JumpForwardToObjectiveSelection,
            /* ObjectiveSelection    */ &EditorBottomToolbarWindow::JumpForwardToScenarioDetails,
            /* ScenarioDetails       */ &EditorBottomToolbarWindow::JumpForwardToSaveScenario,
            /* SaveScenario          */ nullptr,
            /* RollercoasterDesigner */ nullptr,
            /* DesignsManager        */ nullptr,
        };
    };

    /**
     * Creates the main editor top toolbar window.
     * rct2: 0x0066F052 (part of 0x0066EF38)
     */
    WindowBase* EditorBottomToolbarOpen()
    {
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->Create<EditorBottomToolbarWindow>(
            WindowClass::BottomToolbar, ScreenCoordsXY(0, ContextGetHeight() - 32), { ContextGetWidth(), 32 },
            WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_NO_BACKGROUND | WF_NO_TITLE_BAR);

        return window;
    }
} // namespace OpenRCT2::Ui::Windows
