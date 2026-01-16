/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include <openrct2/GameState.h>
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
    static constexpr StringId kWindowTitle = kStringIdNone;
    static constexpr ScreenSize kWindowSize = { 350, 109 };

    constexpr uint16_t kSelectedItemUndefined = 0xFFFF;

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
    static constexpr auto window_new_campaign_widgets = makeWidgets(
        makeWindowShim(kWindowTitle, kWindowSize),
        makeWidget        ({ 14, 24}, {126, 12}, WidgetType::label,        WindowColour::primary, kStringIdEmpty                             ), // ride label
        makeWidget        ({100, 24}, {242, 12}, WidgetType::dropdownMenu, WindowColour::primary, kStringIdEmpty                             ), // ride dropdown
        makeWidget        ({330, 25}, { 11, 10}, WidgetType::button,       WindowColour::primary, STR_DROPDOWN_GLYPH                         ), // ride dropdown button
        makeWidget        ({ 14, 41}, {126, 14}, WidgetType::label,        WindowColour::primary, STR_LENGTH_OF_TIME                         ), // weeks label
        makeSpinnerWidgets({120, 41}, {100, 14}, WidgetType::spinner,      WindowColour::primary, kStringIdEmpty                             ), // weeks (3 widgets)
        makeWidget        ({ 14, 89}, {322, 14}, WidgetType::button,       WindowColour::primary, STR_MARKETING_START_THIS_MARKETING_CAMPAIGN) // start button
    );
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
                rideAName = rideA->getName();

            std::string rideBName = "";
            auto rideB = GetRide(b);
            if (rideB != nullptr)
                rideBName = rideB->getName();

            return String::logicalCmp(rideAName.c_str(), rideBName.c_str()) < 0;
        }

        /**
         *
         *  rct2: 0x0069E320
         */
        void GetShopItems()
        {
            BitSet<EnumValue(ShopItem::count)> items = {};
            const auto& gameState = getGameState();
            for (auto& curRide : RideManager(gameState))
            {
                auto rideEntry = curRide.getRideEntry();
                if (rideEntry != nullptr)
                {
                    for (const auto itemType : rideEntry->shop_item)
                    {
                        if (itemType != ShopItem::none && GetShopItemDescriptor(itemType).IsFoodOrDrink())
                        {
                            items[EnumValue(itemType)] = true;
                        }
                    }
                }
            }

            ShopItems.clear();
            for (auto i = 0; i < EnumValue(ShopItem::count); i++)
            {
                if (items[i])
                {
                    ShopItems.push_back(ShopItem(i));
                }
            }
        }

    public:
        void refreshRides()
        {
            // Get all applicable rides
            RideList.clear();
            const auto& gameState = getGameState();
            for (const auto& curRide : RideManager(gameState))
            {
                if (curRide.status == RideStatus::open)
                {
                    const auto& rtd = curRide.getRideTypeDescriptor();
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

        void onOpen() override
        {
            setWidgets(window_new_campaign_widgets);
            holdDownWidgets = (1uLL << WIDX_WEEKS_INCREASE_BUTTON) | (1uLL << WIDX_WEEKS_DECREASE_BUTTON);
            WindowInitScrollWidgets(*this);
        }

        void setCampaign(int16_t campaignType)
        {
            widgets[WIDX_TITLE].text = kMarketingCampaignNames[campaignType][0];

            // Campaign type
            Campaign.campaign_type = campaignType;

            // Number of weeks
            Campaign.no_weeks = 2;

            // Currently selected ride
            Campaign.RideId = RideId::GetNull();

            refreshRides();
        }

        void onMouseDown(WidgetIndex widgetIndex) override
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
                                gDropdown.items[i] = Dropdown::MenuLabel(GetShopItemDescriptor(ShopItems[i]).Naming.Plural);
                                numItems++;
                            }

                            WindowDropdownShowTextCustomWidth(
                                { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top },
                                dropdownWidget->height(), colours[1], 0, Dropdown::Flag::StayOpen, numItems,
                                dropdownWidget->width() - 4);
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
                                auto name = curRide->getName();
                                gDropdown.items[numItems] = Dropdown::MenuLabel(name);

                                numItems++;
                            }
                        }

                        WindowDropdownShowTextCustomWidth(
                            { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height(),
                            colours[1], 0, Dropdown::Flag::StayOpen, numItems, dropdownWidget->width() - 4);
                    }
                    break;
                    // In RCT2, the maximum was 6 weeks
                case WIDX_WEEKS_INCREASE_BUTTON:
                    Campaign.no_weeks = std::min(Campaign.no_weeks + 1, 12);
                    invalidate();
                    break;
                case WIDX_WEEKS_DECREASE_BUTTON:
                    Campaign.no_weeks = std::max(Campaign.no_weeks - 1, 2);
                    invalidate();
                    break;
            }
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    break;
                case WIDX_START_BUTTON:
                {
                    auto gameAction = GameActions::ParkMarketingAction(
                        Campaign.campaign_type, Campaign.RideId.ToUnderlying(), Campaign.no_weeks);
                    gameAction.SetCallback([](const GameActions::GameAction* ga, const GameActions::Result* result) {
                        if (result->error == GameActions::Status::ok)
                        {
                            auto* windowMgr = GetWindowManager();
                            windowMgr->CloseByClass(WindowClass::newCampaign);
                        }
                    });
                    GameActions::Execute(&gameAction, getGameState());
                    break;
                }
            }
        }

        void onDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex) override
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

            invalidate();
        }

        void onPrepareDraw() override
        {
            widgets[WIDX_RIDE_LABEL].type = WidgetType::empty;
            widgets[WIDX_RIDE_DROPDOWN].type = WidgetType::empty;
            widgets[WIDX_RIDE_DROPDOWN_BUTTON].type = WidgetType::empty;
            widgets[WIDX_RIDE_DROPDOWN].text = STR_MARKETING_NOT_SELECTED;
            switch (Campaign.campaign_type)
            {
                case ADVERTISING_CAMPAIGN_RIDE_FREE:
                case ADVERTISING_CAMPAIGN_RIDE:
                    widgets[WIDX_RIDE_LABEL].type = WidgetType::label;
                    widgets[WIDX_RIDE_DROPDOWN].type = WidgetType::dropdownMenu;
                    widgets[WIDX_RIDE_DROPDOWN_BUTTON].type = WidgetType::button;
                    widgets[WIDX_RIDE_LABEL].text = STR_MARKETING_RIDE;
                    if (Campaign.RideId != RideId::GetNull())
                    {
                        auto curRide = GetRide(Campaign.RideId);
                        if (curRide != nullptr)
                        {
                            widgets[WIDX_RIDE_DROPDOWN].text = STR_STRINGID;

                            auto ft = Formatter::Common();
                            curRide->formatNameTo(ft);
                        }
                    }
                    break;
                case ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE:
                    widgets[WIDX_RIDE_LABEL].type = WidgetType::label;
                    widgets[WIDX_RIDE_DROPDOWN].type = WidgetType::dropdownMenu;
                    widgets[WIDX_RIDE_DROPDOWN_BUTTON].type = WidgetType::button;
                    widgets[WIDX_RIDE_LABEL].text = STR_MARKETING_ITEM;
                    if (Campaign.ShopItemId != kSelectedItemUndefined)
                    {
                        widgets[WIDX_RIDE_DROPDOWN].text = GetShopItemDescriptor(ShopItem(Campaign.ShopItemId)).Naming.Plural;
                    }
                    break;
            }

            // Set current number of weeks spinner (moved to paint due to required parameter)
            widgets[WIDX_WEEKS_SPINNER].text = kStringIdNone;

            // Enable / disable start button based on ride dropdown
            widgetSetDisabled(*this, WIDX_START_BUTTON, false);
            if (widgets[WIDX_RIDE_DROPDOWN].type == WidgetType::dropdownMenu && Campaign.RideId == RideId::GetNull())
                widgetSetDisabled(*this, WIDX_START_BUTTON, true);
        }

        void onDraw(Drawing::RenderTarget& rt) override
        {
            ScreenCoordsXY screenCoords{};

            drawWidgets(rt);

            // Number of weeks
            Widget* spinnerWidget = &widgets[WIDX_WEEKS_SPINNER];
            auto ft = Formatter();
            ft.Add<int16_t>(Campaign.no_weeks);
            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ spinnerWidget->left + 1, spinnerWidget->top },
                Campaign.no_weeks == 1 ? STR_MARKETING_1_WEEK : STR_X_WEEKS, ft, { colours[0] });

            screenCoords = windowPos + ScreenCoordsXY{ 14, spinnerWidget->bottom + 6 };

            // Price per week
            ft = Formatter();
            ft.Add<money64>(AdvertisingCampaignPricePerWeek[Campaign.campaign_type]);
            DrawTextBasic(rt, screenCoords, STR_MARKETING_COST_PER_WEEK, ft);
            screenCoords.y += 13;

            // Total price
            ft = Formatter();
            ft.Add<money64>(AdvertisingCampaignPricePerWeek[Campaign.campaign_type] * Campaign.no_weeks);
            DrawTextBasic(rt, screenCoords, STR_MARKETING_TOTAL_COST, ft);
        }

        int16_t getCampaignType() const
        {
            return Campaign.campaign_type;
        }
    };

    WindowBase* NewCampaignOpen(int16_t campaignType)
    {
        auto* windowMgr = GetWindowManager();
        auto* w = static_cast<NewCampaignWindow*>(windowMgr->BringToFrontByClass(WindowClass::newCampaign));
        if (w != nullptr)
        {
            if (w->getCampaignType() == campaignType)
                return w;

            w->close();
        }

        w = windowMgr->Create<NewCampaignWindow>(WindowClass::newCampaign, kWindowSize, {});
        if (w != nullptr)
        {
            w->setCampaign(campaignType);
        }
        return w;
    }

    void WindowCampaignRefreshRides()
    {
        auto* windowMgr = GetWindowManager();
        auto w = static_cast<NewCampaignWindow*>(windowMgr->FindByClass(WindowClass::newCampaign));
        if (w != nullptr)
        {
            w->refreshRides();
        }
    }
} // namespace OpenRCT2::Ui::Windows
