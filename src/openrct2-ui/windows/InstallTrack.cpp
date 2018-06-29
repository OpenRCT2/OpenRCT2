/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <string>
#include <vector>

#include <openrct2/Context.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/ride/TrackDesignRepository.h>
#include <openrct2/core/Math.hpp>
#include <openrct2-ui/windows/Window.h>

#include <openrct2/audio/audio.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/platform/platform.h>
#include <openrct2/sprites.h>
#include <openrct2/util/Util.h>
#include <openrct2/ride/TrackDesign.h>

// clang-format off
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

#define WW 380
#define WH 448
#define WW_LESS_PADDING (WW - 4)
#define PREVIEW_BUTTONS_LEFT (WW - 25)
#define ACTION_BUTTONS_LEFT (WW - 100)

static rct_widget window_install_track_widgets[] = {
    { WWT_FRAME,            0,  0,                      WW - 1,             0,      WH-1,   STR_NONE,                               STR_NONE                },
    { WWT_CAPTION,          0,  1,                      WW - 2,             1,      14,     STR_TRACK_DESIGN_INSTALL_WINDOW_TITLE,  STR_WINDOW_TITLE_TIP    },
    { WWT_CLOSEBOX,         0,  WW - 13,                WW - 3,             2,      13,     STR_CLOSE_X,                            STR_CLOSE_WINDOW_TIP    },
    { WWT_FLATBTN,          0,  4,                      WW - 5,             18,     236,    STR_NONE,                               STR_NONE                },
    { WWT_FLATBTN,          0,  PREVIEW_BUTTONS_LEFT,   WW_LESS_PADDING,    422,    445,    SPR_ROTATE_ARROW,                       STR_ROTATE_90_TIP       },
    { WWT_FLATBTN,          0,  PREVIEW_BUTTONS_LEFT,   WW_LESS_PADDING,    398,    421,    SPR_SCENERY,                            STR_TOGGLE_SCENERY_TIP  },
    { WWT_BUTTON,           0,  ACTION_BUTTONS_LEFT,    WW_LESS_PADDING,    241,    255,    STR_INSTALL_NEW_TRACK_DESIGN_INSTALL,   STR_NONE                },
    { WWT_BUTTON,           0,  ACTION_BUTTONS_LEFT,    WW_LESS_PADDING,    259,    273,    STR_INSTALL_NEW_TRACK_DESIGN_CANCEL,    STR_NONE                },
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
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_install_track_text_input,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_install_track_invalidate,
    window_install_track_paint,
    nullptr
};
// clang-format on

static rct_track_td6 * _trackDesign;
static std::string _trackPath;
static std::string _trackName;
static std::vector<uint8_t> _trackDesignPreviewPixels;

static void window_install_track_update_preview();
static void window_install_track_design(rct_window *w);

/**
*
*  rct2: 0x006D386D
*/
rct_window * window_install_track_open(const utf8 *path)
{
    _trackDesign = track_design_open(path);
    if (_trackDesign == nullptr) {
        context_show_error(STR_UNABLE_TO_LOAD_FILE, STR_NONE);
        return nullptr;
    }

    object_manager_unload_all_objects();
    if (_trackDesign->type == RIDE_TYPE_NULL){
        log_error("Failed to load track (ride type null): %s", path);
        return nullptr;
    }
    if (object_manager_load_object(&_trackDesign->vehicle_object) == nullptr) {
        log_error("Failed to load track (vehicle load fail): %s", path);
        return nullptr;
    }

    window_close_by_class(WC_EDITOR_OBJECT_SELECTION);
    window_close_construction_windows();

    gTrackDesignSceneryToggle = false;
    _currentTrackPieceDirection = 2;

    int32_t screenWidth = context_get_width();
    int32_t screenHeight = context_get_height();
    int32_t x = screenWidth / 2 - 201;
    int32_t y = std::max(TOP_TOOLBAR_HEIGHT + 1, screenHeight / 2 - 200);

    rct_window *w = window_create(x, y, WW, WH, &window_install_track_events, WC_INSTALL_TRACK, 0);
    w->widgets = window_install_track_widgets;
    w->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_ROTATE) | (1 << WIDX_TOGGLE_SCENERY) | (1 << WIDX_INSTALL) | (1 << WIDX_CANCEL);
    window_init_scroll_widgets(w);
    w->track_list.track_list_being_updated = false;
    window_push_others_right(w);

    _trackPath = path;
    _trackName = GetNameFromTrackPath(path);
    _trackDesignPreviewPixels.resize(4 * TRACK_PREVIEW_IMAGE_SIZE);

    window_install_track_update_preview();
    window_invalidate(w);

    return w;
}

/**
*
*  rct2: 0x006D41DC
*/
static void window_install_track_close(rct_window *w)
{
    _trackPath.clear();
    _trackName.clear();
    _trackDesignPreviewPixels.clear();
    _trackDesignPreviewPixels.shrink_to_fit();
    track_design_dispose(_trackDesign);
    _trackDesign = nullptr;
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
    int32_t x = w->x + widget->left + 1;
    int32_t y = w->y + widget->top + 1;
    int32_t colour = ColourMapA[w->colours[0]].darkest;
    gfx_fill_rect(dpi, x, y, x + 369, y + 216, colour);

    rct_g1_element g1temp = {};
    g1temp.offset = _trackDesignPreviewPixels.data() + (_currentTrackPieceDirection * TRACK_PREVIEW_IMAGE_SIZE);
    g1temp.width = 370;
    g1temp.height = 217;
    g1temp.flags = G1_FLAG_BMP;
    gfx_set_g1_element(SPR_TEMP, &g1temp);
    gfx_draw_sprite(dpi, SPR_TEMP, x, y, 0);

    x = w->x + (widget->left + widget->right) / 2;
    y = w->y + widget->bottom - 12;

    // Warnings
    rct_track_td6 *td6 = _trackDesign;
    if (td6->track_flags & TRACK_DESIGN_FLAG_SCENERY_UNAVAILABLE)
    {
        if (!gTrackDesignSceneryToggle)
        {
            // Scenery not available
            gfx_draw_string_centred_clipped(dpi, STR_DESIGN_INCLUDES_SCENERY_WHICH_IS_UNAVAILABLE, nullptr, COLOUR_BLACK, x, y, 368);
            y -= LIST_ROW_HEIGHT;
        }
    }

    // Information
    x = w->x + widget->left + 1;
    y = w->y + widget->bottom + 4;
    // 0x006D3CF1 -- 0x006d3d71 missing

    // Track design name & type
    gfx_draw_string_left(dpi, STR_TRACK_DESIGN_NAME, &_trackName, COLOUR_BLACK, x-1, y);
    y += LIST_ROW_HEIGHT;

    rct_ride_name rideName;
    rct_string_id friendlyTrackName;

    void * objectEntry = object_manager_load_object(&td6->vehicle_object);
    if (objectEntry != nullptr)
    {
        int32_t groupIndex = object_manager_get_loaded_object_entry_index(objectEntry);
        rideName = get_ride_naming(td6->type, get_ride_entry(groupIndex));
        friendlyTrackName = rideName.name;
    }
    else
    {
        // Fall back on the technical track name if the vehicle object cannot be loaded
        friendlyTrackName = RideNaming[td6->type].name;
    }

    gfx_draw_string_left(dpi, STR_TRACK_DESIGN_TYPE, &friendlyTrackName, COLOUR_BLACK, x, y);
    y += LIST_ROW_HEIGHT + 4;

    // Stats
    fixed32_2dp rating = td6->excitement * 10;
    gfx_draw_string_left(dpi, STR_TRACK_LIST_EXCITEMENT_RATING, &rating, COLOUR_BLACK, x, y);
    y += LIST_ROW_HEIGHT;

    rating = td6->intensity * 10;
    gfx_draw_string_left(dpi, STR_TRACK_LIST_INTENSITY_RATING, &rating, COLOUR_BLACK, x, y);
    y += LIST_ROW_HEIGHT;

    rating = td6->nausea * 10;
    gfx_draw_string_left(dpi, STR_TRACK_LIST_NAUSEA_RATING, &rating, COLOUR_BLACK, x, y);
    y += LIST_ROW_HEIGHT + 4;

    if (td6->type != RIDE_TYPE_MAZE)
    {
        if (td6->type == RIDE_TYPE_MINI_GOLF)
        {
            // Holes
            uint16_t holes = td6->holes & 0x1F;
            gfx_draw_string_left(dpi, STR_HOLES, &holes, COLOUR_BLACK, x, y);
            y += LIST_ROW_HEIGHT;
        }
        else
        {
            // Maximum speed
            uint16_t speed = ((td6->max_speed << 16) * 9) >> 18;
            gfx_draw_string_left(dpi, STR_MAX_SPEED, &speed, COLOUR_BLACK, x, y);
            y += LIST_ROW_HEIGHT;

            // Average speed
            speed = ((td6->average_speed << 16) * 9) >> 18;
            gfx_draw_string_left(dpi, STR_AVERAGE_SPEED, &speed, COLOUR_BLACK, x, y);
            y += LIST_ROW_HEIGHT;
        }

        // Ride length
        set_format_arg(0, rct_string_id, STR_RIDE_LENGTH_ENTRY);
        set_format_arg(2, uint16_t, td6->ride_length);
        gfx_draw_string_left_clipped(dpi, STR_TRACK_LIST_RIDE_LENGTH, gCommonFormatArgs, COLOUR_BLACK, x, y, 214);
        y += LIST_ROW_HEIGHT;
    }

    if (ride_type_has_flag(td6->type, RIDE_TYPE_FLAG_HAS_G_FORCES))
    {
        // Maximum positive vertical Gs
        int32_t gForces = td6->max_positive_vertical_g * 32;
        gfx_draw_string_left(dpi, STR_MAX_POSITIVE_VERTICAL_G, &gForces, COLOUR_BLACK, x, y);
        y += LIST_ROW_HEIGHT;

        // Maximum negative vertical Gs
        gForces = td6->max_negative_vertical_g * 32;
        gfx_draw_string_left(dpi, STR_MAX_NEGATIVE_VERTICAL_G, &gForces, COLOUR_BLACK, x, y);
        y += LIST_ROW_HEIGHT;

        // Maximum lateral Gs
        gForces = td6->max_lateral_g * 32;
        gfx_draw_string_left(dpi, STR_MAX_LATERAL_G, &gForces, COLOUR_BLACK, x, y);
        y += LIST_ROW_HEIGHT;

        // If .TD6
        if (td6->version_and_colour_scheme / 4 >= 2)
        {
            if (td6->total_air_time != 0)
            {
                // Total air time
                int32_t airTime = td6->total_air_time * 25;
                gfx_draw_string_left(dpi, STR_TOTAL_AIR_TIME, &airTime, COLOUR_BLACK, x, y);
                y += LIST_ROW_HEIGHT;
            }
        }
    }

    if (ride_type_has_flag(td6->type, RIDE_TYPE_FLAG_HAS_DROPS))
    {
        // Drops
        uint16_t drops = td6->drops & 0x3F;
        gfx_draw_string_left(dpi, STR_DROPS, &drops, COLOUR_BLACK, x, y);
        y += LIST_ROW_HEIGHT;

        // Drop height is multiplied by 0.75
        gfx_draw_string_left(dpi, STR_HIGHEST_DROP_HEIGHT, &drops, COLOUR_BLACK, x, y);
        y += LIST_ROW_HEIGHT;
    }

    if (td6->type != RIDE_TYPE_MINI_GOLF)
    {
        uint16_t inversions = td6->inversions & 0x1F;
        if (inversions != 0)
        {
            // Inversions
            gfx_draw_string_left(dpi, STR_INVERSIONS, &inversions, COLOUR_BLACK, x, y);
            y += LIST_ROW_HEIGHT;
        }
    }
    y += 4;

    if (td6->space_required_x != 0xFF)
    {
        // Space required
        set_format_arg(0, uint16_t, td6->space_required_x);
        set_format_arg(2, uint16_t, td6->space_required_y);
        gfx_draw_string_left(dpi, STR_TRACK_LIST_SPACE_REQUIRED, gCommonFormatArgs, COLOUR_BLACK, x, y);
        y += LIST_ROW_HEIGHT;
    }

    if (td6->cost != 0)
    {
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

    _trackName = text;

    window_event_mouse_up_call(w, WIDX_INSTALL);
}

static void window_install_track_update_preview()
{
    track_design_draw_preview(_trackDesign, _trackDesignPreviewPixels.data());
}

static void window_install_track_design(rct_window *w)
{
    utf8 destPath[MAX_PATH];

    platform_get_user_directory(destPath, "track", sizeof(destPath));
    if (!platform_ensure_directory_exists(destPath)) {
        log_error("Unable to create directory '%s'", destPath);
        context_show_error(STR_CANT_SAVE_TRACK_DESIGN, STR_NONE);
        return;
    }

    safe_strcat_path(destPath, _trackName.c_str(), sizeof(destPath));
    path_append_extension(destPath, ".td6", sizeof(destPath));

    if (platform_file_exists(destPath)) {
        log_info("%s already exists, prompting user for a different track design name", destPath);
        context_show_error(STR_UNABLE_TO_INSTALL_THIS_TRACK_DESIGN, STR_NONE);
        window_text_input_raw_open(
            w,
            WIDX_INSTALL,
            STR_SELECT_NEW_NAME_FOR_TRACK_DESIGN,
            STR_AN_EXISTING_TRACK_DESIGN_ALREADY_HAS_THIS_NAME,
            _trackName.c_str(),
            255
        );
    } else {
        if (track_repository_install(_trackPath.c_str())) {
            window_close(w);
        } else {
            context_show_error(STR_CANT_SAVE_TRACK_DESIGN, STR_NONE);
        }
    }
}
