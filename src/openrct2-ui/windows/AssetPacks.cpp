/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/AssetPack.h>
#include <openrct2/AssetPackManager.h>
#include <openrct2/Context.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Text.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/sprites.h>

using namespace OpenRCT2;

static constexpr const StringId WINDOW_TITLE = STR_ASSET_PACKS;
static constexpr const int32_t WW = 400;
static constexpr const int32_t WH = 200;

// clang-format off
enum WindowAssetPacksWidgetIdx {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_LIST,
    WIDX_TOGGLE,
    WIDX_MOVE_UP,
    WIDX_MOVE_DOWN,
    WIDX_APPLY,
};

static rct_widget WindowAssetPacksWidgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({ 0, 0 }, { 0, 0 }, WindowWidgetType::Scroll,  WindowColour::Secondary, SCROLL_VERTICAL),
    MakeWidget({ 0, 0 }, { 0, 0 }, WindowWidgetType::FlatBtn, WindowColour::Secondary, SPR_OPEN, STR_NONE),
    MakeWidget({ 0, 0 }, { 0, 0 }, WindowWidgetType::FlatBtn, WindowColour::Secondary, SPR_G2_ARROW_UP, STR_NONE),
    MakeWidget({ 0, 0 }, { 0, 0 }, WindowWidgetType::FlatBtn, WindowColour::Secondary, SPR_G2_ARROW_DOWN, STR_NONE),
    MakeWidget({ 0, 0 }, { 0, 0 }, WindowWidgetType::FlatBtn, WindowColour::Secondary, SPR_ROTATE_ARROW, STR_NONE),
    WIDGETS_END,
};
// clang-format on

class AssetPacksWindow final : public Window
{
private:
    static constexpr int32_t ItemHeight = SCROLLABLE_ROW_HEIGHT + 1;
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
            case WIDX_TOGGLE:
                ToggleSelectedAssetPack();
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
            auto numAssetPacks = assetPackManager->GetCount() + 2; // Add 2 for separators
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
        widgets[WIDX_BACKGROUND].right = width - 1;
        widgets[WIDX_BACKGROUND].bottom = height - 1;
        widgets[WIDX_TITLE].right = width - 2;
        widgets[WIDX_CLOSE].left = width - 13;
        widgets[WIDX_CLOSE].right = width - 3;

        widgets[WIDX_LIST].left = 6;
        widgets[WIDX_LIST].top = 20;
        widgets[WIDX_LIST].right = width - 2 - 24 - 1;
        widgets[WIDX_LIST].bottom = height - 6;

        auto toolstripY = 20;
        auto toolstripRight = width - 2;
        auto toolstripLeft = toolstripRight - 24;
        auto disabled = !_selectedIndex.has_value();
        for (WidgetIndex i = WIDX_TOGGLE; i <= WIDX_APPLY; i++)
        {
            SetWidgetDisabled(i, disabled);
            widgets[i].top = toolstripY;
            widgets[i].bottom = toolstripY + 24;
            widgets[i].left = toolstripLeft;
            widgets[i].right = toolstripRight;
            toolstripY += 24;
        }

        auto isEnabled = IsSelectedAssetPackEnabled();
        widgets[WIDX_TOGGLE].image = isEnabled ? SPR_OPEN : SPR_CLOSED;
        SetWidgetPressed(WIDX_TOGGLE, isEnabled);

        SetWidgetDisabled(WIDX_APPLY, false);
        widgets[WIDX_APPLY].bottom = widgets[WIDX_LIST].bottom;
        widgets[WIDX_APPLY].top = widgets[WIDX_APPLY].bottom - 24;
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        DrawWidgets(dpi);
    }

    void OnScrollDraw(int32_t scrollIndex, rct_drawpixelinfo& dpi) override
    {
        auto dpiCoords = ScreenCoordsXY{ dpi.x, dpi.y };
        gfx_fill_rect(
            &dpi, { dpiCoords, dpiCoords + ScreenCoordsXY{ dpi.width - 1, dpi.height - 1 } }, ColourMapA[colours[1]].mid_light);

        auto listWidth = dpi.width - 1;
        auto y = 0;

        auto assetPackManager = GetContext()->GetAssetPackManager();
        if (assetPackManager == nullptr)
            return;

        auto numAssetPacks = assetPackManager->GetCount();
        for (size_t i = 0; i <= numAssetPacks + 1; i++)
        {
            if (y > dpi.y + dpi.height)
                break;
            if (y + 11 < dpi.y)
                continue;

            if (i == 0 || i == numAssetPacks + 1)
            {
                auto text = i == 0 ? STR_LOW_PRIORITY : STR_HIGH_PRIORITY;
                PaintSeperator(dpi, { 0, y }, { listWidth, y + ItemHeight - 1 }, text);
            }
            else
            {
                auto assetPackIndex = i - 1;
                auto assetPack = assetPackManager->GetAssetPack(assetPackIndex);
                if (assetPack != nullptr)
                {
                    auto stringId = STR_BLACK_STRING;
                    auto fillRectangle = ScreenRect{ { 0, y }, { listWidth, y + ItemHeight - 1 } };
                    if (assetPackIndex == _selectedIndex)
                    {
                        gfx_fill_rect(&dpi, fillRectangle, ColourMapA[colours[1]].mid_dark);
                        stringId = STR_WINDOW_COLOUR_2_STRINGID;
                    }
                    else if (assetPackIndex == _highlightedIndex)
                    {
                        gfx_fill_rect(&dpi, fillRectangle, ColourMapA[colours[1]].mid_dark);
                    }

                    auto ft = Formatter();
                    ft.Add<StringId>(STR_STRING);
                    ft.Add<const char*>(assetPack->Name.c_str());
                    DrawTextEllipsised(&dpi, { 16, y + 1 }, listWidth, stringId, ft);

                    auto checkboxSize = ItemHeight - 3;
                    PaintCheckbox(
                        dpi, { { 2, y + 1 }, { 2 + checkboxSize + 1, y + 1 + checkboxSize } }, assetPack->IsEnabled());
                }
            }
            y += ItemHeight;
        }
    }

private:
    void PaintSeperator(rct_drawpixelinfo& dpi, const ScreenCoordsXY& pos, const ScreenSize& size, StringId text) const
    {
        auto hasText = text != STR_NONE;
        auto left = pos.x + 4;
        auto right = pos.x + size.width - 4;
        auto centreX = size.width / 2;
        auto lineY0 = pos.y + 5;
        auto lineY1 = lineY0 + 1;

        auto baseColour = colours[1];
        auto lightColour = ColourMapA[baseColour].lighter;
        auto darkColour = ColourMapA[baseColour].mid_dark;

        if (hasText)
        {
            // Draw string
            Formatter ft;
            ft.Add<StringId>(text);
            DrawTextBasic(&dpi, { centreX, pos.y }, STR_STRINGID, ft, { baseColour, TextAlignment::CENTRE });

            // Get string dimensions
            format_string(gCommonStringFormatBuffer, sizeof(gCommonStringFormatBuffer), STR_STRING, ft.Data());
            int32_t categoryStringHalfWidth = (gfx_get_string_width(gCommonStringFormatBuffer, FontSpriteBase::MEDIUM) / 2) + 4;
            int32_t strLeft = centreX - categoryStringHalfWidth;
            int32_t strRight = centreX + categoryStringHalfWidth;

            // Draw light horizontal rule
            auto lightLineLeftTop1 = ScreenCoordsXY{ left, lineY0 };
            auto lightLineRightBottom1 = ScreenCoordsXY{ strLeft, lineY0 };
            gfx_draw_line(&dpi, { lightLineLeftTop1, lightLineRightBottom1 }, lightColour);

            auto lightLineLeftTop2 = ScreenCoordsXY{ strRight, lineY0 };
            auto lightLineRightBottom2 = ScreenCoordsXY{ right, lineY0 };
            gfx_draw_line(&dpi, { lightLineLeftTop2, lightLineRightBottom2 }, lightColour);

            // Draw dark horizontal rule
            auto darkLineLeftTop1 = ScreenCoordsXY{ left, lineY1 };
            auto darkLineRightBottom1 = ScreenCoordsXY{ strLeft, lineY1 };
            gfx_draw_line(&dpi, { darkLineLeftTop1, darkLineRightBottom1 }, darkColour);

            auto darkLineLeftTop2 = ScreenCoordsXY{ strRight, lineY1 };
            auto darkLineRightBottom2 = ScreenCoordsXY{ right, lineY1 };
            gfx_draw_line(&dpi, { darkLineLeftTop2, darkLineRightBottom2 }, darkColour);
        }
        else
        {
            // Draw light horizontal rule
            auto lightLineLeftTop1 = ScreenCoordsXY{ left, lineY0 };
            auto lightLineRightBottom1 = ScreenCoordsXY{ right, lineY0 };
            gfx_draw_line(&dpi, { lightLineLeftTop1, lightLineRightBottom1 }, lightColour);

            // Draw dark horizontal rule
            auto darkLineLeftTop1 = ScreenCoordsXY{ left, lineY1 };
            auto darkLineRightBottom1 = ScreenCoordsXY{ right, lineY1 };
            gfx_draw_line(&dpi, { darkLineLeftTop1, darkLineRightBottom1 }, darkColour);
        }
    }

    void PaintCheckbox(rct_drawpixelinfo& dpi, const ScreenRect& rect, bool checked)
    {
        gfx_fill_rect_inset(&dpi, rect, colours[1], INSET_RECT_F_E0);
        if (checked)
        {
            auto checkmark = Formatter();
            checkmark.Add<StringId>(STR_STRING);
            checkmark.Add<char*>(CheckBoxMarkString);
            DrawTextBasic(&dpi, ScreenCoordsXY{ rect.GetLeft() + 1, rect.GetTop() }, STR_WINDOW_COLOUR_2_STRINGID, checkmark);
        }
    }

    std::optional<size_t> GetAssetPackIndexFromPosition(const ScreenCoordsXY& pos, bool& isCheckBox)
    {
        const auto index = (pos.y / ItemHeight) - 1;
        if (index < 0 || static_cast<size_t>(index) >= GetNumAssetPacks())
            return {};

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

    void ToggleSelectedAssetPack()
    {
        if (_selectedIndex)
        {
            auto assetPackManager = GetContext()->GetAssetPackManager();
            if (assetPackManager != nullptr)
            {
                auto assetPack = assetPackManager->GetAssetPack(*_selectedIndex);
                if (assetPack != nullptr)
                {
                    assetPack->SetEnabled(!assetPack->IsEnabled());
                    Invalidate();
                }
            }
        }
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

rct_window* WindowAssetPacksOpen()
{
    auto flags = WF_AUTO_POSITION | WF_CENTRE_SCREEN;
    return WindowFocusOrCreate<AssetPacksWindow>(WindowClass::AssetPacks, WW, WH, flags);
}
