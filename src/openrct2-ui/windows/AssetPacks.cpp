/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../UiStringIds.h"

#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/AssetPack.h>
#include <openrct2/AssetPackManager.h>
#include <openrct2/Context.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/drawing/ColourMap.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/drawing/Text.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/ui/WindowManager.h>

using namespace OpenRCT2::Drawing;

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId kWindowTitle = STR_ASSET_PACKS;
    static constexpr ScreenSize kWindowSize = { 400, 200 };

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
    static constexpr auto WindowAssetPacksWidgets = makeWidgets(
        makeWindowShim(kWindowTitle, kWindowSize),
        makeWidget({ 0, 0 }, { 0,   0 }, WidgetType::labelCentred, WindowColour::secondary, STR_HIGH_PRIORITY),
        makeWidget({ 0, 0 }, { 0, 147 }, WidgetType::scroll,       WindowColour::secondary, SCROLL_VERTICAL),
        makeWidget({ 0, 0 }, { 0,   0 }, WidgetType::labelCentred, WindowColour::secondary, STR_LOW_PRIORITY),
        makeWidget({ 0, 0 }, { 0,   0 }, WidgetType::flatBtn,      WindowColour::secondary, ImageId(SPR_G2_ARROW_UP), STR_INCREASE_PRIOTITY_TIP),
        makeWidget({ 0, 0 }, { 0,   0 }, WidgetType::flatBtn,      WindowColour::secondary, ImageId(SPR_G2_ARROW_DOWN), STR_DECREASE_PRIOTITY_TIP),
        makeWidget({ 0, 0 }, { 0,   0 }, WidgetType::flatBtn,      WindowColour::secondary, ImageId(SPR_G2_RELOAD), STR_RELOAD_ASSET_PACKS_TIP)
    );
    // clang-format on

    class AssetPacksWindow final : public Window
    {
    private:
        static constexpr int32_t ItemHeight = kScrollableRowHeight + 1;
        static constexpr int32_t ItemCheckBoxSize = ItemHeight - 3;
        std::optional<size_t> _highlightedIndex;
        std::optional<size_t> _selectedIndex;

    public:
        void onOpen() override
        {
            setWidgets(WindowAssetPacksWidgets);
            WindowInitScrollWidgets(*this);
        }

        void onClose() override
        {
            Apply();
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
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

        ScreenSize onScrollGetSize(int32_t scrollIndex) override
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
                invalidate();
            }

            return result;
        }

        void onScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
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
                        invalidate();
                    }
                }
            }

            // Select item
            if (_selectedIndex != index)
            {
                _selectedIndex = index;
                invalidate();
            }
        }

        void onScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            auto isCheckBox = false;
            auto index = GetAssetPackIndexFromPosition(screenCoords, isCheckBox);
            if (_highlightedIndex != index)
            {
                _highlightedIndex = index;
                invalidate();
            }
        }

        void onPrepareDraw() override
        {
            auto& list = widgets[WIDX_LIST];
            list.left = 6;
            list.top = widgets[WIDX_TITLE].height() - 1 + 8 + 11 + 3;
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

            auto toolstripY = widgets[WIDX_TITLE].height() - 1 + 8;
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

            setWidgetDisabled(WIDX_MOVE_UP, !_selectedIndex || _selectedIndex == 0u);
            setWidgetDisabled(WIDX_MOVE_DOWN, !_selectedIndex || _selectedIndex >= GetNumAssetPacks() - 1);

            widgets[WIDX_APPLY].bottom = widgets[WIDX_LIST].bottom;
            widgets[WIDX_APPLY].top = widgets[WIDX_APPLY].bottom - 24;
        }

        void onDraw(RenderTarget& rt) override
        {
            drawWidgets(rt);
        }

        void onScrollDraw(int32_t scrollIndex, RenderTarget& rt) override
        {
            auto rtCoords = ScreenCoordsXY{ rt.x, rt.y };
            Rectangle::fill(
                rt, { rtCoords, rtCoords + ScreenCoordsXY{ rt.width - 1, rt.height - 1 } },
                getColourMap(colours[1].colour).midLight);

            auto assetPackManager = GetContext()->GetAssetPackManager();
            if (assetPackManager == nullptr)
                return;

            auto numAssetPacks = assetPackManager->GetCount();
            auto y = 0;
            for (size_t i = 0; i <= numAssetPacks; i++)
            {
                if (y > rt.y + rt.height)
                    break;
                if (y + 11 < rt.y)
                    continue;

                auto isSelected = i == _selectedIndex;
                auto isHighlighted = i == _highlightedIndex;
                if (i == numAssetPacks)
                {
                    auto ft = Formatter();
                    ft.Add<StringId>(STR_BASE_GRAPHICS_MUSIC_SOUND);
                    PaintItem(rt, y, ft, true, isSelected, isHighlighted);
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
                        PaintItem(rt, y, ft, isChecked, isSelected, isHighlighted);
                    }
                }
                y += ItemHeight;
            }
        }

    private:
        void PaintItem(RenderTarget& rt, int32_t y, Formatter& ft, bool isChecked, bool isSelected, bool isHighlighted)
        {
            auto listWidth = widgets[WIDX_LIST].right - widgets[WIDX_LIST].left;
            auto stringId = STR_BLACK_STRING;
            auto fillRectangle = ScreenRect{ { 0, y }, { listWidth, y + ItemHeight - 1 } };
            if (isSelected)
            {
                Rectangle::fill(rt, fillRectangle, getColourMap(colours[1].colour).midDark);
                stringId = STR_WINDOW_COLOUR_2_STRINGID;
            }
            else if (isHighlighted)
            {
                Rectangle::fill(rt, fillRectangle, getColourMap(colours[1].colour).midDark);
            }

            DrawTextEllipsised(rt, { 16, y + 1 }, listWidth, stringId, ft);

            auto checkboxSize = ItemHeight - 3;
            PaintCheckbox(rt, { { 2, y + 1 }, { 2 + checkboxSize + 1, y + 1 + checkboxSize } }, isChecked);
        }

        void PaintCheckbox(RenderTarget& rt, const ScreenRect& rect, bool checked)
        {
            Rectangle::fillInset(
                rt, rect, colours[1], Rectangle::BorderStyle::inset, Rectangle::FillBrightness::dark,
                Rectangle::FillMode::dontLightenWhenInset);
            if (checked)
            {
                auto checkmark = Formatter();
                checkmark.Add<StringId>(STR_STRING);
                checkmark.Add<char*>(kCheckMarkString);
                DrawTextBasic(rt, ScreenCoordsXY{ rect.GetLeft() + 1, rect.GetTop() }, STR_WINDOW_COLOUR_2_STRINGID, checkmark);
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
                invalidate();
            }
            else if (direction > 0 && *_selectedIndex < assetPackManager->GetCount() - 1)
            {
                assetPackManager->Swap(*_selectedIndex, *_selectedIndex + 1);
                (*_selectedIndex)++;
                invalidate();
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
        auto* windowMgr = GetWindowManager();
        WindowFlags flags = { WindowFlag::autoPosition, WindowFlag::centreScreen };

        return windowMgr->FocusOrCreate<AssetPacksWindow>(WindowClass::assetPacks, kWindowSize, flags);
    }
} // namespace OpenRCT2::Ui::Windows
