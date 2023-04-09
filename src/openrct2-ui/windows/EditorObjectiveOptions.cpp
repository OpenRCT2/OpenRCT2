/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/Dropdown.h"
#include "../interface/Widget.h"
#include "../interface/Window.h"
#include "Window.h"

#include <algorithm>
#include <openrct2/Context.h>
#include <openrct2/GameState.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/actions/ParkSetNameAction.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Font.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/localisation/Date.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Language.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/sprites.h>
#include <openrct2/world/Park.h>

static constexpr const StringId WINDOW_TITLE = STR_OBJECTIVE_SELECTION;
static constexpr const int32_t WH = 229;
static constexpr const int32_t WW = 450;

// The number has to leave a bit of room for other entities like vehicles, litter and balloons.
static constexpr const uint16_t MaxObjectiveGuests = 50000;

#pragma region Widgets

// clang-format off
enum {
    WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN,
    WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_RIDES,
    WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_COUNT
};

static constexpr const StringId ObjectiveDropdownOptionNames[] = {
    STR_OBJECTIVE_DROPDOWN_NONE,
    STR_OBJECTIVE_DROPDOWN_NUMBER_OF_GUESTS_AT_A_GIVEN_DATE,
    STR_OBJECTIVE_DROPDOWN_PARK_VALUE_AT_A_GIVEN_DATE,
    STR_OBJECTIVE_DROPDOWN_HAVE_FUN,
    STR_OBJECTIVE_DROPDOWN_BUILD_THE_BEST_RIDE_YOU_CAN,
    STR_OBJECTIVE_DROPDOWN_BUILD_10_ROLLER_COASTERS,
    STR_OBJECTIVE_DROPDOWN_NUMBER_OF_GUESTS_IN_PARK,
    STR_OBJECTIVE_DROPDOWN_MONTHLY_INCOME_FROM_RIDE_TICKETS,
    STR_OBJECTIVE_DROPDOWN_BUILD_10_ROLLER_COASTERS_OF_A_GIVEN_LENGTH,
    STR_OBJECTIVE_DROPDOWN_FINISH_BUILDING_5_ROLLER_COASTERS,
    STR_OBJECTIVE_DROPDOWN_REPAY_LOAN_AND_ACHIEVE_A_GIVEN_PARK_VALUE,
    STR_OBJECTIVE_DROPDOWN_MONTHLY_PROFIT_FROM_FOOD_MERCHANDISE,
};

enum {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_PAGE_BACKGROUND,
    WIDX_TAB_1,
    WIDX_TAB_2,

    WIDX_OBJECTIVE = 6,
    WIDX_OBJECTIVE_DROPDOWN,
    WIDX_OBJECTIVE_ARG_1,
    WIDX_OBJECTIVE_ARG_1_INCREASE,
    WIDX_OBJECTIVE_ARG_1_DECREASE,
    WIDX_OBJECTIVE_ARG_2,
    WIDX_OBJECTIVE_ARG_2_INCREASE,
    WIDX_OBJECTIVE_ARG_2_DECREASE,
    WIDX_PARK_NAME,
    WIDX_SCENARIO_NAME,
    WIDX_CATEGORY,
    WIDX_CATEGORY_DROPDOWN,
    WIDX_DETAILS,

    WIDX_RIDES = 6
};

#define MAIN_OBJECTIVE_OPTIONS_WIDGETS \
    WINDOW_SHIM(WINDOW_TITLE, WW, WH), \
    MakeWidget({  0,  43}, {280, 106}, WindowWidgetType::Resize, WindowColour::Secondary), \
    MakeTab   ({  3,  17}, STR_SELECT_OBJECTIVE_AND_PARK_NAME_TIP         ), \
    MakeTab   ({ 34,  17}, STR_SELECT_RIDES_TO_BE_PRESERVED_TIP           )

static Widget window_editor_objective_options_main_widgets[] = {
    MAIN_OBJECTIVE_OPTIONS_WIDGETS,
    MakeWidget        ({ 98,  48}, {344,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_NONE,           STR_SELECT_OBJECTIVE_FOR_THIS_SCENARIO_TIP     ),
    MakeWidget        ({430,  49}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_SELECT_OBJECTIVE_FOR_THIS_SCENARIO_TIP     ),
    MakeSpinnerWidgets({158,  65}, {120,  12}, WindowWidgetType::Button,   WindowColour::Secondary                                                                     ), // NB: 3 widgets
    MakeSpinnerWidgets({158,  82}, {120,  12}, WindowWidgetType::Button,   WindowColour::Secondary                                                                     ), // NB: 3 widgets
    MakeWidget        ({370,  99}, { 75,  12}, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHANGE,         STR_CHANGE_NAME_OF_PARK_TIP                    ),
    MakeWidget        ({370, 116}, { 75,  12}, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHANGE,         STR_CHANGE_NAME_OF_SCENARIO_TIP                ),
    MakeWidget        ({ 98, 133}, {180,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_NONE,           STR_SELECT_WHICH_GROUP_THIS_SCENARIO_APPEARS_IN),
    MakeWidget        ({266, 134}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_SELECT_WHICH_GROUP_THIS_SCENARIO_APPEARS_IN),
    MakeWidget        ({370, 150}, { 75,  12}, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHANGE,         STR_CHANGE_DETAIL_NOTES_ABOUT_PARK_SCENARIO_TIP),
    WIDGETS_END,
};

static Widget window_editor_objective_options_rides_widgets[] = {
    MAIN_OBJECTIVE_OPTIONS_WIDGETS,
    MakeWidget({  3,  60}, {374, 161}, WindowWidgetType::Scroll, WindowColour::Secondary, SCROLL_VERTICAL),
    WIDGETS_END,
};

static Widget *window_editor_objective_options_widgets[] = {
    window_editor_objective_options_main_widgets,
    window_editor_objective_options_rides_widgets,
};

#pragma endregion

#pragma region Enabled widgets

static uint64_t window_editor_objective_options_page_hold_down_widgets[] = {
    (1uLL << WIDX_OBJECTIVE_ARG_1_INCREASE) |
    (1uLL << WIDX_OBJECTIVE_ARG_1_DECREASE) |
    (1uLL << WIDX_OBJECTIVE_ARG_2_INCREASE) |
    (1uLL << WIDX_OBJECTIVE_ARG_2_DECREASE),

    0,
};
// clang-format on

#pragma endregion

class EditorObjectiveOptionsWindow final : public Window
{
public:
    void OnOpen() override
    {
        widgets = window_editor_objective_options_main_widgets;
        pressed_widgets = 0;
        hold_down_widgets = window_editor_objective_options_page_hold_down_widgets[WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN];
        InitScrollWidgets();
        selected_tab = WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN;
        no_list_items = 0;
        selected_list_item = -1;
        UpdateDisabledWidgets();
    }

    void OnMouseUp(WidgetIndex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                return;
            case WIDX_TAB_1:
            case WIDX_TAB_2:
                SetPage(widgetIndex - WIDX_TAB_1);
                return;
        }

        if (page == WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN)
        {
            OnMouseUpMain(widgetIndex);
        }
    }

    void OnResize() override
    {
        switch (page)
        {
            case WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN:
                OnResizeMain();
                break;
            case WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_RIDES:
                OnResizeRides();
                break;
        }
    }

    void OnPrepareDraw() override
    {
        switch (page)
        {
            case WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN:
                OnPrepareDrawMain();
                break;
            case WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_RIDES:
                OnPrepareDrawRides();
                break;
        }
    }

    void OnUpdate() override
    {
        switch (page)
        {
            case WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN:
                OnUpdateMain();
                break;
            case WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_RIDES:
                OnUpdateRides();
                break;
        }
    }

    void OnDraw(DrawPixelInfo& dpi) override
    {
        switch (page)
        {
            case WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN:
                OnDrawMain(dpi);
                break;
            case WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_RIDES:
                OnDrawRides(dpi);
                break;
        }
    }

    void OnMouseDown(WidgetIndex widgetIndex) override
    {
        if (page == WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN)
        {
            OnMouseDownMain(widgetIndex);
        }
    }

    void OnDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
    {
        if (page == WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN)
        {
            OnDropdownMain(widgetIndex, selectedIndex);
        }
    }

    void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
    {
        if (page == WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN)
        {
            OnTextInputMain(widgetIndex, text);
        }
    }

    void OnScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
    {
        if (page == WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_RIDES)
        {
            OnScrollMouseDownRides(scrollIndex, screenCoords);
        }
    }

    void OnScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
    {
        if (page == WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_RIDES)
        {
            OnScrollMouseOverRides(scrollIndex, screenCoords);
        }
    }

    void OnScrollDraw(int32_t scrollIndex, DrawPixelInfo& dpi) override
    {
        if (page == WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_RIDES)
        {
            OnScrollDrawRides(dpi, scrollIndex);
        }
    }

    ScreenSize OnScrollGetSize(int32_t scrollIndex) override
    {
        if (page == WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_RIDES)
        {
            return OnScrollGetSizeRides(scrollIndex);
        }

        return {};
    }

private:
    /**
     *
     *  rct2: 0x00668496
     */
    void SetPage(int32_t newPage)
    {
        if (page == newPage)
            return;

        page = newPage;
        frame_no = 0;
        var_492 = 0;
        no_list_items = 0;
        selected_list_item = -1;
        hold_down_widgets = window_editor_objective_options_page_hold_down_widgets[newPage];
        widgets = window_editor_objective_options_widgets[newPage];
        Invalidate();
        UpdateDisabledWidgets();
        OnResize();
        OnPrepareDraw();
        InitScrollWidgets();
        Invalidate();
    }

    /**
     *
     *  rct2: 0x00672609
     */
    void UpdateDisabledWidgets()
    {
        // Check if there are any rides (not shops or facilities)
        const auto& rideManager = GetRideManager();
        if (std::any_of(rideManager.begin(), rideManager.end(), [](const Ride& rideToCheck) { return rideToCheck.IsRide(); }))
        {
            disabled_widgets &= ~(1uLL << WIDX_TAB_2);
        }
        else
        {
            disabled_widgets |= (1uLL << WIDX_TAB_2);
        }
    }

    void SetPressedTab()
    {
        int32_t i;
        for (i = WIDX_TAB_1; i <= WIDX_TAB_2; i++)
            pressed_widgets &= ~(1 << i);
        pressed_widgets |= 1LL << (WIDX_TAB_1 + page);
    }

    void AnchorBorderWidgets()
    {
        ResizeFrameWithPage();
    }

    void DrawTabImages(DrawPixelInfo& dpi)
    {
        Widget* widget;
        int32_t spriteIndex;

        // Tab 1
        widget = &widgets[WIDX_TAB_1];

        spriteIndex = SPR_TAB_OBJECTIVE_0;
        if (page == WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN)
            spriteIndex += (frame_no / 4) % 16;

        GfxDrawSprite(dpi, ImageId(spriteIndex), windowPos + ScreenCoordsXY{ widget->left, widget->top });

        // Tab 2
        if (!IsWidgetDisabled(WIDX_TAB_2))
        {
            widget = &widgets[WIDX_TAB_2];
            spriteIndex = SPR_TAB_RIDE_0;
            if (page == WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_RIDES)
                spriteIndex += (frame_no / 4) % 16;

            GfxDrawSprite(dpi, ImageId(spriteIndex), windowPos + ScreenCoordsXY{ widget->left, widget->top });
        }
    }

    /**
     *
     *  rct2: 0x0067201D
     */
    void SetObjective(int32_t objective)
    {
        gScenarioObjective.Type = objective;
        Invalidate();

        // Set default objective arguments
        switch (objective)
        {
            case OBJECTIVE_NONE:
            case OBJECTIVE_HAVE_FUN:
            case OBJECTIVE_BUILD_THE_BEST:
            case OBJECTIVE_10_ROLLERCOASTERS:
                break;
            case OBJECTIVE_GUESTS_BY:
                gScenarioObjective.Year = 3;
                gScenarioObjective.NumGuests = 1500;
                break;
            case OBJECTIVE_PARK_VALUE_BY:
                gScenarioObjective.Year = 3;
                gScenarioObjective.Currency = 50000.00_GBP;
                break;
            case OBJECTIVE_GUESTS_AND_RATING:
                gScenarioObjective.NumGuests = 2000;
                break;
            case OBJECTIVE_MONTHLY_RIDE_INCOME:
                gScenarioObjective.Currency = 10000.00_GBP;
                break;
            case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
                gScenarioObjective.MinimumLength = 1200;
                break;
            case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
                gScenarioObjective.MinimumExcitement = FIXED_2DP(6, 70);
                break;
            case OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE:
                gScenarioObjective.Currency = 50000.00_GBP;
                break;
            case OBJECTIVE_MONTHLY_FOOD_INCOME:
                gScenarioObjective.Currency = 1000.00_GBP;
                break;
        }
    }

    void ShowObjectiveDropdown()
    {
        int32_t numItems = 0, objectiveType;
        Widget* dropdownWidget;
        uint32_t parkFlags;

        dropdownWidget = &widgets[WIDX_OBJECTIVE];
        parkFlags = gParkFlags;

        for (auto i = 0; i < OBJECTIVE_COUNT; i++)
        {
            if (i == OBJECTIVE_NONE || i == OBJECTIVE_BUILD_THE_BEST)
                continue;

            const bool objectiveAllowedByMoneyUsage = !(parkFlags & PARK_FLAGS_NO_MONEY) || !ObjectiveNeedsMoney(i);
            // This objective can only work if the player can ask money for rides.
            const bool objectiveAllowedByPaymentSettings = (i != OBJECTIVE_MONTHLY_RIDE_INCOME) || ParkRidePricesUnlocked();
            if (objectiveAllowedByMoneyUsage && objectiveAllowedByPaymentSettings)
            {
                gDropdownItems[numItems].Format = STR_DROPDOWN_MENU_LABEL;
                gDropdownItems[numItems].Args = ObjectiveDropdownOptionNames[i];
                numItems++;
            }
        }

        WindowDropdownShowTextCustomWidth(
            { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1, colours[1],
            0, Dropdown::Flag::StayOpen, numItems, dropdownWidget->width() - 3);

        objectiveType = gScenarioObjective.Type;
        for (int32_t j = 0; j < numItems; j++)
        {
            if (gDropdownItems[j].Args - STR_OBJECTIVE_DROPDOWN_NONE == objectiveType)
            {
                Dropdown::SetChecked(j, true);
                break;
            }
        }
    }

    void ShowCategoryDropdown()
    {
        int32_t i;
        Widget* dropdownWidget;

        dropdownWidget = &widgets[WIDX_CATEGORY];

        for (i = SCENARIO_CATEGORY_BEGINNER; i <= SCENARIO_CATEGORY_OTHER; i++)
        {
            gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
            gDropdownItems[i].Args = ScenarioCategoryStringIds[i];
        }
        WindowDropdownShowTextCustomWidth(
            { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1, colours[1],
            0, Dropdown::Flag::StayOpen, 5, dropdownWidget->width() - 3);
        Dropdown::SetChecked(gScenarioCategory, true);
    }

    void Arg1Increase()
    {
        switch (gScenarioObjective.Type)
        {
            case OBJECTIVE_PARK_VALUE_BY:
            case OBJECTIVE_MONTHLY_RIDE_INCOME:
            case OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE:
                if (gScenarioObjective.Currency >= 2000000.00_GBP)
                {
                    ContextShowError(STR_CANT_INCREASE_FURTHER, STR_NONE, {});
                }
                else
                {
                    gScenarioObjective.Currency += 1000.00_GBP;
                    Invalidate();
                }
                break;
            case OBJECTIVE_MONTHLY_FOOD_INCOME:
                if (gScenarioObjective.Currency >= 2000000.00_GBP)
                {
                    ContextShowError(STR_CANT_INCREASE_FURTHER, STR_NONE, {});
                }
                else
                {
                    gScenarioObjective.Currency += 100.00_GBP;
                    Invalidate();
                }
                break;
            case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
                if (gScenarioObjective.MinimumLength >= 5000)
                {
                    ContextShowError(STR_CANT_INCREASE_FURTHER, STR_NONE, {});
                }
                else
                {
                    gScenarioObjective.MinimumLength += 100;
                    Invalidate();
                }
                break;
            case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
                if (gScenarioObjective.MinimumExcitement >= FIXED_2DP(9, 90))
                {
                    ContextShowError(STR_CANT_INCREASE_FURTHER, STR_NONE, {});
                }
                else
                {
                    gScenarioObjective.MinimumExcitement += FIXED_2DP(0, 10);
                    Invalidate();
                }
                break;
            default:
                if (gScenarioObjective.NumGuests >= MaxObjectiveGuests)
                {
                    ContextShowError(STR_CANT_INCREASE_FURTHER, STR_NONE, {});
                }
                else
                {
                    gScenarioObjective.NumGuests += 50;
                    Invalidate();
                }
                break;
        }
    }

    void Arg1Decrease()
    {
        switch (gScenarioObjective.Type)
        {
            case OBJECTIVE_PARK_VALUE_BY:
            case OBJECTIVE_MONTHLY_RIDE_INCOME:
            case OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE:
                if (gScenarioObjective.Currency <= 1000.00_GBP)
                {
                    ContextShowError(STR_CANT_REDUCE_FURTHER, STR_NONE, {});
                }
                else
                {
                    gScenarioObjective.Currency -= 1000.00_GBP;
                    Invalidate();
                }
                break;
            case OBJECTIVE_MONTHLY_FOOD_INCOME:
                if (gScenarioObjective.Currency <= 1000.00_GBP)
                {
                    ContextShowError(STR_CANT_REDUCE_FURTHER, STR_NONE, {});
                }
                else
                {
                    gScenarioObjective.Currency -= 100.00_GBP;
                    Invalidate();
                }
                break;
            case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
                if (gScenarioObjective.MinimumLength <= 1000)
                {
                    ContextShowError(STR_CANT_REDUCE_FURTHER, STR_NONE, {});
                }
                else
                {
                    gScenarioObjective.MinimumLength -= 100;
                    Invalidate();
                }
                break;
            case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
                if (gScenarioObjective.MinimumExcitement <= FIXED_2DP(4, 00))
                {
                    ContextShowError(STR_CANT_REDUCE_FURTHER, STR_NONE, {});
                }
                else
                {
                    gScenarioObjective.MinimumExcitement -= FIXED_2DP(0, 10);
                    Invalidate();
                }
                break;
            default:
                if (gScenarioObjective.NumGuests <= 250)
                {
                    ContextShowError(STR_CANT_REDUCE_FURTHER, STR_NONE, {});
                }
                else
                {
                    gScenarioObjective.NumGuests -= 50;
                    Invalidate();
                }
                break;
        }
    }

    void Arg2Increase()
    {
        if (gScenarioObjective.Year >= 25)
        {
            ContextShowError(STR_CANT_INCREASE_FURTHER, STR_NONE, {});
        }
        else
        {
            gScenarioObjective.Year++;
            Invalidate();
        }
    }

    void Arg2Decrease()
    {
        if (gScenarioObjective.Year <= 1)
        {
            ContextShowError(STR_CANT_REDUCE_FURTHER, STR_NONE, {});
        }
        else
        {
            gScenarioObjective.Year--;
            Invalidate();
        }
    }

#pragma region Main

    /**
     *
     *  rct2: 0x006719CA
     */
    void OnMouseUpMain(WidgetIndex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_PARK_NAME:
            {
                auto& park = OpenRCT2::GetContext()->GetGameState()->GetPark();
                WindowTextInputRawOpen(
                    this, WIDX_PARK_NAME, STR_PARK_NAME, STR_ENTER_PARK_NAME, {}, park.Name.c_str(), ParkNameMaxLength);
                break;
            }
            case WIDX_SCENARIO_NAME:
                WindowTextInputRawOpen(
                    this, WIDX_SCENARIO_NAME, STR_SCENARIO_NAME, STR_ENTER_SCENARIO_NAME, {}, gScenarioName.c_str(),
                    ScenarioNameMaxLength);
                break;
            case WIDX_DETAILS:
                WindowTextInputRawOpen(
                    this, WIDX_DETAILS, STR_PARK_SCENARIO_DETAILS, STR_ENTER_SCENARIO_DESCRIPTION, {}, gScenarioDetails.c_str(),
                    ScenarioDetailsNameMaxLength);
                break;
        }
    }

    /**
     *
     *  rct2: 0x00672254
     */
    void OnResizeMain()
    {
        WindowSetResize(*this, 450, 229, 450, 229);
    }

    /**
     *
     *  rct2: 0x00671A0D
     */
    void OnMouseDownMain(WidgetIndex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_OBJECTIVE_DROPDOWN:
                ShowObjectiveDropdown();
                break;
            case WIDX_OBJECTIVE_ARG_1_INCREASE:
                Arg1Increase();
                break;
            case WIDX_OBJECTIVE_ARG_1_DECREASE:
                Arg1Decrease();
                break;
            case WIDX_OBJECTIVE_ARG_2_INCREASE:
                Arg2Increase();
                break;
            case WIDX_OBJECTIVE_ARG_2_DECREASE:
                Arg2Decrease();
                break;
            case WIDX_CATEGORY_DROPDOWN:
                ShowCategoryDropdown();
                break;
        }
    }

    /**
     *
     *  rct2: 0x00671A54
     */
    void OnDropdownMain(WidgetIndex widgetIndex, int32_t dropdownIndex)
    {
        uint8_t newObjectiveType;

        if (dropdownIndex == -1)
            return;

        switch (widgetIndex)
        {
            case WIDX_OBJECTIVE_DROPDOWN:
                // TODO: Don't rely on string ID order
                newObjectiveType = static_cast<uint8_t>(gDropdownItems[dropdownIndex].Args - STR_OBJECTIVE_DROPDOWN_NONE);
                if (gScenarioObjective.Type != newObjectiveType)
                    SetObjective(newObjectiveType);
                break;
            case WIDX_CATEGORY_DROPDOWN:
                if (gScenarioCategory != static_cast<uint8_t>(dropdownIndex))
                {
                    gScenarioCategory = static_cast<SCENARIO_CATEGORY>(dropdownIndex);
                    Invalidate();
                }
                break;
        }
    }

    /**
     *
     *  rct2: 0x006721E7
     */
    void OnUpdateMain()
    {
        uint32_t parkFlags;
        uint8_t objectiveType;

        frame_no++;
        OnPrepareDraw();
        InvalidateWidget(WIDX_TAB_1);

        parkFlags = gParkFlags;
        objectiveType = gScenarioObjective.Type;

        // Check if objective is allowed by money and pay-per-ride settings.
        const bool objectiveAllowedByMoneyUsage = !(parkFlags & PARK_FLAGS_NO_MONEY) || !ObjectiveNeedsMoney(objectiveType);
        // This objective can only work if the player can ask money for rides.
        const bool objectiveAllowedByPaymentSettings = (objectiveType != OBJECTIVE_MONTHLY_RIDE_INCOME)
            || ParkRidePricesUnlocked();
        if (!objectiveAllowedByMoneyUsage || !objectiveAllowedByPaymentSettings)
        {
            // Reset objective
            SetObjective(OBJECTIVE_GUESTS_AND_RATING);
        }
    }

    /**
     *
     *  rct2: 0x00671A73
     */
    void OnTextInputMain(WidgetIndex widgetIndex, std::string_view text)
    {
        if (text.empty())
            return;

        switch (widgetIndex)
        {
            case WIDX_PARK_NAME:
            {
                auto action = ParkSetNameAction(std::string(text));
                GameActions::Execute(&action);

                if (gScenarioName.empty())
                {
                    auto& park = OpenRCT2::GetContext()->GetGameState()->GetPark();
                    gScenarioName = park.Name;
                }
                break;
            }
            case WIDX_SCENARIO_NAME:
                gScenarioName = text;
                Invalidate();
                break;
            case WIDX_DETAILS:
                gScenarioDetails = text;
                Invalidate();
                break;
        }
    }

    /**
     *
     *  rct2: 0x0067161C
     */
    void OnPrepareDrawMain()
    {
        auto widgetsToSet = window_editor_objective_options_widgets[page];
        if (widgets != widgetsToSet)
        {
            widgets = widgetsToSet;
            InitScrollWidgets();
        }

        SetPressedTab();

        switch (gScenarioObjective.Type)
        {
            case OBJECTIVE_GUESTS_BY:
            case OBJECTIVE_PARK_VALUE_BY:
                window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1].type = WindowWidgetType::Spinner;
                window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1_INCREASE].type = WindowWidgetType::Button;
                window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1_DECREASE].type = WindowWidgetType::Button;
                window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2].type = WindowWidgetType::Spinner;
                window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2_INCREASE].type = WindowWidgetType::Button;
                window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2_DECREASE].type = WindowWidgetType::Button;
                break;
            case OBJECTIVE_GUESTS_AND_RATING:
            case OBJECTIVE_MONTHLY_RIDE_INCOME:
            case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
            case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
            case OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE:
            case OBJECTIVE_MONTHLY_FOOD_INCOME:
                window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1].type = WindowWidgetType::Spinner;
                window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1_INCREASE].type = WindowWidgetType::Button;
                window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1_DECREASE].type = WindowWidgetType::Button;
                window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2].type = WindowWidgetType::Empty;
                window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2_INCREASE].type = WindowWidgetType::Empty;
                window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2_DECREASE].type = WindowWidgetType::Empty;
                break;
            default:
                window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1].type = WindowWidgetType::Empty;
                window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1_INCREASE].type = WindowWidgetType::Empty;
                window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1_DECREASE].type = WindowWidgetType::Empty;
                window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2].type = WindowWidgetType::Empty;
                window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2_INCREASE].type = WindowWidgetType::Empty;
                window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2_DECREASE].type = WindowWidgetType::Empty;
                break;
        }

        window_editor_objective_options_main_widgets[WIDX_CLOSE].type = (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
            ? WindowWidgetType::Empty
            : WindowWidgetType::CloseBox;

        AnchorBorderWidgets();
    }

    /**
     *
     *  rct2: 0x0067161C
     */
    void OnDrawMain(DrawPixelInfo& dpi)
    {
        int32_t widthToSet;
        StringId stringId;

        DrawWidgets(dpi);
        DrawTabImages(dpi);

        // Objective label
        auto screenCoords = windowPos + ScreenCoordsXY{ 8, widgets[WIDX_OBJECTIVE].top };
        DrawTextBasic(dpi, screenCoords, STR_OBJECTIVE_WINDOW);

        // Objective value
        screenCoords = windowPos + ScreenCoordsXY{ widgets[WIDX_OBJECTIVE].left + 1, widgets[WIDX_OBJECTIVE].top };
        auto ft = Formatter();
        ft.Add<StringId>(ObjectiveDropdownOptionNames[gScenarioObjective.Type]);
        DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);

        if (widgets[WIDX_OBJECTIVE_ARG_1].type != WindowWidgetType::Empty)
        {
            // Objective argument 1 label
            screenCoords = windowPos + ScreenCoordsXY{ 28, widgets[WIDX_OBJECTIVE_ARG_1].top };
            switch (gScenarioObjective.Type)
            {
                case OBJECTIVE_GUESTS_BY:
                case OBJECTIVE_GUESTS_AND_RATING:
                    stringId = STR_WINDOW_OBJECTIVE_GUEST_COUNT;
                    break;
                case OBJECTIVE_PARK_VALUE_BY:
                case OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE:
                    stringId = STR_WINDOW_OBJECTIVE_PARK_VALUE;
                    break;
                case OBJECTIVE_MONTHLY_RIDE_INCOME:
                    stringId = STR_WINDOW_OBJECTIVE_MONTHLY_INCOME;
                    break;
                case OBJECTIVE_MONTHLY_FOOD_INCOME:
                    stringId = STR_WINDOW_OBJECTIVE_MONTHLY_PROFIT;
                    break;
                case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
                    stringId = STR_WINDOW_OBJECTIVE_MINIMUM_LENGTH;
                    break;
                default:
                    stringId = STR_WINDOW_OBJECTIVE_EXCITEMENT_RATING;
                    break;
            }
            DrawTextBasic(dpi, screenCoords, stringId);

            // Objective argument 1 value
            screenCoords = windowPos
                + ScreenCoordsXY{ widgets[WIDX_OBJECTIVE_ARG_1].left + 1, widgets[WIDX_OBJECTIVE_ARG_1].top };
            ft = Formatter();
            switch (gScenarioObjective.Type)
            {
                case OBJECTIVE_GUESTS_BY:
                case OBJECTIVE_GUESTS_AND_RATING:
                    stringId = STR_WINDOW_COLOUR_2_COMMA16;
                    ft.Add<uint16_t>(gScenarioObjective.NumGuests);
                    break;
                case OBJECTIVE_PARK_VALUE_BY:
                case OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE:
                case OBJECTIVE_MONTHLY_RIDE_INCOME:
                case OBJECTIVE_MONTHLY_FOOD_INCOME:
                    stringId = STR_CURRENCY_FORMAT_LABEL;
                    ft.Add<money64>(gScenarioObjective.Currency);
                    break;
                case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
                    stringId = STR_WINDOW_COLOUR_2_LENGTH;
                    ft.Add<uint16_t>(gScenarioObjective.MinimumLength);
                    break;
                case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
                    stringId = STR_WINDOW_COLOUR_2_COMMA2DP32;
                    ft.Add<uint16_t>(gScenarioObjective.MinimumExcitement);
                    break;
                default:
                    stringId = STR_WINDOW_COLOUR_2_COMMA2DP32;
                    ft.Add<money64>(gScenarioObjective.Currency);
                    break;
            }
            DrawTextBasic(dpi, screenCoords, stringId, ft, COLOUR_BLACK);
        }

        if (widgets[WIDX_OBJECTIVE_ARG_2].type != WindowWidgetType::Empty)
        {
            // Objective argument 2 label
            screenCoords = windowPos + ScreenCoordsXY{ 28, widgets[WIDX_OBJECTIVE_ARG_2].top };
            DrawTextBasic(dpi, screenCoords, STR_WINDOW_OBJECTIVE_DATE);

            // Objective argument 2 value
            screenCoords = windowPos
                + ScreenCoordsXY{ widgets[WIDX_OBJECTIVE_ARG_2].left + 1, widgets[WIDX_OBJECTIVE_ARG_2].top };
            ft = Formatter();
            ft.Add<uint16_t>((gScenarioObjective.Year * MONTH_COUNT) - 1);
            DrawTextBasic(dpi, screenCoords, STR_WINDOW_OBJECTIVE_VALUE_DATE, ft);
        }

        // Park name
        screenCoords = windowPos + ScreenCoordsXY{ 8, widgets[WIDX_PARK_NAME].top };
        widthToSet = widgets[WIDX_PARK_NAME].left - 16;

        {
            auto& park = OpenRCT2::GetContext()->GetGameState()->GetPark();
            auto parkName = park.Name.c_str();

            ft = Formatter();
            ft.Add<StringId>(STR_STRING);
            ft.Add<const char*>(parkName);
            DrawTextEllipsised(dpi, screenCoords, widthToSet, STR_WINDOW_PARK_NAME, ft);
        }

        // Scenario name
        screenCoords = windowPos + ScreenCoordsXY{ 8, widgets[WIDX_SCENARIO_NAME].top };
        widthToSet = widgets[WIDX_SCENARIO_NAME].left - 16;

        ft = Formatter();
        ft.Add<StringId>(STR_STRING);
        ft.Add<const char*>(gScenarioName.c_str());
        DrawTextEllipsised(dpi, screenCoords, widthToSet, STR_WINDOW_SCENARIO_NAME, ft);

        // Scenario details label
        screenCoords = windowPos + ScreenCoordsXY{ 8, widgets[WIDX_DETAILS].top };
        DrawTextBasic(dpi, screenCoords, STR_WINDOW_PARK_DETAILS);

        // Scenario details value
        screenCoords = windowPos + ScreenCoordsXY{ 16, widgets[WIDX_DETAILS].top + 10 };
        widthToSet = widgets[WIDX_DETAILS].left - 4;

        ft = Formatter();
        ft.Add<StringId>(STR_STRING);
        ft.Add<const char*>(gScenarioDetails.c_str());
        DrawTextWrapped(dpi, screenCoords, widthToSet, STR_BLACK_STRING, ft);

        // Scenario category label
        screenCoords = windowPos + ScreenCoordsXY{ 8, widgets[WIDX_CATEGORY].top };
        DrawTextBasic(dpi, screenCoords, STR_WINDOW_SCENARIO_GROUP);

        // Scenario category value
        screenCoords = windowPos + ScreenCoordsXY{ widgets[WIDX_CATEGORY].left + 1, widgets[WIDX_CATEGORY].top };
        ft = Formatter();
        ft.Add<StringId>(ScenarioCategoryStringIds[gScenarioCategory]);
        DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);
    }

#pragma endregion

#pragma region Rides

    /**
     *
     *  rct2: 0x006725A8
     */
    void OnResizeRides()
    {
        WindowSetResize(*this, 380, 224, 380, 224);
    }

    /**
     *
     *  rct2: 0x00672544
     */
    void OnUpdateRides()
    {
        frame_no++;
        OnPrepareDraw();
        OnResize();
        InvalidateWidget(WIDX_TAB_2);

        auto numItems = 0;
        for (auto& currentRide : GetRideManager())
        {
            if (currentRide.IsRide())
            {
                list_item_positions[numItems] = currentRide.id.ToUnderlying();
                numItems++;
            }
        }

        if (no_list_items != numItems)
        {
            no_list_items = numItems;
            Invalidate();
        }
    }

    /**
     *
     *  rct2: 0x006724BF
     */
    ScreenSize OnScrollGetSizeRides(int32_t scrollIndex)
    {
        ScreenSize newSize;
        newSize.height = no_list_items * 12;

        return newSize;
    }

    /**
     *
     *  rct2: 0x006724FC
     */
    void OnScrollMouseDownRides(int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
    {
        auto i = screenCoords.y / 12;
        if (i < 0 || i >= no_list_items)
            return;

        const auto currentRideId = RideId::FromUnderlying(list_item_positions[i]);
        auto currentRide = GetRide(currentRideId);
        if (currentRide != nullptr)
        {
            currentRide->lifecycle_flags ^= RIDE_LIFECYCLE_INDESTRUCTIBLE;
        }
        Invalidate();
    }

    /**
     *
     *  rct2: 0x006724CC
     */
    void OnScrollMouseOverRides(int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
    {
        int32_t i;

        i = screenCoords.y / 12;
        if (i < 0 || i >= no_list_items)
            return;

        if (selected_list_item != i)
        {
            selected_list_item = i;
            Invalidate();
        }
    }

    /**
     *
     *  rct2: 0x006722B5
     */
    void OnPrepareDrawRides()
    {
        Widget* widgetsToSet = window_editor_objective_options_widgets[page];
        if (widgets != widgetsToSet)
        {
            widgets = widgetsToSet;
            InitScrollWidgets();
        }

        SetPressedTab();

        window_editor_objective_options_main_widgets[WIDX_CLOSE].type = (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
            ? WindowWidgetType::Empty
            : WindowWidgetType::CloseBox;

        AnchorBorderWidgets();
    }

    /**
     *
     *  rct2: 0x00672340
     */
    void OnDrawRides(DrawPixelInfo& dpi)
    {
        DrawWidgets(dpi);
        DrawTabImages(dpi);

        DrawTextBasic(
            dpi, windowPos + ScreenCoordsXY{ 6, widgets[WIDX_PAGE_BACKGROUND].top + 3 }, STR_WINDOW_PRESERVATION_ORDER);
    }

    /**
     *
     *  rct2: 0x0067236F
     */
    void OnScrollDrawRides(DrawPixelInfo& dpi, int32_t scrollIndex)
    {
        int32_t colour = ColourMapA[colours[1]].mid_light;
        GfxFillRect(dpi, { { dpi.x, dpi.y }, { dpi.x + dpi.width - 1, dpi.y + dpi.height - 1 } }, colour);

        for (int32_t i = 0; i < no_list_items; i++)
        {
            int32_t y = i * 12;

            if (y + 12 < dpi.y || y >= dpi.y + dpi.height)
                continue;

            // Checkbox
            GfxFillRectInset(dpi, { { 2, y }, { 11, y + 10 } }, colours[1], INSET_RECT_F_E0);

            // Highlighted
            auto stringId = STR_BLACK_STRING;
            if (i == selected_list_item)
            {
                stringId = STR_WINDOW_COLOUR_2_STRINGID;
                GfxFilterRect(dpi, { 0, y, width, y + 11 }, FilterPaletteID::PaletteDarken1);
            }

            // Checkbox mark
            const auto currentRideId = RideId::FromUnderlying(list_item_positions[i]);
            auto currentRide = GetRide(currentRideId);
            if (currentRide != nullptr)
            {
                if (currentRide->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE)
                {
                    auto darkness = stringId == STR_WINDOW_COLOUR_2_STRINGID ? TextDarkness::ExtraDark : TextDarkness::Dark;
                    GfxDrawString(
                        dpi, { 2, y }, static_cast<const char*>(CheckBoxMarkString),
                        { static_cast<colour_t>(colours[1] & 0x7F), FontStyle::Medium, darkness });
                }

                // Ride name

                Formatter ft;
                currentRide->FormatNameTo(ft);
                DrawTextBasic(dpi, { 15, y }, stringId, ft);
            }
        }
    }

#pragma endregion
};

/**
 *
 *  rct2: 0x0067137D
 */
WindowBase* WindowEditorObjectiveOptionsOpen()
{
    auto window = WindowBringToFrontByClass(WindowClass::EditorObjectiveOptions);
    if (window != nullptr)
        return window;

    window = WindowCreate<EditorObjectiveOptionsWindow>(
        WindowClass::EditorObjectiveOptions, 450, 225, WF_10 | WF_CENTRE_SCREEN);

    return window;
}
