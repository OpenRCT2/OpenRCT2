/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <iterator>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Game.h>
#include <openrct2/actions/ParkSetResearchFundingAction.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/management/Finance.h>
#include <openrct2/management/NewsItem.h>
#include <openrct2/management/Research.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/sprites.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Scenery.h>

static constexpr const int32_t WH_DEVELOPMENT = 196;
static constexpr const int32_t WW_DEVELOPMENT = 300;
static constexpr const int32_t WH_FUNDING = 207;
static constexpr const int32_t WW_FUNDING = 320;

// clang-format off
enum {
    WINDOW_RESEARCH_PAGE_DEVELOPMENT,
    WINDOW_RESEARCH_PAGE_FUNDING,
    WINDOW_RESEARCH_PAGE_COUNT
};

enum {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_PAGE_BACKGROUND,
    WIDX_TAB_1,
    WIDX_TAB_2,

    WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP,
    WIDX_LAST_DEVELOPMENT_GROUP,
    WIDX_LAST_DEVELOPMENT_BUTTON,

    WIDX_FUNDING_GROUP = 6,
    WIDX_RESEARCH_FUNDING,
    WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON,
    WIDX_PRIORITIES_GROUP,
    WIDX_TRANSPORT_RIDES,
    WIDX_GENTLE_RIDES,
    WIDX_ROLLER_COASTERS,
    WIDX_THRILL_RIDES,
    WIDX_WATER_RIDES,
    WIDX_SHOPS_AND_STALLS,
    WIDX_SCENERY_AND_THEMING,
};

#pragma region Widgets

static Widget window_research_development_widgets[] = {
    WINDOW_SHIM(STR_RESEARCH_AND_DEVELOPMENT, WW_DEVELOPMENT, WH_DEVELOPMENT),
    MakeWidget({  0,  43}, {     WW_DEVELOPMENT, 153}, WindowWidgetType::Resize,   WindowColour::Secondary                                                                ),
    MakeTab   ({  3,  17},                                                                                                  STR_RESEARCH_AND_DEVELOPMENT_TIP),
    MakeTab   ({ 34,  17},                                                                                                  STR_FINANCES_RESEARCH_TIP       ),
    MakeWidget({  3,  47}, {WW_DEVELOPMENT - 10,  70}, WindowWidgetType::Groupbox, WindowColour::Tertiary , STR_CURRENTLY_IN_DEVELOPMENT                                  ),
    MakeWidget({  3, 124}, {WW_DEVELOPMENT - 10,  65}, WindowWidgetType::Groupbox, WindowColour::Tertiary , STR_LAST_DEVELOPMENT                                          ),
    MakeWidget({265, 161}, {                 24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Tertiary , 0xFFFFFFFF,                   STR_RESEARCH_SHOW_DETAILS_TIP   ),
    WIDGETS_END,
};

static Widget window_research_funding_widgets[] = {
    WINDOW_SHIM(STR_RESEARCH_FUNDING, WW_FUNDING, WH_FUNDING),
    MakeWidget({  0,  43}, {     WW_FUNDING, 164}, WindowWidgetType::Resize,   WindowColour::Secondary                                                                                    ),
    MakeTab   ({  3,  17},                                                                                                      STR_RESEARCH_AND_DEVELOPMENT_TIP            ),
    MakeTab   ({ 34,  17},                                                                                                      STR_FINANCES_RESEARCH_TIP                   ),
    MakeWidget({  3,  47}, { WW_FUNDING - 6,  45}, WindowWidgetType::Groupbox, WindowColour::Tertiary , STR_RESEARCH_FUNDING_                                                             ),
    MakeWidget({  8,  59}, {            160,  14}, WindowWidgetType::DropdownMenu, WindowColour::Tertiary , 0xFFFFFFFF,                           STR_SELECT_LEVEL_OF_RESEARCH_AND_DEVELOPMENT),
    MakeWidget({156,  60}, {             11,  12}, WindowWidgetType::Button,   WindowColour::Tertiary , STR_DROPDOWN_GLYPH,                   STR_SELECT_LEVEL_OF_RESEARCH_AND_DEVELOPMENT),
    MakeWidget({  3,  96}, { WW_FUNDING - 6, 107}, WindowWidgetType::Groupbox, WindowColour::Tertiary , STR_RESEARCH_PRIORITIES                                                           ),
    MakeWidget({  8, 108}, {WW_FUNDING - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_RESEARCH_NEW_TRANSPORT_RIDES,     STR_RESEARCH_NEW_TRANSPORT_RIDES_TIP        ),
    MakeWidget({  8, 121}, {WW_FUNDING - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_RESEARCH_NEW_GENTLE_RIDES,        STR_RESEARCH_NEW_GENTLE_RIDES_TIP           ),
    MakeWidget({  8, 134}, {WW_FUNDING - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_RESEARCH_NEW_ROLLER_COASTERS,     STR_RESEARCH_NEW_ROLLER_COASTERS_TIP        ),
    MakeWidget({  8, 147}, {WW_FUNDING - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_RESEARCH_NEW_THRILL_RIDES,        STR_RESEARCH_NEW_THRILL_RIDES_TIP           ),
    MakeWidget({  8, 160}, {WW_FUNDING - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_RESEARCH_NEW_WATER_RIDES,         STR_RESEARCH_NEW_WATER_RIDES_TIP            ),
    MakeWidget({  8, 173}, {WW_FUNDING - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_RESEARCH_NEW_SHOPS_AND_STALLS,    STR_RESEARCH_NEW_SHOPS_AND_STALLS_TIP       ),
    MakeWidget({  8, 186}, {WW_FUNDING - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_RESEARCH_NEW_SCENERY_AND_THEMING, STR_RESEARCH_NEW_SCENERY_AND_THEMING_TIP    ),
    WIDGETS_END,
};

static Widget *window_research_page_widgets[] = {
    window_research_development_widgets,
    window_research_funding_widgets,
};

#pragma endregion

#pragma region Events

static void WindowResearchDevelopmentMouseup(rct_window *w, WidgetIndex widgetIndex);
static void WindowResearchDevelopmentUpdate(rct_window *w);
static void WindowResearchDevelopmentInvalidate(rct_window *w);
static void WindowResearchDevelopmentPaint(rct_window *w, rct_drawpixelinfo *dpi);

static void WindowResearchFundingMouseup(rct_window *w, WidgetIndex widgetIndex);
static void WindowResearchFundingMousedown(rct_window *w, WidgetIndex widgetIndex, Widget* widget);
static void WindowResearchFundingDropdown(rct_window *w, WidgetIndex widgetIndex, int32_t dropdownIndex);
static void WindowResearchFundingUpdate(rct_window *w);
static void WindowResearchFundingInvalidate(rct_window *w);
static void WindowResearchFundingPaint(rct_window *w, rct_drawpixelinfo *dpi);

//
static WindowEventList window_research_development_events([](auto& events)
{
    events.mouse_up = &WindowResearchDevelopmentMouseup;
    events.update = &WindowResearchDevelopmentUpdate;
    events.invalidate = &WindowResearchDevelopmentInvalidate;
    events.paint = &WindowResearchDevelopmentPaint;
});

// 0x009890E8
static WindowEventList window_research_funding_events([](auto& events)
{
    events.mouse_up = &WindowResearchFundingMouseup;
    events.mouse_down = &WindowResearchFundingMousedown;
    events.dropdown = &WindowResearchFundingDropdown;
    events.update = &WindowResearchFundingUpdate;
    events.invalidate = &WindowResearchFundingInvalidate;
    events.paint = &WindowResearchFundingPaint;
});

static WindowEventList *window_research_page_events[] = {
    &window_research_development_events,
    &window_research_funding_events,
};

#pragma endregion

// clang-format on

const int32_t window_research_tab_animation_loops[] = {
    16,
    16,
};

static constexpr const StringId ResearchStageNames[] = {
    STR_RESEARCH_STAGE_INITIAL_RESEARCH,
    STR_RESEARCH_STAGE_DESIGNING,
    STR_RESEARCH_STAGE_COMPLETING_DESIGN,
    STR_RESEARCH_STAGE_UNKNOWN,
};

static void WindowResearchSetPage(rct_window* w, int32_t page);
static void WindowResearchSetPressedTab(rct_window* w);
static void WindowResearchDrawTabImages(rct_drawpixelinfo* dpi, rct_window* w);

rct_window* WindowResearchOpen()
{
    rct_window* w;

    w = window_bring_to_front_by_class(WindowClass::Research);
    if (w == nullptr)
    {
        w = WindowCreateAutoPos(WW_FUNDING, WH_FUNDING, window_research_page_events[0], WindowClass::Research, WF_10);
        w->widgets = window_research_page_widgets[0];
        w->number = 0;
        w->page = 0;
        w->frame_no = 0;
        w->disabled_widgets = 0;
        ResearchUpdateUncompletedTypes();
    }

    w->page = 0;
    w->Invalidate();
    w->width = WW_DEVELOPMENT;
    w->height = WH_DEVELOPMENT;
    w->Invalidate();

    w->widgets = window_research_page_widgets[0];
    w->hold_down_widgets = 0;
    w->event_handlers = window_research_page_events[0];
    w->pressed_widgets = 0;
    w->disabled_widgets = 0;
    WindowInitScrollWidgets(*w);

    return w;
}

#pragma region Development page

/**
 *
 *  rct2: 0x006B6B38
 */
static void WindowResearchDevelopmentMouseup(rct_window* w, WidgetIndex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(*w);
            break;
        case WIDX_TAB_1:
        case WIDX_TAB_2:
            WindowResearchSetPage(w, widgetIndex - WIDX_TAB_1);
            break;
        case WIDX_LAST_DEVELOPMENT_BUTTON:
            News::OpenSubject(News::ItemType::Research, gResearchLastItem->rawValue);
            break;
    }
}

/**
 *
 *  rct2: 0x0069CBA6
 */
static void WindowResearchDevelopmentUpdate(rct_window* w)
{
    // Tab animation
    if (++w->frame_no >= window_research_tab_animation_loops[w->page])
        w->frame_no = 0;
    widget_invalidate(*w, WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x006B6819
 */
static void WindowResearchDevelopmentInvalidate(rct_window* w)
{
    if (w->widgets != window_research_page_widgets[WINDOW_RESEARCH_PAGE_DEVELOPMENT])
    {
        w->widgets = window_research_page_widgets[WINDOW_RESEARCH_PAGE_DEVELOPMENT];
        WindowInitScrollWidgets(*w);
    }

    WindowResearchSetPressedTab(w);

    window_research_development_widgets[WIDX_LAST_DEVELOPMENT_BUTTON].type = WindowWidgetType::Empty;
    if (gResearchLastItem.has_value())
    {
        auto type = gResearchLastItem->type;
        window_research_development_widgets[WIDX_LAST_DEVELOPMENT_BUTTON].type = WindowWidgetType::FlatBtn;
        const auto image = type == Research::EntryType::Ride ? SPR_NEW_RIDE : SPR_NEW_SCENERY;
        window_research_development_widgets[WIDX_LAST_DEVELOPMENT_BUTTON].image = ImageId(image);
    }
}

/**
 *
 *  rct2: 0x006B689B
 */
static void WindowResearchDevelopmentPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(*w, dpi);
    WindowResearchDrawTabImages(dpi, w);

    WindowResearchDevelopmentPagePaint(w, dpi, WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP);
}

void WindowResearchDevelopmentPagePaint(rct_window* w, rct_drawpixelinfo* dpi, WidgetIndex baseWidgetIndex)
{
    baseWidgetIndex = baseWidgetIndex - WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP;

    auto screenCoords = w->windowPos
        + ScreenCoordsXY{ 10, w->widgets[WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP + baseWidgetIndex].top + 12 };

    if (gResearchProgressStage == RESEARCH_STAGE_FINISHED_ALL)
    {
        // Research type
        auto ft = Formatter();
        ft.Add<StringId>(STR_RESEARCH_UNKNOWN);
        DrawTextWrapped(dpi, screenCoords, 296, STR_RESEARCH_TYPE_LABEL, ft);
        screenCoords.y += 25;

        // Progress
        ft = Formatter();
        ft.Add<StringId>(STR_RESEARCH_COMPLETED_AL);
        DrawTextWrapped(dpi, screenCoords, 296, STR_RESEARCH_PROGRESS_LABEL, ft);
        screenCoords.y += 15;

        // Expected
        ft = Formatter();
        ft.Add<StringId>(STR_RESEARCH_STAGE_UNKNOWN);
        DrawTextBasic(dpi, screenCoords, STR_RESEARCH_EXPECTED_LABEL, ft);
    }
    else
    {
        // Research type
        auto ft = Formatter();
        StringId label = STR_RESEARCH_TYPE_LABEL;
        if (gResearchProgressStage == RESEARCH_STAGE_INITIAL_RESEARCH)
        {
            ft.Add<StringId>(STR_RESEARCH_UNKNOWN);
        }
        else if (gResearchProgressStage == RESEARCH_STAGE_DESIGNING)
        {
            ft.Add<StringId>(gResearchNextItem->GetCategoryName());
        }
        else if (gResearchNextItem->type == Research::EntryType::Ride)
        {
            const auto& rtd = GetRideTypeDescriptor(gResearchNextItem->baseRideType);
            if (rtd.HasFlag(RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY))
            {
                ft.Add<StringId>(gResearchNextItem->GetName());
            }
            else if (gResearchNextItem->flags & RESEARCH_ENTRY_FLAG_FIRST_OF_TYPE)
            {
                ft.Add<StringId>(rtd.Naming.Name);
            }
            else
            {
                ft.Add<StringId>(gResearchNextItem->GetName());
                ft.Add<StringId>(rtd.Naming.Name);
                label = STR_RESEARCH_TYPE_LABEL_VEHICLE;
            }
        }
        else
        {
            ft.Add<StringId>(gResearchNextItem->GetName());
        }
        DrawTextWrapped(dpi, screenCoords, 296, label, ft);
        screenCoords.y += 25;

        // Progress
        ft = Formatter();
        ft.Add<StringId>(ResearchStageNames[gResearchProgressStage]);
        DrawTextWrapped(dpi, screenCoords, 296, STR_RESEARCH_PROGRESS_LABEL, ft);
        screenCoords.y += 15;

        // Expected
        ft = Formatter();
        if (gResearchProgressStage != RESEARCH_STAGE_INITIAL_RESEARCH && gResearchExpectedDay != 255)
        {
            // TODO: Should probably use game date format setting
            ft.Add<StringId>(STR_RESEARCH_EXPECTED_FORMAT);
            ft.Add<StringId>(DateDayNames[gResearchExpectedDay]);
            ft.Add<StringId>(DateGameMonthNames[gResearchExpectedMonth]);
        }
        else
        {
            ft.Add<StringId>(STR_RESEARCH_STAGE_UNKNOWN);
        }
        DrawTextBasic(dpi, screenCoords, STR_RESEARCH_EXPECTED_LABEL, ft);
    }

    // Last development
    screenCoords = w->windowPos + ScreenCoordsXY{ 10, w->widgets[WIDX_LAST_DEVELOPMENT_GROUP + baseWidgetIndex].top + 12 };

    if (gResearchLastItem.has_value())
    {
        StringId lastDevelopmentFormat = STR_EMPTY;
        auto ft = Formatter();
        if (gResearchLastItem->type == Research::EntryType::Scenery)
        {
            lastDevelopmentFormat = STR_RESEARCH_SCENERY_LABEL;
            ft.Add<StringId>(gResearchLastItem->GetName());
        }
        else
        {
            lastDevelopmentFormat = STR_RESEARCH_RIDE_LABEL;
            const auto& rtd = GetRideTypeDescriptor(gResearchLastItem->baseRideType);
            if (rtd.HasFlag(RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY))
            {
                ft.Add<StringId>(gResearchLastItem->GetName());
            }
            else if (gResearchLastItem->flags & RESEARCH_ENTRY_FLAG_FIRST_OF_TYPE)
            {
                ft.Add<StringId>(rtd.Naming.Name);
            }
            else
            {
                ft.Add<StringId>(gResearchLastItem->GetName());
                ft.Add<StringId>(rtd.Naming.Name);
                lastDevelopmentFormat = STR_RESEARCH_VEHICLE_LABEL;
            }
        }

        DrawTextWrapped(dpi, screenCoords, 266, lastDevelopmentFormat, ft);
    }
}

#pragma endregion

#pragma region Funding page

/**
 *
 *  rct2: 0x0069DB3F
 */
static void WindowResearchFundingMouseup(rct_window* w, WidgetIndex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(*w);
            break;
        case WIDX_TAB_1:
        case WIDX_TAB_2:
            WindowResearchSetPage(w, widgetIndex - WIDX_TAB_1);
            break;
        case WIDX_TRANSPORT_RIDES:
        case WIDX_GENTLE_RIDES:
        case WIDX_ROLLER_COASTERS:
        case WIDX_THRILL_RIDES:
        case WIDX_WATER_RIDES:
        case WIDX_SHOPS_AND_STALLS:
        case WIDX_SCENERY_AND_THEMING:
        {
            auto activeResearchTypes = gResearchPriorities;
            activeResearchTypes ^= 1 << (widgetIndex - WIDX_TRANSPORT_RIDES);
            auto gameAction = ParkSetResearchFundingAction(activeResearchTypes, gResearchFundingLevel);
            GameActions::Execute(&gameAction);
            break;
        }
    }
}

/**
 *
 *  rct2: 0x0069DB66
 */
static void WindowResearchFundingMousedown(rct_window* w, WidgetIndex widgetIndex, Widget* widget)
{
    if (widgetIndex != WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON)
        return;

    Widget* dropdownWidget = widget - 1;

    for (std::size_t i = 0; i < std::size(ResearchFundingLevelNames); i++)
    {
        gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
        gDropdownItems[i].Args = ResearchFundingLevelNames[i];
    }
    WindowDropdownShowTextCustomWidth(
        { w->windowPos.x + dropdownWidget->left, w->windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
        w->colours[1], 0, Dropdown::Flag::StayOpen, 4, dropdownWidget->width() - 3);

    int32_t currentResearchLevel = gResearchFundingLevel;
    Dropdown::SetChecked(currentResearchLevel, true);
}

/**
 *
 *  rct2: 0x0069DB6D
 */
static void WindowResearchFundingDropdown(rct_window* w, WidgetIndex widgetIndex, int32_t dropdownIndex)
{
    if (widgetIndex != WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON || dropdownIndex == -1)
        return;

    auto gameAction = ParkSetResearchFundingAction(gResearchPriorities, dropdownIndex);
    GameActions::Execute(&gameAction);
}

/**
 *
 *  rct2: 0x0069DC23
 */
static void WindowResearchFundingUpdate(rct_window* w)
{
    // Tab animation
    if (++w->frame_no >= window_research_tab_animation_loops[w->page])
        w->frame_no = 0;
    widget_invalidate(*w, WIDX_TAB_2);
}

/**
 *
 *  rct2: 0x0069DA64
 */
static void WindowResearchFundingInvalidate(rct_window* w)
{
    if (w->widgets != window_research_page_widgets[WINDOW_RESEARCH_PAGE_FUNDING])
    {
        w->widgets = window_research_page_widgets[WINDOW_RESEARCH_PAGE_FUNDING];
        WindowInitScrollWidgets(*w);
    }

    WindowResearchSetPressedTab(w);

    if ((gParkFlags & PARK_FLAGS_NO_MONEY) || (gResearchProgressStage == RESEARCH_STAGE_FINISHED_ALL))
    {
        window_research_funding_widgets[WIDX_RESEARCH_FUNDING].type = WindowWidgetType::Empty;
        window_research_funding_widgets[WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON].type = WindowWidgetType::Empty;
    }
    else
    {
        window_research_funding_widgets[WIDX_FUNDING_GROUP].type = WindowWidgetType::Groupbox;
        window_research_funding_widgets[WIDX_RESEARCH_FUNDING].type = WindowWidgetType::DropdownMenu;
        window_research_funding_widgets[WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON].type = WindowWidgetType::Button;

        // Current funding
        int32_t currentResearchLevel = gResearchFundingLevel;
        window_research_funding_widgets[WIDX_RESEARCH_FUNDING].text = ResearchFundingLevelNames[currentResearchLevel];
    }

    // Checkboxes
    uint8_t activeResearchTypes = gResearchPriorities;
    for (int32_t i = 0; i < 7; i++)
    {
        int32_t mask = 1 << i;
        int32_t widgetMask = 1 << (i + WIDX_TRANSPORT_RIDES);

        // Set checkbox disabled if research type is complete
        if (gResearchUncompletedCategories & mask)
        {
            w->disabled_widgets &= ~widgetMask;

            // Set checkbox ticked if research type is active
            if (activeResearchTypes & mask)
                w->pressed_widgets |= widgetMask;
            else
                w->pressed_widgets &= ~widgetMask;
        }
        else
        {
            w->disabled_widgets |= widgetMask;
            w->pressed_widgets &= ~widgetMask;
        }
    }
}

/**
 *
 *  rct2: 0x0069DAF0
 */
static void WindowResearchFundingPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(*w, dpi);
    WindowResearchDrawTabImages(dpi, w);

    WindowResearchFundingPagePaint(w, dpi, WIDX_RESEARCH_FUNDING);
}

void WindowResearchFundingPagePaint(rct_window* w, rct_drawpixelinfo* dpi, WidgetIndex baseWidgetIndex)
{
    if (gParkFlags & PARK_FLAGS_NO_MONEY)
        return;

    int32_t currentResearchLevel = gResearchFundingLevel;
    auto ft = Formatter();
    ft.Add<money64>(research_cost_table[currentResearchLevel]);
    DrawTextBasic(dpi, w->windowPos + ScreenCoordsXY{ 10, 77 }, STR_RESEARCH_COST_PER_MONTH, ft);
}

#pragma endregion

#pragma region Common

/**
 *
 *  rct2: 0x0069CAC5
 */
static void WindowResearchSetPage(rct_window* w, int32_t page)
{
    w->page = page;
    w->frame_no = 0;
    w->RemoveViewport();

    w->hold_down_widgets = 0;
    w->event_handlers = window_research_page_events[page];
    w->widgets = window_research_page_widgets[page];
    w->disabled_widgets = 0;
    w->pressed_widgets = 0;

    w->Invalidate();
    if (w->page == WINDOW_RESEARCH_PAGE_DEVELOPMENT)
    {
        w->width = WW_DEVELOPMENT;
        w->height = WH_DEVELOPMENT;
    }
    else
    {
        w->width = WW_FUNDING;
        w->height = WH_FUNDING;
    }
    window_event_resize_call(w);
    window_event_invalidate_call(w);

    WindowInitScrollWidgets(*w);
    w->Invalidate();
}

static void WindowResearchSetPressedTab(rct_window* w)
{
    int32_t i;
    for (i = 0; i < WINDOW_RESEARCH_PAGE_COUNT; i++)
        w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
    w->pressed_widgets |= 1LL << (WIDX_TAB_1 + w->page);
}

static void WindowResearchDrawTabImage(rct_drawpixelinfo* dpi, rct_window* w, int32_t page, int32_t spriteIndex)
{
    WidgetIndex widgetIndex = WIDX_TAB_1 + page;

    if (!WidgetIsDisabled(*w, widgetIndex))
    {
        if (w->page == page)
        {
            int32_t frame = w->frame_no / 2;
            if (page == WINDOW_RESEARCH_PAGE_DEVELOPMENT)
                frame %= 8;
            spriteIndex += frame;
        }

        gfx_draw_sprite(
            dpi, ImageId(spriteIndex),
            w->windowPos + ScreenCoordsXY{ w->widgets[widgetIndex].left, w->widgets[widgetIndex].top });
    }
}

static void WindowResearchDrawTabImages(rct_drawpixelinfo* dpi, rct_window* w)
{
    WindowResearchDrawTabImage(dpi, w, WINDOW_RESEARCH_PAGE_DEVELOPMENT, SPR_TAB_FINANCES_RESEARCH_0);
    WindowResearchDrawTabImage(dpi, w, WINDOW_RESEARCH_PAGE_FUNDING, SPR_TAB_FINANCES_SUMMARY_0);
}

#pragma endregion
