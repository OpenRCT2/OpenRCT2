/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <iterator>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/localisation/Localisation.h>

static constexpr const rct_string_id WINDOW_TITLE = STR_MUSIC_ACKNOWLEDGEMENTS;
static constexpr const int32_t WH = 314;
static constexpr const int32_t WW = 510;

// clang-format off
enum WINDOW_MUSIC_CREDITS_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE
};

static rct_widget window_music_credits_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({4, 18}, {502, 292}, WindowWidgetType::Scroll, WindowColour::Primary, SCROLL_VERTICAL), // scroll
    { WIDGETS_END },
};

static constexpr const rct_string_id music_credits[] = {
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_MARCH_CHILDREN_OF_THE_REGIMENT,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_HEYKENS_SERENADE,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_IN_CONTINENTAL_MOOD,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_WEDDING_JOURNEY,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_TALES_FROM_THE_VIENNA_WOODS,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_SLAVONIC_DANCE,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_DAS_ALPENHORN,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_THE_BLOND_SAILOR,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_OVERTURE_POET_AND_PEASANT,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_WALTZ_MEDLEY,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_BELLA_BELLA_BIMBA,
};

static constexpr const rct_string_id music_credits_rct2[] = {
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_RCT2_TITLE_MUSIC,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_DODGEMS_BEAT,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_MIS_SUMMERS_HEAT,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_PHARAOS_TOMB,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_CAESARS_MARCH,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_DRIFTING_TO_HEAVEN,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_INVADERS,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_ETERNAL_TOYBOX,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_JUNGLE_JUICE,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_NINJAS_NOODLES,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_VOYAGE_TO_ANDROMEDA,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_BRIMBLES_BEAT,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_ATLANTIS,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_WILD_WEST_KID,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_VAMPIRES_LAIR,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_BLOCKUBSTER,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_AIRTIME_ROCK,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_SEARCHLIGHT_RAG,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_FLIGHT_OF_FANTASY,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_BIG_ROCK,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_HYPOTHERMIA,
    STR_MUSIC_ACKNOWLEDGEMENTS_SAMPLES_COURTESY,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_LAST_SLEIGH_RIDE,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_PIPES_OF_GLENCAIRN,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_TRAFFIC_JAM,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_TOCCATA,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_SPACE_ROCK,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_MANIC_MECHANIC,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_TECHNO_TORTURE,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_SWEET_DREAMS,
    STR_MUSIC_ACKNOWLEDGEMENTS_TRACK_WHAT_SHALL_WE_DO_WITH_THE_DRUNKEN_SAILOR,
};

static void window_music_credits_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_music_credits_scrollgetsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void window_music_credits_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_music_credits_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex);

static rct_window_event_list window_music_credits_events([](auto& events)
{
    events.mouse_up = &window_music_credits_mouseup;
    events.get_scroll_size = &window_music_credits_scrollgetsize;
    events.paint = &window_music_credits_paint;
    events.scroll_paint = &window_music_credits_scrollpaint;
});
// clang-format on

/**
 *
 *  rct2: 0x0066D55B
 */
rct_window* window_music_credits_open()
{
    rct_window* window;

    // Check if window is already open
    window = window_bring_to_front_by_class(WC_MUSIC_CREDITS);
    if (window != nullptr)
        return window;

    window = WindowCreateCentred(510, 314, &window_music_credits_events, WC_MUSIC_CREDITS, 0);

    window->widgets = window_music_credits_widgets;
    window->enabled_widgets = 1 << WIDX_CLOSE;

    WindowInitScrollWidgets(window);
    window->colours[0] = COLOUR_LIGHT_BLUE;
    window->colours[1] = COLOUR_LIGHT_BLUE;
    window->colours[2] = COLOUR_LIGHT_BLUE;

    return window;
}

/**
 *
 *  rct2: 0x0066DB2C
 */
static void window_music_credits_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
    }
}

/**
 *
 *  rct2: 0x0066DB37
 */
static void window_music_credits_scrollgetsize(rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height)
{
    int32_t lineHeight = font_get_line_height(FontSpriteBase::MEDIUM);
    *height = static_cast<int32_t>(std::size(music_credits) + std::size(music_credits_rct2)) * lineHeight + 12;
}

/**
 *
 *  rct2: 0x0066D7B9
 */
static void window_music_credits_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
}

/**
 *
 *  rct2: 0x0066D7BF
 */
static void window_music_credits_scrollpaint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex)
{
    int32_t lineHeight = font_get_line_height(FontSpriteBase::MEDIUM);

    auto screenCoords = ScreenCoordsXY{ 245, 2 };

    for (size_t i = 0; i < std::size(music_credits); i++)
    {
        DrawTextBasic(dpi, screenCoords, music_credits[i], nullptr, { TextAlignment::CENTRE });
        screenCoords.y += lineHeight;
    }

    // Add 4 more space before "Original recordings ...".
    screenCoords.y += 4;
    DrawTextBasic(dpi, screenCoords, STR_MUSIC_ACKNOWLEDGEMENTS_ORIGINAL_RECORDINGS, nullptr, { TextAlignment::CENTRE });
    screenCoords.y += lineHeight;

    // Draw the separator
    screenCoords.y += 5;
    gfx_fill_rect_inset(dpi, 4, screenCoords.y, 484, screenCoords.y + 1, w->colours[1], INSET_RECT_FLAG_BORDER_INSET);
    screenCoords.y += lineHeight + 1;

    for (size_t i = 0; i < std::size(music_credits_rct2); i++)
    {
        DrawTextBasic(dpi, screenCoords, music_credits_rct2[i], nullptr, { TextAlignment::CENTRE });
        screenCoords.y += lineHeight;
    }
}
