/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../UiStringIds.h"

#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/AssetPack.h>
#include <openrct2/AssetPackManager.h>
#include <openrct2/Context.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Text.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/sprites.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId WINDOW_TITLE = STR_ASSET_PACKS;
    static constexpr int32_t WW = 400;
    static constexpr int32_t WH = 200;

    enum WindowAssetPacksWidgetIdx
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_HIGH_LABEL,
        WIDX_LIST,
        WIDX_LOW_LABEL,
        WIDX_MOVE_UP,
        WIDX_MOVE_DOWN,
        WIDX_APPLY,
    };

    // clang-format off
    static Widget WindowAssetPacksWidgets[] = {
        WINDOW_SHIM(WINDOW_TITLE, WW, WH),
        MakeWidget({ 0, 0 }, { 0,   0 }, WindowWidgetType::LabelCentred,  WindowColour::Secondary, STR_HIGH_PRIORITY),
        MakeWidget({ 0, 0 }, { 0, 147 }, WindowWidgetType::Scroll,  WindowColour::Secondary, SCROLL_VERTICAL),
        MakeWidget({ 0, 0 }, { 0,   0 }, WindowWidgetType::LabelCentred,  WindowColour::Secondary, STR_LOW_PRIORITY),
        MakeWidget({ 0, 0 }, { 0,   0 }, WindowWidgetType::FlatBtn, WindowColour::Secondary, ImageId(SPR_G2_ARROW_UP), STR_INCREASE_PRIOTITY_TIP),
        MakeWidget({ 0, 0 }, { 0,   0 }, WindowWidgetType::FlatBtn, WindowColour::Secondary, ImageId(SPR_G2_ARROW_DOWN), STR_DECREASE_PRIOTITY_TIP),
        MakeWidget({ 0, 0 }, { 0,   0 }, WindowWidgetType::FlatBtn, WindowColour::Secondary, ImageId(SPR_G2_RELOAD), STR_RELOAD_ASSET_PACKS_TIP),
        kWidgetsEnd,
    };
    // clang-format on

    class AssetPacksWindow final : public Window
    {
    private:
        static constexpr int32_t ItemHeight = kScrollableRowHeight + 1;
        static constexpr int32_t ItemCheckBoxSize = ItemHeight - 3;
        std::optional<size_t> _highlightedIndex;
        std::optional<size_t> _selectedIndex;

    public:
        void OnOpen() override
        {
            widgets = WindowAssetPacksWidgets;
            WindowInitScrollWidgets(*this);
        }

        void OnClose() override
        {
            Apply();
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    Close();
                    break;
                case WIDX_MOVE_UP:
                    ReorderSelectedAssetPack(-1);
                    break;
                case WIDX_MOVE_DOWN:
                    ReorderSelectedAssetPack(1);
                    break;
                case WIDX_APPLY:
                    Apply();
                    break;
            }
        }

        ScreenSize OnScrollGetSize(int32_t scrollIndex) override
        {
            ScreenSize result;
            auto assetPackManager = GetContext()->GetAssetPackManager();
            if (assetPackManager != nullptr)
            {
                auto numAssetPacks = assetPackManager->GetCount() + 1; // +1 for the base assets item
                result.height = static_cast<int32_t>(numAssetPacks * ItemHeight);
            }

            if (_highlightedIndex)
            {
                _highlightedIndex = {};
                Invalidate();
            }

            return result;
        }

        void OnScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            auto isCheckBox = false;
            auto index = GetAssetPackIndexFromPosition(screenCoords, isCheckBox);

            // Click on checkbox
            if (index && isCheckBox)
            {
                auto assetPackManager = GetContext()->GetAssetPackManager();
                if (assetPackManager != nullptr)
                {
                    auto assetPack = assetPackManager->GetAssetPack(*index);
                    if (assetPack != nullptr)
                    {
                        assetPack->SetEnabled(!assetPack->IsEnabled());
                        Invalidate();
                    }
                }
            }

            // Select item
            if (_selectedIndex != index)
            {
                _selectedIndex = index;
                Invalidate();
            }
        }

        void OnScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            auto isCheckBox = false;
            auto index = GetAssetPackIndexFromPosition(screenCoords, isCheckBox);
            if (_highlightedIndex != index)
            {
                _highlightedIndex = index;
                Invalidate();
            }
        }

        void OnPrepareDraw() override
        {
            ResizeFrame();

            auto& list = widgets[WIDX_LIST];
            list.left = 6;
            list.top = 20 + 11 + 3;
            list.right = width - 2 - 24 - 1;
            list.bottom = height - 6 - 11 - 3;

            widgets[WIDX_HIGH_LABEL].bottom = list.top - 1;
            widgets[WIDX_HIGH_LABEL].top = widgets[WIDX_HIGH_LABEL].bottom - 11 - 3;
            widgets[WIDX_HIGH_LABEL].left = list.left;
            widgets[WIDX_HIGH_LABEL].right = list.right;
            widgets[WIDX_LOW_LABEL].top = list.bottom + 1 + 3;
            widgets[WIDX_LOW_LABEL].bottom = widgets[WIDX_LOW_LABEL].top + 11 + 3;
            widgets[WIDX_LOW_LABEL].left = list.left;
            widgets[WIDX_LOW_LABEL].right = list.right;

            auto toolstripY = 20;
            auto toolstripRight = width - 2;
            auto toolstripLeft = toolstripRight - 24;
            for (WidgetIndex i = WIDX_MOVE_UP; i <= WIDX_APPLY; i++)
            {
                widgets[i].top = toolstripY;
                widgets[i].bottom = toolstripY + 24;
                widgets[i].left = toolstripLeft;
                widgets[i].right = toolstripRight;
                toolstripY += 24;
            }

            SetWidgetDisabled(WIDX_MOVE_UP, !_selectedIndex || _selectedIndex == 0u);
            SetWidgetDisabled(WIDX_MOVE_DOWN, !_selectedIndex || _selectedIndex >= GetNumAssetPacks() - 1);

            widgets[WIDX_APPLY].bottom = widgets[WIDX_LIST].bottom;
            widgets[WIDX_APPLY].top = widgets[WIDX_APPLY].bottom - 24;
        }

        void OnDraw(DrawPixelInfo& dpi) override
        {
            DrawWidgets(dpi);
        }

        void OnScrollDraw(int32_t scrollIndex, DrawPixelInfo& dpi) override
        {
            auto dpiCoords = ScreenCoordsXY{ dpi.x, dpi.y };
            GfxFillRect(
                dpi, { dpiCoords, dpiCoords + ScreenCoordsXY{ dpi.width - 1, dpi.height - 1 } },
                ColourMapA[colours[1].colour].mid_light);

            auto assetPackManager = GetContext()->GetAssetPackManager();
            if (assetPackManager == nullptr)
                return;

            auto numAssetPacks = assetPackManager->GetCount();
            auto y = 0;
            for (size_t i = 0; i <= numAssetPacks; i++)
            {
                if (y > dpi.y + dpi.height)
                    break;
                if (y + 11 < dpi.y)
                    continue;

                auto isSelected = i == _selectedIndex;
                auto isHighlighted = i == _highlightedIndex;
                if (i == numAssetPacks)
                {
                    auto ft = Formatter();
                    ft.Add<StringId>(STR_BASE_GRAPHICS_MUSIC_SOUND);
                    PaintItem(dpi, y, ft, true, isSelected, isHighlighted);
                }
                else
                {
                    auto assetPack = assetPackManager->GetAssetPack(i);
                    if (assetPack != nullptr)
                    {
                        auto isChecked = assetPack->IsEnabled();
                        auto ft = Formatter();
                        ft.Add<StringId>(STR_STRING);
                        ft.Add<const char*>(assetPack->Name.c_str());
                        PaintItem(dpi, y, ft, isChecked, isSelected, isHighlighted);
                    }
                }
                y += ItemHeight;
            }
        }

    private:
        void PaintItem(DrawPixelInfo& dpi, int32_t y, Formatter& ft, bool isChecked, bool isSelected, bool isHighlighted)
        {
            auto listWidth = widgets[WIDX_LIST].right - widgets[WIDX_LIST].left;
            auto stringId = STR_BLACK_STRING;
            auto fillRectangle = ScreenRect{ { 0, y }, { listWidth, y + ItemHeight - 1 } };
            if (isSelected)
            {
                GfxFillRect(dpi, fillRectangle, ColourMapA[colours[1].colour].mid_dark);
                stringId = STR_WINDOW_COLOUR_2_STRINGID;
            }
            else if (isHighlighted)
            {
                GfxFillRect(dpi, fillRectangle, ColourMapA[colours[1].colour].mid_dark);
            }

            DrawTextEllipsised(dpi, { 16, y + 1 }, listWidth, stringId, ft);

            auto checkboxSize = ItemHeight - 3;
            PaintCheckbox(dpi, { { 2, y + 1 }, { 2 + checkboxSize + 1, y + 1 + checkboxSize } }, isChecked);
        }

        void PaintCheckbox(DrawPixelInfo& dpi, const ScreenRect& rect, bool checked)
        {
            GfxFillRectInset(dpi, rect, colours[1], INSET_RECT_F_E0);
            if (checked)
            {
                auto checkmark = Formatter();
                checkmark.Add<StringId>(STR_STRING);
                checkmark.Add<char*>(kCheckMarkString);
                DrawTextBasic(
                    dpi, ScreenCoordsXY{ rect.GetLeft() + 1, rect.GetTop() }, STR_WINDOW_COLOUR_2_STRINGID, checkmark);
            }
        }

        std::optional<size_t> GetAssetPackIndexFromPosition(const ScreenCoordsXY& pos, bool& isCheckBox)
        {
            const auto index = pos.y / ItemHeight;
            if (index < 0 || static_cast<size_t>(index) >= GetNumAssetPacks())
                return std::nullopt;

            isCheckBox = pos.x >= 2 && pos.x <= 2 + ItemCheckBoxSize + 1;
            return static_cast<size_t>(index);
        }

        size_t GetNumAssetPacks() const
        {
            auto assetPackManager = GetContext()->GetAssetPackManager();
            if (assetPackManager == nullptr)
                return 0;
            return assetPackManager->GetCount();
        }

        bool IsSelectedAssetPackEnabled() const
        {
            if (_selectedIndex)
            {
                auto assetPackManager = GetContext()->GetAssetPackManager();
                if (assetPackManager != nullptr)
                {
                    auto assetPack = assetPackManager->GetAssetPack(*_selectedIndex);
                    if (assetPack != nullptr)
                    {
                        return assetPack->IsEnabled();
                    }
                }
            }
            return false;
        }

        void ReorderSelectedAssetPack(int32_t direction)
        {
            if (!_selectedIndex)
                return;

            auto assetPackManager = GetContext()->GetAssetPackManager();
            if (assetPackManager == nullptr)
                return;

            if (direction < 0 && *_selectedIndex > 0)
            {
                assetPackManager->Swap(*_selectedIndex, *_selectedIndex - 1);
                (*_selectedIndex)--;
                Invalidate();
            }
            else if (direction > 0 && *_selectedIndex < assetPackManager->GetCount() - 1)
            {
                assetPackManager->Swap(*_selectedIndex, *_selectedIndex + 1);
                (*_selectedIndex)++;
                Invalidate();
            }
        }

        void Apply()
        {
            auto& objectManager = GetContext()->GetObjectManager();
            objectManager.ResetObjects();

            auto assetPackManager = GetContext()->GetAssetPackManager();
            if (assetPackManager != nullptr)
            {
                assetPackManager->SaveEnabledAssetPacks();
            }
        }
    };

    WindowBase* AssetPacksOpen()
    {
        auto flags = WF_AUTO_POSITION | WF_CENTRE_SCREEN;
        return WindowFocusOrCreate<AssetPacksWindow>(WindowClass::AssetPacks, WW, WH, flags);
    }
} // namespace OpenRCT2::Ui::Windows
