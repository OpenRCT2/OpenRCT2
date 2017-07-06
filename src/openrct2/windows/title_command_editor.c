#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../game.h"
#include "../input.h"
#include "../interface/themes.h"
#include "../interface/viewport.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../rct2.h"
#include "../sprites.h"
#include "../title/TitleScreen.h"
#include "../title/TitleSequence.h"
#include "../util/util.h"
#include "../world/map.h"
#include "dropdown.h"

typedef struct TITLE_COMMAND_ORDER {
    uint8 command;
    rct_string_id nameStringId;
    rct_string_id descStringId;
} TITLE_COMMAND_ORDER;

TITLE_COMMAND_ORDER window_title_command_editor_orders[] = {
    { TITLE_SCRIPT_LOAD,        STR_TITLE_EDITOR_ACTION_LOAD, STR_TITLE_EDITOR_ARGUMENT_SAVEFILE },
    { TITLE_SCRIPT_LOCATION,    STR_TITLE_EDITOR_COMMAND_TYPE_LOCATION, STR_TITLE_EDITOR_ARGUMENT_COORDINATES },
    { TITLE_SCRIPT_ROTATE,      STR_TITLE_EDITOR_COMMAND_TYPE_ROTATE, STR_TITLE_EDITOR_ARGUMENT_ROTATIONS },
    { TITLE_SCRIPT_ZOOM,        STR_TITLE_EDITOR_COMMAND_TYPE_ZOOM, STR_TITLE_EDITOR_ARGUMENT_ZOOM_LEVEL },
    { TITLE_SCRIPT_SPEED,       STR_TITLE_EDITOR_COMMAND_TYPE_SPEED, STR_TITLE_EDITOR_ARGUMENT_SPEED },
    { TITLE_SCRIPT_WAIT,        STR_TITLE_EDITOR_COMMAND_TYPE_WAIT, STR_TITLE_EDITOR_ARGUMENT_WAIT_SECONDS },
    { TITLE_SCRIPT_RESTART,     STR_TITLE_EDITOR_RESTART, STR_NONE },
    { TITLE_SCRIPT_END,         STR_TITLE_EDITOR_END, STR_NONE },
};

#define NUM_COMMANDS 8

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
    WIDX_OKAY,
    WIDX_CANCEL
};

#define WW 200
#define WH 120
#define BY 32
#define BY2 70
#define WS 16
#define WHA ((WW-WS*2)/2)

static bool _window_title_command_editor_insert;
static sint32 _window_title_command_editor_index;
#define BUF_SIZE 50
static char textbox1Buffer[BUF_SIZE];
static char textbox2Buffer[BUF_SIZE];
static TitleCommand command = { TITLE_SCRIPT_LOAD, { .SaveIndex = 0 } };
static TitleSequence * _sequence = NULL;

static rct_widget window_title_command_editor_widgets[] = {
    { WWT_FRAME,                1,  0,          WW-1,       0,      WH-1,   0xFFFFFFFF,                         STR_NONE },                         // panel / background
    { WWT_CAPTION,              1,  1,          WW-2,       1,      14,     STR_TITLE_COMMAND_EDITOR_TITLE,                     STR_WINDOW_TITLE_TIP },             // title bar
    { WWT_CLOSEBOX,             1,  WW-13,      WW-3,       2,      13,     STR_CLOSE_X,                        STR_CLOSE_WINDOW_TIP },             // close x button
    { WWT_DROPDOWN,             1,  WS,         WW-WS-1,    BY,     BY+11,  STR_NONE,                   STR_NONE }, // Command dropdown
    { WWT_DROPDOWN_BUTTON,      1,  WW-WS-12,   WW-WS-2,    BY+1,   BY+10,  STR_DROPDOWN_GLYPH,         STR_NONE },
    { WWT_TEXT_BOX,             1,  WS,         WW-WS-1,    BY2,    BY2+11, STR_NONE,                   STR_NONE }, // full textbox

    { WWT_TEXT_BOX,             1,  WS,         WS+WHA-4,   BY2,    BY2+11, STR_NONE,                   STR_NONE }, // x textbox
    { WWT_TEXT_BOX,             1,  WS+WHA+3,   WW-WS-1,    BY2,    BY2+11, STR_NONE,                   STR_NONE }, // y textbox

    { WWT_DROPDOWN,             1,  16,         WW-17,      BY2,    BY2+11, STR_NONE,                   STR_NONE }, // Save dropdown
    { WWT_DROPDOWN_BUTTON,      1,  WW-28,      WW-18,      BY2+1,  BY2+10, STR_DROPDOWN_GLYPH,         STR_NONE },

    { WWT_DROPDOWN_BUTTON,      1,  WS+WHA+3,   WW-WS-1,    BY2-14, BY2-3,  STR_TITLE_COMMAND_EDITOR_ACTION_GET_LOCATION,                       STR_NONE }, // Get location/zoom/etc

    { WWT_DROPDOWN_BUTTON,      1,  10,         80,         WH-21,  WH-10,  STR_OK,                     STR_NONE }, // OKAY
    { WWT_DROPDOWN_BUTTON,      1,  WW-80,      WW-10,      WH-21,  WH-10,  STR_CANCEL,                 STR_NONE }, // Cancel

    { WIDGETS_END },
};

static void window_title_command_editor_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_title_command_editor_mousedown(rct_widgetindex widgetIndex, rct_window*w, rct_widget* widget);
static void window_title_command_editor_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex);
static void window_title_command_editor_update(rct_window *w);
static void window_title_command_editor_invalidate(rct_window *w);
static void window_title_command_editor_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_title_command_editor_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text);
static void window_title_command_editor_inputsize(rct_window *w);
static sint32 get_command_info_index(sint32 index);
static TITLE_COMMAND_ORDER get_command_info(sint32 index);
static rct_xy16 get_location();
static uint8 get_zoom();

static rct_window_event_list window_title_command_editor_events = {
    NULL,
    window_title_command_editor_mouseup,
    NULL,
    window_title_command_editor_mousedown,
    window_title_command_editor_dropdown,
    NULL,
    window_title_command_editor_update,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    window_title_command_editor_textinput,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    window_title_command_editor_invalidate,
    window_title_command_editor_paint,
    NULL
};

static sint32 get_command_info_index(sint32 index)
{
    for (sint32 i = 0; i < NUM_COMMANDS; i++) {
        if (window_title_command_editor_orders[i].command == index)
            return i;
    }
    return 0;
}

static TITLE_COMMAND_ORDER get_command_info(sint32 index)
{
    for (sint32 i = 0; i < NUM_COMMANDS; i++) {
        if (window_title_command_editor_orders[i].command == index)
            return window_title_command_editor_orders[i];
    }
    return window_title_command_editor_orders[0];
}

static rct_xy16 get_location()
{
    rct_xy16 mapCoord = { 0 };
    rct_window *w = window_get_main();
    if (w != NULL) {
        sint32 interactionType;
        rct_map_element *mapElement;

        get_map_coordinates_from_pos(w->viewport->view_width / 2, w->viewport->view_height / 2, VIEWPORT_INTERACTION_MASK_TERRAIN, &mapCoord.x, &mapCoord.y, &interactionType, &mapElement, NULL);
        mapCoord.x -= 16;
        mapCoord.x /= 32;
        mapCoord.y -= 16;
        mapCoord.y /= 32;
        mapCoord.x++;
        mapCoord.y++;
    }
    return mapCoord;
}

static uint8 get_zoom()
{
    uint8 zoom = 0;
    rct_window *w = window_get_main();
    if (w != NULL) {
        zoom = w->viewport->zoom;
    }
    return zoom;
}

void window_title_command_editor_open(TitleSequence * sequence, sint32 index, bool insert)
{
    _sequence = sequence;

    // Check if window is already open
    if (window_find_by_class(WC_TITLE_COMMAND_EDITOR) != NULL)
        return;

    rct_window * window = window_create_centred(
        WW,
        WH,
        &window_title_command_editor_events,
        WC_TITLE_COMMAND_EDITOR,
        WF_STICK_TO_FRONT
    );
    window_title_command_editor_widgets[WIDX_TEXTBOX_FULL].string = textbox1Buffer;
    window_title_command_editor_widgets[WIDX_TEXTBOX_X].string = textbox1Buffer;
    window_title_command_editor_widgets[WIDX_TEXTBOX_Y].string = textbox2Buffer;
    window->widgets = window_title_command_editor_widgets;
    window->enabled_widgets =
        (1 << WIDX_CLOSE) |
        (1 << WIDX_COMMAND) |
        (1 << WIDX_COMMAND_DROPDOWN) |
        (1 << WIDX_TEXTBOX_FULL) |
        (1 << WIDX_TEXTBOX_X) |
        (1 << WIDX_TEXTBOX_Y) |
        (1 << WIDX_INPUT) |
        (1 << WIDX_INPUT_DROPDOWN) |
        (1 << WIDX_GET) |
        (1 << WIDX_OKAY) |
        (1 << WIDX_CANCEL);
    window_init_scroll_widgets(window);

    _window_title_command_editor_index = index;
    _window_title_command_editor_insert = insert;
    if (!insert) {
        command = _sequence->Commands[index];
    }

    switch (command.Type) {
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
    case TITLE_SCRIPT_WAIT:
        snprintf(textbox1Buffer, BUF_SIZE, "%d", command.Rotations);
        break;
    }
}

static void window_title_command_editor_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex) {
    case WIDX_CLOSE:
    case WIDX_CANCEL:
        window_close(w);
        break;
    case WIDX_TEXTBOX_FULL:
        window_start_textbox(w, widgetIndex, STR_STRING, textbox1Buffer, 4);
        break;
    case WIDX_TEXTBOX_X:
        window_start_textbox(w, widgetIndex, STR_STRING, textbox1Buffer, 4);
        break;
    case WIDX_TEXTBOX_Y:
        window_start_textbox(w, widgetIndex, STR_STRING, textbox2Buffer, 4);
        break;
    case WIDX_GET:
        if (command.Type == TITLE_SCRIPT_LOCATION) {
            rct_xy16 mapCoord = get_location();
            command.X = (uint8)mapCoord.x;
            command.Y = (uint8)mapCoord.y;
            snprintf(textbox1Buffer, BUF_SIZE, "%d", command.X);
            snprintf(textbox2Buffer, BUF_SIZE, "%d", command.Y);
        } else if (command.Type == TITLE_SCRIPT_ZOOM) {
            uint8 zoom = get_zoom();
            command.Zoom = zoom;
            snprintf(textbox1Buffer, BUF_SIZE, "%d", command.Zoom);
        }
        window_invalidate(w);
        break;
    case WIDX_OKAY:
        if (_window_title_command_editor_insert) {
            size_t insertIndex = _window_title_command_editor_index;
            _sequence->NumCommands++;
            _sequence->Commands = realloc(_sequence->Commands, _sequence->NumCommands * sizeof(TitleCommand));
            for (size_t i = _sequence->NumCommands - 1; i > insertIndex; i--) {
                _sequence->Commands[i] = _sequence->Commands[i - 1];
            }
            _sequence->Commands[insertIndex] = command;
        } else {
            _sequence->Commands[_window_title_command_editor_index] = command;
            TileSequenceSave(_sequence);
        }
        TileSequenceSave(_sequence);

        rct_window *title_editor_w = window_find_by_class(WC_TITLE_EDITOR);
        if (title_editor_w != NULL) {
            title_editor_w->selected_list_item = _window_title_command_editor_index;
        }
        window_close(w);
        break;
    }
}

static void window_title_command_editor_mousedown(rct_widgetindex widgetIndex, rct_window* w, rct_widget* widget)
{
    widget--;
    switch (widgetIndex) {
    case WIDX_COMMAND_DROPDOWN:
    {
        sint32 numItems = NUM_COMMANDS;
        for (sint32 i = 0; i < numItems; i++) {
            gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
            gDropdownItemsArgs[i] = window_title_command_editor_orders[i].nameStringId;
        }

        window_dropdown_show_text_custom_width(
            w->x + widget->left,
            w->y + widget->top,
            widget->bottom - widget->top + 1,
            w->colours[1],
            0,
            DROPDOWN_FLAG_STAY_OPEN,
            numItems,
            widget->right - widget->left - 3);

        dropdown_set_checked(get_command_info_index(command.Type), true);
        break;
    }
    case WIDX_INPUT_DROPDOWN:
        if (command.Type == TITLE_SCRIPT_SPEED) {
            sint32 numItems = 4;
            for (sint32 i = 0; i < numItems; i++) {
                gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
                gDropdownItemsArgs[i] = SpeedNames[i];
            }

            window_dropdown_show_text_custom_width(
                w->x + widget->left,
                w->y + widget->top,
                widget->bottom - widget->top + 1,
                w->colours[1],
                0,
                DROPDOWN_FLAG_STAY_OPEN,
                numItems,
                widget->right - widget->left - 3);

            dropdown_set_checked(command.Speed - 1, true);
        } else if (command.Type == TITLE_SCRIPT_LOAD) {
            sint32 numItems = (sint32)_sequence->NumSaves;
            for (sint32 i = 0; i < numItems; i++) {
                gDropdownItemsFormat[i] = STR_OPTIONS_DROPDOWN_ITEM;
                gDropdownItemsArgs[i] = (uintptr_t)_sequence->Saves[i];
            }

            window_dropdown_show_text_custom_width(
                w->x + widget->left,
                w->y + widget->top,
                widget->bottom - widget->top + 1,
                w->colours[1],
                0,
                DROPDOWN_FLAG_STAY_OPEN,
                numItems,
                widget->right - widget->left - 3);

            dropdown_set_checked(command.SaveIndex, true);
        }
        break;
    }
}

static void window_title_command_editor_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex)
{
    if (dropdownIndex == -1)
        return;

    switch (widgetIndex) {
    case WIDX_COMMAND_DROPDOWN:
        if (dropdownIndex == get_command_info_index(command.Type)) {
            break;
        }
        command.Type = window_title_command_editor_orders[dropdownIndex].command;
        switch (command.Type) {
        case TITLE_SCRIPT_LOCATION:
        {
            rct_xy16 mapCoord = get_location();
            command.X = (uint8)mapCoord.x;
            command.Y = (uint8)mapCoord.y;
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
        case TITLE_SCRIPT_SPEED:
            command.Speed = 1;
            break;
        case TITLE_SCRIPT_WAIT:
            command.Milliseconds = 10000;
            snprintf(textbox1Buffer, BUF_SIZE, "%d", command.Milliseconds);
            break;
        case TITLE_SCRIPT_LOAD:
            command.SaveIndex = 0;
            if (command.SaveIndex >= _sequence->NumSaves) {
                command.SaveIndex = 0xFF;
            }
            break;
        }
        window_invalidate(w);
        break;
    case WIDX_INPUT_DROPDOWN:
        switch (command.Type) {
        case TITLE_SCRIPT_SPEED:
            if (dropdownIndex != command.Speed - 1) {
                command.Speed = (uint8)(dropdownIndex + 1);
                window_invalidate(w);
            }
            break;
        case TITLE_SCRIPT_LOAD:
            if (dropdownIndex != command.SaveIndex) {
                command.SaveIndex = (uint8)dropdownIndex;
                window_invalidate(w);
            }
            break;
        }
        break;
    }
}

static void window_title_command_editor_textinput(rct_window * w, rct_widgetindex widgetIndex, char * text)
{
    char * end;
    sint32 value = strtol(widgetIndex != WIDX_TEXTBOX_Y ? textbox1Buffer : textbox2Buffer, &end, 10);
    if (value < 0) value = 0;
    if (value > 255 && command.Type != TITLE_SCRIPT_WAIT) value = 255;
    switch (widgetIndex) {
    case WIDX_TEXTBOX_FULL:
        if (text == NULL) {
            if (*end == '\0') {
                if (command.Type == TITLE_SCRIPT_ROTATE || command.Type == TITLE_SCRIPT_ZOOM) {
                    if (value > 3) value = 3;
                }
                else if (command.Type == TITLE_SCRIPT_WAIT) {
                    if (value < 1) value = 100;
                }
                command.Rotations = (uint8)value;
            }
            snprintf(textbox1Buffer, BUF_SIZE, "%d", command.Rotations);
            window_invalidate(w);
        } else {
            safe_strcpy(textbox1Buffer, text, sizeof(textbox1Buffer));
        }
        break;
    case WIDX_TEXTBOX_X:
        if (text == NULL) {
            if (*end == '\0') {
                command.X = (uint8)value;
            }
            snprintf(textbox1Buffer, BUF_SIZE, "%d", command.X);
            window_invalidate(w);
        } else {
            safe_strcpy(textbox1Buffer, text, sizeof(textbox1Buffer));
        }
        break;
    case WIDX_TEXTBOX_Y:
        if (text == NULL) {
            if (*end == '\0') {
                command.Y = (uint8)value;
            }
            snprintf(textbox2Buffer, BUF_SIZE, "%d", command.Y);
            window_invalidate(w);
        } else {
            safe_strcpy(textbox2Buffer, text, sizeof(textbox2Buffer));
        }
        break;
    }
}

static void window_title_command_editor_update(rct_window *w)
{
    if (gCurrentTextBox.window.classification == w->classification &&
        gCurrentTextBox.window.number == w->number
    ) {
        window_update_textbox_caret();
        widget_invalidate(w, gCurrentTextBox.widget_index);
    }
}

static void window_title_command_editor_invalidate(rct_window *w)
{
    colour_scheme_update_by_class(w, WC_TITLE_EDITOR);

    window_title_command_editor_widgets[WIDX_TEXTBOX_FULL].type = WWT_EMPTY;
    window_title_command_editor_widgets[WIDX_TEXTBOX_X].type = WWT_EMPTY;
    window_title_command_editor_widgets[WIDX_TEXTBOX_Y].type = WWT_EMPTY;
    window_title_command_editor_widgets[WIDX_INPUT].type = WWT_EMPTY;
    window_title_command_editor_widgets[WIDX_INPUT_DROPDOWN].type = WWT_EMPTY;
    window_title_command_editor_widgets[WIDX_GET].type = WWT_EMPTY;
    switch (command.Type) {
    case TITLE_SCRIPT_LOAD:
    case TITLE_SCRIPT_SPEED:
        window_title_command_editor_widgets[WIDX_INPUT].type = WWT_DROPDOWN;
        window_title_command_editor_widgets[WIDX_INPUT_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
        break;
    case TITLE_SCRIPT_LOCATION:
        window_title_command_editor_widgets[WIDX_TEXTBOX_X].type = WWT_TEXT_BOX;
        window_title_command_editor_widgets[WIDX_TEXTBOX_Y].type = WWT_TEXT_BOX;
        window_title_command_editor_widgets[WIDX_GET].type = WWT_DROPDOWN_BUTTON;
        break;
    case TITLE_SCRIPT_ROTATE:
    case TITLE_SCRIPT_WAIT:
        window_title_command_editor_widgets[WIDX_TEXTBOX_FULL].type = WWT_TEXT_BOX;
        break;
    case TITLE_SCRIPT_ZOOM:
        window_title_command_editor_widgets[WIDX_GET].type = WWT_DROPDOWN_BUTTON;
        window_title_command_editor_widgets[WIDX_TEXTBOX_FULL].type = WWT_TEXT_BOX;
        break;
    }

    if ((gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) == SCREEN_FLAGS_TITLE_DEMO) {
        w->disabled_widgets |= (1 << WIDX_GET);
    } else {
        w->disabled_widgets &= ~(1 << WIDX_GET);
    }
}

static void window_title_command_editor_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);

    gfx_draw_string_left(dpi, STR_TITLE_COMMAND_EDITOR_COMMAND_LABEL, NULL, w->colours[1], w->x + WS, w->y + BY - 14);
    gfx_draw_string_left(dpi, get_command_info(command.Type).descStringId, NULL, w->colours[1], w->x + WS, w->y + BY2 - 14);

    gfx_draw_string_left_clipped(
        dpi,
        get_command_info(command.Type).nameStringId,
        NULL,
        w->colours[1],
        w->x + w->widgets[WIDX_COMMAND].left + 1,
        w->y + w->widgets[WIDX_COMMAND].top,
        w->widgets[WIDX_COMMAND_DROPDOWN].left - w->widgets[WIDX_COMMAND].left - 4);

    if (command.Type == TITLE_SCRIPT_SPEED) {
        gfx_draw_string_left_clipped(
            dpi,
            SpeedNames[command.Speed - 1],
            NULL,
            w->colours[1],
            w->x + w->widgets[WIDX_INPUT].left + 1,
            w->y + w->widgets[WIDX_INPUT].top,
            w->widgets[WIDX_INPUT_DROPDOWN].left - w->widgets[WIDX_INPUT].left - 4);
    }
    else if (command.Type == TITLE_SCRIPT_LOAD) {
        if (command.SaveIndex == SAVE_INDEX_INVALID) {
            gfx_draw_string_left_clipped(
                dpi,
                STR_TITLE_COMMAND_EDITOR_NO_SAVE_SELECTED,
                NULL,
                w->colours[1],
                w->x + w->widgets[WIDX_INPUT].left + 1,
                w->y + w->widgets[WIDX_INPUT].top,
                w->widgets[WIDX_INPUT_DROPDOWN].left - w->widgets[WIDX_INPUT].left - 4);
        }
        else {
            set_format_arg(0, uintptr_t, (uintptr_t)_sequence->Saves[command.SaveIndex]);
            gfx_draw_string_left_clipped(
                dpi,
                STR_STRING,
                gCommonFormatArgs,
                w->colours[1],
                w->x + w->widgets[WIDX_INPUT].left + 1,
                w->y + w->widgets[WIDX_INPUT].top,
                w->widgets[WIDX_INPUT_DROPDOWN].left - w->widgets[WIDX_INPUT].left - 4);
        }
    }
}
