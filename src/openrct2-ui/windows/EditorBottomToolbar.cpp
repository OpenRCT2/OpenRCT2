/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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

static rct_widget window_editor_bottom_toolbar_widgets[] = {
    MakeWidget({  0, 0}, {200, 34}, WindowWidgetType::ImgBtn,  WindowColour::Primary),
    MakeWidget({  2, 2}, {196, 30}, WindowWidgetType::FlatBtn, WindowColour::Primary),
    MakeWidget({440, 0}, {200, 34}, WindowWidgetType::ImgBtn,  WindowColour::Primary),
    MakeWidget({442, 2}, {196, 30}, WindowWidgetType::FlatBtn, WindowColour::Primary),
    WIDGETS_END,
};

static void WindowEditorBottomToolbarMouseup(rct_window *w, rct_widgetindex widgetIndex);
static void WindowEditorBottomToolbarInvalidate(rct_window *w);
static void WindowEditorBottomToolbarPaint(rct_window *w, rct_drawpixelinfo *dpi);

static void WindowEditorBottomToolbarJumpBackToObjectSelection();
static void WindowEditorBottomToolbarJumpBackToLandscapeEditor();
static void WindowEditorBottomToolbarJumpBackToInventionListSetUp();
static void WindowEditorBottomToolbarJumpBackToOptionsSelection();

static void WindowEditorBottomToolbarJumpForwardFromObjectSelection();
static void WindowEditorBottomToolbarJumpForwardToInventionListSetUp();
static void WindowEditorBottomToolbarJumpForwardToOptionsSelection();
static void WindowEditorBottomToolbarJumpForwardToObjectiveSelection();
static void WindowEditorBottomToolbarJumpForwardToSaveScenario();

static rct_window_event_list window_editor_bottom_toolbar_events([](auto& events)
{
    events.mouse_up = &WindowEditorBottomToolbarMouseup;
    events.invalidate = &WindowEditorBottomToolbarInvalidate;
    events.paint = &WindowEditorBottomToolbarPaint;
});

static EMPTY_ARGS_VOID_POINTER *previous_button_mouseup_events[] = {
    nullptr,
    WindowEditorBottomToolbarJumpBackToObjectSelection,
    WindowEditorBottomToolbarJumpBackToLandscapeEditor,
    WindowEditorBottomToolbarJumpBackToInventionListSetUp,
    WindowEditorBottomToolbarJumpBackToOptionsSelection,
    nullptr,
    WindowEditorBottomToolbarJumpBackToObjectSelection,
    nullptr,
};

static EMPTY_ARGS_VOID_POINTER *next_button_mouseup_events[] = {
    WindowEditorBottomToolbarJumpForwardFromObjectSelection,
    WindowEditorBottomToolbarJumpForwardToInventionListSetUp,
    WindowEditorBottomToolbarJumpForwardToOptionsSelection,
    WindowEditorBottomToolbarJumpForwardToObjectiveSelection,
    WindowEditorBottomToolbarJumpForwardToSaveScenario,
    nullptr,
    nullptr,
    nullptr,
};

static constexpr const rct_string_id EditorStepNames[] = {
    STR_EDITOR_STEP_OBJECT_SELECTION,
    STR_EDITOR_STEP_LANDSCAPE_EDITOR,
    STR_EDITOR_STEP_INVENTIONS_LIST_SET_UP,
    STR_EDITOR_STEP_OPTIONS_SELECTION,
    STR_EDITOR_STEP_OBJECTIVE_SELECTION,
    STR_EDITOR_STEP_SAVE_SCENARIO,
    STR_EDITOR_STEP_ROLLERCOASTER_DESIGNER,
    STR_EDITOR_STEP_TRACK_DESIGNS_MANAGER,
};
// clang-format on

/**
 * Creates the main editor top toolbar window.
 * rct2: 0x0066F052 (part of 0x0066EF38)
 */
rct_window* WindowEditorBottomToolbarOpen()
{
    rct_window* window = WindowCreate(
        ScreenCoordsXY(0, context_get_height() - 32), context_get_width(), 32, &window_editor_bottom_toolbar_events,
        WC_BOTTOM_TOOLBAR, WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_NO_BACKGROUND);
    window->widgets = window_editor_bottom_toolbar_widgets;

    WindowInitScrollWidgets(window);
    set_all_scenery_items_invented();

    return window;
}

/**
 *
 *  rct2: 0x0066F619
 */
void WindowEditorBottomToolbarJumpBackToObjectSelection()
{
    window_close_all();
    gEditorStep = EditorStep::ObjectSelection;
    gfx_invalidate_screen();
}

/**
 *
 *  rct2: 0x0066F62C
 */
void WindowEditorBottomToolbarJumpBackToLandscapeEditor()
{
    window_close_all();
    set_all_scenery_items_invented();
    scenery_set_default_placement_configuration();
    gEditorStep = EditorStep::LandscapeEditor;
    context_open_window(WC_MAP);
    gfx_invalidate_screen();
}

/**
 *
 *  rct2: 0x0066F64E
 */
static void WindowEditorBottomToolbarJumpBackToInventionListSetUp()
{
    window_close_all();
    context_open_window(WC_EDITOR_INVENTION_LIST);
    gEditorStep = EditorStep::InventionsListSetUp;
    gfx_invalidate_screen();
}

/**
 *
 *  rct2: 0x0066F666
 */
void WindowEditorBottomToolbarJumpBackToOptionsSelection()
{
    window_close_all();
    context_open_window(WC_EDITOR_SCENARIO_OPTIONS);
    gEditorStep = EditorStep::OptionsSelection;
    gfx_invalidate_screen();
}

/**
 *
 *  rct2: 0x006AB1CE
 */
static bool WindowEditorBottomToolbarCheckObjectSelection()
{
    rct_window* w;

    auto [missingObjectType, errorString] = Editor::CheckObjectSelection();
    if (missingObjectType == ObjectType::None)
    {
        window_close_by_class(WC_EDITOR_OBJECT_SELECTION);
        return true;
    }

    context_show_error(STR_INVALID_SELECTION_OF_OBJECTS, errorString, {});
    w = window_find_by_class(WC_EDITOR_OBJECT_SELECTION);
    if (w != nullptr)
    {
        // Click tab with missing object
        window_event_mouse_up_call(w, WC_EDITOR_OBJECT_SELECTION__WIDX_TAB_1 + EnumValue(missingObjectType));
    }
    return false;
}

/**
 *
 *  rct2: 0x0066F6B0
 */
void WindowEditorBottomToolbarJumpForwardFromObjectSelection()
{
    if (!WindowEditorBottomToolbarCheckObjectSelection())
        return;

    finish_object_selection();
    if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
    {
        context_open_window(WC_CONSTRUCT_RIDE);
    }
    else
    {
        context_open_window(WC_MAP);
    }
}

/**
 *
 *  rct2: 0x0066F758
 */
void WindowEditorBottomToolbarJumpForwardToInventionListSetUp()
{
    auto [checksPassed, errorString] = Editor::CheckPark();
    if (checksPassed)
    {
        window_close_all();
        context_open_window(WC_EDITOR_INVENTION_LIST);
        gEditorStep = EditorStep::InventionsListSetUp;
    }
    else
    {
        context_show_error(STR_CANT_ADVANCE_TO_NEXT_EDITOR_STAGE, errorString, {});
    }

    gfx_invalidate_screen();
}

/**
 *
 *  rct2: 0x0066f790
 */
void WindowEditorBottomToolbarJumpForwardToOptionsSelection()
{
    window_close_all();
    context_open_window(WC_EDITOR_SCENARIO_OPTIONS);
    gEditorStep = EditorStep::OptionsSelection;
    gfx_invalidate_screen();
}

/**
 *
 *  rct2: 0x0066f7a8
 */
void WindowEditorBottomToolbarJumpForwardToObjectiveSelection()
{
    window_close_all();
    context_open_window(WC_EDITOR_OBJECTIVE_OPTIONS);
    gEditorStep = EditorStep::ObjectiveSelection;
    gfx_invalidate_screen();
}

/**
 *
 *  rct2: 0x0066F7C0
 */
void WindowEditorBottomToolbarJumpForwardToSaveScenario()
{
    if (!scenario_prepare_for_save())
    {
        context_show_error(STR_UNABLE_TO_SAVE_SCENARIO_FILE, gGameCommandErrorText, {});
        gfx_invalidate_screen();
        return;
    }

    window_close_all();
    auto intent = Intent(WC_LOADSAVE);
    intent.putExtra(INTENT_EXTRA_LOADSAVE_TYPE, LOADSAVETYPE_SAVE | LOADSAVETYPE_SCENARIO);
    intent.putExtra(INTENT_EXTRA_PATH, gScenarioName);
    context_open_intent(&intent);
}

/**
 *
 *  rct2: 0x0066F5AE
 */
static void WindowEditorBottomToolbarMouseup([[maybe_unused]] rct_window* w, rct_widgetindex widgetIndex)
{
    if (widgetIndex == WIDX_PREVIOUS_STEP_BUTTON)
    {
        if ((gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
            || (GetNumFreeEntities() == MAX_ENTITIES && !(gParkFlags & PARK_FLAGS_SPRITES_INITIALISED)))
        {
            previous_button_mouseup_events[EnumValue(gEditorStep)]();
        }
    }
    else if (widgetIndex == WIDX_NEXT_STEP_BUTTON)
    {
        next_button_mouseup_events[EnumValue(gEditorStep)]();
    }
}

static void HidePreviousStepButton()
{
    window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_STEP_BUTTON].type = WindowWidgetType::Empty;
    window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].type = WindowWidgetType::Empty;
}

static void HideNextStepButton()
{
    window_editor_bottom_toolbar_widgets[WIDX_NEXT_STEP_BUTTON].type = WindowWidgetType::Empty;
    window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].type = WindowWidgetType::Empty;
}

/**
 *
 *  rct2: 0x0066F1C9
 */
void WindowEditorBottomToolbarInvalidate(rct_window* w)
{
    ColourSchemeUpdateByClass(
        w, (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) ? WC_EDITOR_SCENARIO_BOTTOM_TOOLBAR : WC_EDITOR_TRACK_BOTTOM_TOOLBAR);

    uint16_t screenWidth = context_get_width();
    window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].left = screenWidth - 200;
    window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].right = screenWidth - 1;
    window_editor_bottom_toolbar_widgets[WIDX_NEXT_STEP_BUTTON].left = screenWidth - 198;
    window_editor_bottom_toolbar_widgets[WIDX_NEXT_STEP_BUTTON].right = screenWidth - 3;

    window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_STEP_BUTTON].type = WindowWidgetType::FlatBtn;
    window_editor_bottom_toolbar_widgets[WIDX_NEXT_STEP_BUTTON].type = WindowWidgetType::FlatBtn;
    window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].type = WindowWidgetType::ImgBtn;
    window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].type = WindowWidgetType::ImgBtn;

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

/**
 *
 *  rct2: 0x0066F25C
 */
void WindowEditorBottomToolbarPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    bool drawPreviousButton = false;
    bool drawNextButton = false;

    if (gEditorStep == EditorStep::ObjectSelection)
    {
        drawNextButton = true;
    }
    else if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
    {
        drawPreviousButton = true;
    }
    else if (GetNumFreeEntities() != MAX_ENTITIES)
    {
        drawNextButton = true;
    }
    else if (gParkFlags & PARK_FLAGS_SPRITES_INITIALISED)
    {
        drawNextButton = true;
    }
    else
    {
        drawPreviousButton = true;
    }

    if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER))
    {
        auto previousWidget = window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE];

        if (drawPreviousButton)
        {
            auto leftTop = w->windowPos + ScreenCoordsXY{ previousWidget.left, previousWidget.top };
            auto rightBottom = w->windowPos + ScreenCoordsXY{ previousWidget.right, previousWidget.bottom };
            gfx_filter_rect(dpi, { leftTop, rightBottom }, FilterPaletteID::Palette51);
        }

        if ((drawPreviousButton || drawNextButton) && gEditorStep != EditorStep::RollercoasterDesigner)
        {
            auto nextWidget = window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE];
            auto leftTop = w->windowPos + ScreenCoordsXY{ nextWidget.left, nextWidget.top };
            auto rightBottom = w->windowPos + ScreenCoordsXY{ nextWidget.right, nextWidget.bottom };
            gfx_filter_rect(dpi, { leftTop, rightBottom }, FilterPaletteID::Palette51);
        }
    }

    WindowDrawWidgets(w, dpi);

    if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER))
    {
        const auto topLeft = w->windowPos
            + ScreenCoordsXY{ window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].left + 1,
                              window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].top + 1 };
        const auto bottomRight = w->windowPos
            + ScreenCoordsXY{ window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].right - 1,
                              window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].bottom - 1 };
        if (drawPreviousButton)
        {
            gfx_fill_rect_inset(dpi, { topLeft, bottomRight }, w->colours[1], INSET_RECT_F_30);
        }

        if ((drawPreviousButton || drawNextButton) && gEditorStep != EditorStep::RollercoasterDesigner)
        {
            gfx_fill_rect_inset(dpi, { topLeft, bottomRight }, w->colours[1], INSET_RECT_F_30);
        }

        int16_t stateX = (window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].right
                          + window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].left)
                / 2
            + w->windowPos.x;
        int16_t stateY = w->height - 0x0C + w->windowPos.y;
        DrawTextBasic(
            dpi, { stateX, stateY }, EditorStepNames[EnumValue(gEditorStep)], {},
            { static_cast<colour_t>(NOT_TRANSLUCENT(w->colours[2]) | COLOUR_FLAG_OUTLINE), TextAlignment::CENTRE });

        if (drawPreviousButton)
        {
            gfx_draw_sprite(
                dpi, ImageId(SPR_PREVIOUS),
                w->windowPos
                    + ScreenCoordsXY{ window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].left + 6,
                                      window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].top + 6 });

            colour_t textColour = NOT_TRANSLUCENT(w->colours[1]);
            if (gHoverWidget.window_classification == WC_BOTTOM_TOOLBAR
                && gHoverWidget.widget_index == WIDX_PREVIOUS_STEP_BUTTON)
            {
                textColour = COLOUR_WHITE;
            }

            int16_t textX = (window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].left + 30
                             + window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].right)
                    / 2
                + w->windowPos.x;
            int16_t textY = window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].top + 6 + w->windowPos.y;

            rct_string_id stringId = EditorStepNames[EnumValue(gEditorStep) - 1];
            if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
                stringId = STR_EDITOR_STEP_OBJECT_SELECTION;

            DrawTextBasic(dpi, { textX, textY }, STR_BACK_TO_PREVIOUS_STEP, {}, { textColour, TextAlignment::CENTRE });
            DrawTextBasic(dpi, { textX, textY + 10 }, stringId, {}, { textColour, TextAlignment::CENTRE });
        }

        if ((drawPreviousButton || drawNextButton) && gEditorStep != EditorStep::RollercoasterDesigner)
        {
            gfx_draw_sprite(
                dpi, ImageId(SPR_NEXT),
                w->windowPos
                    + ScreenCoordsXY{ window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].right - 29,
                                      window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].top + 6 });

            colour_t textColour = NOT_TRANSLUCENT(w->colours[1]);

            if (gHoverWidget.window_classification == WC_BOTTOM_TOOLBAR && gHoverWidget.widget_index == WIDX_NEXT_STEP_BUTTON)
            {
                textColour = COLOUR_WHITE;
            }

            int16_t textX = (window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].left
                             + window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].right - 30)
                    / 2
                + w->windowPos.x;
            int16_t textY = window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].top + 6 + w->windowPos.y;

            rct_string_id stringId = EditorStepNames[EnumValue(gEditorStep) + 1];
            if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
                stringId = STR_EDITOR_STEP_ROLLERCOASTER_DESIGNER;

            DrawTextBasic(dpi, { textX, textY }, STR_FORWARD_TO_NEXT_STEP, {}, { textColour, TextAlignment::CENTRE });
            DrawTextBasic(dpi, { textX, textY + 10 }, stringId, {}, { textColour, TextAlignment::CENTRE });
        }
    }
}
