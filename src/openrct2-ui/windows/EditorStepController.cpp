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
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/actions/ResultWithMessage.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/drawing/Text.h>
#include <openrct2/management/Research.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/scenes/editor/EditorController.h>
#include <openrct2/scripting/ScriptEngine.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/windows/Intent.h>
#include <string>

using namespace OpenRCT2::Drawing;

namespace OpenRCT2::Ui::Windows
{
    static constexpr ScreenSize kWindowSize = { 200, 32 };

    enum EditorStepControllerWidgetIdx : WidgetIndex
    {
        WIDX_IMAGE,
        WIDX_STEP_BUTTON,
    };

    // clang-format off
    static constexpr Widget kEditorStepControllerWidgets[] = {
        makeWidget({  0, 0}, {200, 34}, WidgetType::imgBtn,  WindowColour::primary),
        makeWidget({  2, 2}, {196, 30}, WidgetType::flatBtn, WindowColour::primary),
    };
    // clang-format on

    enum class StepDirection : uint8_t
    {
        previous,
        next,
    };

    class EditorStepController final : public Window
    {
    private:
        using FuncPtr = void (*)();

    public:
        StepDirection direction;

        void onOpen() override
        {
            setWidgets(kEditorStepControllerWidgets);

            initScrollWidgets();
            SetAllSceneryItemsInvented();
        }

        bool GameHasEntities() const
        {
            auto& gameState = getGameState();
            return gameState.entities.GetNumFreeEntities() != kMaxEntities
                || gameState.park.flags.has(ParkFlag::spritesInitialised);
        }

        void onPrepareDraw() override
        {
            ColourSchemeUpdateByClass(
                this,
                gLegacyScene == LegacyScene::scenarioEditor ? WindowClass::editorScenarioBottomToolbar
                                                            : WindowClass::editorTrackBottomToolbar);

            auto& gameState = getGameState();
            bool stepVisible = !(
                gLegacyScene == LegacyScene::trackDesignsManager || gameState.editorStep == Editor::Step::saveScenario);
            if (stepVisible)
            {
                if (gameState.editorStep == Editor::Step::objectSelection
                    || (GameHasEntities() && gameState.editorStep == Editor::Step::optionsSelection))
                {
                    stepVisible = direction == StepDirection::next;
                }
                else if (gameState.editorStep == Editor::Step::rollerCoasterDesigner)
                {
                    stepVisible = direction == StepDirection::previous;
                }
            }

            widgets[WIDX_STEP_BUTTON].setVisible(stepVisible);
            widgets[WIDX_IMAGE].setVisible(stepVisible);
        }

        void onDraw(RenderTarget& rt) override
        {
            if (!widgets[WIDX_STEP_BUTTON].isVisible())
                return;

            drawButtonBackground(rt);
            drawWidgets(rt);
            drawButtonCaption(rt);
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            auto& gameState = getGameState();
            if (direction == StepDirection::previous)
            {
                kPreviousButtonMouseUp[EnumValue(gameState.editorStep)]();
            }
            else if (direction == StepDirection::next)
            {
                kNextButtonMouseUp[EnumValue(gameState.editorStep)]();
            }
        }

    private:
        static void jumpBackToObjectSelection()
        {
            auto* windowMgr = GetWindowManager();
            windowMgr->CloseAll();

            getGameState().editorStep = Editor::Step::objectSelection;
            GfxInvalidateScreen();
        }

        static void jumpBackToLandscapeEditor()
        {
            auto* windowMgr = GetWindowManager();
            windowMgr->CloseAll();

            SetAllSceneryItemsInvented();
            WindowScenerySetDefaultPlacementConfiguration();
            getGameState().editorStep = Editor::Step::landscapeEditor;
            ContextOpenWindow(WindowClass::map);
            GfxInvalidateScreen();
        }

        static void jumpBackToInventionListSetUp()
        {
            auto* windowMgr = GetWindowManager();
            windowMgr->CloseAll();

            ContextOpenWindow(WindowClass::editorInventionList);
            getGameState().editorStep = Editor::Step::inventionsListSetUp;
            GfxInvalidateScreen();
        }

        static void jumpBackToObjectiveSelection()
        {
            auto* windowMgr = GetWindowManager();
            windowMgr->CloseAll();

            ContextOpenWindow(WindowClass::editorScenarioOptions);
            getGameState().editorStep = Editor::Step::objectiveSelection;
            GfxInvalidateScreen();
        }

        static void jumpBackToOptionsSelection()
        {
            auto* windowMgr = GetWindowManager();
            windowMgr->CloseAll();

            ContextOpenWindow(WindowClass::editorScenarioOptions);
            getGameState().editorStep = Editor::Step::optionsSelection;
            GfxInvalidateScreen();
        }

        static void jumpForwardFromObjectSelection()
        {
            if (!EditorObjectSelectionWindowCheck())
                return;

            auto* windowMgr = GetWindowManager();
            windowMgr->CloseByClass(WindowClass::editorObjectSelection);

            Editor::FinishObjectSelection();
            if (gLegacyScene == LegacyScene::trackDesigner)
            {
                ContextOpenWindow(WindowClass::constructRide);
            }
            else
            {
                ContextOpenWindow(WindowClass::map);
                ContextOpenWindow(WindowClass::mapgen);
            }
        }

        static void jumpForwardToInventionListSetUp()
        {
            auto [checksPassed, errorString] = Editor::CheckPark();
            if (checksPassed)
            {
                auto* windowMgr = GetWindowManager();
                windowMgr->CloseAll();
                ContextOpenWindow(WindowClass::editorInventionList);
                getGameState().editorStep = Editor::Step::inventionsListSetUp;
            }
            else
            {
                ContextShowError(STR_CANT_ADVANCE_TO_NEXT_EDITOR_STAGE, errorString, {});
            }

            GfxInvalidateScreen();
        }

        static void jumpForwardToObjectiveSelection()
        {
            auto* windowMgr = GetWindowManager();
            windowMgr->CloseAll();

            ContextOpenWindow(WindowClass::editorScenarioOptions);
            getGameState().editorStep = Editor::Step::objectiveSelection;
            GfxInvalidateScreen();
        }

        static void jumpForwardToOptionsSelection()
        {
            auto* windowMgr = GetWindowManager();
            windowMgr->CloseAll();

            ContextOpenWindow(WindowClass::editorScenarioOptions);
            getGameState().editorStep = Editor::Step::optionsSelection;
            GfxInvalidateScreen();
        }

        static void jumpForwardToScenarioDetails()
        {
            auto* windowMgr = GetWindowManager();
            windowMgr->CloseAll();

            ContextOpenWindow(WindowClass::editorScenarioOptions);
            getGameState().editorStep = Editor::Step::scenarioDetails;
            GfxInvalidateScreen();
        }

        static void saveScenarioCallback(ModalResult result, const utf8* path)
        {
            if (result == ModalResult::ok)
            {
                GameUnloadScripts();
            }
            else
            {
                getGameState().editorStep = Editor::Step::scenarioDetails;
            }
        }

        static void jumpForwardToSaveScenario()
        {
            auto& gameState = getGameState();
            gameState.editorStep = Editor::Step::saveScenario;
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

            auto* windowMgr = GetWindowManager();
            windowMgr->CloseAll();

            auto intent = Intent(WindowClass::loadsave);
            intent.PutEnumExtra<LoadSaveAction>(INTENT_EXTRA_LOADSAVE_ACTION, LoadSaveAction::save);
            intent.PutEnumExtra<LoadSaveType>(INTENT_EXTRA_LOADSAVE_TYPE, LoadSaveType::scenario);
            intent.PutExtra(INTENT_EXTRA_PATH, gameState.scenarioOptions.name);
            intent.PutExtra(INTENT_EXTRA_CALLBACK, reinterpret_cast<CloseCallback>(saveScenarioCallback));
            ContextOpenIntent(&intent);
        }

        void drawButtonBackground(RenderTarget& rt)
        {
            const auto& widget = widgets[WIDX_IMAGE];
            auto leftTop = windowPos + ScreenCoordsXY{ widget.left, widget.top };
            auto rightBottom = windowPos + ScreenCoordsXY{ widget.right, widget.bottom };
            Rectangle::filter(rt, { leftTop, rightBottom }, FilterPaletteID::palette51);
        }

        struct StepFrame
        {
            StringId label;
            uint32_t image;
            ScreenCoordsXY labelOffset;
            ScreenCoordsXY imageOffset;
        };

        static constexpr std::array kStepFrames = std::to_array<StepFrame>({
            { STR_BACK_TO_PREVIOUS_STEP, SPR_PREVIOUS, { (kWindowSize.width + 30) / 2, 6 }, { 6, 6 } },
            { STR_FORWARD_TO_NEXT_STEP, SPR_NEXT, { (kWindowSize.width - 31) / 2, 6 }, { kWindowSize.width - 29, 6 } },
        });

        void drawButtonCaption(RenderTarget& rt)
        {
            const auto topLeft = windowPos + ScreenCoordsXY{ widgets[WIDX_IMAGE].left + 1, widgets[WIDX_IMAGE].top + 1 };
            const auto bottomRight = windowPos
                + ScreenCoordsXY{ widgets[WIDX_IMAGE].right - 1, widgets[WIDX_IMAGE].bottom - 1 };
            Rectangle::fillInset(
                rt, { topLeft, bottomRight }, colours[1], Rectangle::BorderStyle::inset, Rectangle::FillBrightness::light,
                Rectangle::FillMode::none);

            const bool isPrevious = direction == StepDirection::previous;
            const auto& layout = isPrevious ? kStepFrames[0] : kStepFrames[1];

            GfxDrawSprite(rt, ImageId(layout.image), windowPos + layout.imageOffset);

            Drawing::Colour textColour = colours[1].colour;
            if (widgetIsHighlighted(*this, WIDX_STEP_BUTTON))
            {
                textColour = Drawing::Colour::white;
            }

            drawText(rt, windowPos + layout.labelOffset, layout.label, { textColour, TextAlignment::centre });

            auto step = Editor::Step(EnumValue(getGameState().editorStep) + (isPrevious ? -1 : 1));
            auto stringId = Editor::getStepStringId(step);

            if (isPrevious && gLegacyScene == LegacyScene::trackDesigner)
                stringId = STR_EDITOR_STEP_OBJECT_SELECTION;
            else if (!isPrevious && gLegacyScene == LegacyScene::trackDesigner)
                stringId = STR_EDITOR_STEP_ROLLERCOASTER_DESIGNER;

            drawText(
                rt, windowPos + layout.labelOffset + ScreenCoordsXY{ 0, 10 }, stringId, { textColour, TextAlignment::centre });
        }

        static constexpr FuncPtr kPreviousButtonMouseUp[] = {
            /* ObjectSelection       */ nullptr,
            /* LandscapeEditor       */ &EditorStepController::jumpBackToObjectSelection,
            /* InventionsListSetUp   */ &EditorStepController::jumpBackToLandscapeEditor,
            /* OptionsSelection      */ &EditorStepController::jumpBackToInventionListSetUp,
            /* ObjectiveSelection    */ &EditorStepController::jumpBackToOptionsSelection,
            /* ScenarioDetails       */ &EditorStepController::jumpBackToObjectiveSelection,
            /* SaveScenario          */ nullptr,
            /* RollercoasterDesigner */ &EditorStepController::jumpBackToObjectSelection,
            /* DesignsManager        */ nullptr,
        };

        static constexpr FuncPtr kNextButtonMouseUp[] = {
            /* ObjectSelection       */ &EditorStepController::jumpForwardFromObjectSelection,
            /* LandscapeEditor       */ &EditorStepController::jumpForwardToInventionListSetUp,
            /* InventionsListSetUp   */ &EditorStepController::jumpForwardToOptionsSelection,
            /* OptionsSelection      */ &EditorStepController::jumpForwardToObjectiveSelection,
            /* ObjectiveSelection    */ &EditorStepController::jumpForwardToScenarioDetails,
            /* ScenarioDetails       */ &EditorStepController::jumpForwardToSaveScenario,
            /* SaveScenario          */ nullptr,
            /* RollercoasterDesigner */ nullptr,
            /* DesignsManager        */ nullptr,
        };
    };

    /**
     * Creates the main editor bottom toolbar window.
     * rct2: 0x0066F052 (part of 0x0066EF38)
     */
    WindowBase* editorStepControllerOpen()
    {
        auto direction = StepDirection::previous;
        auto* windowMgr = GetWindowManager();
        if (windowMgr->FindByNumber(WindowClass::editorStepController, 0) != nullptr)
        {
            direction = StepDirection::next;
        }

        auto xPos = direction == StepDirection::previous ? 0 : ContextGetWidth() - kWindowSize.width;
        auto* window = windowMgr->Create<EditorStepController>(
            WindowClass::editorStepController, ScreenCoordsXY(xPos, ContextGetHeight() - kWindowSize.height), kWindowSize,
            { WindowFlag::stickToFront, WindowFlag::transparent, WindowFlag::noBackground, WindowFlag::noTitleBar });

        window->direction = direction;
        window->number = EnumValue(direction);

        return window;
    }
} // namespace OpenRCT2::Ui::Windows
