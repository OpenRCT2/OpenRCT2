/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Game.h>
#include <openrct2/actions/ParkMarketingAction.h>
#include <openrct2/core/BitSet.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/ShopItem.h>

using namespace OpenRCT2;

static constexpr const rct_string_id WINDOW_TITLE = STR_NONE;
static constexpr const int32_t WH = 109;
static constexpr const int32_t WW = 350;

constexpr uint16_t SELECTED_ITEM_UNDEFINED = 0xFFFF;

// clang-format off
enum WindowNewCampaignWidgetIdx {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_RIDE_LABEL,
    WIDX_RIDE_DROPDOWN,
    WIDX_RIDE_DROPDOWN_BUTTON,
    WIDX_WEEKS_LABEL,
    WIDX_WEEKS_SPINNER,
    WIDX_WEEKS_INCREASE_BUTTON,
    WIDX_WEEKS_DECREASE_BUTTON,
    WIDX_START_BUTTON
};

static rct_widget window_new_campaign_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget        ({ 14, 24}, {126, 12}, WindowWidgetType::Label,    WindowColour::Primary, STR_EMPTY                                  ), // ride label
    MakeWidget        ({100, 24}, {242, 12}, WindowWidgetType::DropdownMenu, WindowColour::Primary, STR_EMPTY                                  ), // ride dropdown
    MakeWidget        ({330, 25}, { 11, 10}, WindowWidgetType::Button,   WindowColour::Primary, STR_DROPDOWN_GLYPH                         ), // ride dropdown button
    MakeWidget        ({ 14, 41}, {126, 14}, WindowWidgetType::Label,    WindowColour::Primary, STR_LENGTH_OF_TIME                         ), // weeks label
    MakeSpinnerWidgets({120, 41}, {100, 14}, WindowWidgetType::Spinner,  WindowColour::Primary, STR_EMPTY                                  ), // weeks (3 widgets)
    MakeWidget        ({ 14, 89}, {322, 14}, WindowWidgetType::Button,   WindowColour::Primary, STR_MARKETING_START_THIS_MARKETING_CAMPAIGN), // start button
    WIDGETS_END,
};
// clang-format on

class NewCampaignWindow final : public Window
{
private:
    std::vector<RideId> RideList;
    std::vector<ShopItem> ShopItems;

    static bool RideValueCompare(const RideId& a, const RideId& b)
    {
        auto valueA = 0;
        auto rideA = get_ride(a);
        if (rideA != nullptr)
            valueA = rideA->value;

        auto valueB = 0;
        auto rideB = get_ride(b);
        if (rideB != nullptr)
            valueB = rideB->value;

        return (valueB - valueA) < 0;
    }

    static int32_t RideNameCompare(const RideId& a, const RideId& b)
    {
        std::string rideAName = "";
        auto rideA = get_ride(a);
        if (rideA != nullptr)
            rideAName = rideA->GetName();

        std::string rideBName = "";
        auto rideB = get_ride(b);
        if (rideB != nullptr)
            rideBName = rideB->GetName();

        return strlogicalcmp(rideAName.c_str(), rideBName.c_str()) < 0;
    }

    /**
     *
     *  rct2: 0x0069E320
     */
    void GetShopItems()
    {
        BitSet<EnumValue(ShopItem::Count)> items = {};
        for (auto& curRide : GetRideManager())
        {
            auto rideEntry = curRide.GetRideEntry();
            if (rideEntry != nullptr)
            {
                for (const auto itemType : rideEntry->shop_item)
                {
                    if (itemType != ShopItem::None && GetShopItemDescriptor(itemType).IsFoodOrDrink())
                    {
                        items[EnumValue(itemType)] = true;
                    }
                }
            }
        }

        ShopItems.clear();
        for (auto i = 0; i < EnumValue(ShopItem::Count); i++)
        {
            if (items[i])
            {
                ShopItems.push_back(ShopItem(i));
            }
        }
    }

public:
    void RefreshRides()
    {
        // Get all applicable rides
        RideList.clear();
        for (const auto& curRide : GetRideManager())
        {
            if (curRide.status == RideStatus::Open)
            {
                if (!curRide.GetRideTypeDescriptor().HasFlag(
                        RIDE_TYPE_FLAG_IS_SHOP | RIDE_TYPE_FLAG_SELLS_FOOD | RIDE_TYPE_FLAG_SELLS_DRINKS
                        | RIDE_TYPE_FLAG_IS_TOILET))
                {
                    RideList.push_back(curRide.id);
                }
            }
        }

        if (RideList.size() > Dropdown::ItemsMaxSize)
        {
            std::sort(RideList.begin(), RideList.end(), RideValueCompare);
            RideList.resize(Dropdown::ItemsMaxSize);
        }

        // Sort rides by name
        std::sort(RideList.begin(), RideList.end(), RideNameCompare);
    }

    void OnOpen() override
    {
        widgets = window_new_campaign_widgets;
        hold_down_widgets = (1ULL << WIDX_WEEKS_INCREASE_BUTTON) | (1ULL << WIDX_WEEKS_DECREASE_BUTTON);
        WindowInitScrollWidgets(*this);
    }

    void SetCampaign(int16_t campaignType)
    {
        widgets[WIDX_TITLE].text = MarketingCampaignNames[campaignType][0];

        // Campaign type
        campaign.campaign_type = campaignType;

        // Number of weeks
        campaign.no_weeks = 2;

        // Currently selected ride
        campaign.RideId = RideId::GetNull();

        RefreshRides();
    }

    void OnMouseDown(rct_widgetindex widgetIndex) override
    {
        rct_widget* widget = &widgets[widgetIndex];
        rct_widget* dropdownWidget;

        switch (widgetIndex)
        {
            case WIDX_RIDE_DROPDOWN_BUTTON:
                dropdownWidget = widget - 1;

                if (campaign.campaign_type == ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE)
                {
                    GetShopItems();
                    if (!ShopItems.empty())
                    {
                        int32_t numItems = 0;
                        int32_t maxSize = std::min(Dropdown::ItemsMaxSize, static_cast<int32_t>(ShopItems.size()));
                        for (int32_t i = 0; i < maxSize; i++)
                        {
                            gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
                            gDropdownItems[i].Args = GetShopItemDescriptor(ShopItems[i]).Naming.Plural;
                            numItems++;
                        }

                        WindowDropdownShowTextCustomWidth(
                            { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top },
                            dropdownWidget->height() + 1, colours[1], 0, Dropdown::Flag::StayOpen, numItems,
                            dropdownWidget->width() - 3);
                    }
                }
                else
                {
                    int32_t numItems = 0;
                    for (auto rideIndex : RideList)
                    {
                        auto curRide = get_ride(rideIndex);
                        if (curRide != nullptr)
                        {
                            // HACK until dropdown items have longer argument buffers
                            gDropdownItems[numItems].Format = STR_DROPDOWN_MENU_LABEL;
                            Formatter ft(reinterpret_cast<uint8_t*>(&gDropdownItems[numItems].Args));
                            if (curRide->custom_name.empty())
                            {
                                curRide->FormatNameTo(ft);
                            }
                            else
                            {
                                gDropdownItems[numItems].Format = STR_OPTIONS_DROPDOWN_ITEM;
                                ft.Add<const char*>(curRide->custom_name.c_str());
                            }
                            numItems++;
                        }
                    }

                    WindowDropdownShowTextCustomWidth(
                        { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
                        colours[1], 0, Dropdown::Flag::StayOpen, numItems, dropdownWidget->width() - 3);
                }
                break;
                // In RCT2, the maximum was 6 weeks
            case WIDX_WEEKS_INCREASE_BUTTON:
                campaign.no_weeks = std::min(campaign.no_weeks + 1, 12);
                Invalidate();
                break;
            case WIDX_WEEKS_DECREASE_BUTTON:
                campaign.no_weeks = std::max(campaign.no_weeks - 1, 2);
                Invalidate();
                break;
        }
    }

    void OnMouseUp(rct_widgetindex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                break;
            case WIDX_START_BUTTON:
            {
                auto gameAction = ParkMarketingAction(
                    campaign.campaign_type, campaign.RideId.ToUnderlying(), campaign.no_weeks);
                gameAction.SetCallback([](const GameAction* ga, const GameActions::Result* result) {
                    if (result->Error == GameActions::Status::Ok)
                    {
                        window_close_by_class(WC_NEW_CAMPAIGN);
                    }
                });
                GameActions::Execute(&gameAction);
                break;
            }
        }
    }

    void OnDropdown(rct_widgetindex widgetIndex, int32_t dropdownIndex) override
    {
        if (widgetIndex != WIDX_RIDE_DROPDOWN_BUTTON)
            return;

        if (dropdownIndex < 0)
            return;

        if (campaign.campaign_type == ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE)
        {
            if (static_cast<size_t>(dropdownIndex) >= ShopItems.size())
                return;

            campaign.ShopItemId = EnumValue(ShopItems[dropdownIndex]);
        }
        else
        {
            if (static_cast<size_t>(dropdownIndex) >= RideList.size())
                return;

            campaign.RideId = RideList[dropdownIndex];
        }

        Invalidate();
    }

    void OnPrepareDraw() override
    {
        widgets[WIDX_RIDE_LABEL].type = WindowWidgetType::Empty;
        widgets[WIDX_RIDE_DROPDOWN].type = WindowWidgetType::Empty;
        widgets[WIDX_RIDE_DROPDOWN_BUTTON].type = WindowWidgetType::Empty;
        widgets[WIDX_RIDE_DROPDOWN].text = STR_MARKETING_NOT_SELECTED;
        switch (campaign.campaign_type)
        {
            case ADVERTISING_CAMPAIGN_RIDE_FREE:
            case ADVERTISING_CAMPAIGN_RIDE:
                widgets[WIDX_RIDE_LABEL].type = WindowWidgetType::Label;
                widgets[WIDX_RIDE_DROPDOWN].type = WindowWidgetType::DropdownMenu;
                widgets[WIDX_RIDE_DROPDOWN_BUTTON].type = WindowWidgetType::Button;
                widgets[WIDX_RIDE_LABEL].text = STR_MARKETING_RIDE;
                if (campaign.RideId != RideId::GetNull())
                {
                    auto curRide = get_ride(campaign.RideId);
                    if (curRide != nullptr)
                    {
                        widgets[WIDX_RIDE_DROPDOWN].text = STR_STRINGID;

                        auto ft = Formatter::Common();
                        curRide->FormatNameTo(ft);
                    }
                }
                break;
            case ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE:
                widgets[WIDX_RIDE_LABEL].type = WindowWidgetType::Label;
                widgets[WIDX_RIDE_DROPDOWN].type = WindowWidgetType::DropdownMenu;
                widgets[WIDX_RIDE_DROPDOWN_BUTTON].type = WindowWidgetType::Button;
                widgets[WIDX_RIDE_LABEL].text = STR_MARKETING_ITEM;
                if (campaign.ShopItemId != SELECTED_ITEM_UNDEFINED)
                {
                    widgets[WIDX_RIDE_DROPDOWN].text = GetShopItemDescriptor(ShopItem(campaign.ShopItemId)).Naming.Plural;
                }
                break;
        }

        // Set current number of weeks spinner (moved to paint due to required parameter)
        widgets[WIDX_WEEKS_SPINNER].text = STR_NONE;

        // Enable / disable start button based on ride dropdown
        WidgetSetDisabled(*this, WIDX_START_BUTTON, false);
        if (widgets[WIDX_RIDE_DROPDOWN].type == WindowWidgetType::DropdownMenu && campaign.RideId == RideId::GetNull())
            WidgetSetDisabled(*this, WIDX_START_BUTTON, true);
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        ScreenCoordsXY screenCoords{};

        DrawWidgets(dpi);

        // Number of weeks
        rct_widget* spinnerWidget = &widgets[WIDX_WEEKS_SPINNER];
        auto ft = Formatter();
        ft.Add<int16_t>(campaign.no_weeks);
        DrawTextBasic(
            &dpi, windowPos + ScreenCoordsXY{ spinnerWidget->left + 1, spinnerWidget->top },
            campaign.no_weeks == 1 ? STR_MARKETING_1_WEEK : STR_X_WEEKS, ft, { colours[0] });

        screenCoords = windowPos + ScreenCoordsXY{ 14, 60 };

        // Price per week
        ft = Formatter();
        ft.Add<money64>(AdvertisingCampaignPricePerWeek[campaign.campaign_type]);
        DrawTextBasic(&dpi, screenCoords, STR_MARKETING_COST_PER_WEEK, ft);
        screenCoords.y += 13;

        // Total price
        ft = Formatter();
        ft.Add<money64>(AdvertisingCampaignPricePerWeek[campaign.campaign_type] * campaign.no_weeks);
        DrawTextBasic(&dpi, screenCoords, STR_MARKETING_TOTAL_COST, ft);
    }
};

rct_window* WindowNewCampaignOpen(int16_t campaignType)
{
    auto w = static_cast<NewCampaignWindow*>(window_bring_to_front_by_class(WC_NEW_CAMPAIGN));
    if (w != nullptr)
    {
        if (w->campaign.campaign_type == campaignType)
            return w;

        window_close(w);
    }

    w = WindowCreate<NewCampaignWindow>(WC_NEW_CAMPAIGN, WW, WH, 0);
    if (w != nullptr)
    {
        w->SetCampaign(campaignType);
    }
    return w;
}

void WindowCampaignRefreshRides()
{
    auto w = static_cast<NewCampaignWindow*>(window_find_by_class(WC_NEW_CAMPAIGN));
    if (w != nullptr)
    {
        w->RefreshRides();
    }
}
