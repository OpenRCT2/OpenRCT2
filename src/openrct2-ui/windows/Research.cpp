/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <iterator>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/actions/ParkSetResearchFundingAction.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.Date.h>
#include <openrct2/management/Finance.h>
#include <openrct2/management/NewsItem.h>
#include <openrct2/management/Research.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Scenery.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr int32_t WH_DEVELOPMENT = 196;
    static constexpr int32_t WW_DEVELOPMENT = 300;
    static constexpr int32_t WH_FUNDING = 207;
    static constexpr int32_t WW_FUNDING = 320;

    enum
    {
        WINDOW_RESEARCH_PAGE_DEVELOPMENT,
        WINDOW_RESEARCH_PAGE_FUNDING,
        WINDOW_RESEARCH_PAGE_COUNT
    };

    enum
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_PAGE_BACKGROUND,
        WIDX_TAB_1,
        WIDX_TAB_2,

        WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP,
        WIDX_LAST_DEVELOPMENT_GROUP,
        WIDX_LAST_DEVELOPMENT_BUTTON,

        WIDX_RESEARCH_FUNDING_GROUP = 6,
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

    // clang-format off
    static constexpr auto window_research_development_widgets = makeWidgets(
        makeWindowShim(STR_RESEARCH_AND_DEVELOPMENT, WW_DEVELOPMENT, WH_DEVELOPMENT),
        makeWidget({  0,  43}, {     WW_DEVELOPMENT, 153}, WindowWidgetType::Resize,   WindowColour::Secondary                                                                ),
        makeTab   ({  3,  17},                                                                                                  STR_RESEARCH_AND_DEVELOPMENT_TIP),
        makeTab   ({ 34,  17},                                                                                                  STR_FINANCES_RESEARCH_TIP       ),
        makeWidget({  3,  47}, {WW_DEVELOPMENT - 10,  70}, WindowWidgetType::Groupbox, WindowColour::Tertiary , STR_CURRENTLY_IN_DEVELOPMENT                                  ),
        makeWidget({  3, 124}, {WW_DEVELOPMENT - 10,  65}, WindowWidgetType::Groupbox, WindowColour::Tertiary , STR_LAST_DEVELOPMENT                                          ),
        makeWidget({265, 161}, {                 24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Tertiary , 0xFFFFFFFF,                   STR_RESEARCH_SHOW_DETAILS_TIP   )
    );

    static constexpr auto window_research_funding_widgets = makeWidgets(
        makeWindowShim(STR_RESEARCH_FUNDING, WW_FUNDING, WH_FUNDING),
        makeWidget({  0,  43}, {     WW_FUNDING, 164}, WindowWidgetType::Resize,   WindowColour::Secondary                                                                                    ),
        makeTab   ({  3,  17},                                                                                                      STR_RESEARCH_AND_DEVELOPMENT_TIP            ),
        makeTab   ({ 34,  17},                                                                                                      STR_FINANCES_RESEARCH_TIP                   ),
        makeWidget({  3,  47}, { WW_FUNDING - 6,  45}, WindowWidgetType::Groupbox, WindowColour::Tertiary , STR_RESEARCH_FUNDING_                                                             ),
        makeWidget({  8,  59}, {            160,  14}, WindowWidgetType::DropdownMenu, WindowColour::Tertiary , 0xFFFFFFFF,                           STR_SELECT_LEVEL_OF_RESEARCH_AND_DEVELOPMENT),
        makeWidget({156,  60}, {             11,  12}, WindowWidgetType::Button,   WindowColour::Tertiary , STR_DROPDOWN_GLYPH,                   STR_SELECT_LEVEL_OF_RESEARCH_AND_DEVELOPMENT),
        makeWidget({  3,  96}, { WW_FUNDING - 6, 107}, WindowWidgetType::Groupbox, WindowColour::Tertiary , STR_RESEARCH_PRIORITIES                                                           ),
        makeWidget({  8, 108}, {WW_FUNDING - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_RESEARCH_NEW_TRANSPORT_RIDES,     STR_RESEARCH_NEW_TRANSPORT_RIDES_TIP        ),
        makeWidget({  8, 121}, {WW_FUNDING - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_RESEARCH_NEW_GENTLE_RIDES,        STR_RESEARCH_NEW_GENTLE_RIDES_TIP           ),
        makeWidget({  8, 134}, {WW_FUNDING - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_RESEARCH_NEW_ROLLER_COASTERS,     STR_RESEARCH_NEW_ROLLER_COASTERS_TIP        ),
        makeWidget({  8, 147}, {WW_FUNDING - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_RESEARCH_NEW_THRILL_RIDES,        STR_RESEARCH_NEW_THRILL_RIDES_TIP           ),
        makeWidget({  8, 160}, {WW_FUNDING - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_RESEARCH_NEW_WATER_RIDES,         STR_RESEARCH_NEW_WATER_RIDES_TIP            ),
        makeWidget({  8, 173}, {WW_FUNDING - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_RESEARCH_NEW_SHOPS_AND_STALLS,    STR_RESEARCH_NEW_SHOPS_AND_STALLS_TIP       ),
        makeWidget({  8, 186}, {WW_FUNDING - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_RESEARCH_NEW_SCENERY_AND_THEMING, STR_RESEARCH_NEW_SCENERY_AND_THEMING_TIP    )
    );

    static constexpr std::span<const Widget> window_research_page_widgets[] = {
        window_research_development_widgets,
        window_research_funding_widgets,
    };
    // clang-format on

#pragma endregion

    const int32_t window_research_tab_animation_loops[] = {
        16,
        16,
    };

    static constexpr StringId ResearchStageNames[] = {
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
            ResearchUpdateUncompletedTypes();
        }

        void SetPage(int32_t newPageIndex)
        {
            if (page == newPageIndex && !widgets.empty())
                return;

            page = newPageIndex;
            frame_no = 0;

            Invalidate();
            if (newPageIndex == WINDOW_RESEARCH_PAGE_DEVELOPMENT)
            {
                width = WW_DEVELOPMENT;
                height = WH_DEVELOPMENT;
            }
            else
            {
                width = WW_FUNDING;
                height = WH_FUNDING;
            }
            Invalidate();

            SetWidgets(window_research_page_widgets[newPageIndex]);
            hold_down_widgets = 0;
            disabled_widgets = 0;
            pressed_widgets = 0;
        }

    private:
        void OnUpdate() override
        {
            // Tab animation
            if (++frame_no >= window_research_tab_animation_loops[page])
                frame_no = 0;

            switch (page)
            {
                case WINDOW_RESEARCH_PAGE_DEVELOPMENT:
                {
                    InvalidateWidget(WIDX_TAB_1);
                    break;
                }
                case WINDOW_RESEARCH_PAGE_FUNDING:
                {
                    InvalidateWidget(WIDX_TAB_2);
                    break;
                }
            }
        }

        void OnMouseDown(WidgetIndex widgetIndex) override
        {
            if (page == WINDOW_RESEARCH_PAGE_FUNDING)
            {
                WindowResearchFundingMouseDown(this, widgetIndex, WIDX_RESEARCH_FUNDING);
            }
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
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
                    WindowResearchDevelopmentMouseUp(widgetIndex, WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP);
                    break;
                }
                case WINDOW_RESEARCH_PAGE_FUNDING:
                {
                    WindowResearchFundingMouseUp(widgetIndex, WIDX_RESEARCH_FUNDING);
                    break;
                }
            }
        }

        void OnDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
        {
            if (page == WINDOW_RESEARCH_PAGE_FUNDING)
            {
                WindowResearchFundingDropdown(widgetIndex, selectedIndex, WIDX_RESEARCH_FUNDING);
            }
        }

        void OnPrepareDraw() override
        {
            for (auto i = 0; i < WINDOW_RESEARCH_PAGE_COUNT; i++)
            {
                SetWidgetPressed(WIDX_TAB_1 + i, false);
            }

            SetWidgetPressed(WIDX_TAB_1 + page, true);

            switch (page)
            {
                case WINDOW_RESEARCH_PAGE_DEVELOPMENT:
                {
                    WindowResearchDevelopmentPrepareDraw(this, WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP);
                    break;
                }
                case WINDOW_RESEARCH_PAGE_FUNDING:
                {
                    WindowResearchFundingPrepareDraw(this, WIDX_RESEARCH_FUNDING);
                    break;
                }
            }
        }

        void OnDraw(RenderTarget& rt) override
        {
            DrawWidgets(rt);
            DrawTabImages(rt);

            switch (page)
            {
                case WINDOW_RESEARCH_PAGE_DEVELOPMENT:
                {
                    WindowResearchDevelopmentDraw(this, rt, WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP);
                    break;
                }
                case WINDOW_RESEARCH_PAGE_FUNDING:
                {
                    WindowResearchFundingDraw(this, rt);
                    break;
                }
            }
        }

        void DrawTabImage(RenderTarget& rt, int32_t tabPage, int32_t spriteIndex)
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
                    rt, ImageId(spriteIndex),
                    windowPos + ScreenCoordsXY{ widgets[widgetIndex].left, widgets[widgetIndex].top });
            }
        }

        void DrawTabImages(RenderTarget& rt)
        {
            DrawTabImage(rt, WINDOW_RESEARCH_PAGE_DEVELOPMENT, SPR_TAB_FINANCES_RESEARCH_0);
            DrawTabImage(rt, WINDOW_RESEARCH_PAGE_FUNDING, SPR_TAB_FINANCES_SUMMARY_0);
        }
    };

    WindowBase* ResearchOpen()
    {
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->FocusOrCreate<ResearchWindow>(WindowClass::Research, WW_DEVELOPMENT, WH_DEVELOPMENT, WF_10);
        window->SetPage(WINDOW_RESEARCH_PAGE_DEVELOPMENT);
        return window;
    }

    static WidgetIndex GetWidgetIndexOffset(WidgetIndex baseWidgetIndex, WidgetIndex currentPageWidgetIndex)
    {
        // Other windows that reuse the logic here will have different values for the widget enums, but they otherwise align to
        // those in this class. Therefore, they can be referenced relative to the widget index for the page in this class, using
        // the difference between them as an offset.
        return baseWidgetIndex - currentPageWidgetIndex;
    }

#pragma region Development page

    void WindowResearchDevelopmentMouseUp(WidgetIndex widgetIndex, WidgetIndex baseWidgetIndex)
    {
        const auto& gameState = getGameState();
        auto widgetOffset = GetWidgetIndexOffset(baseWidgetIndex, WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP);

        if (widgetIndex == (WIDX_LAST_DEVELOPMENT_BUTTON + widgetOffset))
        {
            News::OpenSubject(News::ItemType::research, gameState.researchLastItem->rawValue);
        }
    }

    void WindowResearchDevelopmentPrepareDraw(WindowBase* w, WidgetIndex baseWidgetIndex)
    {
        const auto& gameState = getGameState();
        // Offset the widget index to allow reuse from other windows
        auto widgetOffset = GetWidgetIndexOffset(baseWidgetIndex, WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP);
        w->widgets[WIDX_LAST_DEVELOPMENT_BUTTON + widgetOffset].type = WindowWidgetType::Empty;

        // Display button to link to the last development, if there is one
        if (gameState.researchLastItem.has_value())
        {
            auto type = gameState.researchLastItem->type;
            w->widgets[WIDX_LAST_DEVELOPMENT_BUTTON + widgetOffset].type = WindowWidgetType::FlatBtn;
            const auto image = type == Research::EntryType::Ride ? SPR_NEW_RIDE : SPR_NEW_SCENERY;
            w->widgets[WIDX_LAST_DEVELOPMENT_BUTTON + widgetOffset].image = ImageId(image);
        }
    }

    void WindowResearchDevelopmentDraw(WindowBase* w, RenderTarget& rt, WidgetIndex baseWidgetIndex)
    {
        const auto& gameState = getGameState();

        auto widgetOffset = GetWidgetIndexOffset(baseWidgetIndex, WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP);
        auto screenCoords = w->windowPos
            + ScreenCoordsXY{ 10, w->widgets[WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP + widgetOffset].top + 12 };

        if (gameState.researchProgressStage == RESEARCH_STAGE_FINISHED_ALL)
        {
            // Research type
            auto ft = Formatter();
            ft.Add<StringId>(STR_RESEARCH_UNKNOWN);
            DrawTextWrapped(rt, screenCoords, 296, STR_RESEARCH_TYPE_LABEL, ft);
            screenCoords.y += 25;

            // Progress
            ft = Formatter();
            ft.Add<StringId>(STR_RESEARCH_COMPLETED_AL);
            DrawTextWrapped(rt, screenCoords, 296, STR_RESEARCH_PROGRESS_LABEL, ft);
            screenCoords.y += 15;

            // Expected
            ft = Formatter();
            ft.Add<StringId>(STR_RESEARCH_STAGE_UNKNOWN);
            DrawTextBasic(rt, screenCoords, STR_RESEARCH_EXPECTED_LABEL, ft);
        }
        else
        {
            // Research type
            auto ft = Formatter();
            StringId label = STR_RESEARCH_TYPE_LABEL;
            if (gameState.researchProgressStage == RESEARCH_STAGE_INITIAL_RESEARCH)
            {
                ft.Add<StringId>(STR_RESEARCH_UNKNOWN);
            }
            else if (gameState.researchProgressStage == RESEARCH_STAGE_DESIGNING)
            {
                ft.Add<StringId>(gameState.researchNextItem->GetCategoryName());
            }
            else if (gameState.researchNextItem->type == Research::EntryType::Ride)
            {
                const auto& rtd = GetRideTypeDescriptor(gameState.researchNextItem->baseRideType);
                if (rtd.HasFlag(RtdFlag::listVehiclesSeparately))
                {
                    ft.Add<StringId>(gameState.researchNextItem->GetName());
                }
                else if (gameState.researchNextItem->flags & RESEARCH_ENTRY_FLAG_FIRST_OF_TYPE)
                {
                    ft.Add<StringId>(rtd.Naming.Name);
                }
                else
                {
                    ft.Add<StringId>(gameState.researchNextItem->GetName());
                    ft.Add<StringId>(rtd.Naming.Name);
                    label = STR_RESEARCH_TYPE_LABEL_VEHICLE;
                }
            }
            else
            {
                ft.Add<StringId>(gameState.researchNextItem->GetName());
            }
            DrawTextWrapped(rt, screenCoords, 296, label, ft);
            screenCoords.y += 25;

            // Progress
            ft = Formatter();
            ft.Add<StringId>(ResearchStageNames[gameState.researchProgressStage]);
            DrawTextWrapped(rt, screenCoords, 296, STR_RESEARCH_PROGRESS_LABEL, ft);
            screenCoords.y += 15;

            // Expected
            ft = Formatter();
            if (gameState.researchProgressStage != RESEARCH_STAGE_INITIAL_RESEARCH && gameState.researchExpectedDay != 255)
            {
                // TODO: Should probably use game date format setting
                ft.Add<StringId>(STR_RESEARCH_EXPECTED_FORMAT);
                ft.Add<StringId>(DateDayNames[gameState.researchExpectedDay]);
                ft.Add<StringId>(DateGameMonthNames[gameState.researchExpectedMonth]);
            }
            else
            {
                ft.Add<StringId>(STR_RESEARCH_STAGE_UNKNOWN);
            }
            DrawTextBasic(rt, screenCoords, STR_RESEARCH_EXPECTED_LABEL, ft);
        }

        // Last development
        screenCoords = w->windowPos + ScreenCoordsXY{ 10, w->widgets[WIDX_LAST_DEVELOPMENT_GROUP + widgetOffset].top + 12 };

        if (gameState.researchLastItem.has_value())
        {
            StringId lastDevelopmentFormat = kStringIdEmpty;
            auto ft = Formatter();
            if (gameState.researchLastItem->type == Research::EntryType::Scenery)
            {
                lastDevelopmentFormat = STR_RESEARCH_SCENERY_LABEL;
                ft.Add<StringId>(gameState.researchLastItem->GetName());
            }
            else
            {
                lastDevelopmentFormat = STR_RESEARCH_RIDE_LABEL;
                const auto& rtd = GetRideTypeDescriptor(gameState.researchLastItem->baseRideType);
                if (rtd.HasFlag(RtdFlag::listVehiclesSeparately))
                {
                    ft.Add<StringId>(gameState.researchLastItem->GetName());
                }
                else if (gameState.researchLastItem->flags & RESEARCH_ENTRY_FLAG_FIRST_OF_TYPE)
                {
                    ft.Add<StringId>(rtd.Naming.Name);
                }
                else
                {
                    ft.Add<StringId>(gameState.researchLastItem->GetName());
                    ft.Add<StringId>(rtd.Naming.Name);
                    lastDevelopmentFormat = STR_RESEARCH_VEHICLE_LABEL;
                }
            }

            DrawTextWrapped(rt, screenCoords, 266, lastDevelopmentFormat, ft);
        }
    }

#pragma endregion

#pragma region Funding page

    void WindowResearchFundingMouseDown(WindowBase* w, WidgetIndex widgetIndex, WidgetIndex baseWidgetIndex)
    {
        const auto& gameState = getGameState();
        auto widgetOffset = GetWidgetIndexOffset(baseWidgetIndex, WIDX_RESEARCH_FUNDING);

        if (widgetIndex != (WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON + widgetOffset))
            return;

        Widget* dropdownWidget = &w->widgets[widgetIndex - 1];

        for (std::size_t i = 0; i < std::size(kResearchFundingLevelNames); i++)
        {
            gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
            gDropdownItems[i].Args = kResearchFundingLevelNames[i];
        }
        WindowDropdownShowTextCustomWidth(
            { w->windowPos.x + dropdownWidget->left, w->windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
            w->colours[1], 0, Dropdown::Flag::StayOpen, 4, dropdownWidget->width() - 3);

        int32_t currentResearchLevel = gameState.researchFundingLevel;
        Dropdown::SetChecked(currentResearchLevel, true);
    }

    void WindowResearchFundingMouseUp(WidgetIndex widgetIndex, WidgetIndex baseWidgetIndex)
    {
        const auto& gameState = getGameState();
        auto widgetOffset = GetWidgetIndexOffset(baseWidgetIndex, WIDX_RESEARCH_FUNDING);

        switch (widgetIndex - widgetOffset)
        {
            case WIDX_TRANSPORT_RIDES:
            case WIDX_GENTLE_RIDES:
            case WIDX_ROLLER_COASTERS:
            case WIDX_THRILL_RIDES:
            case WIDX_WATER_RIDES:
            case WIDX_SHOPS_AND_STALLS:
            case WIDX_SCENERY_AND_THEMING:
            {
                auto activeResearchTypes = gameState.researchPriorities;
                activeResearchTypes ^= 1uLL << (widgetIndex - (WIDX_TRANSPORT_RIDES + widgetOffset));
                auto gameAction = ParkSetResearchFundingAction(activeResearchTypes, gameState.researchFundingLevel);
                GameActions::Execute(&gameAction);
                break;
            }
        }
    }

    void WindowResearchFundingDropdown(WidgetIndex widgetIndex, int32_t selectedIndex, WidgetIndex baseWidgetIndex)
    {
        const auto& gameState = getGameState();
        auto widgetOffset = GetWidgetIndexOffset(baseWidgetIndex, WIDX_RESEARCH_FUNDING);

        if (widgetIndex != (WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON + widgetOffset) || selectedIndex == -1)
            return;

        auto gameAction = ParkSetResearchFundingAction(gameState.researchPriorities, selectedIndex);
        GameActions::Execute(&gameAction);
    }

    void WindowResearchFundingPrepareDraw(WindowBase* w, WidgetIndex baseWidgetIndex)
    {
        const auto& gameState = getGameState();
        auto widgetOffset = GetWidgetIndexOffset(baseWidgetIndex, WIDX_RESEARCH_FUNDING);

        if ((gameState.park.Flags & PARK_FLAGS_NO_MONEY) || gameState.researchProgressStage == RESEARCH_STAGE_FINISHED_ALL)
        {
            w->widgets[WIDX_RESEARCH_FUNDING + widgetOffset].type = WindowWidgetType::Empty;
            w->widgets[WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON + widgetOffset].type = WindowWidgetType::Empty;
        }
        else
        {
            w->widgets[WIDX_RESEARCH_FUNDING + widgetOffset].type = WindowWidgetType::DropdownMenu;
            w->widgets[WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON + widgetOffset].type = WindowWidgetType::Button;
        }

        // Current funding
        int32_t currentResearchLevel = gameState.researchFundingLevel;
        w->widgets[WIDX_RESEARCH_FUNDING + widgetOffset].text = kResearchFundingLevelNames[currentResearchLevel];

        // Checkboxes
        uint8_t activeResearchTypes = gameState.researchPriorities;
        int32_t uncompletedResearchTypes = gameState.researchUncompletedCategories;
        for (int32_t i = 0; i < 7; i++)
        {
            int32_t mask = 1 << i;
            int32_t widgetMask = 1uLL << (i + WIDX_TRANSPORT_RIDES + widgetOffset);

            // Set checkbox disabled if research type is complete
            if (uncompletedResearchTypes & mask)
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

    void WindowResearchFundingDraw(WindowBase* w, RenderTarget& rt)
    {
        const auto& gameState = getGameState();
        if (gameState.park.Flags & PARK_FLAGS_NO_MONEY)
            return;

        int32_t currentResearchLevel = gameState.researchFundingLevel;
        auto ft = Formatter();
        ft.Add<money64>(research_cost_table[currentResearchLevel]);
        DrawTextBasic(
            rt, w->windowPos + ScreenCoordsXY{ 10, w->widgets[WIDX_TAB_1].top + 60 }, STR_RESEARCH_COST_PER_MONTH, ft);
    }

#pragma endregion
} // namespace OpenRCT2::Ui::Windows
