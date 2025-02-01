/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../UiStringIds.h"

#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Game.h>
#include <openrct2/actions/BannerRemoveAction.h>
#include <openrct2/actions/BannerSetNameAction.h>
#include <openrct2/actions/BannerSetStyleAction.h>
#include <openrct2/config/Config.h>
#include <openrct2/object/BannerSceneryEntry.h>
#include <openrct2/object/ObjectEntryManager.h>
#include <openrct2/sprites.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Banner.h>
#include <openrct2/world/Scenery.h>
#include <openrct2/world/tile_element/BannerElement.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr int32_t WW = 113;
    static constexpr int32_t WH = 96;
    static constexpr StringId WINDOW_TITLE = STR_BANNER_WINDOW_TITLE;

    enum WindowBannerWidgetIdx
    {
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

    // clang-format off
    static constexpr StringId BannerColouredTextFormats[] = {
        STR_TEXT_COLOUR_BLACK,
        STR_TEXT_COLOUR_GREY,
        STR_TEXT_COLOUR_WHITE,
        STR_TEXT_COLOUR_RED,
        STR_TEXT_COLOUR_GREEN,
        STR_TEXT_COLOUR_YELLOW,
        STR_TEXT_COLOUR_TOPAZ,
        STR_TEXT_COLOUR_CELADON,
        STR_TEXT_COLOUR_BABYBLUE,
        STR_TEXT_COLOUR_PALELAVENDER,
        STR_TEXT_COLOUR_PALEGOLD,
        STR_TEXT_COLOUR_LIGHTPINK,
        STR_TEXT_COLOUR_PEARLAQUA,
        STR_TEXT_COLOUR_PALESILVER,
    };

    static constexpr Widget window_banner_widgets[] = {
        WINDOW_SHIM(WINDOW_TITLE, WW, WH),
        MakeWidget({      3,      17}, {85, 60}, WindowWidgetType::Viewport,  WindowColour::Secondary, 0x0FFFFFFFE                                        ), // tab content panel
        MakeWidget({WW - 25,      19}, {24, 24}, WindowWidgetType::FlatBtn,   WindowColour::Secondary, ImageId(SPR_RENAME),         STR_CHANGE_BANNER_TEXT_TIP     ), // change banner button
        MakeWidget({WW - 25,      43}, {24, 24}, WindowWidgetType::FlatBtn,   WindowColour::Secondary, ImageId(SPR_NO_ENTRY),       STR_SET_AS_NO_ENTRY_BANNER_TIP ), // no entry button
        MakeWidget({WW - 25,      67}, {24, 24}, WindowWidgetType::FlatBtn,   WindowColour::Secondary, ImageId(SPR_DEMOLISH),       STR_DEMOLISH_BANNER_TIP        ), // demolish button
        MakeWidget({      5, WH - 16}, {12, 12}, WindowWidgetType::ColourBtn, WindowColour::Secondary, 0xFFFFFFFF,         STR_SELECT_MAIN_SIGN_COLOUR_TIP), // high money
        MakeWidget({     43, WH - 16}, {39, 12}, WindowWidgetType::DropdownMenu,  WindowColour::Secondary                                                     ), // high money
        MakeWidget({     70, WH - 15}, {11, 10}, WindowWidgetType::Button,    WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_SELECT_TEXT_COLOUR_TIP     ), // high money
    };
    // clang-format on

    class BannerWindow final : public Window
    {
    private:
        CoordsXYZ _bannerViewPos;

        void CreateViewport()
        {
            const auto& viewportWidget = widgets[WIDX_VIEWPORT];
            ViewportCreate(
                this, windowPos + ScreenCoordsXY{ viewportWidget.left + 1, viewportWidget.top + 1 },
                (viewportWidget.width()) - 1, (viewportWidget.height()) - 1, Focus(_bannerViewPos));

            if (viewport != nullptr)
                viewport->flags = Config::Get().general.AlwaysShowGridlines ? VIEWPORT_FLAG_GRIDLINES : VIEWPORT_FLAG_NONE;
            Invalidate();
        }

        BannerIndex GetBannerIndex() const
        {
            return BannerIndex::FromUnderlying(number);
        }

        BannerElement* GetBannerElement()
        {
            auto* banner = GetBanner(GetBannerIndex());
            if (banner == nullptr)
            {
                return nullptr;
            }

            TileElement* tileElement = MapGetFirstElementAt(banner->position);
            if (tileElement == nullptr)
            {
                return nullptr;
            }

            do
            {
                auto* bannerElement = tileElement->AsBanner();
                if (bannerElement == nullptr)
                {
                    continue;
                }
                if (bannerElement->GetIndex() == GetBannerIndex())
                {
                    return bannerElement;
                }
            } while (!(tileElement++)->IsLastForTile());

            return nullptr;
        }

    public:
        void OnOpen() override
        {
            SetWidgets(window_banner_widgets);
            WindowInitScrollWidgets(*this);
        }

        void Initialise(rct_windownumber _number)
        {
            number = _number;
            auto* banner = GetBanner(BannerIndex::FromUnderlying(number));

            auto* bannerElement = GetBannerElement();
            if (bannerElement == nullptr)
                return;

            _bannerViewPos = CoordsXYZ{ banner->position.ToCoordsXY().ToTileCentre(), bannerElement->GetBaseZ() };
            CreateViewport();
        }

        void OnMouseDown(WidgetIndex widgetIndex) override
        {
            Widget* widget = &widgets[widgetIndex];
            auto* banner = GetBanner(GetBannerIndex());
            if (banner == nullptr)
            {
                Close();
                return;
            }
            switch (widgetIndex)
            {
                case WIDX_MAIN_COLOUR:
                    WindowDropdownShowColour(this, widget, colours[1].withFlag(ColourFlag::translucent, true), banner->colour);
                    break;
                case WIDX_TEXT_COLOUR_DROPDOWN_BUTTON:

                    for (int32_t i = 0; i < 13; ++i)
                    {
                        gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
                        gDropdownItems[i].Args = BannerColouredTextFormats[i + 1];
                    }

                    // Switch to the dropdown box widget.
                    widget--;

                    WindowDropdownShowTextCustomWidth(
                        { widget->left + windowPos.x, widget->top + windowPos.y }, widget->height() + 1, colours[1], 0,
                        Dropdown::Flag::StayOpen, 13, widget->width() - 3);

                    Dropdown::SetChecked(banner->text_colour - 1, true);
                    break;
            }
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            auto* banner = GetBanner(GetBannerIndex());
            if (banner == nullptr)
            {
                Close();
                return;
            }
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    Close();
                    break;
                case WIDX_BANNER_DEMOLISH:
                {
                    auto* bannerElement = GetBannerElement();
                    if (bannerElement == nullptr)
                        break;

                    auto bannerRemoveAction = BannerRemoveAction(
                        { banner->position.ToCoordsXY(), bannerElement->GetBaseZ(), bannerElement->GetPosition() });
                    GameActions::Execute(&bannerRemoveAction);
                    break;
                }
                case WIDX_BANNER_TEXT:
                    WindowTextInputRawOpen(
                        this, WIDX_BANNER_TEXT, STR_BANNER_TEXT, STR_ENTER_BANNER_TEXT, {}, banner->GetText().c_str(), 32);
                    break;
                case WIDX_BANNER_NO_ENTRY:
                {
                    TextinputCancel();
                    auto bannerSetStyle = BannerSetStyleAction(
                        BannerSetStyleType::NoEntry, GetBannerIndex(), banner->flags ^ BANNER_FLAG_NO_ENTRY);
                    GameActions::Execute(&bannerSetStyle);
                    break;
                }
            }
        }

        void OnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_MAIN_COLOUR:
                {
                    if (dropdownIndex == -1)
                        break;

                    auto bannerSetStyle = BannerSetStyleAction(
                        BannerSetStyleType::PrimaryColour, GetBannerIndex(), ColourDropDownIndexToColour(dropdownIndex));
                    GameActions::Execute(&bannerSetStyle);
                    break;
                }
                case WIDX_TEXT_COLOUR_DROPDOWN_BUTTON:
                {
                    if (dropdownIndex == -1)
                        break;
                    auto bannerSetStyle = BannerSetStyleAction(
                        BannerSetStyleType::TextColour, GetBannerIndex(), dropdownIndex + 1);
                    GameActions::Execute(&bannerSetStyle);
                    break;
                }
            }
        }

        void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            if (widgetIndex == WIDX_BANNER_TEXT)
            {
                auto bannerSetNameAction = BannerSetNameAction(GetBannerIndex(), std::string(text));
                GameActions::Execute(&bannerSetNameAction);
            }
        }

        void OnViewportRotate() override
        {
            RemoveViewport();
            CreateViewport();
        }

        void OnDraw(DrawPixelInfo& dpi) override
        {
            DrawWidgets(dpi);

            if (viewport != nullptr)
            {
                WindowDrawViewport(dpi, *this);
            }
        }

        void OnPrepareDraw() override
        {
            auto* banner = GetBanner(GetBannerIndex());
            if (banner == nullptr)
            {
                return;
            }
            Widget& colourBtn = widgets[WIDX_MAIN_COLOUR];
            colourBtn.type = WindowWidgetType::Empty;

            auto* bannerEntry = OpenRCT2::ObjectManager::GetObjectEntry<BannerSceneryEntry>(banner->type);
            if (bannerEntry != nullptr && (bannerEntry->flags & BANNER_ENTRY_FLAG_HAS_PRIMARY_COLOUR))
            {
                colourBtn.type = WindowWidgetType::ColourBtn;
            }
            pressed_widgets &= ~(1uLL << WIDX_BANNER_NO_ENTRY);
            disabled_widgets &= ~(
                (1uLL << WIDX_BANNER_TEXT) | (1uLL << WIDX_TEXT_COLOUR_DROPDOWN) | (1uLL << WIDX_TEXT_COLOUR_DROPDOWN_BUTTON));
            if (banner->flags & BANNER_FLAG_NO_ENTRY)
            {
                pressed_widgets |= (1uLL << WIDX_BANNER_NO_ENTRY);
                disabled_widgets |= (1uLL << WIDX_BANNER_TEXT) | (1uLL << WIDX_TEXT_COLOUR_DROPDOWN)
                    | (1uLL << WIDX_TEXT_COLOUR_DROPDOWN_BUTTON);
            }
            colourBtn.image = GetColourButtonImage(banner->colour);
            Widget& dropDownWidget = widgets[WIDX_TEXT_COLOUR_DROPDOWN];
            dropDownWidget.text = BannerColouredTextFormats[banner->text_colour];
        }

        void OnResize() override
        {
            ResizeFrame();
        }
    };

    /**
     *
     *  rct2: 0x006BA305
     */
    WindowBase* BannerOpen(rct_windownumber number)
    {
        auto* windowMgr = GetWindowManager();
        auto w = static_cast<BannerWindow*>(windowMgr->BringToFrontByNumber(WindowClass::Banner, number));

        if (w != nullptr)
            return w;

        w = windowMgr->Create<BannerWindow>(WindowClass::Banner, WW, WH, 0);

        if (w != nullptr)
            w->Initialise(number);

        return w;
    }
} // namespace OpenRCT2::Ui::Windows
