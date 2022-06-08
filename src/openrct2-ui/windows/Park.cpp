/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/Theme.h"

#include <algorithm>
#include <limits>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Graph.h>
#include <openrct2-ui/interface/LandTool.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/actions/ParkSetNameAction.h>
#include <openrct2/config/Config.h>
#include <openrct2/localisation/Date.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/management/Award.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/util/Util.h>
#include <openrct2/world/Entrance.h>
#include <openrct2/world/Park.h>

static constexpr const rct_string_id WINDOW_TITLE = STR_STRINGID;
static constexpr const int32_t WH = 224;

// clang-format off
enum WindowParkPage {
    WINDOW_PARK_PAGE_ENTRANCE,
    WINDOW_PARK_PAGE_RATING,
    WINDOW_PARK_PAGE_GUESTS,
    WINDOW_PARK_PAGE_PRICE,
    WINDOW_PARK_PAGE_STATS,
    WINDOW_PARK_PAGE_OBJECTIVE,
    WINDOW_PARK_PAGE_AWARDS
};

enum WindowParkWidgetIdx {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_PAGE_BACKGROUND,
    WIDX_TAB_1,
    WIDX_TAB_2,
    WIDX_TAB_3,
    WIDX_TAB_4,
    WIDX_TAB_5,
    WIDX_TAB_6,
    WIDX_TAB_7,

    WIDX_VIEWPORT = 11,
    WIDX_STATUS,
    WIDX_OPEN_OR_CLOSE,
    WIDX_BUY_LAND_RIGHTS,
    WIDX_LOCATE,
    WIDX_RENAME,
    WIDX_CLOSE_LIGHT,
    WIDX_OPEN_LIGHT,

    WIDX_PRICE_LABEL = 11,
    WIDX_PRICE,
    WIDX_INCREASE_PRICE,
    WIDX_DECREASE_PRICE,

    WIDX_ENTER_NAME = 11
};

#pragma region Widgets

#define MAIN_PARK_WIDGETS(WW) \
    WINDOW_SHIM(WINDOW_TITLE, WW, WH), \
    MakeWidget({  0, 43}, {WW, 131}, WindowWidgetType::Resize, WindowColour::Secondary), /* tab content panel */ \
    MakeTab   ({  3, 17}, STR_PARK_ENTRANCE_TAB_TIP                     ), /* tab 1 */ \
    MakeTab   ({ 34, 17}, STR_PARK_RATING_TAB_TIP                       ), /* tab 2 */ \
    MakeTab   ({ 65, 17}, STR_PARK_GUESTS_TAB_TIP                       ), /* tab 3 */ \
    MakeTab   ({ 96, 17}, STR_PARK_PRICE_TAB_TIP                        ), /* tab 4 */ \
    MakeTab   ({127, 17}, STR_PARK_STATS_TAB_TIP                        ), /* tab 5 */ \
    MakeTab   ({158, 17}, STR_PARK_OBJECTIVE_TAB_TIP                    ), /* tab 6 */ \
    MakeTab   ({189, 17}, STR_PARK_AWARDS_TAB_TIP                       )  /* tab 7 */

static rct_widget window_park_entrance_widgets[] = {
    MAIN_PARK_WIDGETS(230),
    MakeWidget({  3,  46}, {202, 115}, WindowWidgetType::Viewport,      WindowColour::Secondary                                                                      ), // viewport
    MakeWidget({  3, 161}, {202,  11}, WindowWidgetType::LabelCentred, WindowColour::Secondary                                                                      ), // status
    MakeWidget({205,  49}, { 24,  24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, 0xFFFFFFFF,                 STR_OPEN_OR_CLOSE_PARK_TIP              ), // open / close
    MakeWidget({205,  73}, { 24,  24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, SPR_BUY_LAND_RIGHTS,        STR_BUY_LAND_AND_CONSTRUCTION_RIGHTS_TIP), // buy land rights
    MakeWidget({205,  97}, { 24,  24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, SPR_LOCATE,                 STR_LOCATE_SUBJECT_TIP                  ), // locate
    MakeWidget({205, 121}, { 24,  24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, SPR_RENAME,                 STR_NAME_PARK_TIP                       ), // rename
    MakeWidget({210,  51}, { 14,  15}, WindowWidgetType::ImgBtn,        WindowColour::Secondary, SPR_G2_RCT1_CLOSE_BUTTON_0, STR_CLOSE_PARK_TIP                      ),
    MakeWidget({210,  66}, { 14,  14}, WindowWidgetType::ImgBtn,        WindowColour::Secondary, SPR_G2_RCT1_OPEN_BUTTON_0,  STR_OPEN_PARK_TIP                       ),
    WIDGETS_END,
};

static rct_widget window_park_rating_widgets[] = {
    MAIN_PARK_WIDGETS(255),
    WIDGETS_END,
};

static rct_widget window_park_guests_widgets[] = {
    MAIN_PARK_WIDGETS(255),
    WIDGETS_END,
};

static rct_widget window_park_price_widgets[] = {
    MAIN_PARK_WIDGETS(230),
    MakeWidget        ({ 21, 50}, {126, 14}, WindowWidgetType::Label,   WindowColour::Secondary, STR_ADMISSION_PRICE),
    MakeSpinnerWidgets({147, 50}, { 76, 14}, WindowWidgetType::Spinner, WindowColour::Secondary                     ), // Price (3 widgets)
    WIDGETS_END,
};

static rct_widget window_park_stats_widgets[] = {
    MAIN_PARK_WIDGETS(230),
    WIDGETS_END,
};

static rct_widget window_park_objective_widgets[] = {
    MAIN_PARK_WIDGETS(230),
    MakeWidget({7, 207}, {216, 14}, WindowWidgetType::Button, WindowColour::Secondary, STR_ENTER_NAME_INTO_SCENARIO_CHART), // enter name
    WIDGETS_END,
};

static rct_widget window_park_awards_widgets[] = {
    MAIN_PARK_WIDGETS(230),
    WIDGETS_END,
};

static rct_widget *window_park_page_widgets[] = {
    window_park_entrance_widgets,
    window_park_rating_widgets,
    window_park_guests_widgets,
    window_park_price_widgets,
    window_park_stats_widgets,
    window_park_objective_widgets,
    window_park_awards_widgets,
};

#pragma endregion

#pragma region Events

static void WindowParkEntranceClose(rct_window *w);
static void WindowParkEntranceMouseup(rct_window *w, rct_widgetindex widgetIndex);
static void WindowParkEntranceResize(rct_window *w);
static void WindowParkEntranceMousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void WindowParkEntranceDropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void WindowParkEntranceUpdate(rct_window *w);
static void WindowParkEntranceTextinput(rct_window *w, rct_widgetindex widgetIndex, char *text);
static void WindowParkEntranceInvalidate(rct_window *w);
static void WindowParkEntrancePaint(rct_window *w, rct_drawpixelinfo *dpi);

static void WindowParkRatingMouseup(rct_window *w, rct_widgetindex widgetIndex);
static void WindowParkRatingResize(rct_window *w);
static void WindowParkRatingUpdate(rct_window *w);
static void WindowParkRatingInvalidate(rct_window *w);
static void WindowParkRatingPaint(rct_window *w, rct_drawpixelinfo *dpi);

static void WindowParkGuestsMouseup(rct_window *w, rct_widgetindex widgetIndex);
static void WindowParkGuestsResize(rct_window *w);
static void WindowParkGuestsUpdate(rct_window *w);
static void WindowParkGuestsInvalidate(rct_window *w);
static void WindowParkGuestsPaint(rct_window *w, rct_drawpixelinfo *dpi);

static void WindowParkPriceMouseup(rct_window *w, rct_widgetindex widgetIndex);
static void WindowParkPriceResize(rct_window *w);
static void WindowParkPriceMousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void WindowParkPriceUpdate(rct_window *w);
static void WindowParkPriceInvalidate(rct_window *w);
static void WindowParkPricePaint(rct_window *w, rct_drawpixelinfo *dpi);

static void WindowParkStatsMouseup(rct_window *w, rct_widgetindex widgetIndex);
static void WindowParkStatsResize(rct_window *w);
static void WindowParkStatsUpdate(rct_window *w);
static void WindowParkStatsInvalidate(rct_window *w);
static void WindowParkStatsPaint(rct_window *w, rct_drawpixelinfo *dpi);

static void WindowParkObjectiveMouseup(rct_window *w, rct_widgetindex widgetIndex);
static void WindowParkObjectiveResize(rct_window *w);
static void WindowParkObjectiveUpdate(rct_window *w);
static void WindowParkObjectiveTextinput(rct_window *w, rct_widgetindex widgetIndex, char *text);
static void WindowParkObjectiveInvalidate(rct_window *w);
static void WindowParkObjectivePaint(rct_window *w, rct_drawpixelinfo *dpi);

static void WindowParkAwardsMouseup(rct_window *w, rct_widgetindex widgetIndex);
static void WindowParkAwardsResize(rct_window *w);
static void WindowParkAwardsUpdate(rct_window *w);
static void WindowParkAwardsInvalidate(rct_window *w);
static void WindowParkAwardsPaint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_park_entrance_events([](auto& events)
{
    events.close = &WindowParkEntranceClose;
    events.mouse_up = &WindowParkEntranceMouseup;
    events.resize = &WindowParkEntranceResize;
    events.mouse_down = &WindowParkEntranceMousedown;
    events.dropdown = &WindowParkEntranceDropdown;
    events.update = &WindowParkEntranceUpdate;
    events.text_input = &WindowParkEntranceTextinput;
    events.invalidate = &WindowParkEntranceInvalidate;
    events.paint = &WindowParkEntrancePaint;
});

static rct_window_event_list window_park_rating_events([](auto& events)
{
    events.mouse_up = &WindowParkRatingMouseup;
    events.resize = &WindowParkRatingResize;
    events.update = &WindowParkRatingUpdate;
    events.invalidate = &WindowParkRatingInvalidate;
    events.paint = &WindowParkRatingPaint;
});

static rct_window_event_list window_park_guests_events([](auto& events)
{
    events.mouse_up = &WindowParkGuestsMouseup;
    events.resize = &WindowParkGuestsResize;
    events.update = &WindowParkGuestsUpdate;
    events.invalidate = &WindowParkGuestsInvalidate;
    events.paint = &WindowParkGuestsPaint;
});

static rct_window_event_list window_park_price_events([](auto& events)
{
    events.mouse_up = &WindowParkPriceMouseup;
    events.resize = &WindowParkPriceResize;
    events.mouse_down = &WindowParkPriceMousedown;
    events.update = &WindowParkPriceUpdate;
    events.invalidate = &WindowParkPriceInvalidate;
    events.paint = &WindowParkPricePaint;
});

static rct_window_event_list window_park_stats_events([](auto& events)
{
    events.mouse_up = &WindowParkStatsMouseup;
    events.resize = &WindowParkStatsResize;
    events.update = &WindowParkStatsUpdate;
    events.invalidate = &WindowParkStatsInvalidate;
    events.paint = &WindowParkStatsPaint;
});

static rct_window_event_list window_park_objective_events([](auto& events)
{
    events.mouse_up = &WindowParkObjectiveMouseup;
    events.resize = &WindowParkObjectiveResize;
    events.update = &WindowParkObjectiveUpdate;
    events.text_input = &WindowParkObjectiveTextinput;
    events.invalidate = &WindowParkObjectiveInvalidate;
    events.paint = &WindowParkObjectivePaint;
});

static rct_window_event_list window_park_awards_events([](auto& events)
{
    events.mouse_up = &WindowParkAwardsMouseup;
    events.resize = &WindowParkAwardsResize;
    events.update = &WindowParkAwardsUpdate;
    events.invalidate = &WindowParkAwardsInvalidate;
    events.paint = &WindowParkAwardsPaint;
});

static rct_window_event_list *window_park_page_events[] = {
    &window_park_entrance_events,
    &window_park_rating_events,
    &window_park_guests_events,
    &window_park_price_events,
    &window_park_stats_events,
    &window_park_objective_events,
    &window_park_awards_events,
};

static uint32_t window_park_page_hold_down_widgets[] = {
    0,
    0,
    0,

    (1ULL << WIDX_INCREASE_PRICE) |
    (1ULL << WIDX_DECREASE_PRICE),

    0,
    0,
    0,
};

#pragma endregion

struct WindowParkAward {
    rct_string_id text;
    uint32_t sprite;
};

static constexpr const WindowParkAward ParkAwards[] = {
    { STR_AWARD_MOST_UNTIDY,                SPR_AWARD_MOST_UNTIDY },
    { STR_AWARD_MOST_TIDY,                  SPR_AWARD_MOST_TIDY },
    { STR_AWARD_BEST_ROLLERCOASTERS,        SPR_AWARD_BEST_ROLLERCOASTERS },
    { STR_AWARD_BEST_VALUE,                 SPR_AWARD_BEST_VALUE },
    { STR_AWARD_MOST_BEAUTIFUL,             SPR_AWARD_MOST_BEAUTIFUL },
    { STR_AWARD_WORST_VALUE,                SPR_AWARD_WORST_VALUE },
    { STR_AWARD_SAFEST,                     SPR_AWARD_SAFEST },
    { STR_AWARD_BEST_STAFF,                 SPR_AWARD_BEST_STAFF },
    { STR_AWARD_BEST_FOOD,                  SPR_AWARD_BEST_FOOD },
    { STR_AWARD_WORST_FOOD,                 SPR_AWARD_WORST_FOOD },
    { STR_AWARD_BEST_RESTROOMS,             SPR_AWARD_BEST_RESTROOMS },
    { STR_AWARD_MOST_DISAPPOINTING,         SPR_AWARD_MOST_DISAPPOINTING },
    { STR_AWARD_BEST_WATER_RIDES,           SPR_AWARD_BEST_WATER_RIDES },
    { STR_AWARD_BEST_CUSTOM_DESIGNED_RIDES, SPR_AWARD_BEST_CUSTOM_DESIGNED_RIDES },
    { STR_AWARD_MOST_DAZZLING_RIDE_COLOURS, SPR_AWARD_MOST_DAZZLING_RIDE_COLOURS },
    { STR_AWARD_MOST_CONFUSING_LAYOUT,      SPR_AWARD_MOST_CONFUSING_LAYOUT },
    { STR_AWARD_BEST_GENTLE_RIDES,          SPR_AWARD_BEST_GENTLE_RIDES },
};
// clang-format on

static void WindowParkInitViewport(rct_window* w);
static void WindowParkSetPage(rct_window* w, int32_t page);
static void WindowParkAnchorBorderWidgets(rct_window* w);
static void WindowParkSetPressedTab(rct_window* w);
static void WindowParkDrawTabImages(rct_drawpixelinfo* dpi, rct_window* w);
static void WindowParkSetDisabledTabs(rct_window* w);

class ParkWindow final : public Window
{
public:
    void OnOpen() override
    {
    }

    void OnClose() override
    {
        if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE) && classification == gCurrentToolWidget.window_classification
            && number == gCurrentToolWidget.window_number)
        {
            tool_cancel();
        }
    }

    void OnMouseUp(rct_widgetindex idx) override
    {
        switch (idx)
        {
            case WIDX_CLOSE:
                Close();
                return;
            case WIDX_TAB_1:
            case WIDX_TAB_2:
            case WIDX_TAB_3:
            case WIDX_TAB_4:
            case WIDX_TAB_5:
            case WIDX_TAB_6:
            case WIDX_TAB_7:
                SetPage(idx - WIDX_TAB_1);
                return;
        }
        switch (page)
        {
            case WINDOW_PARK_PAGE_ENTRANCE:
                OnMouseUpEntrance(idx);
                break;
            case WINDOW_PARK_PAGE_OBJECTIVE:
                OnMouseUpObjective(idx);
                break;
        }
    }

    void OnResize() override
    {
        switch (page)
        {
            case WINDOW_PARK_PAGE_ENTRANCE:
                OnResizeEntrance();
                break;
            case WINDOW_PARK_PAGE_RATING:
                OnResizeRating();
                break;
            case WINDOW_PARK_PAGE_GUESTS:
                OnResizeGuests();
                break;
            case WINDOW_PARK_PAGE_PRICE:
                OnResizePrice();
                break;
            case WINDOW_PARK_PAGE_STATS:
                OnResizeStats();
                break;
            case WINDOW_PARK_PAGE_OBJECTIVE:
                OnResizeObjective();
                break;
            case WINDOW_PARK_PAGE_AWARDS:
                OnResizeAwards();
                break;
        }
    }

    void OnMouseDown(rct_widgetindex idx) override
    {
        switch (page)
        {
            case WINDOW_PARK_PAGE_ENTRANCE:
                OnMouseDownEntrance(idx);
                break;
            case WINDOW_PARK_PAGE_PRICE:
                OnMouseDownPrice();
                break;
        }
    }

    void OnDropdown(rct_widgetindex widgetIndex, int32_t selectedIndex) override
    {
        switch (page)
        {
            case WINDOW_PARK_PAGE_ENTRANCE:
                OnDropdownEntrance(widgetIndex, selectedIndex);
                break;
        }
    }

    void OnUpdate() override
    {
        switch (page)
        {
            case WINDOW_PARK_PAGE_ENTRANCE:
                OnUpdateEntrance();
                break;
            case WINDOW_PARK_PAGE_RATING:
                OnUpdateRating();
                break;
            case WINDOW_PARK_PAGE_GUESTS:
                OnUpdateGuests();
                break;
            case WINDOW_PARK_PAGE_PRICE:
                OnUpdatePrice();
                break;
            case WINDOW_PARK_PAGE_STATS:
                OnUpdateStats();
                break;
            case WINDOW_PARK_PAGE_OBJECTIVE:
                OnUpdateObjective();
                break;
            case WINDOW_PARK_PAGE_AWARDS:
                OnUpdateAwards();
                break;
        }
    }

    void OnTextInput(rct_widgetindex widgetIndex, std::string_view text) override
    {
        switch (page)
        {
            case WINDOW_PARK_PAGE_ENTRANCE:
                OnTextInputEntrance(widgetIndex, text);
                break;
            case WINDOW_PARK_PAGE_OBJECTIVE:
                OnTextInputObjective(widgetIndex, text);
                break;
        }
    }

    void OnPrepareDraw() override
    {
        switch (page)
        {
            case WINDOW_PARK_PAGE_ENTRANCE:
                OnPrepareDrawEntrance();
                break;
            case WINDOW_PARK_PAGE_RATING:
                OnPrepareDrawRating();
                break;
            case WINDOW_PARK_PAGE_GUESTS:
                OnPrepareDrawGuests();
                break;
            case WINDOW_PARK_PAGE_PRICE:
                OnPrepareDrawPrice();
                break;
            case WINDOW_PARK_PAGE_STATS:
                OnPrepareDrawStats();
                break;
            case WINDOW_PARK_PAGE_OBJECTIVE:
                OnPrepareDrawObjective();
                break;
            case WINDOW_PARK_PAGE_AWARDS:
                OnPrepareDrawAwards();
                break;
        }
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        switch (page)
        {
            case WINDOW_PARK_PAGE_ENTRANCE:
                OnDrawEntrance(dpi);
                break;
            case WINDOW_PARK_PAGE_RATING:
                OnDrawRating(dpi);
                break;
            case WINDOW_PARK_PAGE_GUESTS:
                OnDrawGuests(dpi);
                break;
            case WINDOW_PARK_PAGE_PRICE:
                OnDrawPrice(dpi);
                break;
            case WINDOW_PARK_PAGE_STATS:
                OnDrawStats(dpi);
                break;
            case WINDOW_PARK_PAGE_OBJECTIVE:
                OnDrawObjective(dpi);
                break;
            case WINDOW_PARK_PAGE_AWARDS:
                OnDrawAwards(dpi);
                break;
        }
    }

private:
};
/**
 *
 *  rct2: 0x00667F11
 */
static rct_window* WindowParkOpen()
{
    rct_window* w;

    w = WindowCreateAutoPos(230, 174 + 9, &window_park_entrance_events, WC_PARK_INFORMATION, WF_10);
    w->widgets = window_park_entrance_widgets;
    w->number = 0;
    w->page = WINDOW_PARK_PAGE_ENTRANCE;
    w->frame_no = 0;
    w->list_information_type = std::numeric_limits<uint16_t>::max();
    w->numberOfStaff = -1;
    w->var_492 = 0;
    WindowParkSetDisabledTabs(w);

    return w;
}

/**
 *
 *  rct2: 0x00667F8B
 */
static void WindowParkSetDisabledTabs(rct_window* w)
{
    // Disable price tab if money is disabled
    w->disabled_widgets = (gParkFlags & PARK_FLAGS_NO_MONEY) ? (1ULL << WIDX_TAB_4) : 0;
}

static void WindowParkPrepareWindowTitleText()
{
    auto& park = OpenRCT2::GetContext()->GetGameState()->GetPark();
    auto parkName = park.Name.c_str();

    auto ft = Formatter::Common();
    ft.Add<rct_string_id>(STR_STRING);
    ft.Add<const char*>(parkName);
}

#pragma region Entrance page

/**
 *
 *  rct2: 0x00667C48
 */
rct_window* WindowParkEntranceOpen()
{
    rct_window* window;

    window = window_bring_to_front_by_class(WC_PARK_INFORMATION);
    if (window == nullptr)
    {
        window = WindowParkOpen();
    }

    window->page = WINDOW_PARK_PAGE_ENTRANCE;
    window->Invalidate();
    window->widgets = window_park_entrance_widgets;
    window->event_handlers = &window_park_entrance_events;
    window->pressed_widgets = 0;
    WindowInitScrollWidgets(window);
    WindowParkInitViewport(window);

    return window;
}

/**
 *
 *  rct2: 0x0066860C
 */
static void WindowParkEntranceClose(rct_window* w)
{
    if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
        if (w->classification == gCurrentToolWidget.window_classification && w->number == gCurrentToolWidget.window_number)
            tool_cancel();
}

/**
 *
 *  rct2: 0x0066817C
 */
static void WindowParkEntranceMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_TAB_1:
        case WIDX_TAB_2:
        case WIDX_TAB_3:
        case WIDX_TAB_4:
        case WIDX_TAB_5:
        case WIDX_TAB_6:
        case WIDX_TAB_7:
            WindowParkSetPage(w, widgetIndex - WIDX_TAB_1);
            break;
        case WIDX_BUY_LAND_RIGHTS:
            context_open_window(WC_LAND_RIGHTS);
            break;
        case WIDX_LOCATE:
            w->ScrollToViewport();
            break;
        case WIDX_RENAME:
        {
            auto& park = OpenRCT2::GetContext()->GetGameState()->GetPark();
            WindowTextInputRawOpen(
                w, WIDX_RENAME, STR_PARK_NAME, STR_ENTER_PARK_NAME, {}, park.Name.c_str(), USER_STRING_MAX_LENGTH);
            break;
        }
        case WIDX_CLOSE_LIGHT:
            park_set_open(false);
            break;
        case WIDX_OPEN_LIGHT:
            park_set_open(true);
            break;
    }
}

/**
 *
 *  rct2: 0x00668637
 */
static void WindowParkEntranceResize(rct_window* w)
{
    w->flags |= WF_RESIZABLE;
    window_set_resize(w, 230, 174 + 9, 230 * 3, (274 + 9) * 3);
    WindowParkInitViewport(w);
}

/**
 *
 *  rct2: 0x006681BF
 */
static void WindowParkEntranceMousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    if (widgetIndex == WIDX_OPEN_OR_CLOSE)
    {
        gDropdownItems[0].Format = STR_DROPDOWN_MENU_LABEL;
        gDropdownItems[1].Format = STR_DROPDOWN_MENU_LABEL;
        gDropdownItems[0].Args = STR_CLOSE_PARK;
        gDropdownItems[1].Args = STR_OPEN_PARK;
        WindowDropdownShowText(
            { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[1], 0, 2);

        if (park_is_open())
        {
            gDropdownDefaultIndex = 0;
            Dropdown::SetChecked(1, true);
        }
        else
        {
            gDropdownDefaultIndex = 1;
            Dropdown::SetChecked(0, true);
        }
    }
}

/**
 *
 *  rct2: 0x006682B8
 */
static void WindowParkEntranceDropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (widgetIndex == WIDX_OPEN_OR_CLOSE)
    {
        if (dropdownIndex == -1)
            dropdownIndex = gDropdownHighlightedIndex;

        if (dropdownIndex != 0)
        {
            park_set_open(true);
        }
        else
        {
            park_set_open(false);
        }
    }
}

/**
 *
 *  rct2: 0x006686B5
 */
static void WindowParkEntranceUpdate(rct_window* w)
{
    w->frame_no++;
    widget_invalidate(w, WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x0066848B
 */
static void WindowParkEntranceTextinput(rct_window* w, rct_widgetindex widgetIndex, char* text)
{
    if (widgetIndex == WIDX_RENAME && text != nullptr)
    {
        auto action = ParkSetNameAction(text);
        GameActions::Execute(&action);
    }
}

/**
 *
 *  rct2: 0x00667FDC
 */
static void WindowParkEntranceInvalidate(rct_window* w)
{
    int32_t i, height;

    w->widgets = window_park_page_widgets[w->page];
    WindowInitScrollWidgets(w);

    WindowParkSetPressedTab(w);

    // Set open / close park button state
    {
        auto& park = OpenRCT2::GetContext()->GetGameState()->GetPark();
        auto parkName = park.Name.c_str();

        auto ft = Formatter::Common();
        ft.Add<rct_string_id>(STR_STRING);
        ft.Add<const char*>(parkName);
    }
    window_park_entrance_widgets[WIDX_OPEN_OR_CLOSE].image = park_is_open() ? SPR_OPEN : SPR_CLOSED;
    window_park_entrance_widgets[WIDX_CLOSE_LIGHT].image = SPR_G2_RCT1_CLOSE_BUTTON_0 + !park_is_open() * 2
        + WidgetIsPressed(w, WIDX_CLOSE_LIGHT);
    window_park_entrance_widgets[WIDX_OPEN_LIGHT].image = SPR_G2_RCT1_OPEN_BUTTON_0 + park_is_open() * 2
        + WidgetIsPressed(w, WIDX_OPEN_LIGHT);

    // Only allow closing of park for guest / rating objective
    if (gScenarioObjective.Type == OBJECTIVE_GUESTS_AND_RATING)
        w->disabled_widgets |= (1ULL << WIDX_OPEN_OR_CLOSE) | (1ULL << WIDX_CLOSE_LIGHT) | (1ULL << WIDX_OPEN_LIGHT);
    else
        w->disabled_widgets &= ~((1ULL << WIDX_OPEN_OR_CLOSE) | (1ULL << WIDX_CLOSE_LIGHT) | (1ULL << WIDX_OPEN_LIGHT));

    // Only allow purchase of land when there is money
    if (gParkFlags & PARK_FLAGS_NO_MONEY)
        window_park_entrance_widgets[WIDX_BUY_LAND_RIGHTS].type = WindowWidgetType::Empty;
    else
        window_park_entrance_widgets[WIDX_BUY_LAND_RIGHTS].type = WindowWidgetType::FlatBtn;

    window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_7);
    WindowParkAnchorBorderWidgets(w);

    // Anchor entrance page specific widgets
    window_park_entrance_widgets[WIDX_VIEWPORT].right = w->width - 26;
    window_park_entrance_widgets[WIDX_VIEWPORT].bottom = w->height - 14;
    window_park_entrance_widgets[WIDX_STATUS].right = w->width - 26;
    window_park_entrance_widgets[WIDX_STATUS].top = w->height - 13;
    window_park_entrance_widgets[WIDX_STATUS].bottom = w->height - 3;

    if (ThemeGetFlags() & UITHEME_FLAG_USE_LIGHTS_PARK)
    {
        window_park_entrance_widgets[WIDX_OPEN_OR_CLOSE].type = WindowWidgetType::Empty;
        if (gScenarioObjective.Type == OBJECTIVE_GUESTS_AND_RATING)
        {
            window_park_entrance_widgets[WIDX_CLOSE_LIGHT].type = WindowWidgetType::FlatBtn;
            window_park_entrance_widgets[WIDX_OPEN_LIGHT].type = WindowWidgetType::FlatBtn;
        }
        else
        {
            window_park_entrance_widgets[WIDX_CLOSE_LIGHT].type = WindowWidgetType::ImgBtn;
            window_park_entrance_widgets[WIDX_OPEN_LIGHT].type = WindowWidgetType::ImgBtn;
        }
        height = window_park_entrance_widgets[WIDX_OPEN_LIGHT].bottom + 5;
    }
    else
    {
        window_park_entrance_widgets[WIDX_OPEN_OR_CLOSE].type = WindowWidgetType::FlatBtn;
        window_park_entrance_widgets[WIDX_CLOSE_LIGHT].type = WindowWidgetType::Empty;
        window_park_entrance_widgets[WIDX_OPEN_LIGHT].type = WindowWidgetType::Empty;
        height = 49;
    }

    for (i = WIDX_CLOSE_LIGHT; i <= WIDX_OPEN_LIGHT; i++)
    {
        window_park_entrance_widgets[i].left = w->width - 20;
        window_park_entrance_widgets[i].right = w->width - 7;
    }
    for (i = WIDX_OPEN_OR_CLOSE; i <= WIDX_RENAME; i++)
    {
        if (window_park_entrance_widgets[i].type == WindowWidgetType::Empty)
            continue;

        window_park_entrance_widgets[i].left = w->width - 25;
        window_park_entrance_widgets[i].right = w->width - 2;
        window_park_entrance_widgets[i].top = height;
        window_park_entrance_widgets[i].bottom = height + 23;
        height += 24;
    }
}

/**
 *
 *  rct2: 0x006680D0
 */
static void WindowParkEntrancePaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    rct_widget* labelWidget;

    WindowDrawWidgets(w, dpi);
    WindowParkDrawTabImages(dpi, w);

    // Draw viewport
    if (w->viewport != nullptr)
    {
        window_draw_viewport(dpi, w);
        if (w->viewport->flags & VIEWPORT_FLAG_SOUND_ON)
            gfx_draw_sprite(dpi, ImageId(SPR_HEARING_VIEWPORT), w->windowPos + ScreenCoordsXY{ 2, 2 });
    }

    // Draw park closed / open label
    auto ft = Formatter();
    ft.Add<rct_string_id>(park_is_open() ? STR_PARK_OPEN : STR_PARK_CLOSED);

    labelWidget = &window_park_entrance_widgets[WIDX_STATUS];
    DrawTextEllipsised(
        dpi, w->windowPos + ScreenCoordsXY{ labelWidget->midX(), labelWidget->top }, labelWidget->width(), STR_BLACK_STRING, ft,
        { TextAlignment::CENTRE });
}

/**
 *
 *  rct2: 0x00669B55
 */
static void WindowParkInitViewport(rct_window* w)
{
    int32_t viewportFlags;

    if (w->page != WINDOW_PARK_PAGE_ENTRANCE)
        return;

    std::optional<Focus> focus = std::nullopt;
    if (!gParkEntrances.empty())
    {
        const auto& entrance = gParkEntrances[0];
        focus = Focus(CoordsXYZ{ entrance.x + 16, entrance.y + 16, entrance.z + 32 });
    }

    if (w->viewport == nullptr)
    {
        viewportFlags = gConfigGeneral.always_show_gridlines ? VIEWPORT_FLAG_GRIDLINES : 0;
    }
    else
    {
        viewportFlags = w->viewport->flags;
        w->RemoveViewport();
    }

    // Call invalidate event
    window_event_invalidate_call(w);

    w->focus = focus;

    if (focus.has_value())
    {
        // Create viewport
        if (w->viewport == nullptr)
        {
            rct_widget* viewportWidget = &window_park_entrance_widgets[WIDX_VIEWPORT];
            viewport_create(
                w, w->windowPos + ScreenCoordsXY{ viewportWidget->left + 1, viewportWidget->top + 1 },
                viewportWidget->width() - 1, viewportWidget->height() - 1, focus.value());
            w->flags |= (1 << 2);
            w->Invalidate();
        }
    }

    if (w->viewport != nullptr)
        w->viewport->flags = viewportFlags;
    w->Invalidate();
}

#pragma endregion

#pragma region Rating page

/**
 *
 *  rct2: 0x00667CA4
 */
rct_window* WindowParkRatingOpen()
{
    rct_window* window;

    window = window_bring_to_front_by_class(WC_PARK_INFORMATION);
    if (window == nullptr)
    {
        window = WindowParkOpen();
    }

    if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
        if (window->classification == gCurrentToolWidget.window_classification
            && window->number == gCurrentToolWidget.window_number)
            tool_cancel();

    window->viewport = nullptr;
    window->page = WINDOW_PARK_PAGE_RATING;
    window->Invalidate();
    window->widgets = window_park_rating_widgets;
    window->hold_down_widgets = window_park_page_hold_down_widgets[WINDOW_PARK_PAGE_RATING];
    window->event_handlers = &window_park_rating_events;
    WindowInitScrollWidgets(window);

    return window;
}

/**
 *
 *  rct2: 0x00668A06
 */
static void WindowParkRatingMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    if (widgetIndex == WIDX_CLOSE)
        window_close(w);
    else if (widgetIndex >= WIDX_TAB_1 && widgetIndex <= WIDX_TAB_7)
        WindowParkSetPage(w, widgetIndex - WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x00668A36
 */
static void WindowParkRatingResize(rct_window* w)
{
    window_set_resize(w, 255, 182, 255, 182);
}

/**
 *
 *  rct2: 0x00668A21
 */
static void WindowParkRatingUpdate(rct_window* w)
{
    w->frame_no++;
    widget_invalidate(w, WIDX_TAB_2);
}

/**
 *
 *  rct2: 0x006686CB
 */
static void WindowParkRatingInvalidate(rct_window* w)
{
    rct_widget* widgets;

    widgets = window_park_page_widgets[w->page];
    if (w->widgets != widgets)
    {
        w->widgets = widgets;
        WindowInitScrollWidgets(w);
    }

    WindowParkSetPressedTab(w);
    WindowParkPrepareWindowTitleText();

    window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_7);
    WindowParkAnchorBorderWidgets(w);
}

/**
 *
 *  rct2: 0x0066875D
 */
static void WindowParkRatingPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    WindowParkDrawTabImages(dpi, w);

    auto screenPos = w->windowPos;
    rct_widget* widget = &window_park_rating_widgets[WIDX_PAGE_BACKGROUND];

    // Current value
    auto ft = Formatter();
    ft.Add<uint16_t>(gParkRating);
    DrawTextBasic(dpi, screenPos + ScreenCoordsXY{ widget->left + 3, widget->top + 2 }, STR_PARK_RATING_LABEL, ft);

    // Graph border
    gfx_fill_rect_inset(
        dpi,
        { screenPos + ScreenCoordsXY{ widget->left + 4, widget->top + 15 },
          screenPos + ScreenCoordsXY{ widget->right - 4, widget->bottom - 4 } },
        w->colours[1], INSET_RECT_F_30);

    // Y axis labels
    screenPos = screenPos + ScreenCoordsXY{ widget->left + 27, widget->top + 23 };
    for (int i = 5; i >= 0; i--)
    {
        uint32_t axisValue = i * 200;
        ft = Formatter();
        ft.Add<uint32_t>(axisValue);
        DrawTextBasic(
            dpi, screenPos + ScreenCoordsXY{ 10, 0 }, STR_GRAPH_AXIS_LABEL, ft,
            { FontSpriteBase::SMALL, TextAlignment::RIGHT });
        gfx_fill_rect_inset(
            dpi, { screenPos + ScreenCoordsXY{ 15, 5 }, screenPos + ScreenCoordsXY{ w->width - 32, 5 } }, w->colours[2],
            INSET_RECT_FLAG_BORDER_INSET);
        screenPos.y += 20;
    }

    // Graph
    screenPos = w->windowPos + ScreenCoordsXY{ widget->left + 47, widget->top + 26 };

    Graph::Draw(dpi, gParkRatingHistory, 32, screenPos);
}

#pragma endregion

#pragma region Guests page

/**
 *
 *  rct2: 0x00667D35
 */
rct_window* WindowParkGuestsOpen()
{
    rct_window* window;

    window = window_bring_to_front_by_class(WC_PARK_INFORMATION);
    if (window == nullptr)
    {
        window = WindowParkOpen();
    }

    if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
        if (window->classification == gCurrentToolWidget.window_classification
            && window->number == gCurrentToolWidget.window_number)
            tool_cancel();

    window->viewport = nullptr;
    window->page = WINDOW_PARK_PAGE_GUESTS;
    window->Invalidate();
    window->widgets = window_park_guests_widgets;
    window->hold_down_widgets = window_park_page_hold_down_widgets[WINDOW_PARK_PAGE_GUESTS];
    window->event_handlers = &window_park_guests_events;
    WindowInitScrollWidgets(window);

    return window;
}

/**
 *
 *  rct2: 0x00668DEB
 */
static void WindowParkGuestsMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    if (widgetIndex == WIDX_CLOSE)
        window_close(w);
    else if (widgetIndex >= WIDX_TAB_1 && widgetIndex <= WIDX_TAB_7)
        WindowParkSetPage(w, widgetIndex - WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x00668E33
 */
static void WindowParkGuestsResize(rct_window* w)
{
    window_set_resize(w, 255, 182, 255, 182);
}

/**
 *
 *  rct2: 0x00668E06
 */
static void WindowParkGuestsUpdate(rct_window* w)
{
    w->frame_no++;
    w->var_492 = (w->var_492 + 1) % 24;
    widget_invalidate(w, WIDX_TAB_3);
}

/**
 *
 *  rct2: 0x00668AB0
 */
static void WindowParkGuestsInvalidate(rct_window* w)
{
    rct_widget* widgets;

    widgets = window_park_page_widgets[w->page];
    if (w->widgets != widgets)
    {
        w->widgets = widgets;
        WindowInitScrollWidgets(w);
    }

    WindowParkSetPressedTab(w);
    WindowParkPrepareWindowTitleText();

    window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_7);
    WindowParkAnchorBorderWidgets(w);
}

/**
 *
 *  rct2: 0x00668B42
 */
static void WindowParkGuestsPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    WindowParkDrawTabImages(dpi, w);

    auto screenPos = ScreenCoordsXY{ w->windowPos.x, w->windowPos.y };
    rct_widget* widget = &window_park_guests_widgets[WIDX_PAGE_BACKGROUND];

    // Current value
    auto ft = Formatter();
    ft.Add<uint32_t>(gNumGuestsInPark);
    DrawTextBasic(dpi, screenPos + ScreenCoordsXY{ widget->left + 3, widget->top + 2 }, STR_GUESTS_IN_PARK_LABEL, ft);

    // Graph border
    gfx_fill_rect_inset(
        dpi,
        { screenPos + ScreenCoordsXY{ widget->left + 4, widget->top + 15 },
          screenPos + ScreenCoordsXY{ widget->right - 4, widget->bottom - 4 } },
        w->colours[1], INSET_RECT_F_30);

    // Y axis labels
    screenPos = screenPos + ScreenCoordsXY{ widget->left + 27, widget->top + 23 };
    for (int i = 5; i >= 0; i--)
    {
        uint32_t axisValue = i * 1000;
        ft = Formatter();
        ft.Add<uint32_t>(axisValue);
        DrawTextBasic(
            dpi, screenPos + ScreenCoordsXY{ 10, 0 }, STR_GRAPH_AXIS_LABEL, ft,
            { FontSpriteBase::SMALL, TextAlignment::RIGHT });
        gfx_fill_rect_inset(
            dpi, { screenPos + ScreenCoordsXY{ 15, 5 }, screenPos + ScreenCoordsXY{ w->width - 32, 5 } }, w->colours[2],
            INSET_RECT_FLAG_BORDER_INSET);
        screenPos.y += 20;
    }

    // Graph
    screenPos = w->windowPos + ScreenCoordsXY{ widget->left + 47, widget->top + 26 };

    uint8_t cappedHistory[32];
    for (size_t i = 0; i < std::size(cappedHistory); i++)
    {
        auto value = gGuestsInParkHistory[i];
        if (value != std::numeric_limits<uint32_t>::max())
        {
            cappedHistory[i] = static_cast<uint8_t>(std::min<uint32_t>(value, 5000) / 20);
        }
        else
        {
            cappedHistory[i] = std::numeric_limits<uint8_t>::max();
        }
    }
    Graph::Draw(dpi, cappedHistory, static_cast<int32_t>(std::size(cappedHistory)), screenPos);
}

#pragma endregion

#pragma region Price page

/**
 *
 *  rct2: 0x00669011
 */
static void WindowParkPriceMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    if (widgetIndex == WIDX_CLOSE)
        window_close(w);
    else if (widgetIndex >= WIDX_TAB_1 && widgetIndex <= WIDX_TAB_7)
        WindowParkSetPage(w, widgetIndex - WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x0066908C
 */
static void WindowParkPriceResize(rct_window* w)
{
    window_set_resize(w, 230, 124, 230, 124);
}

/**
 *
 *  rct2: 0x0066902C
 */
static void WindowParkPriceMousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    int32_t newFee;

    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_INCREASE_PRICE:
            newFee = std::min(MAX_ENTRANCE_FEE, gParkEntranceFee + 1.00_GBP);
            park_set_entrance_fee(newFee);
            break;
        case WIDX_DECREASE_PRICE:
            newFee = std::max(0.00_GBP, gParkEntranceFee - 1.00_GBP);
            park_set_entrance_fee(newFee);
            break;
    }
}

/**
 *
 *  rct2: 0x00669077
 */
static void WindowParkPriceUpdate(rct_window* w)
{
    w->frame_no++;
    widget_invalidate(w, WIDX_TAB_4);
}

/**
 *
 *  rct2: 0x00668EAD
 */
static void WindowParkPriceInvalidate(rct_window* w)
{
    rct_widget* widgets;

    widgets = window_park_page_widgets[w->page];
    if (w->widgets != widgets)
    {
        w->widgets = widgets;
        WindowInitScrollWidgets(w);
    }

    WindowParkSetPressedTab(w);
    WindowParkPrepareWindowTitleText();

    // Show a tooltip if the park is pay per ride.
    window_park_price_widgets[WIDX_PRICE_LABEL].tooltip = STR_NONE;
    window_park_price_widgets[WIDX_PRICE].tooltip = STR_NONE;

    if (!park_entry_price_unlocked())
    {
        window_park_price_widgets[WIDX_PRICE_LABEL].tooltip = STR_ADMISSION_PRICE_PAY_PER_RIDE_TIP;
        window_park_price_widgets[WIDX_PRICE].tooltip = STR_ADMISSION_PRICE_PAY_PER_RIDE_TIP;
    }

    // If the entry price is locked at free, disable the widget, unless the unlock_all_prices cheat is active.
    if ((gParkFlags & PARK_FLAGS_NO_MONEY) || !park_entry_price_unlocked())
    {
        window_park_price_widgets[WIDX_PRICE].type = WindowWidgetType::LabelCentred;
        window_park_price_widgets[WIDX_INCREASE_PRICE].type = WindowWidgetType::Empty;
        window_park_price_widgets[WIDX_DECREASE_PRICE].type = WindowWidgetType::Empty;
    }
    else
    {
        window_park_price_widgets[WIDX_PRICE].type = WindowWidgetType::Spinner;
        window_park_price_widgets[WIDX_INCREASE_PRICE].type = WindowWidgetType::Button;
        window_park_price_widgets[WIDX_DECREASE_PRICE].type = WindowWidgetType::Button;
    }

    window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_7);
    WindowParkAnchorBorderWidgets(w);
}

/**
 *
 *  rct2: 0x00668F99
 */
static void WindowParkPricePaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    WindowParkDrawTabImages(dpi, w);

    auto screenCoords = w->windowPos
        + ScreenCoordsXY{ w->widgets[WIDX_PAGE_BACKGROUND].left + 4, w->widgets[WIDX_PAGE_BACKGROUND].top + 30 };
    auto ft = Formatter();
    ft.Add<money64>(gTotalIncomeFromAdmissions);
    DrawTextBasic(dpi, screenCoords, STR_INCOME_FROM_ADMISSIONS, ft);

    money64 parkEntranceFee = park_get_entrance_fee();
    auto stringId = parkEntranceFee == 0 ? STR_FREE : STR_BOTTOM_TOOLBAR_CASH;
    screenCoords = w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_PRICE].left + 1, w->widgets[WIDX_PRICE].top + 1 };
    ft = Formatter();
    ft.Add<money64>(parkEntranceFee);
    DrawTextBasic(dpi, screenCoords, stringId, ft, { w->colours[1] });
}

#pragma endregion

#pragma region Stats page

/**
 *
 *  rct2: 0x0066928C
 */
static void WindowParkStatsMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    if (widgetIndex == WIDX_CLOSE)
        window_close(w);
    else if (widgetIndex >= WIDX_TAB_1 && widgetIndex <= WIDX_TAB_7)
        WindowParkSetPage(w, widgetIndex - WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x00669338
 */
static void WindowParkStatsResize(rct_window* w)
{
    window_set_resize(w, 230, 119, 230, 119);
}

/**
 *
 *  rct2: 0x006692A8
 */
static void WindowParkStatsUpdate(rct_window* w)
{
    int32_t i;

    w->frame_no++;
    widget_invalidate(w, WIDX_TAB_5);

    // Invalidate ride count if changed
    i = ride_get_count();
    if (w->list_information_type != i)
    {
        w->list_information_type = i;
        widget_invalidate(w, WIDX_PAGE_BACKGROUND);
    }

    // Invalidate number of staff if changed
    i = peep_get_staff_count();
    if (w->numberOfStaff != i)
    {
        w->numberOfStaff = i;
        widget_invalidate(w, WIDX_PAGE_BACKGROUND);
    }
}

/**
 *
 *  rct2: 0x00669106
 */
static void WindowParkStatsInvalidate(rct_window* w)
{
    rct_widget* widgets;

    widgets = window_park_page_widgets[w->page];
    if (w->widgets != widgets)
    {
        w->widgets = widgets;
        WindowInitScrollWidgets(w);
    }

    WindowParkSetPressedTab(w);
    WindowParkPrepareWindowTitleText();

    window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_7);
    WindowParkAnchorBorderWidgets(w);
}

/**
 *
 *  rct2: 0x00669198
 */
static void WindowParkStatsPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    int32_t parkSize, stringIndex;

    WindowDrawWidgets(w, dpi);
    WindowParkDrawTabImages(dpi, w);

    auto screenCoords = w->windowPos
        + ScreenCoordsXY{ window_park_awards_widgets[WIDX_PAGE_BACKGROUND].left + 4,
                          window_park_awards_widgets[WIDX_PAGE_BACKGROUND].top + 4 };

    // Draw park size
    parkSize = gParkSize * 10;
    stringIndex = STR_PARK_SIZE_METRIC_LABEL;
    if (gConfigGeneral.measurement_format == MeasurementFormat::Imperial)
    {
        stringIndex = STR_PARK_SIZE_IMPERIAL_LABEL;
        parkSize = squaredmetres_to_squaredfeet(parkSize);
    }
    auto ft = Formatter();
    ft.Add<uint32_t>(parkSize);
    DrawTextBasic(dpi, screenCoords, stringIndex, ft);
    screenCoords.y += LIST_ROW_HEIGHT;

    // Draw number of rides / attractions
    if (w->list_information_type != 0xFFFF)
    {
        ft = Formatter();
        ft.Add<uint32_t>(w->list_information_type);
        DrawTextBasic(dpi, screenCoords, STR_NUMBER_OF_RIDES_LABEL, ft);
    }
    screenCoords.y += LIST_ROW_HEIGHT;

    // Draw number of staff
    if (w->numberOfStaff != -1)
    {
        ft = Formatter();
        ft.Add<uint32_t>(w->numberOfStaff);
        DrawTextBasic(dpi, screenCoords, STR_STAFF_LABEL, ft);
    }
    screenCoords.y += LIST_ROW_HEIGHT;

    // Draw number of guests in park
    ft = Formatter();
    ft.Add<uint32_t>(gNumGuestsInPark);
    DrawTextBasic(dpi, screenCoords, STR_GUESTS_IN_PARK_LABEL, ft);
    screenCoords.y += LIST_ROW_HEIGHT;
    ft = Formatter();
    ft.Add<uint32_t>(gTotalAdmissions);
    DrawTextBasic(dpi, screenCoords, STR_TOTAL_ADMISSIONS, ft);
}

#pragma endregion

#pragma region Objective page

/**
 *
 *  rct2: 0x00667E57
 */
rct_window* WindowParkObjectiveOpen()
{
    rct_window* window;

    window = window_bring_to_front_by_class(WC_PARK_INFORMATION);
    if (window == nullptr)
    {
        window = WindowParkOpen();
    }

    if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
        if (window->classification == gCurrentToolWidget.window_classification
            && window->number == gCurrentToolWidget.window_number)
            tool_cancel();

    window->viewport = nullptr;
    window->page = WINDOW_PARK_PAGE_OBJECTIVE;
    window->Invalidate();
    window->widgets = window_park_objective_widgets;
    window->hold_down_widgets = window_park_page_hold_down_widgets[WINDOW_PARK_PAGE_OBJECTIVE];
    window->event_handlers = &window_park_objective_events;
    WindowInitScrollWidgets(window);
    window->windowPos.x = context_get_width() / 2 - 115;
    window->windowPos.y = context_get_height() / 2 - 87;
    window->Invalidate();

    return window;
}

/**
 *
 *  rct2: 0x006695AA
 */
static void WindowParkObjectiveMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_TAB_1:
        case WIDX_TAB_2:
        case WIDX_TAB_3:
        case WIDX_TAB_4:
        case WIDX_TAB_5:
        case WIDX_TAB_6:
        case WIDX_TAB_7:
            WindowParkSetPage(w, widgetIndex - WIDX_TAB_1);
            break;
        case WIDX_ENTER_NAME:
            WindowTextInputOpen(
                w, WIDX_ENTER_NAME, STR_ENTER_NAME, STR_PLEASE_ENTER_YOUR_NAME_FOR_THE_SCENARIO_CHART, {}, 0, 0, 32);
            break;
    }
}

/**
 *
 *  rct2: 0x00669681
 */
static void WindowParkObjectiveResize(rct_window* w)
{
#ifndef NO_TTF
    if (gCurrentTTFFontSet != nullptr)
        window_set_resize(w, 230, 270, 230, 270);
    else
#endif
        window_set_resize(w, 230, 226, 230, 226);
}

/**
 *
 *  rct2: 0x0066966C
 */
static void WindowParkObjectiveUpdate(rct_window* w)
{
    w->frame_no++;
    widget_invalidate(w, WIDX_TAB_6);
}

/**
 *
 *  rct2: 0x006695CC
 */
static void WindowParkObjectiveTextinput(rct_window* w, rct_widgetindex widgetIndex, char* text)
{
    if (widgetIndex == WIDX_ENTER_NAME && text != nullptr && text[0] != 0)
    {
        scenario_success_submit_name(text);
        w->Invalidate();
    }
}

/**
 *
 *  rct2: 0x006693B2
 */
static void WindowParkObjectiveInvalidate(rct_window* w)
{
    WindowParkSetPressedTab(w);
    WindowParkPrepareWindowTitleText();

    // Show name input button on scenario completion.
    if (gParkFlags & PARK_FLAGS_SCENARIO_COMPLETE_NAME_INPUT)
    {
        window_park_objective_widgets[WIDX_ENTER_NAME].type = WindowWidgetType::Button;
        window_park_objective_widgets[WIDX_ENTER_NAME].top = w->height - 19;
        window_park_objective_widgets[WIDX_ENTER_NAME].bottom = w->height - 6;
    }
    else
        window_park_objective_widgets[WIDX_ENTER_NAME].type = WindowWidgetType::Empty;

    window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_7);
    WindowParkAnchorBorderWidgets(w);
}

/**
 *
 *  rct2: 0x0066945C
 */
static void WindowParkObjectivePaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    WindowParkDrawTabImages(dpi, w);

    // Scenario description
    auto screenCoords = w->windowPos
        + ScreenCoordsXY{ window_park_objective_widgets[WIDX_PAGE_BACKGROUND].left + 4,
                          window_park_objective_widgets[WIDX_PAGE_BACKGROUND].top + 7 };
    auto ft = Formatter();
    ft.Add<rct_string_id>(STR_STRING);
    ft.Add<const char*>(gScenarioDetails.c_str());
    screenCoords.y += DrawTextWrapped(dpi, screenCoords, 222, STR_BLACK_STRING, ft);
    screenCoords.y += 5;

    // Your objective:
    DrawTextBasic(dpi, screenCoords, STR_OBJECTIVE_LABEL);
    screenCoords.y += LIST_ROW_HEIGHT;

    // Objective
    ft = Formatter();
    if (gScenarioObjective.Type == OBJECTIVE_BUILD_THE_BEST)
    {
        rct_string_id rideTypeString = STR_NONE;
        auto rideTypeId = gScenarioObjective.RideId;
        if (rideTypeId != RIDE_TYPE_NULL && rideTypeId < RIDE_TYPE_COUNT)
        {
            rideTypeString = GetRideTypeDescriptor(rideTypeId).Naming.Name;
        }
        ft.Add<rct_string_id>(rideTypeString);
    }
    else
    {
        ft.Add<uint16_t>(gScenarioObjective.NumGuests);
        ft.Add<int16_t>(date_get_total_months(MONTH_OCTOBER, gScenarioObjective.Year));
        if (gScenarioObjective.Type == OBJECTIVE_FINISH_5_ROLLERCOASTERS)
            ft.Add<uint16_t>(gScenarioObjective.MinimumExcitement);
        else
            ft.Add<money64>(gScenarioObjective.Currency);
    }

    screenCoords.y += DrawTextWrapped(dpi, screenCoords, 221, ObjectiveNames[gScenarioObjective.Type], ft);
    screenCoords.y += 5;

    // Objective outcome
    if (gScenarioCompletedCompanyValue != MONEY64_UNDEFINED)
    {
        if (gScenarioCompletedCompanyValue == COMPANY_VALUE_ON_FAILED_OBJECTIVE)
        {
            // Objective failed
            DrawTextWrapped(dpi, screenCoords, 222, STR_OBJECTIVE_FAILED);
        }
        else
        {
            // Objective completed
            ft = Formatter();
            ft.Add<money64>(gScenarioCompletedCompanyValue);
            DrawTextWrapped(dpi, screenCoords, 222, STR_OBJECTIVE_ACHIEVED, ft);
        }
    }
}

#pragma endregion

#pragma region Awards page

/**
 *
 *  rct2: 0x00667DC6
 */
rct_window* WindowParkAwardsOpen()
{
    rct_window* window;

    window = window_bring_to_front_by_class(WC_PARK_INFORMATION);
    if (window == nullptr)
    {
        window = WindowParkOpen();
    }

    if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
        if (window->classification == gCurrentToolWidget.window_classification
            && window->number == gCurrentToolWidget.window_number)
            tool_cancel();

    window->viewport = nullptr;
    window->page = WINDOW_PARK_PAGE_AWARDS;
    window->Invalidate();
    window->widgets = window_park_awards_widgets;
    window->hold_down_widgets = window_park_page_hold_down_widgets[WINDOW_PARK_PAGE_AWARDS];
    window->event_handlers = &window_park_awards_events;
    WindowInitScrollWidgets(window);

    return window;
}

/**
 *
 *  rct2: 0x00669851
 */
static void WindowParkAwardsMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    if (widgetIndex == WIDX_CLOSE)
        window_close(w);
    else if (widgetIndex >= WIDX_TAB_1 && widgetIndex <= WIDX_TAB_7)
        WindowParkSetPage(w, widgetIndex - WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x00669882
 */
static void WindowParkAwardsResize(rct_window* w)
{
    window_set_resize(w, 230, 182, 230, 182);
}

/**
 *
 *  rct2: 0x0066986D
 */
static void WindowParkAwardsUpdate(rct_window* w)
{
    w->frame_no++;
    widget_invalidate(w, WIDX_TAB_7);
}

/**
 *
 *  rct2: 0x006696FB
 */
static void WindowParkAwardsInvalidate(rct_window* w)
{
    rct_widget* widgets;

    widgets = window_park_page_widgets[w->page];
    if (w->widgets != widgets)
    {
        w->widgets = widgets;
        WindowInitScrollWidgets(w);
    }

    WindowParkSetPressedTab(w);
    WindowParkPrepareWindowTitleText();

    window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_7);
    WindowParkAnchorBorderWidgets(w);
}

/**
 *
 *  rct2: 0x0066978D
 */
static void WindowParkAwardsPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    WindowParkDrawTabImages(dpi, w);

    auto screenCoords = w->windowPos
        + ScreenCoordsXY{ window_park_awards_widgets[WIDX_PAGE_BACKGROUND].left + 4,
                          window_park_awards_widgets[WIDX_PAGE_BACKGROUND].top + 4 };
    int32_t count = 0;
    for (const auto& award : GetAwards())
    {
        gfx_draw_sprite(dpi, ImageId(ParkAwards[EnumValue(award.Type)].sprite), screenCoords);
        DrawTextWrapped(dpi, screenCoords + ScreenCoordsXY{ 34, 6 }, 180, ParkAwards[EnumValue(award.Type)].text);

        screenCoords.y += 32;
        count++;
    }

    if (count == 0)
        DrawTextBasic(dpi, screenCoords + ScreenCoordsXY{ 6, 6 }, STR_NO_RECENT_AWARDS);
}

#pragma endregion

#pragma region Common

/**
 *
 *  rct2: 0x00668496
 */
static void WindowParkSetPage(rct_window* w, int32_t page)
{
    int32_t listen;

    if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
        if (w->classification == gCurrentToolWidget.window_classification && w->number == gCurrentToolWidget.window_number)
            tool_cancel();

    // Set listen only to viewport
    listen = 0;
    if (page == WINDOW_PARK_PAGE_ENTRANCE && w->page == WINDOW_PARK_PAGE_ENTRANCE && w->viewport != nullptr
        && !(w->viewport->flags & VIEWPORT_FLAG_SOUND_ON))
        listen++;

    w->page = page;
    w->frame_no = 0;
    w->var_492 = 0;
    w->RemoveViewport();

    w->hold_down_widgets = window_park_page_hold_down_widgets[page];
    w->event_handlers = window_park_page_events[page];
    w->widgets = window_park_page_widgets[page];
    WindowParkSetDisabledTabs(w);
    w->Invalidate();

    window_event_resize_call(w);
    window_event_invalidate_call(w);
    window_event_update_call(w);
    if (listen != 0 && w->viewport != nullptr)
        w->viewport->flags |= VIEWPORT_FLAG_SOUND_ON;
}

static void WindowParkAnchorBorderWidgets(rct_window* w)
{
    w->widgets[WIDX_BACKGROUND].right = w->width - 1;
    w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;
    w->widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
    w->widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;
    w->widgets[WIDX_TITLE].right = w->width - 2;
    w->widgets[WIDX_CLOSE].left = w->width - 13;
    w->widgets[WIDX_CLOSE].right = w->width - 3;
}

static void WindowParkSetPressedTab(rct_window* w)
{
    int32_t i;
    for (i = 0; i < 7; i++)
        w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
    w->pressed_widgets |= 1LL << (WIDX_TAB_1 + w->page);
}

static void WindowParkDrawTabImages(rct_drawpixelinfo* dpi, rct_window* w)
{
    int32_t sprite_idx;

    // Entrance tab
    if (!WidgetIsDisabled(w, WIDX_TAB_1))
    {
        gfx_draw_sprite(
            dpi, ImageId(SPR_TAB_PARK_ENTRANCE),
            w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_TAB_1].left, w->widgets[WIDX_TAB_1].top });
    }

    // Rating tab
    if (!WidgetIsDisabled(w, WIDX_TAB_2))
    {
        sprite_idx = SPR_TAB_GRAPH_0;
        if (w->page == WINDOW_PARK_PAGE_RATING)
            sprite_idx += (w->frame_no / 8) % 8;
        gfx_draw_sprite(
            dpi, ImageId(sprite_idx), w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_TAB_2].left, w->widgets[WIDX_TAB_2].top });
        gfx_draw_sprite(
            dpi, ImageId(SPR_RATING_HIGH),
            w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_TAB_2].left + 7, w->widgets[WIDX_TAB_2].top + 1 });
        gfx_draw_sprite(
            dpi, ImageId(SPR_RATING_LOW),
            w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_TAB_2].left + 16, w->widgets[WIDX_TAB_2].top + 12 });
    }

    // Guests tab
    if (!WidgetIsDisabled(w, WIDX_TAB_3))
    {
        sprite_idx = SPR_TAB_GRAPH_0;
        if (w->page == WINDOW_PARK_PAGE_GUESTS)
            sprite_idx += (w->frame_no / 8) % 8;
        gfx_draw_sprite(
            dpi, ImageId(sprite_idx), w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_TAB_3].left, w->widgets[WIDX_TAB_3].top });

        sprite_idx = GetPeepAnimation(PeepSpriteType::Normal).base_image + 1;
        if (w->page == WINDOW_PARK_PAGE_GUESTS)
            sprite_idx += w->var_492 & 0xFFFFFFFC;

        sprite_idx |= 0xA9E00000;
        gfx_draw_sprite(
            dpi, ImageId::FromUInt32(sprite_idx),
            w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_TAB_3].midX(), w->widgets[WIDX_TAB_3].bottom - 9 });
    }

    // Price tab
    if (!WidgetIsDisabled(w, WIDX_TAB_4))
    {
        sprite_idx = SPR_TAB_ADMISSION_0;
        if (w->page == WINDOW_PARK_PAGE_PRICE)
            sprite_idx += (w->frame_no / 2) % 8;
        gfx_draw_sprite(
            dpi, ImageId(sprite_idx), w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_TAB_4].left, w->widgets[WIDX_TAB_4].top });
    }

    // Statistics tab
    if (!WidgetIsDisabled(w, WIDX_TAB_5))
    {
        sprite_idx = SPR_TAB_STATS_0;
        if (w->page == WINDOW_PARK_PAGE_STATS)
            sprite_idx += (w->frame_no / 4) % 7;
        gfx_draw_sprite(
            dpi, ImageId(sprite_idx), w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_TAB_5].left, w->widgets[WIDX_TAB_5].top });
    }

    // Objective tab
    if (!WidgetIsDisabled(w, WIDX_TAB_6))
    {
        sprite_idx = SPR_TAB_OBJECTIVE_0;
        if (w->page == WINDOW_PARK_PAGE_OBJECTIVE)
            sprite_idx += (w->frame_no / 4) % 16;
        gfx_draw_sprite(
            dpi, ImageId(sprite_idx), w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_TAB_6].left, w->widgets[WIDX_TAB_6].top });
    }

    // Awards tab
    if (!WidgetIsDisabled(w, WIDX_TAB_7))
    {
        gfx_draw_sprite(
            dpi, ImageId(SPR_TAB_AWARDS),
            w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_TAB_7].left, w->widgets[WIDX_TAB_7].top });
    }
}

#pragma endregion
