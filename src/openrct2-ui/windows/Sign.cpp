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
#include <openrct2/actions/LargeSceneryRemoveAction.h>
#include <openrct2/actions/SignSetNameAction.h>
#include <openrct2/actions/SignSetStyleAction.h>
#include <openrct2/actions/WallRemoveAction.h>
#include <openrct2/config/Config.h>
#include <openrct2/object/LargeSceneryEntry.h>
#include <openrct2/object/ObjectEntryManager.h>
#include <openrct2/object/WallSceneryEntry.h>
#include <openrct2/sprites.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Banner.h>
#include <openrct2/world/Scenery.h>
#include <openrct2/world/tile_element/LargeSceneryElement.h>
#include <openrct2/world/tile_element/WallElement.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId WINDOW_TITLE = STR_SIGN;
    static constexpr int32_t WW = 113;
    static constexpr int32_t WH = 96;

    enum WindowSignWidgetIdx
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_VIEWPORT,
        WIDX_SIGN_TEXT,
        WIDX_SIGN_DEMOLISH,
        WIDX_MAIN_COLOUR,
        WIDX_TEXT_COLOUR
    };

    // clang-format off
    // 0x9AEE00
    static constexpr Widget _signWidgets[] = {
        WINDOW_SHIM(WINDOW_TITLE, WW, WH),
        MakeWidget({      3,      17}, {85, 60}, WindowWidgetType::Viewport,  WindowColour::Secondary                                                        ), // Viewport
        MakeWidget({WW - 25,      19}, {24, 24}, WindowWidgetType::FlatBtn,   WindowColour::Secondary, ImageId(SPR_RENAME),   STR_CHANGE_SIGN_TEXT_TIP       ), // change sign button
        MakeWidget({WW - 25,      67}, {24, 24}, WindowWidgetType::FlatBtn,   WindowColour::Secondary, ImageId(SPR_DEMOLISH), STR_DEMOLISH_SIGN_TIP          ), // demolish button
        MakeWidget({      5, WH - 16}, {12, 12}, WindowWidgetType::ColourBtn, WindowColour::Secondary, kWidgetContentEmpty,   STR_SELECT_MAIN_SIGN_COLOUR_TIP), // Main colour
        MakeWidget({     17, WH - 16}, {12, 12}, WindowWidgetType::ColourBtn, WindowColour::Secondary, kWidgetContentEmpty,   STR_SELECT_TEXT_COLOUR_TIP     ), // Text colour
    };
    // clang-format on

    class SignWindow final : public Window
    {
    private:
        bool _isSmall = false;
        ObjectEntryIndex _sceneryEntry = kObjectEntryIndexNull;
        colour_t _mainColour = {};
        colour_t _textColour = {};

        BannerIndex GetBannerIndex() const
        {
            return BannerIndex::FromUnderlying(number);
        }

        void ShowTextInput()
        {
            auto* banner = GetBanner(GetBannerIndex());
            if (banner != nullptr)
            {
                auto bannerText = banner->GetText();
                WindowTextInputRawOpen(
                    this, WIDX_SIGN_TEXT, STR_SIGN_TEXT_TITLE, STR_SIGN_TEXT_PROMPT, {}, bannerText.c_str(), 32);
            }
        }

    public:
        void OnOpen() override
        {
            SetWidgets(_signWidgets);
            WindowInitScrollWidgets(*this);
        }

        /*
         * Initializes the window and sets it's number and if it's small
         * @return true if successfull
         */
        bool Initialize(rct_windownumber windowNumber, const bool isSmall)
        {
            number = windowNumber;
            _isSmall = isSmall;
            auto* banner = GetBanner(GetBannerIndex());
            if (banner == nullptr)
            {
                return false;
            }

            auto signViewPosition = banner->position.ToCoordsXY().ToTileCentre();
            auto* tileElement = BannerGetTileElement(GetBannerIndex());
            if (tileElement == nullptr)
                return false;

            int32_t viewZ = tileElement->GetBaseZ();
            frame_no = viewZ;

            if (_isSmall)
            {
                auto* wallElement = tileElement->AsWall();
                if (wallElement == nullptr)
                {
                    return false;
                }
                _mainColour = wallElement->GetPrimaryColour();
                _textColour = wallElement->GetSecondaryColour();
                _sceneryEntry = wallElement->GetEntryIndex();
            }
            else
            {
                auto* sceneryElement = tileElement->AsLargeScenery();
                if (sceneryElement == nullptr)
                {
                    return false;
                }
                _mainColour = sceneryElement->GetPrimaryColour();
                _textColour = sceneryElement->GetSecondaryColour();
                _sceneryEntry = sceneryElement->GetEntryIndex();
            }

            // Create viewport
            Widget& viewportWidget = widgets[WIDX_VIEWPORT];
            ViewportCreate(
                this, windowPos + ScreenCoordsXY{ viewportWidget.left + 1, viewportWidget.top + 1 }, viewportWidget.width() - 1,
                viewportWidget.height() - 1, Focus(CoordsXYZ{ signViewPosition, viewZ }));

            viewport->flags = Config::Get().general.AlwaysShowGridlines ? VIEWPORT_FLAG_GRIDLINES : VIEWPORT_FLAG_NONE;
            Invalidate();

            return true;
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
                case WIDX_SIGN_DEMOLISH:
                {
                    auto* tileElement = BannerGetTileElement(GetBannerIndex());
                    if (tileElement == nullptr)
                    {
                        Close();
                        return;
                    }
                    auto bannerCoords = banner->position.ToCoordsXY();

                    if (_isSmall)
                    {
                        CoordsXYZD wallLocation = { bannerCoords, tileElement->GetBaseZ(), tileElement->GetDirection() };
                        auto wallRemoveAction = WallRemoveAction(wallLocation);
                        GameActions::Execute(&wallRemoveAction);
                    }
                    else
                    {
                        auto sceneryRemoveAction = LargeSceneryRemoveAction(
                            { bannerCoords, tileElement->GetBaseZ(), tileElement->GetDirection() },
                            tileElement->AsLargeScenery()->GetSequenceIndex());
                        GameActions::Execute(&sceneryRemoveAction);
                    }
                    break;
                }
                case WIDX_SIGN_TEXT:
                    ShowTextInput();
                    break;
            }
        }

        void OnMouseDown(WidgetIndex widgetIndex) override
        {
            Widget* widget = &widgets[widgetIndex];
            switch (widgetIndex)
            {
                case WIDX_MAIN_COLOUR:
                    WindowDropdownShowColour(this, widget, colours[1].withFlag(ColourFlag::translucent, true), _mainColour);
                    break;
                case WIDX_TEXT_COLOUR:
                    WindowDropdownShowColour(this, widget, colours[1].withFlag(ColourFlag::translucent, true), _textColour);
                    break;
            }
        }

        void OnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_MAIN_COLOUR:
                {
                    if (dropdownIndex == -1)
                        return;
                    _mainColour = ColourDropDownIndexToColour(dropdownIndex);
                    auto signSetStyleAction = SignSetStyleAction(GetBannerIndex(), _mainColour, _textColour, !_isSmall);
                    GameActions::Execute(&signSetStyleAction);
                    break;
                }
                case WIDX_TEXT_COLOUR:
                {
                    if (dropdownIndex == -1)
                        return;
                    _textColour = ColourDropDownIndexToColour(dropdownIndex);
                    auto signSetStyleAction = SignSetStyleAction(GetBannerIndex(), _mainColour, _textColour, !_isSmall);
                    GameActions::Execute(&signSetStyleAction);
                    break;
                }
                default:
                    return;
            }

            Invalidate();
        }

        void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            if (widgetIndex == WIDX_SIGN_TEXT && !text.empty())
            {
                auto signSetNameAction = SignSetNameAction(GetBannerIndex(), std::string(text));
                GameActions::Execute(&signSetNameAction);
            }
        }

        void OnPrepareDraw() override
        {
            Widget* main_colour_btn = &widgets[WIDX_MAIN_COLOUR];
            Widget* text_colour_btn = &widgets[WIDX_TEXT_COLOUR];

            if (_isSmall)
            {
                auto* wallEntry = OpenRCT2::ObjectManager::GetObjectEntry<WallSceneryEntry>(_sceneryEntry);

                main_colour_btn->type = WindowWidgetType::Empty;
                text_colour_btn->type = WindowWidgetType::Empty;
                if (wallEntry == nullptr)
                {
                    return;
                }
                if (wallEntry->flags & WALL_SCENERY_HAS_PRIMARY_COLOUR)
                {
                    main_colour_btn->type = WindowWidgetType::ColourBtn;
                }
                if (wallEntry->flags & WALL_SCENERY_HAS_SECONDARY_COLOUR)
                {
                    text_colour_btn->type = WindowWidgetType::ColourBtn;
                }
            }
            else
            {
                auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<LargeSceneryEntry>(_sceneryEntry);

                main_colour_btn->type = WindowWidgetType::Empty;
                text_colour_btn->type = WindowWidgetType::Empty;
                if (sceneryEntry == nullptr)
                {
                    return;
                }
                if (sceneryEntry->flags & LARGE_SCENERY_FLAG_HAS_PRIMARY_COLOUR)
                {
                    main_colour_btn->type = WindowWidgetType::ColourBtn;
                }
                if (sceneryEntry->flags & LARGE_SCENERY_FLAG_HAS_SECONDARY_COLOUR)
                {
                    text_colour_btn->type = WindowWidgetType::ColourBtn;
                }
            }

            main_colour_btn->image = GetColourButtonImage(_mainColour);
            text_colour_btn->image = GetColourButtonImage(_textColour);
        }

        void OnDraw(DrawPixelInfo& dpi) override
        {
            DrawWidgets(dpi);

            if (viewport != nullptr)
            {
                WindowDrawViewport(dpi, *this);
            }
        }

        void OnViewportRotate() override
        {
            RemoveViewport();

            auto banner = GetBanner(GetBannerIndex());
            if (banner == nullptr)
            {
                return;
            }

            auto signViewPos = CoordsXYZ{ banner->position.ToCoordsXY().ToTileCentre(), frame_no };

            // Create viewport
            Widget* viewportWidget = &widgets[WIDX_VIEWPORT];
            ViewportCreate(
                this, windowPos + ScreenCoordsXY{ viewportWidget->left + 1, viewportWidget->top + 1 },
                viewportWidget->width() - 1, viewportWidget->height() - 1, Focus(CoordsXYZ{ signViewPos }));
            if (viewport != nullptr)
                viewport->flags = Config::Get().general.AlwaysShowGridlines ? VIEWPORT_FLAG_GRIDLINES : VIEWPORT_FLAG_NONE;
            Invalidate();
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
    WindowBase* SignOpen(rct_windownumber number)
    {
        auto* windowMgr = GetWindowManager();
        auto* w = static_cast<SignWindow*>(windowMgr->BringToFrontByNumber(WindowClass::Banner, number));

        if (w != nullptr)
            return w;

        w = windowMgr->Create<SignWindow>(WindowClass::Banner, WW, WH, 0);

        if (w == nullptr)
            return nullptr;

        bool result = w->Initialize(number, false);
        if (result != true)
            return nullptr;

        return w;
    }

    /**
     *
     *  rct2: 0x6E5F52
     */
    WindowBase* SignSmallOpen(rct_windownumber number)
    {
        auto* windowMgr = GetWindowManager();
        auto* w = static_cast<SignWindow*>(windowMgr->BringToFrontByNumber(WindowClass::Banner, number));

        if (w != nullptr)
            return w;

        w = windowMgr->Create<SignWindow>(WindowClass::Banner, WW, WH, 0);

        if (w == nullptr)
            return nullptr;

        bool result = w->Initialize(number, true);
        if (result != true)
            return nullptr;

        return w;
    }
} // namespace OpenRCT2::Ui::Windows
