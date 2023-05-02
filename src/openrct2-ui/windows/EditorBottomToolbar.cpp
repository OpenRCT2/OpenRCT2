/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/Theme.h"

#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Editor.h>
#include <openrct2/EditorObjectSelectionSession.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/audio/audio.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/management/Research.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/sprites.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Scenery.h>
#include <string>

// clang-format off
enum {
    WIDX_PREVIOUS_IMAGE,        // 1
    WIDX_PREVIOUS_STEP_BUTTON,  // 2
    WIDX_NEXT_IMAGE,            // 4
    WIDX_NEXT_STEP_BUTTON,      // 8
};

static Widget _editorBottomToolbarWidgets[] = {
    MakeWidget({  0, 0}, {200, 34}, WindowWidgetType::ImgBtn,  WindowColour::Primary),
    MakeWidget({  2, 2}, {196, 30}, WindowWidgetType::FlatBtn, WindowColour::Primary),
    MakeWidget({440, 0}, {200, 34}, WindowWidgetType::ImgBtn,  WindowColour::Primary),
    MakeWidget({442, 2}, {196, 30}, WindowWidgetType::FlatBtn, WindowColour::Primary),
    WIDGETS_END,
};
// clang-format on

class EditorBottomToolbarWindow final : public Window
{
private:
    using FuncPtr = void (EditorBottomToolbarWindow::*)() const;

    static constexpr const StringId _editorStepNames[] = {
        STR_EDITOR_STEP_OBJECT_SELECTION,       STR_EDITOR_STEP_LANDSCAPE_EDITOR,      STR_EDITOR_STEP_INVENTIONS_LIST_SET_UP,
        STR_EDITOR_STEP_OPTIONS_SELECTION,      STR_EDITOR_STEP_OBJECTIVE_SELECTION,   STR_EDITOR_STEP_SAVE_SCENARIO,
        STR_EDITOR_STEP_ROLLERCOASTER_DESIGNER, STR_EDITOR_STEP_TRACK_DESIGNS_MANAGER,
    };

public:
    void OnOpen() override
    {
        widgets = _editorBottomToolbarWidgets;

        InitScrollWidgets();
        SetAllSceneryItemsInvented();
    }

    void OnPrepareDraw() override
    {
        ColourSchemeUpdateByClass(
            this,
            (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) ? WindowClass::EditorScenarioBottomToolbar
                                                          : WindowClass::EditorTrackBottomToolbar);

        uint16_t screenWidth = ContextGetWidth();
        widgets[WIDX_NEXT_IMAGE].left = screenWidth - 200;
        widgets[WIDX_NEXT_IMAGE].right = screenWidth - 1;
        widgets[WIDX_NEXT_STEP_BUTTON].left = screenWidth - 198;
        widgets[WIDX_NEXT_STEP_BUTTON].right = screenWidth - 3;

        widgets[WIDX_PREVIOUS_STEP_BUTTON].type = WindowWidgetType::FlatBtn;
        widgets[WIDX_NEXT_STEP_BUTTON].type = WindowWidgetType::FlatBtn;
        widgets[WIDX_PREVIOUS_IMAGE].type = WindowWidgetType::ImgBtn;
        widgets[WIDX_NEXT_IMAGE].type = WindowWidgetType::ImgBtn;

        if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
        {
            HidePreviousStepButton();
            HideNextStepButton();
        }
        else
        {
            if (gEditorStep == EditorStep::ObjectSelection)
            {
                HidePreviousStepButton();
            }
            else if (gEditorStep == EditorStep::RollercoasterDesigner)
            {
                HideNextStepButton();
            }
            else if (!(gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER))
            {
                if (GetNumFreeEntities() != MAX_ENTITIES || gParkFlags & PARK_FLAGS_SPRITES_INITIALISED)
                {
                    HidePreviousStepButton();
                }
            }
        }
    }

    void OnDraw(DrawPixelInfo& dpi) override
    {
        auto drawPreviousButton = widgets[WIDX_PREVIOUS_STEP_BUTTON].type != WindowWidgetType::Empty;
        auto drawNextButton = widgets[WIDX_NEXT_STEP_BUTTON].type != WindowWidgetType::Empty;

        if (drawPreviousButton)
            DrawLeftButtonBack(dpi);

        if (drawNextButton)
            DrawRightButtonBack(dpi);

        DrawWidgets(dpi);

        if (drawPreviousButton)
            DrawLeftButton(dpi);

        if (drawNextButton)
            DrawRightButton(dpi);

        DrawStepText(dpi);
    }

    void OnMouseUp(WidgetIndex widgetIndex) override
    {
        if (widgetIndex == WIDX_PREVIOUS_STEP_BUTTON)
        {
            if ((gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
                || (GetNumFreeEntities() == MAX_ENTITIES && !(gParkFlags & PARK_FLAGS_SPRITES_INITIALISED)))
            {
                ((this)->*(_previousButtonMouseUp[EnumValue(gEditorStep)]))();
            }
        }
        else if (widgetIndex == WIDX_NEXT_STEP_BUTTON)
        {
            ((this)->*(_nextButtonMouseUp[EnumValue(gEditorStep)]))();
        }
    }

private:
    void JumpBackToObjectSelection() const
    {
        WindowCloseAll();
        gEditorStep = EditorStep::ObjectSelection;
        GfxInvalidateScreen();
    }

    void JumpBackToLandscapeEditor() const
    {
        WindowCloseAll();
        SetAllSceneryItemsInvented();
        ScenerySetDefaultPlacementConfiguration();
        gEditorStep = EditorStep::LandscapeEditor;
        ContextOpenWindow(WindowClass::Map);
        GfxInvalidateScreen();
    }

    void JumpBackToInventionListSetUp() const
    {
        WindowCloseAll();
        ContextOpenWindow(WindowClass::EditorInventionList);
        gEditorStep = EditorStep::InventionsListSetUp;
        GfxInvalidateScreen();
    }

    void JumpBackToOptionsSelection() const
    {
        WindowCloseAll();
        ContextOpenWindow(WindowClass::EditorScenarioOptions);
        gEditorStep = EditorStep::OptionsSelection;
        GfxInvalidateScreen();
    }

    bool CheckObjectSelection() const
    {
        WindowBase* w;

        auto [missingObjectType, errorString] = Editor::CheckObjectSelection();
        if (missingObjectType == ObjectType::None)
        {
            WindowCloseByClass(WindowClass::EditorObjectSelection);
            return true;
        }

        ContextShowError(STR_INVALID_SELECTION_OF_OBJECTS, errorString, {});
        w = WindowFindByClass(WindowClass::EditorObjectSelection);
        if (w != nullptr)
        {
            // Click tab with missing object
            WindowEventMouseUpCall(w, WC_EDITOR_OBJECT_SELECTION__WIDX_TAB_1 + EnumValue(missingObjectType));
        }
        return false;
    }

    void JumpForwardFromObjectSelection() const
    {
        if (!CheckObjectSelection())
            return;

        FinishObjectSelection();
        if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
        {
            ContextOpenWindow(WindowClass::ConstructRide);
        }
        else
        {
            ContextOpenWindow(WindowClass::Map);
        }
    }

    void JumpForwardToInventionListSetUp() const
    {
        auto [checksPassed, errorString] = Editor::CheckPark();
        if (checksPassed)
        {
            WindowCloseAll();
            ContextOpenWindow(WindowClass::EditorInventionList);
            gEditorStep = EditorStep::InventionsListSetUp;
        }
        else
        {
            ContextShowError(STR_CANT_ADVANCE_TO_NEXT_EDITOR_STAGE, errorString, {});
        }

        GfxInvalidateScreen();
    }

    void JumpForwardToOptionsSelection() const
    {
        WindowCloseAll();
        ContextOpenWindow(WindowClass::EditorScenarioOptions);
        gEditorStep = EditorStep::OptionsSelection;
        GfxInvalidateScreen();
    }

    void JumpForwardToObjectiveSelection() const
    {
        WindowCloseAll();
        ContextOpenWindow(WindowClass::EditorObjectiveOptions);
        gEditorStep = EditorStep::ObjectiveSelection;
        GfxInvalidateScreen();
    }

    void JumpForwardToSaveScenario() const
    {
        const auto savePrepareResult = ScenarioPrepareForSave();
        if (!savePrepareResult.Successful)
        {
            ContextShowError(STR_UNABLE_TO_SAVE_SCENARIO_FILE, savePrepareResult.Message, {});
            GfxInvalidateScreen();
            return;
        }

        WindowCloseAll();
        auto intent = Intent(WindowClass::Loadsave);
        intent.PutExtra(INTENT_EXTRA_LOADSAVE_TYPE, LOADSAVETYPE_SAVE | LOADSAVETYPE_SCENARIO);
        intent.PutExtra(INTENT_EXTRA_PATH, gScenarioName);
        ContextOpenIntent(&intent);
    }

    void HidePreviousStepButton()
    {
        widgets[WIDX_PREVIOUS_STEP_BUTTON].type = WindowWidgetType::Empty;
        widgets[WIDX_PREVIOUS_IMAGE].type = WindowWidgetType::Empty;
    }

    void HideNextStepButton()
    {
        widgets[WIDX_NEXT_STEP_BUTTON].type = WindowWidgetType::Empty;
        widgets[WIDX_NEXT_IMAGE].type = WindowWidgetType::Empty;
    }

    void DrawLeftButtonBack(DrawPixelInfo& dpi)
    {
        auto previousWidget = widgets[WIDX_PREVIOUS_IMAGE];
        auto leftTop = windowPos + ScreenCoordsXY{ previousWidget.left, previousWidget.top };
        auto rightBottom = windowPos + ScreenCoordsXY{ previousWidget.right, previousWidget.bottom };
        GfxFilterRect(dpi, { leftTop, rightBottom }, FilterPaletteID::Palette51);
    }

    void DrawLeftButton(DrawPixelInfo& dpi)
    {
        const auto topLeft = windowPos
            + ScreenCoordsXY{ widgets[WIDX_PREVIOUS_IMAGE].left + 1, widgets[WIDX_PREVIOUS_IMAGE].top + 1 };
        const auto bottomRight = windowPos
            + ScreenCoordsXY{ widgets[WIDX_PREVIOUS_IMAGE].right - 1, widgets[WIDX_PREVIOUS_IMAGE].bottom - 1 };
        GfxFillRectInset(dpi, { topLeft, bottomRight }, colours[1], INSET_RECT_F_30);

        GfxDrawSprite(
            dpi, ImageId(SPR_PREVIOUS),
            windowPos + ScreenCoordsXY{ widgets[WIDX_PREVIOUS_IMAGE].left + 6, widgets[WIDX_PREVIOUS_IMAGE].top + 6 });

        colour_t textColour = NOT_TRANSLUCENT(colours[1]);
        if (gHoverWidget.window_classification == WindowClass::BottomToolbar
            && gHoverWidget.widget_index == WIDX_PREVIOUS_STEP_BUTTON)
        {
            textColour = COLOUR_WHITE;
        }

        int16_t textX = (widgets[WIDX_PREVIOUS_IMAGE].left + 30 + widgets[WIDX_PREVIOUS_IMAGE].right) / 2 + windowPos.x;
        int16_t textY = widgets[WIDX_PREVIOUS_IMAGE].top + 6 + windowPos.y;

        StringId stringId = _editorStepNames[EnumValue(gEditorStep) - 1];
        if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
            stringId = STR_EDITOR_STEP_OBJECT_SELECTION;

        DrawTextBasic(dpi, { textX, textY }, STR_BACK_TO_PREVIOUS_STEP, {}, { textColour, TextAlignment::CENTRE });
        DrawTextBasic(dpi, { textX, textY + 10 }, stringId, {}, { textColour, TextAlignment::CENTRE });
    }

    void DrawRightButtonBack(DrawPixelInfo& dpi)
    {
        auto nextWidget = widgets[WIDX_NEXT_IMAGE];
        auto leftTop = windowPos + ScreenCoordsXY{ nextWidget.left, nextWidget.top };
        auto rightBottom = windowPos + ScreenCoordsXY{ nextWidget.right, nextWidget.bottom };
        GfxFilterRect(dpi, { leftTop, rightBottom }, FilterPaletteID::Palette51);
    }

    void DrawRightButton(DrawPixelInfo& dpi)
    {
        const auto topLeft = windowPos + ScreenCoordsXY{ widgets[WIDX_NEXT_IMAGE].left + 1, widgets[WIDX_NEXT_IMAGE].top + 1 };
        const auto bottomRight = windowPos
            + ScreenCoordsXY{ widgets[WIDX_NEXT_IMAGE].right - 1, widgets[WIDX_NEXT_IMAGE].bottom - 1 };
        GfxFillRectInset(dpi, { topLeft, bottomRight }, colours[1], INSET_RECT_F_30);

        GfxDrawSprite(
            dpi, ImageId(SPR_NEXT),
            windowPos + ScreenCoordsXY{ widgets[WIDX_NEXT_IMAGE].right - 29, widgets[WIDX_NEXT_IMAGE].top + 6 });

        colour_t textColour = NOT_TRANSLUCENT(colours[1]);

        if (gHoverWidget.window_classification == WindowClass::BottomToolbar
            && gHoverWidget.widget_index == WIDX_NEXT_STEP_BUTTON)
        {
            textColour = COLOUR_WHITE;
        }

        int16_t textX = (widgets[WIDX_NEXT_IMAGE].left + widgets[WIDX_NEXT_IMAGE].right - 30) / 2 + windowPos.x;
        int16_t textY = widgets[WIDX_NEXT_IMAGE].top + 6 + windowPos.y;

        StringId stringId = _editorStepNames[EnumValue(gEditorStep) + 1];
        if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
            stringId = STR_EDITOR_STEP_ROLLERCOASTER_DESIGNER;

        DrawTextBasic(dpi, { textX, textY }, STR_FORWARD_TO_NEXT_STEP, {}, { textColour, TextAlignment::CENTRE });
        DrawTextBasic(dpi, { textX, textY + 10 }, stringId, {}, { textColour, TextAlignment::CENTRE });
    }

    void DrawStepText(DrawPixelInfo& dpi)
    {
        int16_t stateX = (widgets[WIDX_PREVIOUS_IMAGE].right + widgets[WIDX_NEXT_IMAGE].left) / 2 + windowPos.x;
        int16_t stateY = height - 0x0C + windowPos.y;
        DrawTextBasic(
            dpi, { stateX, stateY }, _editorStepNames[EnumValue(gEditorStep)], {},
            { static_cast<colour_t>(NOT_TRANSLUCENT(colours[2]) | COLOUR_FLAG_OUTLINE), TextAlignment::CENTRE });
    }

    static constexpr FuncPtr _previousButtonMouseUp[] = {
        nullptr,
        &EditorBottomToolbarWindow::JumpBackToObjectSelection,
        &EditorBottomToolbarWindow::JumpBackToLandscapeEditor,
        &EditorBottomToolbarWindow::JumpBackToInventionListSetUp,
        &EditorBottomToolbarWindow::JumpBackToOptionsSelection,
        nullptr,
        &EditorBottomToolbarWindow::JumpBackToObjectSelection,
        nullptr,
    };

    static constexpr const FuncPtr _nextButtonMouseUp[] = {
        &EditorBottomToolbarWindow::JumpForwardFromObjectSelection,
        &EditorBottomToolbarWindow::JumpForwardToInventionListSetUp,
        &EditorBottomToolbarWindow::JumpForwardToOptionsSelection,
        &EditorBottomToolbarWindow::JumpForwardToObjectiveSelection,
        &EditorBottomToolbarWindow::JumpForwardToSaveScenario,
        nullptr,
        nullptr,
        nullptr,
    };
};

/**
 * Creates the main editor top toolbar window.
 * rct2: 0x0066F052 (part of 0x0066EF38)
 */
WindowBase* WindowEditorBottomToolbarOpen()
{
    auto* window = WindowCreate<EditorBottomToolbarWindow>(
        WindowClass::BottomToolbar, ScreenCoordsXY(0, ContextGetHeight() - 32), ContextGetWidth(), 32,
        WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_NO_BACKGROUND);

    return window;
}
