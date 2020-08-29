/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/Theme.h"

#include <iterator>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/core/Memory.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/scenario/ScenarioRepository.h>
#include <openrct2/scenario/ScenarioSources.h>
#include <openrct2/sprites.h>
#include <openrct2/title/TitleSequence.h>
#include <openrct2/util/Util.h>
#include <openrct2/world/Sprite.h>

// clang-format off
struct TITLE_COMMAND_ORDER {
    // originally a uint8_t, but the new millisecond wait times require a uint16_t.
    uint16_t command;
    rct_string_id nameStringId;
    rct_string_id descStringId;
};

static TITLE_COMMAND_ORDER _window_title_command_editor_orders[] = {
    { TITLE_SCRIPT_LOAD,        STR_TITLE_EDITOR_ACTION_LOAD_SAVE, STR_TITLE_EDITOR_ARGUMENT_SAVEFILE },
    { TITLE_SCRIPT_LOADSC,      STR_TITLE_EDITOR_ACTION_LOAD_SCENARIO, STR_TITLE_EDITOR_ARGUMENT_SCENARIO },
    { TITLE_SCRIPT_LOCATION,    STR_TITLE_EDITOR_COMMAND_TYPE_LOCATION, STR_TITLE_EDITOR_ARGUMENT_COORDINATES },
    { TITLE_SCRIPT_ROTATE,      STR_TITLE_EDITOR_COMMAND_TYPE_ROTATE,   STR_TITLE_EDITOR_ARGUMENT_ROTATIONS },
    { TITLE_SCRIPT_ZOOM,        STR_TITLE_EDITOR_COMMAND_TYPE_ZOOM,     STR_TITLE_EDITOR_ARGUMENT_ZOOM_LEVEL },
    { TITLE_SCRIPT_SPEED,       STR_TITLE_EDITOR_COMMAND_TYPE_SPEED,    STR_TITLE_EDITOR_ARGUMENT_SPEED },
    { TITLE_SCRIPT_FOLLOW,      STR_TITLE_EDITOR_COMMAND_TYPE_FOLLOW,   STR_NONE },
    { TITLE_SCRIPT_WAIT,        STR_TITLE_EDITOR_COMMAND_TYPE_WAIT,     STR_TITLE_EDITOR_ARGUMENT_WAIT_SECONDS },
    { TITLE_SCRIPT_RESTART,     STR_TITLE_EDITOR_RESTART,               STR_NONE },
    { TITLE_SCRIPT_END,         STR_TITLE_EDITOR_END,                   STR_NONE },
};

#define NUM_COMMANDS std::size(_window_title_command_editor_orders)

enum WINDOW_WATER_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_COMMAND,
    WIDX_COMMAND_DROPDOWN,
    WIDX_TEXTBOX_FULL,
    WIDX_TEXTBOX_X,
    WIDX_TEXTBOX_Y,
    WIDX_INPUT,
    WIDX_INPUT_DROPDOWN,
    WIDX_GET,
    WIDX_SELECT_SCENARIO,
    WIDX_SELECT_SPRITE,
    WIDX_VIEWPORT,
    WIDX_OKAY,
    WIDX_CANCEL
};

static constexpr rct_string_id WINDOW_TITLE = STR_TITLE_COMMAND_EDITOR_TITLE;
static constexpr const int32_t WW = 200;
static constexpr const int32_t WH = 120;
static constexpr int32_t BY = 32;
static constexpr int32_t BY2 = 70;
static constexpr int32_t WS = 16;

static bool _window_title_command_editor_insert;
static int32_t _window_title_command_editor_index;
constexpr size_t BUF_SIZE = 50;
static char textbox1Buffer[BUF_SIZE];
static char textbox2Buffer[BUF_SIZE];
static TitleCommand command = { TITLE_SCRIPT_LOAD, { 0 } };
static TitleSequence * _sequence = nullptr;

static rct_widget window_title_command_editor_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({ 16, 32}, { 168,  12}, WWT_DROPDOWN, WindowColour::Secondary                                                 ), // Command dropdown
    MakeWidget({172, 33}, {  11,  12}, WWT_BUTTON,   WindowColour::Secondary, STR_DROPDOWN_GLYPH                             ),
    MakeWidget({ 16, 70}, { 168,  12}, WWT_TEXT_BOX, WindowColour::Secondary                                                 ), // full textbox

    MakeWidget({ 16, 70}, {  81,  12}, WWT_TEXT_BOX, WindowColour::Secondary                                                 ), // x textbox
    MakeWidget({103, 70}, {  81,  12}, WWT_TEXT_BOX, WindowColour::Secondary                                                 ), // y textbox

    MakeWidget({ 16, 70}, { 168,  12}, WWT_DROPDOWN, WindowColour::Secondary                                                 ), // Save dropdown
    MakeWidget({172, 71}, {  11,  10}, WWT_BUTTON,   WindowColour::Secondary, STR_DROPDOWN_GLYPH                             ),

    MakeWidget({103, 56}, {  81,  12}, WWT_BUTTON,   WindowColour::Secondary, STR_TITLE_COMMAND_EDITOR_ACTION_GET_LOCATION   ), // Get location/zoom/etc
    MakeWidget({112, 56}, {  72,  12}, WWT_BUTTON,   WindowColour::Secondary, STR_TITLE_COMMAND_EDITOR_ACTION_SELECT_SCENARIO), // Select scenario

    MakeWidget({ 16, 56}, { 168,  12}, WWT_BUTTON,   WindowColour::Secondary, STR_TITLE_COMMAND_EDITOR_SELECT_SPRITE         ), // Select sprite
    MakeWidget({ 16, 70}, { 168,  24}, WWT_VIEWPORT, WindowColour::Secondary                                                 ), // Viewport

    MakeWidget({ 10, 99}, {  71,  14}, WWT_BUTTON,   WindowColour::Secondary, STR_OK                                         ), // OKAY
    MakeWidget({120, 99}, {  71,  14}, WWT_BUTTON,   WindowColour::Secondary, STR_CANCEL                                     ), // Cancel
    { WIDGETS_END },
};

static void window_title_command_editor_close(rct_window * w);
static void window_title_command_editor_mouseup(rct_window * w, rct_widgetindex widgetIndex);
static void window_title_command_editor_mousedown(rct_window * w, rct_widgetindex widgetIndex, rct_widget * widget);
static void window_title_command_editor_dropdown(rct_window * w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_title_command_editor_update(rct_window * w);
static void window_title_command_editor_tool_down(rct_window * w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void window_title_command_editor_invalidate(rct_window * w);
static void window_title_command_editor_paint(rct_window * w, rct_drawpixelinfo * dpi);
static void window_title_command_editor_textinput(rct_window * w, rct_widgetindex widgetIndex, char * text);
static void scenario_select_callback(const utf8 * path);
static int32_t get_command_info_index(int32_t index);
static TITLE_COMMAND_ORDER get_command_info(int32_t index);
static TileCoordsXY get_location();
static uint8_t get_zoom();

static rct_window_event_list window_title_command_editor_events = {
    window_title_command_editor_close,
    window_title_command_editor_mouseup,
    nullptr,
    window_title_command_editor_mousedown,
    window_title_command_editor_dropdown,
    nullptr,
    window_title_command_editor_update,
    nullptr,
    nullptr,
    nullptr,
    window_title_command_editor_tool_down,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_title_command_editor_textinput,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_title_command_editor_invalidate,
    window_title_command_editor_paint,
    nullptr
};
// clang-format on

static void scenario_select_callback(const utf8* path)
{
    if (command.Type == TITLE_SCRIPT_LOADSC)
    {
        const utf8* fileName = path_get_filename(path);
        auto scenario = GetScenarioRepository()->GetByFilename(fileName);
        safe_strcpy(command.Scenario, scenario->internal_name, sizeof(command.Scenario));
    }
}

static int32_t get_command_info_index(int32_t index)
{
    for (int32_t i = 0; i < static_cast<int32_t>(NUM_COMMANDS); i++)
    {
        if (_window_title_command_editor_orders[i].command == index)
            return i;
    }
    return 0;
}

static TITLE_COMMAND_ORDER get_command_info(int32_t index)
{
    for (int32_t i = 0; i < static_cast<int32_t>(NUM_COMMANDS); i++)
    {
        if (_window_title_command_editor_orders[i].command == index)
            return _window_title_command_editor_orders[i];
    }
    return _window_title_command_editor_orders[0];
}

static TileCoordsXY get_location()
{
    TileCoordsXY tileCoord = {};
    rct_window* w = window_get_main();
    if (w != nullptr)
    {
        auto info = get_map_coordinates_from_pos_window(
            w, { w->viewport->view_width / 2, w->viewport->view_height / 2 }, VIEWPORT_INTERACTION_MASK_TERRAIN);
        auto mapCoord = info.Loc;
        mapCoord.x -= 16;
        mapCoord.y -= 16;
        tileCoord = TileCoordsXY{ mapCoord };
        tileCoord.x++;
        tileCoord.y++;
    }
    return tileCoord;
}

static uint8_t get_zoom()
{
    uint8_t zoom = 0;
    rct_window* w = window_get_main();
    if (w != nullptr)
    {
        zoom = static_cast<int8_t>(w->viewport->zoom);
    }
    return zoom;
}

static bool sprite_selector_tool_is_active()
{
    if (!(input_test_flag(INPUT_FLAG_TOOL_ACTIVE)))
        return false;
    if (gCurrentToolWidget.window_classification != WC_TITLE_COMMAND_EDITOR)
        return false;
    return true;
}

void window_title_command_editor_open(TitleSequence* sequence, int32_t index, bool insert)
{
    _sequence = sequence;

    // Check if window is already open
    if (window_find_by_class(WC_TITLE_COMMAND_EDITOR) != nullptr)
        return;

    rct_window* window = window_create_centred(
        WW, WH, &window_title_command_editor_events, WC_TITLE_COMMAND_EDITOR, WF_STICK_TO_FRONT);
    window_title_command_editor_widgets[WIDX_TEXTBOX_FULL].string = textbox1Buffer;
    window_title_command_editor_widgets[WIDX_TEXTBOX_X].string = textbox1Buffer;
    window_title_command_editor_widgets[WIDX_TEXTBOX_Y].string = textbox2Buffer;
    window->widgets = window_title_command_editor_widgets;
    window->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_COMMAND) | (1 << WIDX_COMMAND_DROPDOWN) | (1 << WIDX_TEXTBOX_FULL)
        | (1 << WIDX_TEXTBOX_X) | (1 << WIDX_TEXTBOX_Y) | (1 << WIDX_INPUT) | (1 << WIDX_INPUT_DROPDOWN) | (1 << WIDX_GET)
        | (1 << WIDX_SELECT_SCENARIO) | (1 << WIDX_SELECT_SPRITE) | (1 << WIDX_OKAY) | (1 << WIDX_CANCEL);
    window_init_scroll_widgets(window);

    rct_widget* const viewportWidget = &window_title_command_editor_widgets[WIDX_VIEWPORT];
    viewport_create(
        window, window->windowPos + ScreenCoordsXY{ viewportWidget->left + 1, viewportWidget->top + 1 },
        viewportWidget->width() - 1, viewportWidget->height() - 1, 0, { 0, 0, 0 }, 0, SPRITE_INDEX_NULL);

    _window_title_command_editor_index = index;
    _window_title_command_editor_insert = insert;
    if (!insert)
    {
        command = _sequence->Commands[index];
    }

    switch (command.Type)
    {
        case TITLE_SCRIPT_LOAD:
            if (command.SaveIndex >= _sequence->NumSaves)
                command.SaveIndex = SAVE_INDEX_INVALID;
            break;
        case TITLE_SCRIPT_LOCATION:
            snprintf(textbox1Buffer, BUF_SIZE, "%d", command.X);
            snprintf(textbox2Buffer, BUF_SIZE, "%d", command.Y);
            break;
        case TITLE_SCRIPT_ROTATE:
        case TITLE_SCRIPT_ZOOM:
            snprintf(textbox1Buffer, BUF_SIZE, "%d", command.Rotations);
            break;
        case TITLE_SCRIPT_WAIT:
            snprintf(textbox1Buffer, BUF_SIZE, "%d", command.Milliseconds);
            break;
        case TITLE_SCRIPT_FOLLOW:
            if (command.SpriteIndex != SPRITE_INDEX_NULL)
            {
                window_follow_sprite(window, static_cast<size_t>(command.SpriteIndex));
            }
            break;
    }
}

static void window_title_command_editor_close(rct_window* w)
{
    if (sprite_selector_tool_is_active())
    {
        tool_cancel();
    }
}

static void window_title_command_editor_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
        case WIDX_CANCEL:
            window_close(w);
            break;
        case WIDX_TEXTBOX_FULL:
            // The only commands that use TEXTBOX_FULL currently are Wait, Rotate, and Zoom. Rotate and Zoom have single-digit
            // maximum values, while Wait has 5-digit maximum values.
            if (command.Type == TITLE_SCRIPT_WAIT)
            {
                window_start_textbox(w, widgetIndex, STR_STRING, textbox1Buffer, 6);
            }
            else
            {
                window_start_textbox(w, widgetIndex, STR_STRING, textbox1Buffer, 2);
            }
            break;
        case WIDX_TEXTBOX_X:
            window_start_textbox(w, widgetIndex, STR_STRING, textbox1Buffer, 4);
            break;
        case WIDX_TEXTBOX_Y:
            window_start_textbox(w, widgetIndex, STR_STRING, textbox2Buffer, 4);
            break;
        case WIDX_GET:
            if (command.Type == TITLE_SCRIPT_LOCATION)
            {
                auto tileCoord = get_location();
                command.X = static_cast<uint8_t>(tileCoord.x);
                command.Y = static_cast<uint8_t>(tileCoord.y);
                snprintf(textbox1Buffer, BUF_SIZE, "%d", command.X);
                snprintf(textbox2Buffer, BUF_SIZE, "%d", command.Y);
            }
            else if (command.Type == TITLE_SCRIPT_ZOOM)
            {
                uint8_t zoom = get_zoom();
                command.Zoom = zoom;
                snprintf(textbox1Buffer, BUF_SIZE, "%d", command.Zoom);
            }
            w->Invalidate();
            break;
        case WIDX_SELECT_SCENARIO:
            window_scenarioselect_open(scenario_select_callback, true);
            break;
        case WIDX_SELECT_SPRITE:
            if (!sprite_selector_tool_is_active())
            {
                tool_set(w, WIDX_BACKGROUND, TOOL_CROSSHAIR);
            }
            else
            {
                tool_cancel();
            }
            break;
        case WIDX_OKAY:
            if (_window_title_command_editor_insert)
            {
                size_t insertIndex = _window_title_command_editor_index;
                _sequence->NumCommands++;
                _sequence->Commands = Memory::ReallocateArray(_sequence->Commands, _sequence->NumCommands);
                for (size_t i = _sequence->NumCommands - 1; i > insertIndex; i--)
                {
                    _sequence->Commands[i] = _sequence->Commands[i - 1];
                }
                _sequence->Commands[insertIndex] = command;
            }
            else
            {
                _sequence->Commands[_window_title_command_editor_index] = command;
                TitleSequenceSave(_sequence);
            }
            TitleSequenceSave(_sequence);

            rct_window* title_editor_w = window_find_by_class(WC_TITLE_EDITOR);
            if (title_editor_w != nullptr)
            {
                title_editor_w->selected_list_item = _window_title_command_editor_index;
            }
            window_close(w);
            break;
    }
}

static void window_title_command_editor_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    widget--;
    switch (widgetIndex)
    {
        case WIDX_COMMAND_DROPDOWN:
        {
            size_t numItems = NUM_COMMANDS;
            for (size_t i = 0; i < numItems; i++)
            {
                gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
                gDropdownItemsArgs[i] = _window_title_command_editor_orders[i].nameStringId;
            }

            window_dropdown_show_text_custom_width(
                { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[1], 0,
                DROPDOWN_FLAG_STAY_OPEN, numItems, widget->width() - 3);

            dropdown_set_checked(get_command_info_index(command.Type), true);
            break;
        }
        case WIDX_INPUT_DROPDOWN:
            if (command.Type == TITLE_SCRIPT_SPEED)
            {
                int32_t numItems = 4;
                for (int32_t i = 0; i < numItems; i++)
                {
                    gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItemsArgs[i] = SpeedNames[i];
                }

                window_dropdown_show_text_custom_width(
                    { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[1], 0,
                    DROPDOWN_FLAG_STAY_OPEN, numItems, widget->width() - 3);

                dropdown_set_checked(command.Speed - 1, true);
            }
            else if (command.Type == TITLE_SCRIPT_LOAD)
            {
                int32_t numItems = static_cast<int32_t>(_sequence->NumSaves);
                for (int32_t i = 0; i < numItems; i++)
                {
                    gDropdownItemsFormat[i] = STR_OPTIONS_DROPDOWN_ITEM;
                    gDropdownItemsArgs[i] = reinterpret_cast<uintptr_t>(_sequence->Saves[i]);
                }

                window_dropdown_show_text_custom_width(
                    { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[1], 0,
                    DROPDOWN_FLAG_STAY_OPEN, numItems, widget->width() - 3);

                dropdown_set_checked(command.SaveIndex, true);
            }
            break;
    }
}

static void window_title_command_editor_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (dropdownIndex == -1)
        return;

    // Cancel sprite selector tool if it's active
    if (sprite_selector_tool_is_active())
    {
        tool_cancel();
    }

    switch (widgetIndex)
    {
        case WIDX_COMMAND_DROPDOWN:
            if (command.SpriteIndex != SPRITE_INDEX_NULL)
            {
                window_unfollow_sprite(w);
            }
            if (dropdownIndex == get_command_info_index(command.Type))
            {
                break;
            }
            command.Type = _window_title_command_editor_orders[dropdownIndex].command;
            switch (command.Type)
            {
                case TITLE_SCRIPT_LOCATION:
                {
                    auto tileCoord = get_location();
                    command.X = static_cast<uint8_t>(tileCoord.x);
                    command.Y = static_cast<uint8_t>(tileCoord.y);
                    snprintf(textbox1Buffer, BUF_SIZE, "%d", command.X);
                    snprintf(textbox2Buffer, BUF_SIZE, "%d", command.Y);
                    break;
                }
                case TITLE_SCRIPT_ROTATE:
                    command.Rotations = 1;
                    snprintf(textbox1Buffer, BUF_SIZE, "%d", command.Rotations);
                    break;
                case TITLE_SCRIPT_ZOOM:
                    command.Zoom = 0;
                    snprintf(textbox1Buffer, BUF_SIZE, "%d", command.Zoom);
                    break;
                case TITLE_SCRIPT_FOLLOW:
                    command.SpriteIndex = SPRITE_INDEX_NULL;
                    command.SpriteName[0] = '\0';
                    window_unfollow_sprite(w);
                    w->viewport->flags &= ~VIEWPORT_FOCUS_TYPE_SPRITE;
                    break;
                case TITLE_SCRIPT_SPEED:
                    command.Speed = 1;
                    break;
                case TITLE_SCRIPT_WAIT:
                    command.Milliseconds = 10000;
                    snprintf(textbox1Buffer, BUF_SIZE, "%d", command.Milliseconds);
                    break;
                case TITLE_SCRIPT_LOAD:
                    command.SaveIndex = 0;
                    if (command.SaveIndex >= _sequence->NumSaves)
                    {
                        command.SaveIndex = 0xFF;
                    }
                    break;
                case TITLE_SCRIPT_LOADSC:
                    command.Scenario[0] = '\0';
            }
            w->Invalidate();
            break;
        case WIDX_INPUT_DROPDOWN:
            switch (command.Type)
            {
                case TITLE_SCRIPT_SPEED:
                    if (dropdownIndex != command.Speed - 1)
                    {
                        command.Speed = static_cast<uint8_t>(dropdownIndex + 1);
                        w->Invalidate();
                    }
                    break;
                case TITLE_SCRIPT_LOAD:
                    if (dropdownIndex != command.SaveIndex)
                    {
                        command.SaveIndex = static_cast<uint8_t>(dropdownIndex);
                        w->Invalidate();
                    }
                    break;
            }
            break;
    }
}

static void window_title_command_editor_textinput(rct_window* w, rct_widgetindex widgetIndex, char* text)
{
    char* end;
    int32_t value = strtol(widgetIndex != WIDX_TEXTBOX_Y ? textbox1Buffer : textbox2Buffer, &end, 10);
    if (value < 0)
        value = 0;
    // The Wait command is the only one with acceptable values greater than 255.
    if (value > 255 && command.Type != TITLE_SCRIPT_WAIT)
        value = 255;
    switch (widgetIndex)
    {
        case WIDX_TEXTBOX_FULL:
            if (text == nullptr)
            {
                if (*end == '\0')
                {
                    if (command.Type == TITLE_SCRIPT_WAIT)
                    {
                        if (value < 100)
                            value = 100;
                        if (value > 65000)
                            value = 65000;
                        command.Milliseconds = static_cast<uint16_t>(value);
                        snprintf(textbox1Buffer, BUF_SIZE, "%d", command.Milliseconds);
                    }
                    else
                    {
                        // Both Rotate and Zoom have a maximum value of 3, but Rotate has a min value of 1 not 0.
                        if (value > 3)
                            value = 3;
                        if (value < 1 && command.Type == TITLE_SCRIPT_ROTATE)
                            value = 1;
                        command.Rotations = static_cast<uint8_t>(value);
                        snprintf(textbox1Buffer, BUF_SIZE, "%d", command.Rotations);
                    }
                }
                w->Invalidate();
            }
            else
            {
                safe_strcpy(textbox1Buffer, text, sizeof(textbox1Buffer));
            }
            break;
        case WIDX_TEXTBOX_X:
            if (text == nullptr)
            {
                if (*end == '\0')
                {
                    command.X = static_cast<uint8_t>(value);
                }
                snprintf(textbox1Buffer, BUF_SIZE, "%d", command.X);
                w->Invalidate();
            }
            else
            {
                safe_strcpy(textbox1Buffer, text, sizeof(textbox1Buffer));
            }
            break;
        case WIDX_TEXTBOX_Y:
            if (text == nullptr)
            {
                if (*end == '\0')
                {
                    command.Y = static_cast<uint8_t>(value);
                }
                snprintf(textbox2Buffer, BUF_SIZE, "%d", command.Y);
                w->Invalidate();
            }
            else
            {
                safe_strcpy(textbox2Buffer, text, sizeof(textbox2Buffer));
            }
            break;
    }
}

static void window_title_command_editor_update(rct_window* w)
{
    if (gCurrentTextBox.window.classification == w->classification && gCurrentTextBox.window.number == w->number)
    {
        window_update_textbox_caret();
        widget_invalidate(w, gCurrentTextBox.widget_index);
    }
}

static void window_title_command_editor_tool_down(
    rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    auto info = viewport_interaction_get_item_left(screenCoords);

    if (info.SpriteType == VIEWPORT_INTERACTION_ITEM_SPRITE)
    {
        auto entity = info.Entity;
        bool validSprite = false;
        auto peep = entity->As<Peep>();
        auto vehicle = entity->As<Vehicle>();
        auto litter = entity->As<Litter>();
        auto duck = entity->As<Duck>();
        auto balloon = entity->As<Balloon>();
        if (peep != nullptr)
        {
            validSprite = true;
            uint8_t formatArgs[32]{};
            Formatter ft(formatArgs);
            peep->FormatNameTo(ft);
            format_string(command.SpriteName, USER_STRING_MAX_LENGTH, STR_STRINGID, &peep->Id);
        }
        else if (vehicle != nullptr)
        {
            validSprite = true;

            auto ride = vehicle->GetRide();
            if (ride != nullptr)
            {
                uint8_t formatArgs[32]{};
                Formatter ft(formatArgs);
                ride->FormatNameTo(ft);
                format_string(command.SpriteName, USER_STRING_MAX_LENGTH, STR_STRINGID, formatArgs);
            }
        }
        else if (litter != nullptr)
        {
            if (litter->type < std::size(litterNames))
            {
                validSprite = true;
                format_string(command.SpriteName, USER_STRING_MAX_LENGTH, litterNames[litter->type], nullptr);
            }
        }
        else if (balloon != nullptr)
        {
            validSprite = true;
            format_string(command.SpriteName, USER_STRING_MAX_LENGTH, STR_SHOP_ITEM_SINGULAR_BALLOON, nullptr);
        }
        else if (duck != nullptr)
        {
            validSprite = true;
            format_string(command.SpriteName, USER_STRING_MAX_LENGTH, STR_DUCK, nullptr);
        }

        if (validSprite)
        {
            command.SpriteIndex = entity->sprite_index;
            window_follow_sprite(w, static_cast<size_t>(command.SpriteIndex));
            tool_cancel();
            w->Invalidate();
        }
    }
}

static void window_title_command_editor_invalidate(rct_window* w)
{
    colour_scheme_update_by_class(w, WC_TITLE_EDITOR);

    window_title_command_editor_widgets[WIDX_TEXTBOX_FULL].type = WWT_EMPTY;
    window_title_command_editor_widgets[WIDX_TEXTBOX_X].type = WWT_EMPTY;
    window_title_command_editor_widgets[WIDX_TEXTBOX_Y].type = WWT_EMPTY;
    window_title_command_editor_widgets[WIDX_INPUT].type = WWT_EMPTY;
    window_title_command_editor_widgets[WIDX_INPUT_DROPDOWN].type = WWT_EMPTY;
    window_title_command_editor_widgets[WIDX_GET].type = WWT_EMPTY;
    window_title_command_editor_widgets[WIDX_SELECT_SCENARIO].type = WWT_EMPTY;
    window_title_command_editor_widgets[WIDX_SELECT_SPRITE].type = WWT_EMPTY;
    window_title_command_editor_widgets[WIDX_VIEWPORT].type = WWT_EMPTY;
    switch (command.Type)
    {
        case TITLE_SCRIPT_LOAD:
        case TITLE_SCRIPT_SPEED:
            window_title_command_editor_widgets[WIDX_INPUT].type = WWT_DROPDOWN;
            window_title_command_editor_widgets[WIDX_INPUT_DROPDOWN].type = WWT_BUTTON;
            break;
        case TITLE_SCRIPT_LOADSC:
            window_title_command_editor_widgets[WIDX_INPUT].type = WWT_DROPDOWN;
            window_title_command_editor_widgets[WIDX_SELECT_SCENARIO].type = WWT_BUTTON;
            break;
        case TITLE_SCRIPT_LOCATION:
            window_title_command_editor_widgets[WIDX_TEXTBOX_X].type = WWT_TEXT_BOX;
            window_title_command_editor_widgets[WIDX_TEXTBOX_Y].type = WWT_TEXT_BOX;
            window_title_command_editor_widgets[WIDX_GET].type = WWT_BUTTON;
            break;
        case TITLE_SCRIPT_ROTATE:
        case TITLE_SCRIPT_WAIT:
            window_title_command_editor_widgets[WIDX_TEXTBOX_FULL].type = WWT_TEXT_BOX;
            break;
        case TITLE_SCRIPT_ZOOM:
            window_title_command_editor_widgets[WIDX_GET].type = WWT_BUTTON;
            window_title_command_editor_widgets[WIDX_TEXTBOX_FULL].type = WWT_TEXT_BOX;
            break;
        case TITLE_SCRIPT_FOLLOW:
            window_title_command_editor_widgets[WIDX_SELECT_SPRITE].type = WWT_BUTTON;
            window_title_command_editor_widgets[WIDX_VIEWPORT].type = WWT_VIEWPORT;

            // Draw button pressed while the tool is active
            if (sprite_selector_tool_is_active())
                w->pressed_widgets |= (1 << WIDX_SELECT_SPRITE);
            else
                w->pressed_widgets &= ~(1 << WIDX_SELECT_SPRITE);

            break;
    }

    if ((gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) == SCREEN_FLAGS_TITLE_DEMO)
    {
        w->disabled_widgets |= (1 << WIDX_GET) | (1 << WIDX_SELECT_SPRITE);
        window_title_command_editor_widgets[WIDX_SELECT_SPRITE].tooltip = STR_TITLE_COMMAND_EDITOR_SELECT_SPRITE_TOOLTIP;
    }
    else
    {
        w->disabled_widgets &= ~((1 << WIDX_GET) | (1 << WIDX_SELECT_SPRITE));
        window_title_command_editor_widgets[WIDX_SELECT_SPRITE].tooltip = STR_NONE;
    }
}

static void window_title_command_editor_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    window_draw_widgets(w, dpi);

    TITLE_COMMAND_ORDER command_info = get_command_info(command.Type);

    // "Command:" label
    gfx_draw_string_left(
        dpi, STR_TITLE_COMMAND_EDITOR_COMMAND_LABEL, nullptr, w->colours[1], w->windowPos + ScreenCoordsXY{ WS, BY - 14 });

    // Command dropdown name
    gfx_draw_string_left_clipped(
        dpi, command_info.nameStringId, nullptr, w->colours[1],
        { w->windowPos.x + w->widgets[WIDX_COMMAND].left + 1, w->windowPos.y + w->widgets[WIDX_COMMAND].top },
        w->widgets[WIDX_COMMAND_DROPDOWN].left - w->widgets[WIDX_COMMAND].left - 4);

    // Label (e.g. "Location:")
    gfx_draw_string_left(dpi, command_info.descStringId, nullptr, w->colours[1], w->windowPos + ScreenCoordsXY{ WS, BY2 - 14 });

    if (command.Type == TITLE_SCRIPT_SPEED)
    {
        gfx_draw_string_left_clipped(
            dpi, SpeedNames[command.Speed - 1], nullptr, w->colours[1],
            { w->windowPos.x + w->widgets[WIDX_INPUT].left + 1, w->windowPos.y + w->widgets[WIDX_INPUT].top },
            w->widgets[WIDX_INPUT_DROPDOWN].left - w->widgets[WIDX_INPUT].left - 4);
    }
    if (command.Type == TITLE_SCRIPT_FOLLOW)
    {
        uint8_t colour = COLOUR_BLACK;
        rct_string_id spriteString = STR_TITLE_COMMAND_EDITOR_FORMAT_SPRITE_NAME;
        if (command.SpriteIndex != SPRITE_INDEX_NULL)
        {
            window_draw_viewport(dpi, w);
            Formatter::Common().Add<utf8*>(command.SpriteName);
        }
        else
        {
            colour = w->colours[1];
            spriteString = STR_TITLE_COMMAND_EDITOR_FOLLOW_NO_SPRITE;
        }

        gfx_set_dirty_blocks(
            { { w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_VIEWPORT].left, w->widgets[WIDX_VIEWPORT].top } },
              { w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_VIEWPORT].right, w->widgets[WIDX_VIEWPORT].bottom } } });
        gfx_draw_string_left_clipped(
            dpi, spriteString, gCommonFormatArgs, colour,
            { w->windowPos.x + w->widgets[WIDX_VIEWPORT].left + 2, w->windowPos.y + w->widgets[WIDX_VIEWPORT].top + 1 },
            w->widgets[WIDX_VIEWPORT].width() - 2);
    }
    else if (command.Type == TITLE_SCRIPT_LOAD)
    {
        if (command.SaveIndex == SAVE_INDEX_INVALID)
        {
            gfx_draw_string_left_clipped(
                dpi, STR_TITLE_COMMAND_EDITOR_NO_SAVE_SELECTED, nullptr, w->colours[1],
                { w->windowPos.x + w->widgets[WIDX_INPUT].left + 1, w->windowPos.y + w->widgets[WIDX_INPUT].top },
                w->widgets[WIDX_INPUT_DROPDOWN].left - w->widgets[WIDX_INPUT].left - 4);
        }
        else
        {
            Formatter::Common().Add<utf8*>(_sequence->Saves[command.SaveIndex]);
            gfx_draw_string_left_clipped(
                dpi, STR_STRING, gCommonFormatArgs, w->colours[1],
                { w->windowPos.x + w->widgets[WIDX_INPUT].left + 1, w->windowPos.y + w->widgets[WIDX_INPUT].top },
                w->widgets[WIDX_INPUT_DROPDOWN].left - w->widgets[WIDX_INPUT].left - 4);
        }
    }
    else if (command.Type == TITLE_SCRIPT_LOADSC)
    {
        if (command.Scenario[0] == '\0')
        {
            gfx_draw_string_left_clipped(
                dpi, STR_TITLE_COMMAND_EDITOR_NO_SCENARIO_SELECTED, nullptr, w->colours[1],
                { w->windowPos.x + w->widgets[WIDX_INPUT].left + 1, w->windowPos.y + w->widgets[WIDX_INPUT].top },
                w->widgets[WIDX_INPUT_DROPDOWN].left - w->widgets[WIDX_INPUT].left - 4);
        }
        else
        {
            const char* name = "";
            rct_string_id nameString = STR_STRING;
            auto scenario = GetScenarioRepository()->GetByInternalName(command.Scenario);
            if (scenario != nullptr)
            {
                name = scenario->name;
            }
            else
            {
                nameString = STR_TITLE_COMMAND_EDITOR_MISSING_SCENARIO;
            }
            Formatter::Common().Add<const char*>(name);
            gfx_draw_string_left_clipped(
                dpi, nameString, gCommonFormatArgs, w->colours[1],
                { w->windowPos.x + w->widgets[WIDX_INPUT].left + 1, w->windowPos.y + w->widgets[WIDX_INPUT].top },
                w->widgets[WIDX_INPUT_DROPDOWN].left - w->widgets[WIDX_INPUT].left - 4);
        }
    }
}
