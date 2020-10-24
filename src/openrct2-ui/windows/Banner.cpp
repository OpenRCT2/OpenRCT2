/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Game.h>
#include <openrct2/actions/BannerRemoveAction.hpp>
#include <openrct2/actions/BannerSetColourAction.hpp>
#include <openrct2/actions/BannerSetNameAction.hpp>
#include <openrct2/actions/BannerSetStyleAction.hpp>
#include <openrct2/config/Config.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/sprites.h>
#include <openrct2/world/Banner.h>
#include <openrct2/world/Scenery.h>

static constexpr const int32_t WW = 113;
static constexpr const int32_t WH = 96;
static constexpr const rct_string_id WINDOW_TITLE = STR_BANNER_WINDOW_TITLE;

// clang-format off
enum WINDOW_BANNER_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_VIEWPORT,
    WIDX_BANNER_TEXT,
    WIDX_BANNER_NO_ENTRY,
    WIDX_BANNER_DEMOLISH,
    WIDX_MAIN_COLOUR,
    WIDX_TEXT_COLOUR_DROPDOWN,
    WIDX_TEXT_COLOUR_DROPDOWN_BUTTON
};

static constexpr const rct_string_id BannerColouredTextFormats[] = {
    STR_TEXT_COLOR_BLACK,
    STR_TEXT_COLOR_GREY,
    STR_TEXT_COLOR_WHITE,
    STR_TEXT_COLOR_RED,
    STR_TEXT_COLOR_GREEN,
    STR_TEXT_COLOR_YELLOW,
    STR_TEXT_COLOR_TOPAZ,
    STR_TEXT_COLOR_CELADON,
    STR_TEXT_COLOR_BABYBLUE,
    STR_TEXT_COLOR_PALELAVENDER,
    STR_TEXT_COLOR_PALEGOLD,
    STR_TEXT_COLOR_LIGHTPINK,
    STR_TEXT_COLOR_PEARLAQUA,
    STR_TEXT_COLOR_PALESILVER,
};

static rct_widget window_banner_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({      3,      17}, {85, 60}, WWT_VIEWPORT,  WindowColour::Secondary, 0x0FFFFFFFE                                        ), // tab content panel
    MakeWidget({WW - 25,      19}, {24, 24}, WWT_FLATBTN,   WindowColour::Secondary, SPR_RENAME,         STR_CHANGE_BANNER_TEXT_TIP     ), // change banner button
    MakeWidget({WW - 25,      43}, {24, 24}, WWT_FLATBTN,   WindowColour::Secondary, SPR_NO_ENTRY,       STR_SET_AS_NO_ENTRY_BANNER_TIP ), // no entry button
    MakeWidget({WW - 25,      67}, {24, 24}, WWT_FLATBTN,   WindowColour::Secondary, SPR_DEMOLISH,       STR_DEMOLISH_BANNER_TIP        ), // demolish button
    MakeWidget({      5, WH - 16}, {12, 12}, WWT_COLOURBTN, WindowColour::Secondary, 0xFFFFFFFF,         STR_SELECT_MAIN_SIGN_COLOUR_TIP), // high money
    MakeWidget({     43, WH - 16}, {39, 12}, WWT_DROPDOWN,  WindowColour::Secondary                                                     ), // high money
    MakeWidget({     70, WH - 15}, {11, 10}, WWT_BUTTON,    WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_SELECT_TEXT_COLOUR_TIP     ), // high money
    { WIDGETS_END },
};

static void window_banner_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_banner_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_banner_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_banner_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text);
static void window_banner_viewport_rotate(rct_window *w);
static void window_banner_invalidate(rct_window *w);
static void window_banner_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_banner_events = {
    nullptr,
    window_banner_mouseup,
    nullptr,
    window_banner_mousedown,
    window_banner_dropdown,
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
    window_banner_textinput,
    window_banner_viewport_rotate,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_banner_invalidate,
    window_banner_paint,
    nullptr
};
// clang-format on

/**
 *
 *  rct2: 0x006BA305
 */
rct_window* window_banner_open(rct_windownumber number)
{
    rct_window* w;
    rct_widget* viewportWidget;

    // Check if window is already open
    w = window_bring_to_front_by_number(WC_BANNER, number);
    if (w != nullptr)
        return w;

    w = window_create_auto_pos(WW, WH, &window_banner_events, WC_BANNER, WF_NO_SCROLLING);
    w->widgets = window_banner_widgets;
    w->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_BANNER_TEXT) | (1 << WIDX_BANNER_NO_ENTRY) | (1 << WIDX_BANNER_DEMOLISH)
        | (1 << WIDX_MAIN_COLOUR) | (1 << WIDX_TEXT_COLOUR_DROPDOWN) | (1 << WIDX_TEXT_COLOUR_DROPDOWN_BUTTON);

    w->number = number;
    window_init_scroll_widgets(w);

    auto banner = GetBanner(w->number);
    auto bannerViewPos = banner->position.ToCoordsXY().ToTileCentre();

    TileElement* tile_element = map_get_first_element_at(bannerViewPos);
    if (tile_element == nullptr)
        return nullptr;
    while (1)
    {
        if ((tile_element->GetType() == TILE_ELEMENT_TYPE_BANNER) && (tile_element->AsBanner()->GetIndex() == w->number))
        {
            break;
        }

        tile_element++;
    }

    int32_t view_z = tile_element->GetBaseZ();
    w->frame_no = view_z;

    // Create viewport
    viewportWidget = &window_banner_widgets[WIDX_VIEWPORT];
    viewport_create(
        w, w->windowPos + ScreenCoordsXY{ viewportWidget->left + 1, viewportWidget->top + 1 }, viewportWidget->width() - 2,
        viewportWidget->height() - 2, 0, { bannerViewPos, view_z }, 0, SPRITE_INDEX_NULL);

    w->viewport->flags = gConfigGeneral.always_show_gridlines ? VIEWPORT_FLAG_GRIDLINES : 0;
    w->Invalidate();

    return w;
}

/**
 *
 *  rct2: 0x6ba4d6
 */
static void window_banner_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    auto banner = GetBanner(w->number);
    auto bannerPos = banner->position.ToCoordsXY();

    TileElement* tile_element = map_get_first_element_at(bannerPos);
    if (tile_element == nullptr)
        return;

    while (1)
    {
        if ((tile_element->GetType() == TILE_ELEMENT_TYPE_BANNER) && (tile_element->AsBanner()->GetIndex() == w->number))
            break;
        tile_element++;
    }

    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_BANNER_DEMOLISH:
        {
            auto bannerRemoveAction = BannerRemoveAction(
                { bannerPos, tile_element->GetBaseZ(), tile_element->AsBanner()->GetPosition() });
            GameActions::Execute(&bannerRemoveAction);
            break;
        }
        case WIDX_BANNER_TEXT:
            window_text_input_raw_open(
                w, WIDX_BANNER_TEXT, STR_BANNER_TEXT, STR_ENTER_BANNER_TEXT, banner->GetText().c_str(), 32);
            break;
        case WIDX_BANNER_NO_ENTRY:
        {
            textinput_cancel();
            auto bannerSetStyle = BannerSetStyleAction(
                BannerSetStyleType::NoEntry, w->number, banner->flags ^ BANNER_FLAG_NO_ENTRY);
            GameActions::Execute(&bannerSetStyle);
            break;
        }
    }
}

/**
 *
 *  rct2: 0x6ba4ff
 */
static void window_banner_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    auto banner = GetBanner(w->number);

    switch (widgetIndex)
    {
        case WIDX_MAIN_COLOUR:
            window_dropdown_show_colour(w, widget, TRANSLUCENT(w->colours[1]), banner->colour);
            break;
        case WIDX_TEXT_COLOUR_DROPDOWN_BUTTON:

            for (int32_t i = 0; i < 13; ++i)
            {
                gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
                gDropdownItemsArgs[i] = BannerColouredTextFormats[i + 1];
            }

            // Switch to the dropdown box widget.
            widget--;

            window_dropdown_show_text_custom_width(
                { widget->left + w->windowPos.x, widget->top + w->windowPos.y }, widget->height() + 1, w->colours[1], 0,
                DROPDOWN_FLAG_STAY_OPEN, 13, widget->width() - 3);

            dropdown_set_checked(banner->text_colour - 1, true);
            break;
    }
}

/**
 *
 *  rct2: 0x6ba517
 */
static void window_banner_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    switch (widgetIndex)
    {
        case WIDX_MAIN_COLOUR:
        {
            if (dropdownIndex == -1)
                break;

            auto bannerSetStyle = BannerSetStyleAction(BannerSetStyleType::PrimaryColour, w->number, dropdownIndex);
            GameActions::Execute(&bannerSetStyle);
            break;
        }
        case WIDX_TEXT_COLOUR_DROPDOWN_BUTTON:
        {
            if (dropdownIndex == -1)
                break;
            auto bannerSetStyle = BannerSetStyleAction(BannerSetStyleType::TextColour, w->number, dropdownIndex + 1);
            GameActions::Execute(&bannerSetStyle);
            break;
        }
    }
}

/**
 *
 *  rct2: 0x6ba50c
 */
static void window_banner_textinput(rct_window* w, rct_widgetindex widgetIndex, char* text)
{
    if (widgetIndex == WIDX_BANNER_TEXT && text != nullptr)
    {
        auto bannerSetNameAction = BannerSetNameAction(w->number, text);
        GameActions::Execute(&bannerSetNameAction);
    }
}

/**
 *
 *  rct2: 0x006BA44D
 */
static void window_banner_invalidate(rct_window* w)
{
    auto banner = GetBanner(w->number);
    rct_widget* colour_btn = &window_banner_widgets[WIDX_MAIN_COLOUR];
    colour_btn->type = WWT_EMPTY;

    // Scenery item not sure why we use this instead of banner?
    rct_scenery_entry* sceneryEntry = get_banner_entry(banner->type);
    if (sceneryEntry->banner.flags & BANNER_ENTRY_FLAG_HAS_PRIMARY_COLOUR)
    {
        colour_btn->type = WWT_COLOURBTN;
    }

    w->pressed_widgets &= ~(1ULL << WIDX_BANNER_NO_ENTRY);
    w->disabled_widgets &= ~(
        (1ULL << WIDX_BANNER_TEXT) | (1ULL << WIDX_TEXT_COLOUR_DROPDOWN) | (1ULL << WIDX_TEXT_COLOUR_DROPDOWN_BUTTON));

    if (banner->flags & BANNER_FLAG_NO_ENTRY)
    {
        w->pressed_widgets |= (1ULL << WIDX_BANNER_NO_ENTRY);
        w->disabled_widgets |= (1ULL << WIDX_BANNER_TEXT) | (1ULL << WIDX_TEXT_COLOUR_DROPDOWN)
            | (1ULL << WIDX_TEXT_COLOUR_DROPDOWN_BUTTON);
    }

    colour_btn->image = SPRITE_ID_PALETTE_COLOUR_1(banner->colour) | IMAGE_TYPE_TRANSPARENT | SPR_PALETTE_BTN;

    rct_widget* drop_down_widget = &window_banner_widgets[WIDX_TEXT_COLOUR_DROPDOWN];
    drop_down_widget->text = BannerColouredTextFormats[banner->text_colour];
}

/* rct2: 0x006BA4C5 */
static void window_banner_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    window_draw_widgets(w, dpi);

    // Draw viewport
    if (w->viewport != nullptr)
    {
        window_draw_viewport(dpi, w);
    }
}

/**
 *
 *  rct2: 0x6BA7B5
 */
static void window_banner_viewport_rotate(rct_window* w)
{
    rct_viewport* view = w->viewport;
    w->viewport = nullptr;

    view->width = 0;

    auto banner = GetBanner(w->number);

    auto bannerViewPos = CoordsXYZ{ banner->position.ToCoordsXY().ToTileCentre(), w->frame_no };

    // Create viewport
    rct_widget* viewportWidget = &window_banner_widgets[WIDX_VIEWPORT];
    viewport_create(
        w, w->windowPos + ScreenCoordsXY{ viewportWidget->left + 1, viewportWidget->top + 1 }, (viewportWidget->width()) - 1,
        (viewportWidget->height()) - 1, 0, bannerViewPos, 0, SPRITE_INDEX_NULL);

    if (w->viewport != nullptr)
        w->viewport->flags = gConfigGeneral.always_show_gridlines ? VIEWPORT_FLAG_GRIDLINES : 0;
    w->Invalidate();
}
