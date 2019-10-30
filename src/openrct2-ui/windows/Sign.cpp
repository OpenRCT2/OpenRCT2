/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
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
#include <openrct2/actions/LargeSceneryRemoveAction.hpp>
#include <openrct2/actions/SignSetNameAction.hpp>
#include <openrct2/actions/SignSetStyleAction.hpp>
#include <openrct2/actions/WallRemoveAction.hpp>
#include <openrct2/config/Config.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/sprites.h>
#include <openrct2/world/Banner.h>
#include <openrct2/world/LargeScenery.h>
#include <openrct2/world/Scenery.h>
#include <openrct2/world/Wall.h>

constexpr int32_t WW = 113;
constexpr int32_t WH = 96;

// clang-format off
enum WINDOW_SIGN_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_VIEWPORT,
    WIDX_SIGN_TEXT,
    WIDX_SIGN_DEMOLISH,
    WIDX_MAIN_COLOUR,
    WIDX_TEXT_COLOUR
};

// 0x9AEE00
static rct_widget window_sign_widgets[] = {
        { WWT_FRAME,    0, 0,       WW - 1,     0,          WH - 1,     0xFFFFFFFF,     STR_NONE },                         // panel / background
        { WWT_CAPTION,  0, 1,       WW - 2,     1,          14,         STR_SIGN,       STR_WINDOW_TITLE_TIP },             // title bar
        { WWT_CLOSEBOX, 0, WW - 13, WW - 3,     2,          13,         STR_CLOSE_X,    STR_CLOSE_WINDOW_TIP },             // close x button
        { WWT_VIEWPORT, 1, 3,       WW - 26,    17,         WH - 20,    STR_VIEWPORT,   STR_NONE },                         // Viewport
        { WWT_FLATBTN,  1, WW - 25, WW - 2,     19,         42,         SPR_RENAME,     STR_CHANGE_SIGN_TEXT_TIP },         // change sign button
        { WWT_FLATBTN,  1, WW - 25, WW - 2,     67,         90,         SPR_DEMOLISH,   STR_DEMOLISH_SIGN_TIP },            // demolish button
        { WWT_COLOURBTN, 1, 5,      16,         WH - 16,    WH - 5,     0xFFFFFFFF,     STR_SELECT_MAIN_SIGN_COLOUR_TIP },  // Main colour
        { WWT_COLOURBTN, 1, 17,     28,         WH - 16,    WH - 5,     0xFFFFFFFF,     STR_SELECT_TEXT_COLOUR_TIP },       // Text colour
        { WIDGETS_END },
};

static void window_sign_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_sign_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_sign_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_sign_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text);
static void window_sign_viewport_rotate(rct_window *w);
static void window_sign_invalidate(rct_window *w);
static void window_sign_paint(rct_window *w, rct_drawpixelinfo *dpi);


// 0x98E44C
static rct_window_event_list window_sign_events = {
    nullptr,
    window_sign_mouseup,
    nullptr,
    window_sign_mousedown,
    window_sign_dropdown,
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
    window_sign_textinput,
    window_sign_viewport_rotate,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_sign_invalidate,
    window_sign_paint,
    nullptr
};

static void window_sign_small_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_sign_small_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_sign_small_invalidate(rct_window *w);

// 0x9A410C
static rct_window_event_list window_sign_small_events = {
    nullptr,
    window_sign_small_mouseup,
    nullptr,
    window_sign_mousedown,
    window_sign_small_dropdown,
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
    window_sign_textinput,
    window_sign_viewport_rotate,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_sign_small_invalidate,
    window_sign_paint,
    nullptr
};
// clang-format on

static void window_sign_show_text_input(rct_window* w);

/**
 *
 *  rct2: 0x006BA305
 */
rct_window* window_sign_open(rct_windownumber number)
{
    rct_window* w;
    rct_widget* viewportWidget;

    // Check if window is already open
    w = window_bring_to_front_by_number(WC_BANNER, number);
    if (w != nullptr)
        return w;

    w = window_create_auto_pos(WW, WH, &window_sign_events, WC_BANNER, WF_NO_SCROLLING);
    w->widgets = window_sign_widgets;
    w->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_SIGN_TEXT) | (1 << WIDX_SIGN_DEMOLISH) | (1 << WIDX_MAIN_COLOUR)
        | (1 << WIDX_TEXT_COLOUR);

    w->number = number;
    window_init_scroll_widgets(w);

    auto banner = GetBanner(w->number);
    int32_t view_x = banner->position.x << 5;
    int32_t view_y = banner->position.y << 5;

    TileElement* tile_element = map_get_first_element_at(view_x / 32, view_y / 32);
    if (tile_element == nullptr)
        return nullptr;

    while (1)
    {
        if (tile_element->GetType() == TILE_ELEMENT_TYPE_LARGE_SCENERY)
        {
            rct_scenery_entry* scenery_entry = tile_element->AsLargeScenery()->GetEntry();
            if (scenery_entry != nullptr && scenery_entry->large_scenery.scrolling_mode != SCROLLING_MODE_NONE)
            {
                BannerIndex bannerIndex = tile_element->AsLargeScenery()->GetBannerIndex();

                if (bannerIndex == w->number)
                    break;
            }
        }
        tile_element++;
    }

    int32_t view_z = tile_element->base_height << 3;
    w->frame_no = view_z;

    w->list_information_type = tile_element->AsLargeScenery()->GetPrimaryColour();
    w->var_492 = tile_element->AsLargeScenery()->GetSecondaryColour();
    w->var_48C = tile_element->AsLargeScenery()->GetEntryIndex();

    view_x += 16;
    view_y += 16;

    // Create viewport
    viewportWidget = &window_sign_widgets[WIDX_VIEWPORT];
    viewport_create(
        w, w->x + viewportWidget->left + 1, w->y + viewportWidget->top + 1, (viewportWidget->right - viewportWidget->left) - 1,
        (viewportWidget->bottom - viewportWidget->top) - 1, 0, view_x, view_y, view_z, 0, SPRITE_INDEX_NULL);

    w->viewport->flags = gConfigGeneral.always_show_gridlines ? VIEWPORT_FLAG_GRIDLINES : 0;
    w->Invalidate();

    return w;
}

/**
 *
 *  rct2: 0x6B9765
 */
static void window_sign_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_SIGN_DEMOLISH:
        {
            auto banner = GetBanner(w->number);
            int32_t x = banner->position.x << 5;
            int32_t y = banner->position.y << 5;
            auto tile_element = map_get_first_element_at(x / 32, y / 32);
            if (tile_element == nullptr)
                return;
            while (1)
            {
                if (tile_element->GetType() == TILE_ELEMENT_TYPE_LARGE_SCENERY)
                {
                    rct_scenery_entry* scenery_entry = tile_element->AsLargeScenery()->GetEntry();
                    if (scenery_entry->large_scenery.scrolling_mode != SCROLLING_MODE_NONE)
                    {
                        BannerIndex bannerIndex = tile_element->AsLargeScenery()->GetBannerIndex();
                        if (bannerIndex == w->number)
                            break;
                    }
                }
                tile_element++;
            }

            auto sceneryRemoveAction = LargeSceneryRemoveAction(
                { x, y, tile_element->base_height * 8, tile_element->GetDirection() },
                tile_element->AsLargeScenery()->GetSequenceIndex());
            GameActions::Execute(&sceneryRemoveAction);
            break;
        }
        case WIDX_SIGN_TEXT:
            window_sign_show_text_input(w);
            break;
    }
}

/**
 *
 *  rct2: 0x6B9784
  & 0x6E6164 */
static void window_sign_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    switch (widgetIndex)
    {
        case WIDX_MAIN_COLOUR:
            window_dropdown_show_colour(w, widget, TRANSLUCENT(w->colours[1]), (uint8_t)w->list_information_type);
            break;
        case WIDX_TEXT_COLOUR:
            window_dropdown_show_colour(w, widget, TRANSLUCENT(w->colours[1]), (uint8_t)w->var_492);
            break;
    }
}

/**
 *
 *  rct2: 0x6B979C
 */
static void window_sign_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    switch (widgetIndex)
    {
        case WIDX_MAIN_COLOUR:
        {
            if (dropdownIndex == -1)
                return;
            w->list_information_type = dropdownIndex;
            auto signSetStyleAction = SignSetStyleAction(w->number, dropdownIndex, w->var_492, true);
            GameActions::Execute(&signSetStyleAction);
            break;
        }
        case WIDX_TEXT_COLOUR:
        {
            if (dropdownIndex == -1)
                return;
            w->var_492 = dropdownIndex;
            auto signSetStyleAction = SignSetStyleAction(w->number, w->list_information_type, dropdownIndex, true);
            GameActions::Execute(&signSetStyleAction);
            break;
        }
        default:
            return;
    }

    w->Invalidate();
}

/**
 *
 *  rct2: 0x6B9791, 0x6E6171
 */
static void window_sign_textinput(rct_window* w, rct_widgetindex widgetIndex, char* text)
{
    if (widgetIndex == WIDX_SIGN_TEXT && text != nullptr)
    {
        auto signSetNameAction = SignSetNameAction(w->number, text);
        GameActions::Execute(&signSetNameAction);
    }
}

/**
 *
 *  rct2: 0x006B96F5
 */
static void window_sign_invalidate(rct_window* w)
{
    rct_widget* main_colour_btn = &window_sign_widgets[WIDX_MAIN_COLOUR];
    rct_widget* text_colour_btn = &window_sign_widgets[WIDX_TEXT_COLOUR];

    rct_scenery_entry* scenery_entry = get_large_scenery_entry(w->var_48C);

    main_colour_btn->type = WWT_EMPTY;
    text_colour_btn->type = WWT_EMPTY;

    if (scenery_entry->large_scenery.flags & LARGE_SCENERY_FLAG_HAS_PRIMARY_COLOUR)
    {
        main_colour_btn->type = WWT_COLOURBTN;
    }
    if (scenery_entry->large_scenery.flags & LARGE_SCENERY_FLAG_HAS_SECONDARY_COLOUR)
    {
        text_colour_btn->type = WWT_COLOURBTN;
    }

    main_colour_btn->image = SPRITE_ID_PALETTE_COLOUR_1(w->list_information_type) | IMAGE_TYPE_TRANSPARENT | SPR_PALETTE_BTN;
    text_colour_btn->image = SPRITE_ID_PALETTE_COLOUR_1(w->var_492) | IMAGE_TYPE_TRANSPARENT | SPR_PALETTE_BTN;
}

/**
 *
 *  rct2: 0x006B9754, 0x006E6134
 */
static void window_sign_paint(rct_window* w, rct_drawpixelinfo* dpi)
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
 *  rct2: 0x6B9A6C, 0x6E6424
 */
static void window_sign_viewport_rotate(rct_window* w)
{
    rct_viewport* view = w->viewport;
    w->viewport = nullptr;

    view->width = 0;

    auto banner = GetBanner(w->number);

    int32_t view_x = (banner->position.x << 5) + 16;
    int32_t view_y = (banner->position.y << 5) + 16;
    int32_t view_z = w->frame_no;

    // Create viewport
    rct_widget* viewportWidget = &window_sign_widgets[WIDX_VIEWPORT];
    viewport_create(
        w, w->x + viewportWidget->left + 1, w->y + viewportWidget->top + 1, (viewportWidget->right - viewportWidget->left) - 1,
        (viewportWidget->bottom - viewportWidget->top) - 1, 0, view_x, view_y, view_z, 0, SPRITE_INDEX_NULL);
    if (w->viewport != nullptr)
        w->viewport->flags = gConfigGeneral.always_show_gridlines ? VIEWPORT_FLAG_GRIDLINES : 0;
    w->Invalidate();
}

/**
 *
 *  rct2: 0x6E5F52
 */
rct_window* window_sign_small_open(rct_windownumber number)
{
    rct_window* w;
    rct_widget* viewportWidget;

    // Check if window is already open
    w = window_bring_to_front_by_number(WC_BANNER, number);
    if (w != nullptr)
        return w;

    w = window_create_auto_pos(WW, WH, &window_sign_small_events, WC_BANNER, 0);
    w->widgets = window_sign_widgets;
    w->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_SIGN_TEXT) | (1 << WIDX_SIGN_DEMOLISH) | (1 << WIDX_MAIN_COLOUR)
        | (1 << WIDX_TEXT_COLOUR);

    w->number = number;
    window_init_scroll_widgets(w);
    w->colours[0] = COLOUR_DARK_BROWN;
    w->colours[1] = COLOUR_DARK_BROWN;
    w->colours[2] = COLOUR_DARK_BROWN;

    auto banner = GetBanner(w->number);
    int32_t view_x = banner->position.x << 5;
    int32_t view_y = banner->position.y << 5;

    TileElement* tile_element = map_get_first_element_at(view_x / 32, view_y / 32);
    if (tile_element == nullptr)
        return nullptr;

    while (1)
    {
        if (tile_element->GetType() == TILE_ELEMENT_TYPE_WALL)
        {
            rct_scenery_entry* scenery_entry = tile_element->AsWall()->GetEntry();
            if (scenery_entry->wall.scrolling_mode != SCROLLING_MODE_NONE)
            {
                if (tile_element->AsWall()->GetBannerIndex() == w->number)
                    break;
            }
        }
        tile_element++;
    }

    int32_t view_z = tile_element->base_height << 3;
    w->frame_no = view_z;

    w->list_information_type = tile_element->AsWall()->GetPrimaryColour();
    w->var_492 = tile_element->AsWall()->GetSecondaryColour();
    w->var_48C = tile_element->AsWall()->GetEntryIndex();

    view_x += 16;
    view_y += 16;

    // Create viewport
    viewportWidget = &window_sign_widgets[WIDX_VIEWPORT];
    viewport_create(
        w, w->x + viewportWidget->left + 1, w->y + viewportWidget->top + 1, (viewportWidget->right - viewportWidget->left) - 1,
        (viewportWidget->bottom - viewportWidget->top) - 1, 0, view_x, view_y, view_z, 0, SPRITE_INDEX_NULL);

    w->viewport->flags = gConfigGeneral.always_show_gridlines ? VIEWPORT_FLAG_GRIDLINES : 0;
    w->flags |= WF_NO_SCROLLING;
    w->Invalidate();

    return w;
}

/**
 *
 *  rct2: 0x6E6145
 */
static void window_sign_small_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_SIGN_DEMOLISH:
        {
            auto banner = GetBanner(w->number);
            int32_t x = banner->position.x << 5;
            int32_t y = banner->position.y << 5;
            auto tile_element = map_get_first_element_at(x / 32, y / 32);
            if (tile_element == nullptr)
                return;
            while (true)
            {
                if (tile_element->GetType() == TILE_ELEMENT_TYPE_WALL)
                {
                    rct_scenery_entry* scenery_entry = tile_element->AsWall()->GetEntry();
                    if (scenery_entry->wall.scrolling_mode != SCROLLING_MODE_NONE)
                    {
                        if (tile_element->AsWall()->GetBannerIndex() == w->number)
                            break;
                    }
                }
                tile_element++;
            }
            CoordsXYZD wallLocation = { x, y, tile_element->base_height * 8, tile_element->GetDirection() };
            auto wallRemoveAction = WallRemoveAction(wallLocation);
            GameActions::Execute(&wallRemoveAction);
            break;
        }
        case WIDX_SIGN_TEXT:
            window_sign_show_text_input(w);
            break;
    }
}

/**
 *
 *  rct2: 0x6E617C
 */
static void window_sign_small_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    switch (widgetIndex)
    {
        case WIDX_MAIN_COLOUR:
        {
            if (dropdownIndex == -1)
                return;
            w->list_information_type = dropdownIndex;
            auto signSetStyleAction = SignSetStyleAction(w->number, dropdownIndex, w->var_492, false);
            GameActions::Execute(&signSetStyleAction);
            break;
        }
        case WIDX_TEXT_COLOUR:
        {
            if (dropdownIndex == -1)
                return;
            w->var_492 = dropdownIndex;
            auto signSetStyleAction = SignSetStyleAction(w->number, w->list_information_type, dropdownIndex, false);
            GameActions::Execute(&signSetStyleAction);
            break;
        }
        default:
            return;
    }

    w->Invalidate();
}

/**
 *
 *  rct2: 0x006E60D5
 */
static void window_sign_small_invalidate(rct_window* w)
{
    rct_widget* main_colour_btn = &window_sign_widgets[WIDX_MAIN_COLOUR];
    rct_widget* text_colour_btn = &window_sign_widgets[WIDX_TEXT_COLOUR];

    rct_scenery_entry* scenery_entry = get_wall_entry(w->var_48C);

    main_colour_btn->type = WWT_EMPTY;
    text_colour_btn->type = WWT_EMPTY;

    if (scenery_entry->wall.flags & WALL_SCENERY_HAS_PRIMARY_COLOUR)
    {
        main_colour_btn->type = WWT_COLOURBTN;
    }
    if (scenery_entry->wall.flags & WALL_SCENERY_HAS_SECONDARY_COLOUR)
    {
        text_colour_btn->type = WWT_COLOURBTN;
    }

    main_colour_btn->image = SPRITE_ID_PALETTE_COLOUR_1(w->list_information_type) | IMAGE_TYPE_TRANSPARENT | SPR_PALETTE_BTN;
    text_colour_btn->image = SPRITE_ID_PALETTE_COLOUR_1(w->var_492) | IMAGE_TYPE_TRANSPARENT | SPR_PALETTE_BTN;
}

static void window_sign_show_text_input(rct_window* w)
{
    auto banner = GetBanner(w->number);
    if (banner != nullptr)
    {
        auto bannerText = banner->GetText();
        window_text_input_raw_open(w, WIDX_SIGN_TEXT, STR_SIGN_TEXT_TITLE, STR_SIGN_TEXT_PROMPT, bannerText.c_str(), 32);
    }
}
