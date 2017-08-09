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

#include "../Context.h"
#include "../object/ObjectManager.h"
#include "../ride/TrackDesignRepository.h"
#include "../core/Math.hpp"
#include "../core/Memory.hpp"

extern "C"
{
    #include "../audio/audio.h"
    #include "../interface/widget.h"
    #include "../localisation/localisation.h"
    #include "../platform/platform.h"
    #include "../sprites.h"
    #include "../util/util.h"
    #include "error.h"
}

enum {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_TRACK_PREVIEW,
    WIDX_ROTATE,
    WIDX_TOGGLE_SCENERY,
    WIDX_INSTALL,
    WIDX_CANCEL
};

static rct_widget window_install_track_widgets[] = {
    { WWT_FRAME,            0,  0,      401,    0,      399,    0xFFFFFFFF,                             STR_NONE                },
    { WWT_CAPTION,          0,  1,      400,    1,      14,     STR_TRACK_DESIGN_INSTALL_WINDOW_TITLE,  STR_WINDOW_TITLE_TIP    },
    { WWT_CLOSEBOX,         0,  389,    399,    2,      13,     STR_CLOSE_X,                            STR_CLOSE_WINDOW_TIP    },
    { WWT_FLATBTN,          0,  15,     386,    18,     236,    0xFFFFFFFF,                             STR_NONE                },
    { WWT_FLATBTN,          0,  376,    399,    374,    397,    SPR_ROTATE_ARROW,                       STR_ROTATE_90_TIP       },
    { WWT_FLATBTN,          0,  376,    399,    350,    373,    SPR_SCENERY,                            STR_TOGGLE_SCENERY_TIP  },
    { WWT_DROPDOWN_BUTTON,  0,  303,    397,    241,    252,    STR_INSTALL_NEW_TRACK_DESIGN_INSTALL,   STR_NONE                },
    { WWT_DROPDOWN_BUTTON,  0,  303,    397,    256,    267,    STR_INSTALL_NEW_TRACK_DESIGN_CANCEL,    STR_NONE                },
    { WIDGETS_END },
};

static void window_install_track_close(rct_window *w);
static void window_install_track_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_install_track_invalidate(rct_window *w);
static void window_install_track_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_install_track_text_input(rct_window *w, rct_widgetindex widgetIndex, char *text);

static rct_window_event_list window_install_track_events = {
    window_install_track_close,
    window_install_track_mouseup,
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
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    window_install_track_text_input,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    window_install_track_invalidate,
    window_install_track_paint,
    NULL
};

static rct_track_td6 *_trackDesign;
static utf8 *_trackPath;
static utf8 *_trackName;
static uint8 *_trackDesignPreviewPixels;

static void window_install_track_update_preview();
static void window_install_track_design(rct_window *w);

/**
*
*  rct2: 0x006D386D
*/
void window_install_track_open(const utf8 *path)
{
    _trackDesign = track_design_open(path);
    if (_trackDesign == NULL) {
        window_error_open(STR_UNABLE_TO_LOAD_FILE, STR_NONE);
        return;
    }

    object_manager_unload_all_objects();
    if (_trackDesign->type == RIDE_TYPE_NULL){
        log_error("Failed to load track (ride type null): %s", path);
        return;
    }
    if (object_manager_load_object(&_trackDesign->vehicle_object) == NULL) {
        log_error("Failed to load track (vehicle load fail): %s", path);
        return;
    }

    window_close_by_class(WC_EDITOR_OBJECT_SELECTION);
    window_close_construction_windows();

    gTrackDesignSceneryToggle = false;
    _currentTrackPieceDirection = 2;

    sint32 screenWidth = context_get_width();
    sint32 screenHeight = context_get_height();
    sint32 x = screenWidth / 2 - 201;
    sint32 y = Math::Max(TOP_TOOLBAR_HEIGHT + 1, screenHeight / 2 - 200);

    rct_window *w = window_create(x, y, 402, 400, &window_install_track_events, WC_INSTALL_TRACK, 0);
    w->widgets = window_install_track_widgets;
    w->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_ROTATE) | (1 << WIDX_TOGGLE_SCENERY) | (1 << WIDX_INSTALL) | (1 << WIDX_CANCEL);
    window_init_scroll_widgets(w);
    w->track_list.var_484 = 0;
    window_push_others_right(w);

    _trackPath = _strdup(path);
    _trackName = track_repository_get_name_from_path(path);
    _trackDesignPreviewPixels = Memory::AllocateArray<uint8>(4 * TRACK_PREVIEW_IMAGE_SIZE);

    window_install_track_update_preview();
    window_invalidate(w);
}

/**
*
*  rct2: 0x006D41DC
*/
static void window_install_track_close(rct_window *w)
{
    SafeFree(_trackPath);
    SafeFree(_trackName);
    SafeFree(_trackDesignPreviewPixels);
    track_design_dispose(_trackDesign);
    _trackDesign = NULL;
}

/**
*
*  rct2: 0x006D407A
*/
static void window_install_track_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex) {
    case WIDX_CLOSE:
    case WIDX_CANCEL:
        window_close(w);
        break;
    case WIDX_ROTATE:
        _currentTrackPieceDirection++;
        _currentTrackPieceDirection %= 4;
        window_invalidate(w);
        break;
    case WIDX_TOGGLE_SCENERY:
        gTrackDesignSceneryToggle = !gTrackDesignSceneryToggle;
        window_install_track_update_preview();
        window_invalidate(w);
        break;
    case WIDX_INSTALL:
        window_install_track_design(w);
        break;
    }
}


/**
*
*  rct2: 0x006D3B06
*/
static void window_install_track_invalidate(rct_window *w)
{
    w->pressed_widgets |= 1 << WIDX_TRACK_PREVIEW;
    if (!gTrackDesignSceneryToggle) {
        w->pressed_widgets |= (1 << WIDX_TOGGLE_SCENERY);
    } else {
        w->pressed_widgets &= ~(1 << WIDX_TOGGLE_SCENERY);
    }

    // if (w->track_list.var_482 != 0xFFFF) {
    //  w->disabled_widgets &= ~(1 << WIDX_TRACK_PREVIEW);
    // } else {
    //  w->disabled_widgets |= (1 << WIDX_TRACK_PREVIEW);
    // }
}

/**
*
*  rct2: 0x006D3B1F
*/
static void window_install_track_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);

    // Track preview
    rct_widget *widget = &window_install_track_widgets[WIDX_TRACK_PREVIEW];
    sint32 x = w->x + widget->left + 1;
    sint32 y = w->y + widget->top + 1;
    sint32 colour = ColourMapA[w->colours[0]].darkest;
    gfx_fill_rect(dpi, x, y, x + 369, y + 216, colour);

    rct_g1_element *substituteElement = &g1Elements[SPR_TEMP];
    rct_g1_element tmpElement = *substituteElement;
    substituteElement->offset = _trackDesignPreviewPixels + (_currentTrackPieceDirection * TRACK_PREVIEW_IMAGE_SIZE);
    substituteElement->width = 370;
    substituteElement->height = 217;
    substituteElement->x_offset = 0;
    substituteElement->y_offset = 0;
    substituteElement->flags = G1_FLAG_BMP;
    gfx_draw_sprite(dpi, 0, x, y, 0);
    *substituteElement = tmpElement;

    x = w->x + (widget->left + widget->right) / 2;
    y = w->y + widget->bottom - 12;

    // Warnings
    rct_track_td6 *td6 = _trackDesign;
    if (td6->track_flags & TRACK_DESIGN_FLAG_SCENERY_UNAVAILABLE) {
        if (!gTrackDesignSceneryToggle) {
            // Scenery not available
            gfx_draw_string_centred_clipped(dpi, STR_DESIGN_INCLUDES_SCENERY_WHICH_IS_UNAVAILABLE, NULL, COLOUR_BLACK, x, y, 368);
            y -= 10;
        }
    }

    // Track design name
    gfx_draw_string_centred_clipped(dpi, STR_TRACK_PREVIEW_NAME_FORMAT, &_trackName, COLOUR_BLACK, x, y, 368);

    // Information
    x = w->x + widget->left + 1;
    y = w->y + widget->bottom + 2;
    // 0x006D3CF1 -- 0x006d3d71 missing

    // Stats
    fixed32_2dp rating = td6->excitement * 10;
    gfx_draw_string_left(dpi, STR_TRACK_LIST_EXCITEMENT_RATING, &rating, COLOUR_BLACK, x, y);
    y += 10;

    rating = td6->intensity * 10;
    gfx_draw_string_left(dpi, STR_TRACK_LIST_INTENSITY_RATING, &rating, COLOUR_BLACK, x, y);
    y += 10;

    rating = td6->nausea * 10;
    gfx_draw_string_left(dpi, STR_TRACK_LIST_NAUSEA_RATING, &rating, COLOUR_BLACK, x, y);
    y += 14;

    if (td6->type != RIDE_TYPE_MAZE) {
        if (td6->type == RIDE_TYPE_MINI_GOLF) {
            // Holes
            uint16 holes = td6->holes & 0x1F;
            gfx_draw_string_left(dpi, STR_HOLES, &holes, COLOUR_BLACK, x, y);
            y += 10;
        }
        else {
            // Maximum speed
            uint16 speed = ((td6->max_speed << 16) * 9) >> 18;
            gfx_draw_string_left(dpi, STR_MAX_SPEED, &speed, COLOUR_BLACK, x, y);
            y += 10;

            // Average speed
            speed = ((td6->average_speed << 16) * 9) >> 18;
            gfx_draw_string_left(dpi, STR_AVERAGE_SPEED, &speed, COLOUR_BLACK, x, y);
            y += 10;
        }

        // Ride length
        set_format_arg(0, rct_string_id, STR_RIDE_LENGTH_ENTRY);
        set_format_arg(2, uint16, td6->ride_length);
        gfx_draw_string_left_clipped(dpi, STR_TRACK_LIST_RIDE_LENGTH, gCommonFormatArgs, COLOUR_BLACK, x, y, 214);
        y += 10;
    }

    if (ride_type_has_flag(td6->type, RIDE_TYPE_FLAG_HAS_G_FORCES)) {
        // Maximum positive vertical Gs
        sint32 gForces = td6->max_positive_vertical_g * 32;
        gfx_draw_string_left(dpi, STR_MAX_POSITIVE_VERTICAL_G, &gForces, COLOUR_BLACK, x, y);
        y += 10;

        // Maximum negative vertical Gs
        gForces = td6->max_negative_vertical_g * 32;
        gfx_draw_string_left(dpi, STR_MAX_NEGATIVE_VERTICAL_G, &gForces, COLOUR_BLACK, x, y);
        y += 10;

        // Maximum lateral Gs
        gForces = td6->max_lateral_g * 32;
        gfx_draw_string_left(dpi, STR_MAX_LATERAL_G, &gForces, COLOUR_BLACK, x, y);
        y += 10;

        // If .TD6
        if (td6->version_and_colour_scheme / 4 >= 2) {
            if (td6->total_air_time != 0) {
                // Total air time
                sint32 airTime = td6->total_air_time * 25;
                gfx_draw_string_left(dpi, STR_TOTAL_AIR_TIME, &airTime, COLOUR_BLACK, x, y);
                y += 10;
            }
        }
    }

    if (ride_type_has_flag(td6->type, RIDE_TYPE_FLAG_HAS_DROPS)) {
        // Drops
        uint16 drops = td6->drops & 0x3F;
        gfx_draw_string_left(dpi, STR_DROPS, &drops, COLOUR_BLACK, x, y);
        y += 10;

        // Drop height is multiplied by 0.75
        gfx_draw_string_left(dpi, STR_HIGHEST_DROP_HEIGHT, &drops, COLOUR_BLACK, x, y);
        y += 10;
    }

    if (td6->type != RIDE_TYPE_MINI_GOLF) {
        uint16 inversions = td6->inversions & 0x1F;
        if (inversions != 0) {
            // Inversions
            gfx_draw_string_left(dpi, STR_INVERSIONS, &inversions, COLOUR_BLACK, x, y);
            y += 10;
        }
    }
    y += 4;

    if (td6->space_required_x != 0xFF) {
        // Space required
        set_format_arg(0, uint16, td6->space_required_x);
        set_format_arg(2, uint16, td6->space_required_y);
        gfx_draw_string_left(dpi, STR_TRACK_LIST_SPACE_REQUIRED, gCommonFormatArgs, COLOUR_BLACK, x, y);
        y += 10;
    }

    if (td6->cost != 0) {
        gfx_draw_string_left(dpi, STR_TRACK_LIST_COST_AROUND, &td6->cost, COLOUR_BLACK, x, y);
    }
}

/**
*
*  rct2: 0x006D40A7
*/
static void window_install_track_text_input(rct_window *w, rct_widgetindex widgetIndex, char *text)
{
    if (widgetIndex != WIDX_INSTALL || str_is_null_or_empty(text)) {
        return;
    }

    free(_trackName);
    _trackName = _strdup(text);

    window_event_mouse_up_call(w, WIDX_INSTALL);
}

static void window_install_track_update_preview()
{
    track_design_draw_preview(_trackDesign, _trackDesignPreviewPixels);
}

static void window_install_track_design(rct_window *w)
{
    utf8 destPath[MAX_PATH];

    platform_get_user_directory(destPath, "track", sizeof(destPath));
    if (!platform_ensure_directory_exists(destPath)) {
        log_error("Unable to create directory '%s'", destPath);
        window_error_open(STR_CANT_SAVE_TRACK_DESIGN, STR_NONE);
        return;
    }

    safe_strcat_path(destPath, _trackName, sizeof(destPath));
    path_append_extension(destPath, ".td6", sizeof(destPath));

    if (platform_file_exists(destPath)) {
        log_info("%s already exists, prompting user for a different track design name", destPath);
        window_error_open(STR_UNABLE_TO_INSTALL_THIS_TRACK_DESIGN, STR_NONE);
        window_text_input_raw_open(
            w,
            WIDX_INSTALL,
            STR_SELECT_NEW_NAME_FOR_TRACK_DESIGN,
            STR_AN_EXISTING_TRACK_DESIGN_ALREADY_HAS_THIS_NAME,
            _trackName,
            255
        );
    } else {
        if (track_repository_install(_trackPath)) {
            window_close(w);
        } else {
            window_error_open(STR_CANT_SAVE_TRACK_DESIGN, STR_NONE);
        }
    }
}
