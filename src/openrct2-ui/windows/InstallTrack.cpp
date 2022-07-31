/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <memory>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/PlatformEnvironment.h>
#include <openrct2/audio/audio.h>
#include <openrct2/core/File.h>
#include <openrct2/core/Path.hpp>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/platform/Platform.h>
#include <openrct2/ride/RideConstruction.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/TrackDesign.h>
#include <openrct2/ride/TrackDesignRepository.h>
#include <openrct2/sprites.h>
#include <openrct2/util/Util.h>
#include <string>
#include <vector>

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

static constexpr const StringId WINDOW_TITLE = STR_TRACK_DESIGN_INSTALL_WINDOW_TITLE;
static constexpr const int32_t WW = 380;
static constexpr const int32_t WH = 460;
constexpr int32_t PREVIEW_BUTTONS_LEFT = WW - 25;
constexpr int32_t ACTION_BUTTONS_LEFT = WW - 100;

static rct_widget window_install_track_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({                   4,  18}, {372, 219}, WindowWidgetType::FlatBtn, WindowColour::Primary                                                              ),
    MakeWidget({PREVIEW_BUTTONS_LEFT, 422}, { 22,  24}, WindowWidgetType::FlatBtn, WindowColour::Primary, SPR_ROTATE_ARROW,                     STR_ROTATE_90_TIP     ),
    MakeWidget({PREVIEW_BUTTONS_LEFT, 398}, { 22,  24}, WindowWidgetType::FlatBtn, WindowColour::Primary, SPR_SCENERY,                          STR_TOGGLE_SCENERY_TIP),
    MakeWidget({ ACTION_BUTTONS_LEFT, 241}, { 97,  15}, WindowWidgetType::Button,  WindowColour::Primary, STR_INSTALL_NEW_TRACK_DESIGN_INSTALL                        ),
    MakeWidget({ ACTION_BUTTONS_LEFT, 259}, { 97,  15}, WindowWidgetType::Button,  WindowColour::Primary, STR_INSTALL_NEW_TRACK_DESIGN_CANCEL                         ),
    WIDGETS_END,
};

static void WindowInstallTrackClose(rct_window *w);
static void WindowInstallTrackMouseup(rct_window *w, rct_widgetindex widgetIndex);
static void WindowInstallTrackInvalidate(rct_window *w);
static void WindowInstallTrackPaint(rct_window *w, rct_drawpixelinfo *dpi);
static void WindowInstallTrackTextInput(rct_window *w, rct_widgetindex widgetIndex, char *text);

static rct_window_event_list window_install_track_events([](auto& events)
{
    events.close = &WindowInstallTrackClose;
    events.mouse_up = &WindowInstallTrackMouseup;
    events.text_input = &WindowInstallTrackTextInput;
    events.invalidate = &WindowInstallTrackInvalidate;
    events.paint = &WindowInstallTrackPaint;
});
// clang-format on

static std::unique_ptr<TrackDesign> _trackDesign;
static std::string _trackPath;
static std::string _trackName;
static std::vector<uint8_t> _trackDesignPreviewPixels;

static void WindowInstallTrackUpdatePreview();
static void WindowInstallTrackDesign(rct_window* w);

/**
 *
 *  rct2: 0x006D386D
 */
rct_window* WindowInstallTrackOpen(const utf8* path)
{
    _trackDesign = TrackDesignImport(path);
    if (_trackDesign == nullptr)
    {
        context_show_error(STR_UNABLE_TO_LOAD_FILE, STR_NONE, {});
        return nullptr;
    }

    object_manager_unload_all_objects();
    if (_trackDesign->type == RIDE_TYPE_NULL)
    {
        log_error("Failed to load track (ride type null): %s", path);
        return nullptr;
    }
    if (object_manager_load_object(&_trackDesign->vehicle_object.Entry) == nullptr)
    {
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

    rct_window* w = WindowCreate(ScreenCoordsXY(x, y), WW, WH, &window_install_track_events, WC_INSTALL_TRACK, 0);
    w->widgets = window_install_track_widgets;
    WindowInitScrollWidgets(*w);
    w->track_list.track_list_being_updated = false;
    window_push_others_right(*w);

    _trackPath = path;
    _trackName = GetNameFromTrackPath(path);
    _trackDesignPreviewPixels.resize(4 * TRACK_PREVIEW_IMAGE_SIZE);

    WindowInstallTrackUpdatePreview();
    w->Invalidate();

    return w;
}

/**
 *
 *  rct2: 0x006D41DC
 */
static void WindowInstallTrackClose(rct_window* w)
{
    _trackPath.clear();
    _trackName.clear();
    _trackDesignPreviewPixels.clear();
    _trackDesignPreviewPixels.shrink_to_fit();
    _trackDesign = nullptr;
}

/**
 *
 *  rct2: 0x006D407A
 */
static void WindowInstallTrackMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
        case WIDX_CANCEL:
            window_close(*w);
            break;
        case WIDX_ROTATE:
            _currentTrackPieceDirection++;
            _currentTrackPieceDirection %= 4;
            w->Invalidate();
            break;
        case WIDX_TOGGLE_SCENERY:
            gTrackDesignSceneryToggle = !gTrackDesignSceneryToggle;
            WindowInstallTrackUpdatePreview();
            w->Invalidate();
            break;
        case WIDX_INSTALL:
            WindowInstallTrackDesign(w);
            break;
    }
}

/**
 *
 *  rct2: 0x006D3B06
 */
static void WindowInstallTrackInvalidate(rct_window* w)
{
    w->pressed_widgets |= 1ULL << WIDX_TRACK_PREVIEW;
    if (!gTrackDesignSceneryToggle)
    {
        w->pressed_widgets |= (1ULL << WIDX_TOGGLE_SCENERY);
    }
    else
    {
        w->pressed_widgets &= ~(1ULL << WIDX_TOGGLE_SCENERY);
    }
}

/**
 *
 *  rct2: 0x006D3B1F
 */
static void WindowInstallTrackPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(*w, dpi);

    // Track preview
    rct_widget* widget = &window_install_track_widgets[WIDX_TRACK_PREVIEW];
    auto screenPos = w->windowPos + ScreenCoordsXY{ widget->left + 1, widget->top + 1 };
    int32_t colour = ColourMapA[w->colours[0]].darkest;
    gfx_fill_rect(dpi, { screenPos, screenPos + ScreenCoordsXY{ 369, 216 } }, colour);

    rct_g1_element g1temp = {};
    g1temp.offset = _trackDesignPreviewPixels.data() + (_currentTrackPieceDirection * TRACK_PREVIEW_IMAGE_SIZE);
    g1temp.width = 370;
    g1temp.height = 217;
    g1temp.flags = G1_FLAG_HAS_TRANSPARENCY;
    gfx_set_g1_element(SPR_TEMP, &g1temp);
    drawing_engine_invalidate_image(SPR_TEMP);
    gfx_draw_sprite(dpi, ImageId(SPR_TEMP), screenPos);

    screenPos = w->windowPos + ScreenCoordsXY{ widget->midX(), widget->bottom - 12 };

    // Warnings
    const TrackDesign* td6 = _trackDesign.get();
    if (td6->track_flags & TRACK_DESIGN_FLAG_SCENERY_UNAVAILABLE)
    {
        if (!gTrackDesignSceneryToggle)
        {
            // Scenery not available
            DrawTextEllipsised(
                dpi, screenPos, 308, STR_DESIGN_INCLUDES_SCENERY_WHICH_IS_UNAVAILABLE, {}, { TextAlignment::CENTRE });
            screenPos.y -= LIST_ROW_HEIGHT;
        }
    }

    // Information
    screenPos = w->windowPos + ScreenCoordsXY{ widget->left + 1, widget->bottom + 4 };
    // 0x006D3CF1 -- 0x006d3d71 missing

    // Track design name & type
    {
        auto trackName = _trackName.c_str();
        auto ft = Formatter();
        ft.Add<const char*>(trackName);
        DrawTextBasic(dpi, screenPos - ScreenCoordsXY{ 1, 0 }, STR_TRACK_DESIGN_NAME, ft);
        screenPos.y += LIST_ROW_HEIGHT;
    }

    // Friendly Track name
    {
        auto ft = Formatter();

        const auto* objectEntry = object_manager_load_object(&td6->vehicle_object.Entry);
        if (objectEntry != nullptr)
        {
            auto groupIndex = object_manager_get_loaded_object_entry_index(objectEntry);
            auto rideName = get_ride_naming(td6->type, get_ride_entry(groupIndex));
            ft.Add<StringId>(rideName.Name);
        }
        else
        {
            // Fall back on the technical track name if the vehicle object cannot be loaded
            ft.Add<StringId>(GetRideTypeDescriptor(td6->type).Naming.Name);
        }

        DrawTextBasic(dpi, screenPos, STR_TRACK_DESIGN_TYPE, ft);
        screenPos.y += LIST_ROW_HEIGHT + 4;
    }

    // Stats
    {
        fixed32_2dp rating = td6->excitement * 10;
        auto ft = Formatter();
        ft.Add<int32_t>(rating);
        DrawTextBasic(dpi, screenPos, STR_TRACK_LIST_EXCITEMENT_RATING, ft);
        screenPos.y += LIST_ROW_HEIGHT;
    }
    {
        fixed32_2dp rating = td6->intensity * 10;
        auto ft = Formatter();
        ft.Add<int32_t>(rating);
        DrawTextBasic(dpi, screenPos, STR_TRACK_LIST_INTENSITY_RATING, ft);
        screenPos.y += LIST_ROW_HEIGHT;
    }
    {
        fixed32_2dp rating = td6->nausea * 10;
        auto ft = Formatter();
        ft.Add<int32_t>(rating);
        DrawTextBasic(dpi, screenPos, STR_TRACK_LIST_NAUSEA_RATING, ft);
        screenPos.y += LIST_ROW_HEIGHT + 4;
    }

    if (td6->type != RIDE_TYPE_MAZE)
    {
        if (td6->type == RIDE_TYPE_MINI_GOLF)
        {
            // Holes
            uint16_t holes = td6->holes & 0x1F;
            auto ft = Formatter();
            ft.Add<uint16_t>(holes);
            DrawTextBasic(dpi, screenPos, STR_HOLES, ft);
            screenPos.y += LIST_ROW_HEIGHT;
        }
        else
        {
            // Maximum speed
            {
                uint16_t speed = ((td6->max_speed << 16) * 9) >> 18;
                auto ft = Formatter();
                ft.Add<uint16_t>(speed);
                DrawTextBasic(dpi, screenPos, STR_MAX_SPEED, ft);
                screenPos.y += LIST_ROW_HEIGHT;
            }
            // Average speed
            {
                uint16_t speed = ((td6->average_speed << 16) * 9) >> 18;
                auto ft = Formatter();
                ft.Add<uint16_t>(speed);
                DrawTextBasic(dpi, screenPos, STR_AVERAGE_SPEED, ft);
                screenPos.y += LIST_ROW_HEIGHT;
            }
        }

        // Ride length
        auto ft = Formatter();
        ft.Add<StringId>(STR_RIDE_LENGTH_ENTRY);
        ft.Add<uint16_t>(td6->ride_length);
        DrawTextEllipsised(dpi, screenPos, 214, STR_TRACK_LIST_RIDE_LENGTH, ft);
        screenPos.y += LIST_ROW_HEIGHT;
    }

    if (GetRideTypeDescriptor(td6->type).HasFlag(RIDE_TYPE_FLAG_HAS_G_FORCES))
    {
        // Maximum positive vertical Gs
        {
            int32_t gForces = td6->max_positive_vertical_g * 32;
            auto ft = Formatter();
            ft.Add<int32_t>(gForces);
            DrawTextBasic(dpi, screenPos, STR_MAX_POSITIVE_VERTICAL_G, ft);
            screenPos.y += LIST_ROW_HEIGHT;
        }
        // Maximum negative vertical Gs
        {
            int32_t gForces = td6->max_negative_vertical_g * 32;
            auto ft = Formatter();
            ft.Add<int32_t>(gForces);
            DrawTextBasic(dpi, screenPos, STR_MAX_NEGATIVE_VERTICAL_G, ft);
            screenPos.y += LIST_ROW_HEIGHT;
        }
        // Maximum lateral Gs
        {
            int32_t gForces = td6->max_lateral_g * 32;
            auto ft = Formatter();
            ft.Add<int32_t>(gForces);
            DrawTextBasic(dpi, screenPos, STR_MAX_LATERAL_G, ft);
            screenPos.y += LIST_ROW_HEIGHT;
        }
        if (td6->total_air_time != 0)
        {
            // Total air time
            int32_t airTime = td6->total_air_time * 25;
            auto ft = Formatter();
            ft.Add<int32_t>(airTime);
            DrawTextBasic(dpi, screenPos, STR_TOTAL_AIR_TIME, ft);
            screenPos.y += LIST_ROW_HEIGHT;
        }
    }

    if (GetRideTypeDescriptor(td6->type).HasFlag(RIDE_TYPE_FLAG_HAS_DROPS))
    {
        // Drops
        uint16_t drops = td6->drops & 0x3F;
        auto ft = Formatter();
        ft.Add<uint16_t>(drops);
        DrawTextBasic(dpi, screenPos, STR_DROPS, ft);
        screenPos.y += LIST_ROW_HEIGHT;

        // Drop height is multiplied by 0.75
        DrawTextBasic(dpi, screenPos, STR_HIGHEST_DROP_HEIGHT, ft);
        screenPos.y += LIST_ROW_HEIGHT;
    }

    if (td6->type != RIDE_TYPE_MINI_GOLF)
    {
        uint16_t inversions = td6->inversions & 0x1F;
        if (inversions != 0)
        {
            // Inversions
            auto ft = Formatter();
            ft.Add<uint16_t>(inversions);
            DrawTextBasic(dpi, screenPos, STR_INVERSIONS, ft);
            screenPos.y += LIST_ROW_HEIGHT;
        }
    }
    screenPos.y += 4;

    if (td6->space_required_x != 0xFF)
    {
        // Space required
        auto ft = Formatter();
        ft.Add<uint16_t>(td6->space_required_x);
        ft.Add<uint16_t>(td6->space_required_y);
        DrawTextBasic(dpi, screenPos, STR_TRACK_LIST_SPACE_REQUIRED, ft);
        screenPos.y += LIST_ROW_HEIGHT;
    }

    if (td6->cost != 0)
    {
        auto ft = Formatter();
        ft.Add<money64>(td6->cost);
        DrawTextBasic(dpi, screenPos, STR_TRACK_LIST_COST_AROUND, ft);
    }
}

/**
 *
 *  rct2: 0x006D40A7
 */
static void WindowInstallTrackTextInput(rct_window* w, rct_widgetindex widgetIndex, char* text)
{
    if (widgetIndex != WIDX_INSTALL || str_is_null_or_empty(text))
    {
        return;
    }

    _trackName = text;

    window_event_mouse_up_call(w, WIDX_INSTALL);
}

static void WindowInstallTrackUpdatePreview()
{
    TrackDesignDrawPreview(_trackDesign.get(), _trackDesignPreviewPixels.data());
}

static void WindowInstallTrackDesign(rct_window* w)
{
    auto env = OpenRCT2::GetContext()->GetPlatformEnvironment();
    auto destPath = env->GetDirectoryPath(OpenRCT2::DIRBASE::USER, OpenRCT2::DIRID::TRACK);
    if (!Platform::EnsureDirectoryExists(destPath.c_str()))
    {
        log_error("Unable to create directory '%s'", destPath.c_str());
        context_show_error(STR_CANT_SAVE_TRACK_DESIGN, STR_NONE, {});
        return;
    }

    destPath = Path::Combine(destPath, _trackName + u8".td6");

    if (File::Exists(destPath))
    {
        log_info("%s already exists, prompting user for a different track design name", destPath.c_str());
        context_show_error(STR_UNABLE_TO_INSTALL_THIS_TRACK_DESIGN, STR_NONE, {});
        WindowTextInputRawOpen(
            w, WIDX_INSTALL, STR_SELECT_NEW_NAME_FOR_TRACK_DESIGN, STR_AN_EXISTING_TRACK_DESIGN_ALREADY_HAS_THIS_NAME, {},
            _trackName.c_str(), 255);
    }
    else
    {
        if (track_repository_install(_trackPath.c_str(), _trackName.c_str()))
        {
            window_close(*w);
        }
        else
        {
            context_show_error(STR_CANT_SAVE_TRACK_DESIGN, STR_NONE, {});
        }
    }
}
