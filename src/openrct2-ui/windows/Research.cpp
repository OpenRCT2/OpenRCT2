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

//
//static WindowEventList window_research_development_events([](auto& events)
//{
//    events.mouse_up = &WindowResearchDevelopmentMouseup;
//    events.update = &WindowResearchDevelopmentUpdate;
//    events.invalidate = &WindowResearchDevelopmentInvalidate;
//    events.paint = &WindowResearchDevelopmentPaint;
//});

// 0x009890E8
//static WindowEventList window_research_funding_events([](auto& events)
//{
//    events.mouse_up = &WindowResearchFundingMouseup;
//    events.mouse_down = &WindowResearchFundingMousedown;
//    events.dropdown = &WindowResearchFundingDropdown;
//    events.update = &WindowResearchFundingUpdate;
//    events.invalidate = &WindowResearchFundingInvalidate;
//    events.paint = &WindowResearchFundingPaint;
//});

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

class ResearchWindow final : public Window
{
public:
    void OnOpen() override
    {
        SetPage(WINDOW_RESEARCH_PAGE_DEVELOPMENT);

        // TCTODO: Is any of this required? Where does it need to go?
        /*w = WindowBringToFrontByClass(WindowClass::Research);
        if (w == nullptr)
        {
            w = WindowCreateAutoPos(WW_FUNDING, WH_FUNDING, window_research_page_events[0], WindowClass::Research, WF_10);
            w->number = 0;
            ResearchUpdateUncompletedTypes();
        }*/
    }

    /**
     *
     *  rct2: 0x0069CAC5
     */
    void SetPage(int32_t p)
    {
        page = p;
        frame_no = 0;
        RemoveViewport();

        hold_down_widgets = 0;
        widgets = window_research_page_widgets[p];
        disabled_widgets = 0;
        pressed_widgets = 0;

        Invalidate();
        if (p == WINDOW_RESEARCH_PAGE_DEVELOPMENT)
        {
            width = WW_DEVELOPMENT;
            height = WH_DEVELOPMENT;
        }
        else
        {
            width = WW_FUNDING;
            height = WH_FUNDING;
        }
        WindowEventResizeCall(this);
        WindowEventInvalidateCall(this);

        WindowInitScrollWidgets(*this);
        Invalidate();
    }

    /**
     *
     *  rct2: 0x0069CBA6
     *  rct2: 0x0069DC23
     */
    void OnUpdate() override
    {
        // Tab animation
        if (++frame_no >= window_research_tab_animation_loops[page])
            frame_no = 0;
        InvalidateWidget(WIDX_TAB_1 + page);
    }

    void OnMouseDown(WidgetIndex widgetIndex)
    {
        OnMouseDownFunding(widgetIndex);
    }

    void OnMouseUp(WidgetIndex widgetIndex)
    {
        // Switch tab or close
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
            {
                Close();
                break;
            }
            case WIDX_TAB_1:
            case WIDX_TAB_2:
            {
                SetPage(widgetIndex - WIDX_TAB_1);
                break;
            }
        }

        // Process mouse up for specific tab
        switch (page)
        {
            case WINDOW_RESEARCH_PAGE_DEVELOPMENT:
            {
                OnMouseUpDevelopment(widgetIndex);
                break;
            }
            case WINDOW_RESEARCH_PAGE_FUNDING:
            {
                OnMouseUpFunding(widgetIndex);
                break;
            }
        }
    }

    void OnDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
    {
        if (page == WINDOW_RESEARCH_PAGE_FUNDING)
        {
            OnDropdownFunding(widgetIndex, selectedIndex);
        }
    }

    void OnPrepareDraw() override
    {
        auto* targetWidgets = window_research_page_widgets[page];

        if (widgets != targetWidgets)
        {
            widgets = targetWidgets;
            WindowInitScrollWidgets(*this);
        }

        disabled_widgets = 0;

        for (auto i = 0; i < WINDOW_RESEARCH_PAGE_COUNT; i++)
        {
            SetWidgetPressed(WIDX_TAB_1 + i, false);
        }

        SetWidgetPressed(WIDX_TAB_1 + page, true);

        switch (page)
        {
            case WINDOW_RESEARCH_PAGE_DEVELOPMENT:
            {
                OnPrepareDrawDevelopment();
                break;
            }
            case WINDOW_RESEARCH_PAGE_FUNDING:
            {
                OnPrepareDrawFunding();
                break;
            }
        }
    }

    void OnDraw(DrawPixelInfo& dpi) override
    {
        DrawWidgets(dpi);
        DrawTabImages(dpi);

        switch (page)
        {
            case WINDOW_RESEARCH_PAGE_DEVELOPMENT:
            {
                OnDrawDevelopment(dpi);
                break;
            }
            case WINDOW_RESEARCH_PAGE_FUNDING:
            {
                OnDrawFunding(dpi);
                break;
            }
        }
    }

#pragma region Development page

    /**
     *
     *  rct2: 0x006B6B38
     */
    void OnMouseUpDevelopment(WidgetIndex widgetIndex)
    {
        if (widgetIndex == WIDX_LAST_DEVELOPMENT_BUTTON)
        {
            News::OpenSubject(News::ItemType::Research, gResearchLastItem->rawValue);
        }
    }

    /**
     *
     *  rct2: 0x006B6819
     */
    void OnPrepareDrawDevelopment()
    {
        window_research_development_widgets[WIDX_LAST_DEVELOPMENT_BUTTON].type = WindowWidgetType::Empty;
        if (gResearchLastItem.has_value())
        {
            auto type = gResearchLastItem->type;
            window_research_development_widgets[WIDX_LAST_DEVELOPMENT_BUTTON].type = WindowWidgetType::FlatBtn;
            const auto image = type == Research::EntryType::Ride ? SPR_NEW_RIDE : SPR_NEW_SCENERY;
            window_research_development_widgets[WIDX_LAST_DEVELOPMENT_BUTTON].image = ImageId(image);
        }
    }

    // TCTODO: Delete this
    static void WindowResearchDevelopmentInvalidate(WindowBase* w)
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
    void OnDrawDevelopment(DrawPixelInfo& dpi)
    {
        auto screenCoords = windowPos
            + ScreenCoordsXY{ 10, widgets[WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP].top + 12 };

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
        screenCoords = windowPos + ScreenCoordsXY{ 10, widgets[WIDX_LAST_DEVELOPMENT_GROUP].top + 12 };

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

    // TCTODO: Delete this
    static void WindowResearchDevelopmentPaint(WindowBase* w, DrawPixelInfo* dpi)
    {
        //WindowDrawWidgets(*w, dpi);
        //WindowResearchDrawTabImages(dpi, w);

        //WindowResearchDevelopmentPagePaint(w, dpi, WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP);
    }

    // TCTODO: Delete this
    void WindowResearchDevelopmentPagePaint(WindowBase* w, DrawPixelInfo* dpi, WidgetIndex baseWidgetIndex)
    {
        baseWidgetIndex = baseWidgetIndex - WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP;

        auto screenCoords = w->windowPos
            + ScreenCoordsXY{ 10, w->widgets[WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP + baseWidgetIndex].top + 12 };

        if (gResearchProgressStage == RESEARCH_STAGE_FINISHED_ALL)
        {
            // Research type
            auto ft = Formatter();
            ft.Add<StringId>(STR_RESEARCH_UNKNOWN);
            DrawTextWrapped(*dpi, screenCoords, 296, STR_RESEARCH_TYPE_LABEL, ft);
            screenCoords.y += 25;

            // Progress
            ft = Formatter();
            ft.Add<StringId>(STR_RESEARCH_COMPLETED_AL);
            DrawTextWrapped(*dpi, screenCoords, 296, STR_RESEARCH_PROGRESS_LABEL, ft);
            screenCoords.y += 15;

            // Expected
            ft = Formatter();
            ft.Add<StringId>(STR_RESEARCH_STAGE_UNKNOWN);
            DrawTextBasic(*dpi, screenCoords, STR_RESEARCH_EXPECTED_LABEL, ft);
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
            DrawTextWrapped(*dpi, screenCoords, 296, label, ft);
            screenCoords.y += 25;

            // Progress
            ft = Formatter();
            ft.Add<StringId>(ResearchStageNames[gResearchProgressStage]);
            DrawTextWrapped(*dpi, screenCoords, 296, STR_RESEARCH_PROGRESS_LABEL, ft);
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
            DrawTextBasic(*dpi, screenCoords, STR_RESEARCH_EXPECTED_LABEL, ft);
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

            DrawTextWrapped(*dpi, screenCoords, 266, lastDevelopmentFormat, ft);
        }
    }

#pragma endregion

#pragma region Funding page

    /**
     *
     *  rct2: 0x0069DB3F
     */
    void OnMouseUpFunding(WidgetIndex widgetIndex)
    {
        if (widgetIndex >= WIDX_TRANSPORT_RIDES && widgetIndex <= WIDX_SCENERY_AND_THEMING)
        {
            auto activeResearchTypes = gResearchPriorities;
            activeResearchTypes ^= 1uLL << (widgetIndex - WIDX_TRANSPORT_RIDES);
            auto gameAction = ParkSetResearchFundingAction(activeResearchTypes, gResearchFundingLevel);
            GameActions::Execute(&gameAction);
        }
    }

    /**
     *
     *  rct2: 0x0069DB66
     */
    void OnMouseDownFunding(WidgetIndex widgetIndex)
    {
        if (widgetIndex != WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON)
            return;

        Widget* dropdownWidget = &widgets[widgetIndex - 1];

        for (std::size_t i = 0; i < std::size(ResearchFundingLevelNames); i++)
        {
            gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
            gDropdownItems[i].Args = ResearchFundingLevelNames[i];
        }
        WindowDropdownShowTextCustomWidth(
            { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
            colours[1], 0, Dropdown::Flag::StayOpen, 4, dropdownWidget->width() - 3);

        int32_t currentResearchLevel = gResearchFundingLevel;
        Dropdown::SetChecked(currentResearchLevel, true);
    }

    /**
     *
     *  rct2: 0x0069DB6D
     */
    void OnDropdownFunding(WidgetIndex widgetIndex, int32_t selectedIndex)
    {
        if (widgetIndex != WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON || selectedIndex == -1)
            return;

        auto gameAction = ParkSetResearchFundingAction(gResearchPriorities, selectedIndex);
        GameActions::Execute(&gameAction);
    }

    /**
     *
     *  rct2: 0x0069DA64
     */
    void OnPrepareDrawFunding()
    {
        if (gResearchProgressStage == RESEARCH_STAGE_FINISHED_ALL)
        {
            window_research_funding_widgets[WIDX_RESEARCH_FUNDING].type = WindowWidgetType::Empty;
            window_research_funding_widgets[WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON].type = WindowWidgetType::Empty;
        }
        else
        {
            window_research_funding_widgets[WIDX_RESEARCH_FUNDING].type = WindowWidgetType::DropdownMenu;
            window_research_funding_widgets[WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON].type = WindowWidgetType::Button;
        }
        int32_t currentResearchLevel = gResearchFundingLevel;

        // Current funding
        window_research_funding_widgets[WIDX_RESEARCH_FUNDING].text = ResearchFundingLevelNames[currentResearchLevel];

        // Checkboxes
        uint8_t activeResearchTypes = gResearchPriorities;
        int32_t uncompletedResearchTypes = gResearchUncompletedCategories;
        for (int32_t i = 0; i < 7; i++)
        {
            int32_t mask = 1 << i;
            int32_t widgetMask = 1uLL << (i + WIDX_TRANSPORT_RIDES);

            // Set checkbox disabled if research type is complete
            if (uncompletedResearchTypes & mask)
            {
                disabled_widgets &= ~widgetMask;

                // Set checkbox ticked if research type is active
                if (activeResearchTypes & mask)
                    pressed_widgets |= widgetMask;
                else
                    pressed_widgets &= ~widgetMask;
            }
            else
            {
                disabled_widgets |= widgetMask;
                pressed_widgets &= ~widgetMask;
            }
        }
    }

    /**
     *
     *  rct2: 0x0069DAF0
     */
    void OnDrawFunding(DrawPixelInfo& dpi)
    {
        if (gParkFlags & PARK_FLAGS_NO_MONEY)
            return;

        int32_t currentResearchLevel = gResearchFundingLevel;
        auto ft = Formatter();
        ft.Add<money64>(research_cost_table[currentResearchLevel]);
        DrawTextBasic(dpi, windowPos + ScreenCoordsXY{ 10, 77 }, STR_RESEARCH_COST_PER_MONTH, ft);
    }

#pragma endregion

#pragma region Common

    static void WindowResearchSetPressedTab(WindowBase* w)
    {
        int32_t i;
        for (i = 0; i < WINDOW_RESEARCH_PAGE_COUNT; i++)
            w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
        w->pressed_widgets |= 1LL << (WIDX_TAB_1 + w->page);
    }

    void DrawTabImage(DrawPixelInfo& dpi, int32_t tabPage, int32_t spriteIndex)
    {
        WidgetIndex widgetIndex = WIDX_TAB_1 + tabPage;

        if (!IsWidgetDisabled(widgetIndex))
        {
            if (page == tabPage)
            {
                int32_t frame = frame_no / 2;
                if (tabPage == WINDOW_RESEARCH_PAGE_DEVELOPMENT)
                    frame %= 8;
                spriteIndex += frame;
            }

            GfxDrawSprite(
                &dpi, ImageId(spriteIndex),
                windowPos + ScreenCoordsXY{ widgets[widgetIndex].left, widgets[widgetIndex].top });
        }
    }

    void DrawTabImages(DrawPixelInfo& dpi)
    {
        DrawTabImage(dpi, WINDOW_RESEARCH_PAGE_DEVELOPMENT, SPR_TAB_FINANCES_RESEARCH_0);
        DrawTabImage(dpi, WINDOW_RESEARCH_PAGE_FUNDING, SPR_TAB_FINANCES_SUMMARY_0);
    }
};
#pragma endregion


WindowBase* WindowResearchOpen()
{
    return WindowFocusOrCreate<ResearchWindow>(WindowClass::Research, WW_FUNDING, WH_FUNDING, WF_10);
}
