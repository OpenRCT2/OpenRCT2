/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Game.h>
#include <openrct2/actions/ParkMarketingAction.h>
#include <openrct2/core/BitSet.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/management/Marketing.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/RideManager.hpp>
#include <openrct2/ride/ShopItem.h>
#include <openrct2/ui/WindowManager.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId WINDOW_TITLE = kStringIdNone;
    static constexpr int32_t WH = 109;
    static constexpr int32_t WW = 350;

    constexpr uint16_t SELECTED_ITEM_UNDEFINED = 0xFFFF;

    enum WindowNewCampaignWidgetIdx
    {
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

    // clang-format off
    static constexpr Widget window_new_campaign_widgets[] = {
        WINDOW_SHIM(WINDOW_TITLE, WW, WH),
        MakeWidget        ({ 14, 24}, {126, 12}, WindowWidgetType::Label,    WindowColour::Primary, kStringIdEmpty                                  ), // ride label
        MakeWidget        ({100, 24}, {242, 12}, WindowWidgetType::DropdownMenu, WindowColour::Primary, kStringIdEmpty                                  ), // ride dropdown
        MakeWidget        ({330, 25}, { 11, 10}, WindowWidgetType::Button,   WindowColour::Primary, STR_DROPDOWN_GLYPH                         ), // ride dropdown button
        MakeWidget        ({ 14, 41}, {126, 14}, WindowWidgetType::Label,    WindowColour::Primary, STR_LENGTH_OF_TIME                         ), // weeks label
        MakeSpinnerWidgets({120, 41}, {100, 14}, WindowWidgetType::Spinner,  WindowColour::Primary, kStringIdEmpty                                  ), // weeks (3 widgets)
        MakeWidget        ({ 14, 89}, {322, 14}, WindowWidgetType::Button,   WindowColour::Primary, STR_MARKETING_START_THIS_MARKETING_CAMPAIGN), // start button
    };
    // clang-format on

    class NewCampaignWindow final : public Window
    {
    private:
        std::vector<RideId> RideList;
        std::vector<ShopItem> ShopItems;
        struct CampaignVariables
        {
            int16_t campaign_type;
            int16_t no_weeks; // 0x482
            union
            {
                ::RideId RideId;
                ObjectEntryIndex ShopItemId;
            };
        } Campaign;

        static bool RideValueCompare(const RideId& a, const RideId& b)
        {
            auto valueA = 0;
            auto rideA = GetRide(a);
            if (rideA != nullptr)
                valueA = rideA->value;

            auto valueB = 0;
            auto rideB = GetRide(b);
            if (rideB != nullptr)
                valueB = rideB->value;

            return (valueB - valueA) < 0;
        }

        static int32_t RideNameCompare(const RideId& a, const RideId& b)
        {
            std::string rideAName = "";
            auto rideA = GetRide(a);
            if (rideA != nullptr)
                rideAName = rideA->GetName();

            std::string rideBName = "";
            auto rideB = GetRide(b);
            if (rideB != nullptr)
                rideBName = rideB->GetName();

            return String::logicalCmp(rideAName.c_str(), rideBName.c_str()) < 0;
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
                    const auto& rtd = curRide.GetRideTypeDescriptor();
                    if (rtd.HasFlag(RtdFlag::isShopOrFacility))
                        continue;
                    if (rtd.HasFlag(RtdFlag::sellsFood))
                        continue;
                    if (rtd.HasFlag(RtdFlag::sellsDrinks))
                        continue;
                    if (rtd.specialType == RtdSpecialType::toilet)
                        continue;

                    RideList.push_back(curRide.id);
                }
            }

            if (RideList.size() > Dropdown::kItemsMaxSize)
            {
                std::sort(RideList.begin(), RideList.end(), RideValueCompare);
                RideList.resize(Dropdown::kItemsMaxSize);
            }

            // Sort rides by name
            std::sort(RideList.begin(), RideList.end(), RideNameCompare);
        }

        void OnOpen() override
        {
            SetWidgets(window_new_campaign_widgets);
            hold_down_widgets = (1uLL << WIDX_WEEKS_INCREASE_BUTTON) | (1uLL << WIDX_WEEKS_DECREASE_BUTTON);
            WindowInitScrollWidgets(*this);
        }

        void SetCampaign(int16_t campaignType)
        {
            widgets[WIDX_TITLE].text = kMarketingCampaignNames[campaignType][0];

            // Campaign type
            Campaign.campaign_type = campaignType;

            // Number of weeks
            Campaign.no_weeks = 2;

            // Currently selected ride
            Campaign.RideId = RideId::GetNull();

            RefreshRides();
        }

        void OnMouseDown(WidgetIndex widgetIndex) override
        {
            Widget* widget = &widgets[widgetIndex];
            Widget* dropdownWidget;

            switch (widgetIndex)
            {
                case WIDX_RIDE_DROPDOWN_BUTTON:
                    dropdownWidget = widget - 1;

                    if (Campaign.campaign_type == ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE)
                    {
                        GetShopItems();
                        if (!ShopItems.empty())
                        {
                            int32_t numItems = 0;
                            int32_t maxSize = std::min(Dropdown::kItemsMaxSize, static_cast<int32_t>(ShopItems.size()));
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
                            auto curRide = GetRide(rideIndex);
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
                            { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top },
                            dropdownWidget->height() + 1, colours[1], 0, Dropdown::Flag::StayOpen, numItems,
                            dropdownWidget->width() - 3);
                    }
                    break;
                    // In RCT2, the maximum was 6 weeks
                case WIDX_WEEKS_INCREASE_BUTTON:
                    Campaign.no_weeks = std::min(Campaign.no_weeks + 1, 12);
                    Invalidate();
                    break;
                case WIDX_WEEKS_DECREASE_BUTTON:
                    Campaign.no_weeks = std::max(Campaign.no_weeks - 1, 2);
                    Invalidate();
                    break;
            }
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    Close();
                    break;
                case WIDX_START_BUTTON:
                {
                    auto gameAction = ParkMarketingAction(
                        Campaign.campaign_type, Campaign.RideId.ToUnderlying(), Campaign.no_weeks);
                    gameAction.SetCallback([](const GameAction* ga, const GameActions::Result* result) {
                        if (result->Error == GameActions::Status::Ok)
                        {
                            auto* windowMgr = Ui::GetWindowManager();
                            windowMgr->CloseByClass(WindowClass::NewCampaign);
                        }
                    });
                    GameActions::Execute(&gameAction);
                    break;
                }
            }
        }

        void OnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex) override
        {
            if (widgetIndex != WIDX_RIDE_DROPDOWN_BUTTON)
                return;

            if (dropdownIndex < 0)
                return;

            if (Campaign.campaign_type == ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE)
            {
                if (static_cast<size_t>(dropdownIndex) >= ShopItems.size())
                    return;

                Campaign.ShopItemId = EnumValue(ShopItems[dropdownIndex]);
            }
            else
            {
                if (static_cast<size_t>(dropdownIndex) >= RideList.size())
                    return;

                Campaign.RideId = RideList[dropdownIndex];
            }

            Invalidate();
        }

        void OnPrepareDraw() override
        {
            widgets[WIDX_RIDE_LABEL].type = WindowWidgetType::Empty;
            widgets[WIDX_RIDE_DROPDOWN].type = WindowWidgetType::Empty;
            widgets[WIDX_RIDE_DROPDOWN_BUTTON].type = WindowWidgetType::Empty;
            widgets[WIDX_RIDE_DROPDOWN].text = STR_MARKETING_NOT_SELECTED;
            switch (Campaign.campaign_type)
            {
                case ADVERTISING_CAMPAIGN_RIDE_FREE:
                case ADVERTISING_CAMPAIGN_RIDE:
                    widgets[WIDX_RIDE_LABEL].type = WindowWidgetType::Label;
                    widgets[WIDX_RIDE_DROPDOWN].type = WindowWidgetType::DropdownMenu;
                    widgets[WIDX_RIDE_DROPDOWN_BUTTON].type = WindowWidgetType::Button;
                    widgets[WIDX_RIDE_LABEL].text = STR_MARKETING_RIDE;
                    if (Campaign.RideId != RideId::GetNull())
                    {
                        auto curRide = GetRide(Campaign.RideId);
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
                    if (Campaign.ShopItemId != SELECTED_ITEM_UNDEFINED)
                    {
                        widgets[WIDX_RIDE_DROPDOWN].text = GetShopItemDescriptor(ShopItem(Campaign.ShopItemId)).Naming.Plural;
                    }
                    break;
            }

            // Set current number of weeks spinner (moved to paint due to required parameter)
            widgets[WIDX_WEEKS_SPINNER].text = kStringIdNone;

            // Enable / disable start button based on ride dropdown
            WidgetSetDisabled(*this, WIDX_START_BUTTON, false);
            if (widgets[WIDX_RIDE_DROPDOWN].type == WindowWidgetType::DropdownMenu && Campaign.RideId == RideId::GetNull())
                WidgetSetDisabled(*this, WIDX_START_BUTTON, true);
        }

        void OnDraw(DrawPixelInfo& dpi) override
        {
            ScreenCoordsXY screenCoords{};

            DrawWidgets(dpi);

            // Number of weeks
            Widget* spinnerWidget = &widgets[WIDX_WEEKS_SPINNER];
            auto ft = Formatter();
            ft.Add<int16_t>(Campaign.no_weeks);
            DrawTextBasic(
                dpi, windowPos + ScreenCoordsXY{ spinnerWidget->left + 1, spinnerWidget->top },
                Campaign.no_weeks == 1 ? STR_MARKETING_1_WEEK : STR_X_WEEKS, ft, { colours[0] });

            screenCoords = windowPos + ScreenCoordsXY{ 14, 60 };

            // Price per week
            ft = Formatter();
            ft.Add<money64>(AdvertisingCampaignPricePerWeek[Campaign.campaign_type]);
            DrawTextBasic(dpi, screenCoords, STR_MARKETING_COST_PER_WEEK, ft);
            screenCoords.y += 13;

            // Total price
            ft = Formatter();
            ft.Add<money64>(AdvertisingCampaignPricePerWeek[Campaign.campaign_type] * Campaign.no_weeks);
            DrawTextBasic(dpi, screenCoords, STR_MARKETING_TOTAL_COST, ft);
        }

        void OnResize() override
        {
            ResizeFrame();
        }

        int16_t GetCampaignType() const
        {
            return Campaign.campaign_type;
        }
    };

    WindowBase* NewCampaignOpen(int16_t campaignType)
    {
        auto* windowMgr = GetWindowManager();
        auto* w = static_cast<NewCampaignWindow*>(windowMgr->BringToFrontByClass(WindowClass::NewCampaign));
        if (w != nullptr)
        {
            if (w->GetCampaignType() == campaignType)
                return w;

            w->Close();
        }

        w = windowMgr->Create<NewCampaignWindow>(WindowClass::NewCampaign, WW, WH, 0);
        if (w != nullptr)
        {
            w->SetCampaign(campaignType);
        }
        return w;
    }

    void WindowCampaignRefreshRides()
    {
        auto* windowMgr = GetWindowManager();
        auto w = static_cast<NewCampaignWindow*>(windowMgr->FindByClass(WindowClass::NewCampaign));
        if (w != nullptr)
        {
            w->RefreshRides();
        }
    }
} // namespace OpenRCT2::Ui::Windows
