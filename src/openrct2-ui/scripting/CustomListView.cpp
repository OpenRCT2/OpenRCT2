/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "CustomListView.h"

    #include "../interface/Viewport.h"
    #include "../interface/Widget.h"
    #include "../windows/Windows.h"

    #include <numeric>
    #include <openrct2/Context.h>
    #include <openrct2/core/String.hpp>
    #include <openrct2/drawing/ColourMap.h>
    #include <openrct2/drawing/Drawing.h>
    #include <openrct2/drawing/Rectangle.h>
    #include <openrct2/localisation/Formatter.h>
    #include <openrct2/localisation/Formatting.h>

using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Scripting;
using namespace OpenRCT2::Ui::Windows;

namespace OpenRCT2::Scripting
{
    constexpr size_t kColumnHeaderHeight = kListRowHeight + 1;

    template<>
    ColumnSortOrder FromDuk(const DukValue& d)
    {
        if (d.type() == DukValue::Type::STRING)
        {
            auto s = d.as_string();
            if (s == "ascending")
                return ColumnSortOrder::Ascending;
            if (s == "descending")
                return ColumnSortOrder::Descending;
        }
        return ColumnSortOrder::None;
    }

    template<>
    DukValue ToDuk(duk_context* ctx, const ColumnSortOrder& value)
    {
        switch (value)
        {
            case ColumnSortOrder::Ascending:
                return ToDuk(ctx, "ascending");
            case ColumnSortOrder::Descending:
                return ToDuk(ctx, "descending");
            default:
                return ToDuk(ctx, "none");
        }
    }

    template<>
    std::optional<int32_t> FromDuk(const DukValue& d)
    {
        if (d.type() == DukValue::Type::NUMBER)
        {
            return d.as_int();
        }
        return std::nullopt;
    }

    template<>
    ListViewColumn FromDuk(const DukValue& d)
    {
        ListViewColumn result;
        result.CanSort = AsOrDefault(d["canSort"], false);
        result.SortOrder = FromDuk<ColumnSortOrder>(d["sortOrder"]);
        result.Header = AsOrDefault(d["header"], "");
        result.HeaderTooltip = AsOrDefault(d["headerTooltip"], "");
        result.MinWidth = FromDuk<std::optional<int32_t>>(d["minWidth"]);
        result.MaxWidth = FromDuk<std::optional<int32_t>>(d["maxWidth"]);
        result.RatioWidth = FromDuk<std::optional<int32_t>>(d["ratioWidth"]);
        if (d["width"].type() == DukValue::Type::NUMBER)
        {
            result.MinWidth = d["width"].as_int();
            result.MaxWidth = result.MinWidth;
            result.RatioWidth = std::nullopt;
        }
        else if (!result.RatioWidth)
        {
            result.RatioWidth = 1;
        }
        return result;
    }

    template<>
    DukValue ToDuk(duk_context* ctx, const ListViewColumn& value)
    {
        DukObject obj(ctx);
        obj.Set("canSort", value.CanSort);
        obj.Set("sortOrder", ToDuk(ctx, value.SortOrder));
        obj.Set("header", value.Header);
        obj.Set("headerTooltip", value.HeaderTooltip);
        obj.Set("minWidth", value.MinWidth);
        obj.Set("maxWidth", value.MaxWidth);
        obj.Set("ratioWidth", value.RatioWidth);
        obj.Set("width", value.Width);
        return obj.Take();
    }

    template<>
    ListViewItem FromDuk(const DukValue& d)
    {
        ListViewItem result;
        if (d.type() == DukValue::Type::STRING)
        {
            result = ListViewItem(ProcessString(d));
        }
        else if (d.is_array())
        {
            std::vector<std::string> cells;
            for (const auto& dukCell : d.as_array())
            {
                cells.push_back(ProcessString(dukCell));
            }
            result = ListViewItem(std::move(cells));
        }
        else if (d.type() == DukValue::Type::OBJECT)
        {
            auto type = ProcessString(d["type"]);
            // This type was misspelt between 2020 and 2025.
            if (type == "separator" || type == "seperator")
            {
                auto text = ProcessString(d["text"]);
                result = ListViewItem(text);
                result.IsSeparator = true;
            }
        }
        return result;
    }

    template<>
    std::vector<ListViewColumn> FromDuk(const DukValue& d)
    {
        std::vector<ListViewColumn> result;
        if (d.is_array())
        {
            auto dukColumns = d.as_array();
            for (const auto& dukColumn : dukColumns)
            {
                result.push_back(FromDuk<ListViewColumn>(dukColumn));
            }
        }
        return result;
    }

    template<>
    std::vector<ListViewItem> FromDuk(const DukValue& d)
    {
        std::vector<ListViewItem> result;
        if (d.is_array())
        {
            auto dukItems = d.as_array();
            for (const auto& dukItem : dukItems)
            {
                result.push_back(FromDuk<ListViewItem>(dukItem));
            }
        }
        return result;
    }

    template<>
    std::optional<RowColumn> FromDuk(const DukValue& d)
    {
        if (d.type() == DukValue::Type::OBJECT)
        {
            auto dukRow = d["row"];
            auto dukColumn = d["column"];
            if (dukRow.type() == DukValue::Type::NUMBER && dukColumn.type() == DukValue::Type::NUMBER)
            {
                return RowColumn(dukRow.as_int(), dukColumn.as_int());
            }
        }
        return std::nullopt;
    }

    template<>
    DukValue ToDuk(duk_context* ctx, const RowColumn& value)
    {
        DukObject obj(ctx);
        obj.Set("row", value.Row);
        obj.Set("column", value.Column);
        return obj.Take();
    }

    template<>
    ScrollbarType FromDuk(const DukValue& d)
    {
        auto value = AsOrDefault(d, "");
        if (value == "horizontal")
            return ScrollbarType::Horizontal;
        if (value == "vertical")
            return ScrollbarType::Vertical;
        if (value == "both")
            return ScrollbarType::Both;
        return ScrollbarType::None;
    }

    template<>
    DukValue ToDuk(duk_context* ctx, const ScrollbarType& value)
    {
        switch (value)
        {
            default:
            case ScrollbarType::None:
                return ToDuk(ctx, "none");
            case ScrollbarType::Horizontal:
                return ToDuk(ctx, "horizontal");
            case ScrollbarType::Vertical:
                return ToDuk(ctx, "vertical");
            case ScrollbarType::Both:
                return ToDuk(ctx, "both");
        }
    }

} // namespace OpenRCT2::Scripting

CustomListView::CustomListView(WindowBase* parent, size_t scrollIndex)
    : ParentWindow(parent)
    , ScrollIndex(scrollIndex)
{
}

ScrollbarType CustomListView::GetScrollbars() const
{
    return Scrollbars;
}

void CustomListView::SetScrollbars(ScrollbarType value, bool initialising)
{
    Scrollbars = value;

    if (!initialising)
    {
        auto widget = GetWidget();
        if (widget != nullptr)
        {
            if (value == ScrollbarType::Horizontal)
                widget->content = SCROLL_HORIZONTAL;
            else if (value == ScrollbarType::Vertical)
                widget->content = SCROLL_VERTICAL;
            else if (value == ScrollbarType::Both)
                widget->content = SCROLL_BOTH;
            else
                widget->content = 0;
        }
        WindowInitScrollWidgets(*ParentWindow);
        Invalidate();
    }
}

const std::vector<ListViewColumn>& CustomListView::GetColumns() const
{
    return Columns;
}

void CustomListView::SetColumns(const std::vector<ListViewColumn>& columns, bool initialising)
{
    SelectedCell = std::nullopt;
    Columns = columns;
    LastKnownSize = {};
    SortItems(0, ColumnSortOrder::None);
    if (!initialising)
    {
        WindowUpdateScrollWidgets(*ParentWindow);
        Invalidate();
    }
}

const std::vector<ListViewItem>& CustomListView::GetItems() const
{
    return Items;
}

void CustomListView::SetItems(const std::vector<ListViewItem>& items, bool initialising)
{
    SelectedCell = std::nullopt;
    Items = items;
    SortItems(0, ColumnSortOrder::None);
    if (!initialising)
    {
        WindowUpdateScrollWidgets(*ParentWindow);
        Invalidate();
    }
}

void CustomListView::SetItems(std::vector<ListViewItem>&& items, bool initialising)
{
    Items = std::move(items);
    SortItems(0, ColumnSortOrder::None);
    if (!initialising)
    {
        WindowUpdateScrollWidgets(*ParentWindow);
        Invalidate();
    }
}

bool CustomListView::SortItem(size_t indexA, size_t indexB, int32_t column)
{
    const auto& cellA = Items[indexA].Cells[column];
    const auto& cellB = Items[indexB].Cells[column];
    return String::logicalCmp(cellA.c_str(), cellB.c_str()) < 0;
}

void CustomListView::SortItems(int32_t column)
{
    auto sortOrder = ColumnSortOrder::Ascending;
    if (CurrentSortColumn == column)
    {
        if (CurrentSortOrder == ColumnSortOrder::Ascending)
        {
            sortOrder = ColumnSortOrder::Descending;
        }
        else if (CurrentSortOrder == ColumnSortOrder::Descending)
        {
            sortOrder = ColumnSortOrder::None;
        }
    }
    SortItems(column, sortOrder);
}

void CustomListView::SortItems(int32_t column, ColumnSortOrder order)
{
    // Reset the sorted index map
    SortedItems.resize(Items.size());
    for (size_t i = 0; i < SortedItems.size(); i++)
    {
        SortedItems[i] = i;
    }

    if (order != ColumnSortOrder::None)
    {
        std::sort(
            SortedItems.begin(), SortedItems.end(), [this, column](size_t a, size_t b) { return SortItem(a, b, column); });
        if (order == ColumnSortOrder::Descending)
        {
            std::reverse(SortedItems.begin(), SortedItems.end());
        }
    }

    CurrentSortOrder = order;
    CurrentSortColumn = column;
    if (column >= 0 && static_cast<size_t>(column) < Columns.size())
    {
        Columns[column].SortOrder = order;
    }
    Invalidate();
}

void CustomListView::Resize(const ScreenSize& size)
{
    if (size == LastKnownSize)
        return;

    LastKnownSize = size;

    // Calculate the total of all ratios
    int32_t totalRatio = 0;
    for (size_t c = 0; c < Columns.size(); c++)
    {
        auto& column = Columns[c];
        if (column.RatioWidth)
        {
            totalRatio += *column.RatioWidth;
        }
    }

    // Calculate column widths
    bool hasHorizontalScroll = Scrollbars == ScrollbarType::Horizontal || Scrollbars == ScrollbarType::Both;
    int32_t widthRemaining = size.width;
    for (size_t c = 0; c < Columns.size(); c++)
    {
        auto& column = Columns[c];
        auto isLastColumn = c == Columns.size() - 1;
        if (!hasHorizontalScroll && isLastColumn)
        {
            column.Width = widthRemaining;
        }
        else
        {
            column.Width = 0;
            if (column.RatioWidth && *column.RatioWidth > 0)
            {
                if (isLastColumn)
                {
                    column.Width = widthRemaining;
                }
                else
                {
                    column.Width = (size.width * *column.RatioWidth) / totalRatio;
                }
            }
            if (column.MinWidth)
            {
                column.Width = std::max(column.Width, *column.MinWidth);
            }
            if (column.MaxWidth)
            {
                column.Width = std::min(column.Width, *column.MaxWidth);
            }
        }
        widthRemaining = std::max(0, widthRemaining - column.Width);
    }
}

ScreenSize CustomListView::GetSize()
{
    LastHighlightedCell = HighlightedCell;
    HighlightedCell = std::nullopt;
    ColumnHeaderPressedCurrentState = false;
    LastIsMouseDown = IsMouseDown;
    IsMouseDown = false;

    ScreenSize result;
    if (Scrollbars == ScrollbarType::Horizontal || Scrollbars == ScrollbarType::Both)
    {
        result.width = std::accumulate(
            Columns.begin(), Columns.end(), 0, [](int32_t acc, const ListViewColumn& column) { return acc + column.Width; });

        // Fixes an off-by-one error that causes the scrollbar thumb to not fill when the widget is wide enough
        result.width--;
    }
    if (Scrollbars == ScrollbarType::Vertical || Scrollbars == ScrollbarType::Both)
    {
        result.height = static_cast<int32_t>(Items.size() * kListRowHeight);
        if (ShowColumnHeaders)
        {
            result.height += kColumnHeaderHeight;
        }
    }

    // If the list is getting bigger than the contents, pan towards top left scroll
    auto widget = GetWidget();
    if (widget != nullptr)
    {
        auto& scroll = ParentWindow->scrolls[ScrollIndex];

        // Horizontal
        auto left = result.width - widget->right + widget->left + 21;
        if (left < 0)
            left = 0;
        if (left < scroll.contentOffsetX)
        {
            scroll.contentOffsetX = left;
            Invalidate();
        }

        // Vertical
        auto top = result.height - widget->bottom + widget->top + 21;
        if (top < 0)
            top = 0;
        if (top < scroll.contentOffsetY)
        {
            scroll.contentOffsetY = top;
            Invalidate();
        }
    }
    return result;
}

void CustomListView::MouseOver(const ScreenCoordsXY& pos, bool isMouseDown)
{
    auto hitResult = GetItemIndexAt(pos);
    if (hitResult)
    {
        HighlightedCell = hitResult;
        if (HighlightedCell != LastHighlightedCell)
        {
            if (hitResult->Row != kHeaderRow && OnHighlight.context() != nullptr && OnHighlight.is_function())
            {
                auto ctx = OnHighlight.context();
                duk_push_int(ctx, static_cast<int32_t>(HighlightedCell->Row));
                auto dukRow = DukValue::take_from_stack(ctx, -1);
                duk_push_int(ctx, static_cast<int32_t>(HighlightedCell->Column));
                auto dukColumn = DukValue::take_from_stack(ctx, -1);
                auto& scriptEngine = GetContext()->GetScriptEngine();
                scriptEngine.ExecutePluginCall(Owner, OnHighlight, { dukRow, dukColumn }, false);
            }
            Invalidate();
        }
    }

    // Update the header currently held down
    if (isMouseDown)
    {
        if (hitResult && hitResult->Row == kHeaderRow)
        {
            ColumnHeaderPressedCurrentState = (hitResult->Column == ColumnHeaderPressed);
            Invalidate();
        }
        IsMouseDown = true;
    }
    else
    {
        if (LastIsMouseDown)
        {
            MouseUp(pos);
        }
        IsMouseDown = false;
    }
}

void CustomListView::MouseDown(const ScreenCoordsXY& pos)
{
    auto hitResult = GetItemIndexAt(pos);
    if (hitResult)
    {
        if (hitResult->Row != kHeaderRow)
        {
            if (CanSelect)
            {
                SelectedCell = hitResult;
                Invalidate();
            }

            auto ctx = OnClick.context();
            if (ctx != nullptr && OnClick.is_function())
            {
                duk_push_int(ctx, static_cast<int32_t>(hitResult->Row));
                auto dukRow = DukValue::take_from_stack(ctx, -1);
                duk_push_int(ctx, static_cast<int32_t>(hitResult->Column));
                auto dukColumn = DukValue::take_from_stack(ctx, -1);
                auto& scriptEngine = GetContext()->GetScriptEngine();
                scriptEngine.ExecutePluginCall(Owner, OnClick, { dukRow, dukColumn }, false);
            }
        }
    }
    if (hitResult && hitResult->Row == kHeaderRow)
    {
        if (Columns[hitResult->Column].CanSort)
        {
            ColumnHeaderPressed = hitResult->Column;
            ColumnHeaderPressedCurrentState = true;
            Invalidate();
        }
    }
    IsMouseDown = true;
}

void CustomListView::MouseUp(const ScreenCoordsXY& pos)
{
    auto hitResult = GetItemIndexAt(pos);
    if (hitResult && hitResult->Row == kHeaderRow)
    {
        if (hitResult->Column == ColumnHeaderPressed)
        {
            SortItems(hitResult->Column);
        }
    }

    if (!ColumnHeaderPressedCurrentState)
    {
        ColumnHeaderPressed = std::nullopt;
        Invalidate();
    }
}

void CustomListView::Paint(WindowBase* w, RenderTarget& rt, const ScrollArea* scroll) const
{
    auto paletteIndex = getColourMap(w->colours[1].colour).midLight;
    Rectangle::fill(rt, { { rt.x, rt.y }, { rt.x + rt.width, rt.y + rt.height } }, paletteIndex);

    int32_t y = ShowColumnHeaders ? kColumnHeaderHeight : 0;
    for (size_t i = 0; i < Items.size(); i++)
    {
        if (y > rt.y + rt.height)
        {
            // Past the scroll view area
            break;
        }

        if (y + kListRowHeight >= rt.y)
        {
            const auto& itemIndex = static_cast<int32_t>(SortedItems[i]);
            const auto& item = Items[itemIndex];

            if (item.IsSeparator)
            {
                const auto& text = item.Cells[0];
                ScreenSize cellSize = { LastKnownSize.width, kListRowHeight };
                PaintSeparator(rt, { 0, y }, cellSize, text.c_str());
            }
            else
            {
                // Background colour
                auto isStriped = IsStriped && (i & 1);
                auto isHighlighted = (HighlightedCell && itemIndex == HighlightedCell->Row);
                auto isSelected = (SelectedCell && itemIndex == SelectedCell->Row);
                if (isSelected)
                {
                    Rectangle::filter(
                        rt, { { rt.x, y }, { rt.x + rt.width, y + (kListRowHeight - 1) } }, FilterPaletteID::paletteDarken2);
                }
                else if (isHighlighted)
                {
                    Rectangle::filter(
                        rt, { { rt.x, y }, { rt.x + rt.width, y + (kListRowHeight - 1) } }, FilterPaletteID::paletteDarken2);
                }
                else if (isStriped)
                {
                    Rectangle::fill(
                        rt, { { rt.x, y }, { rt.x + rt.width, y + (kListRowHeight - 1) } },
                        getColourMap(w->colours[1].colour).lighter, true);
                }

                // Columns
                if (Columns.size() == 0)
                {
                    if (item.Cells.size() != 0)
                    {
                        const auto& text = item.Cells[0];
                        if (!text.empty())
                        {
                            ScreenSize cellSize = { std::numeric_limits<int32_t>::max(), kListRowHeight };
                            PaintCell(rt, { 0, y }, cellSize, text.c_str(), isHighlighted);
                        }
                    }
                }
                else
                {
                    int32_t x = 0;
                    for (size_t j = 0; j < Columns.size(); j++)
                    {
                        const auto& column = Columns[j];
                        if (item.Cells.size() > j)
                        {
                            const auto& text = item.Cells[j];
                            if (!text.empty())
                            {
                                ScreenSize cellSize = { column.Width, kListRowHeight };
                                PaintCell(rt, { x, y }, cellSize, text.c_str(), isHighlighted);
                            }
                        }
                        x += column.Width;
                    }
                }
            }
        }

        y += kListRowHeight;
    }

    if (ShowColumnHeaders)
    {
        y = scroll->contentOffsetY;

        auto bgColour = getColourMap(w->colours[1].colour).midLight;
        Rectangle::fill(rt, { { rt.x, y }, { rt.x + rt.width, y + 12 } }, bgColour);

        int32_t x = 0;
        for (int32_t j = 0; j < static_cast<int32_t>(Columns.size()); j++)
        {
            const auto& column = Columns[j];
            auto columnWidth = column.Width;
            if (columnWidth != 0)
            {
                auto sortOrder = ColumnSortOrder::None;
                if (CurrentSortColumn == j)
                {
                    sortOrder = CurrentSortOrder;
                }

                bool isPressed = ColumnHeaderPressed == j && ColumnHeaderPressedCurrentState;
                PaintHeading(w, rt, { x, y }, { column.Width, kListRowHeight }, column.Header, sortOrder, isPressed);
                x += columnWidth;
            }
        }
    }
}

void CustomListView::PaintHeading(
    WindowBase* w, RenderTarget& rt, const ScreenCoordsXY& pos, const ScreenSize& size, const std::string& text,
    ColumnSortOrder sortOrder, bool isPressed) const
{
    auto borderStyle = Rectangle::BorderStyle::outset;
    if (isPressed)
    {
        borderStyle = Rectangle::BorderStyle::inset;
    }
    Rectangle::fillInset(rt, { pos, pos + ScreenCoordsXY{ size.width - 1, size.height - 1 } }, w->colours[1], borderStyle);
    if (!text.empty())
    {
        PaintCell(rt, pos, size, text.c_str(), false);
    }

    if (sortOrder == ColumnSortOrder::Ascending)
    {
        auto ft = Formatter();
        ft.Add<StringId>(STR_UP);
        DrawTextBasic(rt, pos + ScreenCoordsXY{ size.width - 1, 0 }, STR_BLACK_STRING, ft, { TextAlignment::right });
    }
    else if (sortOrder == ColumnSortOrder::Descending)
    {
        auto ft = Formatter();
        ft.Add<StringId>(STR_DOWN);
        DrawTextBasic(rt, pos + ScreenCoordsXY{ size.width - 1, 0 }, STR_BLACK_STRING, ft, { TextAlignment::right });
    }
}

void CustomListView::PaintSeparator(RenderTarget& rt, const ScreenCoordsXY& pos, const ScreenSize& size, const char* text) const
{
    auto hasText = text != nullptr && text[0] != '\0';
    auto left = pos.x + 4;
    auto right = pos.x + size.width - 4;
    auto centreX = size.width / 2;
    auto lineY0 = pos.y + 5;
    auto lineY1 = lineY0 + 1;

    auto baseColour = ParentWindow->colours[1];
    auto lightColour = getColourMap(baseColour.colour).lighter;
    auto darkColour = getColourMap(baseColour.colour).midDark;

    if (hasText)
    {
        // Draw string
        Formatter ft;
        ft.Add<const char*>(text);
        DrawTextBasic(rt, { centreX, pos.y }, STR_STRING, ft, { baseColour, TextAlignment::centre });

        // Get string dimensions
        utf8 stringBuffer[512]{};
        FormatStringLegacy(stringBuffer, sizeof(stringBuffer), STR_STRING, ft.Data());
        int32_t categoryStringHalfWidth = (GfxGetStringWidth(stringBuffer, FontStyle::medium) / 2) + 4;
        int32_t strLeft = centreX - categoryStringHalfWidth;
        int32_t strRight = centreX + categoryStringHalfWidth;

        // Draw light horizontal rule
        auto lightLineLeftTop1 = ScreenCoordsXY{ left, lineY0 };
        auto lightLineRightBottom1 = ScreenCoordsXY{ strLeft, lineY0 };
        GfxDrawLine(rt, { lightLineLeftTop1, lightLineRightBottom1 }, lightColour);

        auto lightLineLeftTop2 = ScreenCoordsXY{ strRight, lineY0 };
        auto lightLineRightBottom2 = ScreenCoordsXY{ right, lineY0 };
        GfxDrawLine(rt, { lightLineLeftTop2, lightLineRightBottom2 }, lightColour);

        // Draw dark horizontal rule
        auto darkLineLeftTop1 = ScreenCoordsXY{ left, lineY1 };
        auto darkLineRightBottom1 = ScreenCoordsXY{ strLeft, lineY1 };
        GfxDrawLine(rt, { darkLineLeftTop1, darkLineRightBottom1 }, darkColour);

        auto darkLineLeftTop2 = ScreenCoordsXY{ strRight, lineY1 };
        auto darkLineRightBottom2 = ScreenCoordsXY{ right, lineY1 };
        GfxDrawLine(rt, { darkLineLeftTop2, darkLineRightBottom2 }, darkColour);
    }
    else
    {
        // Draw light horizontal rule
        auto lightLineLeftTop1 = ScreenCoordsXY{ left, lineY0 };
        auto lightLineRightBottom1 = ScreenCoordsXY{ right, lineY0 };
        GfxDrawLine(rt, { lightLineLeftTop1, lightLineRightBottom1 }, lightColour);

        // Draw dark horizontal rule
        auto darkLineLeftTop1 = ScreenCoordsXY{ left, lineY1 };
        auto darkLineRightBottom1 = ScreenCoordsXY{ right, lineY1 };
        GfxDrawLine(rt, { darkLineLeftTop1, darkLineRightBottom1 }, darkColour);
    }
}

void CustomListView::PaintCell(
    RenderTarget& rt, const ScreenCoordsXY& pos, const ScreenSize& size, const char* text, bool isHighlighted) const
{
    StringId stringId = isHighlighted ? STR_WINDOW_COLOUR_2_STRINGID : STR_BLACK_STRING;

    auto ft = Formatter();
    ft.Add<StringId>(STR_STRING);
    ft.Add<const char*>(text);
    DrawTextEllipsised(rt, pos, size.width, stringId, ft, {});
}

std::optional<RowColumn> CustomListView::GetItemIndexAt(const ScreenCoordsXY& pos)
{
    std::optional<RowColumn> result;
    if (pos.x >= 0)
    {
        // Check if we pressed the header
        auto& scroll = ParentWindow->scrolls[ScrollIndex];
        int32_t absoluteY = pos.y - scroll.contentOffsetY;
        if (ShowColumnHeaders && absoluteY >= 0 && absoluteY < kListRowHeight)
        {
            result = RowColumn();
            result->Row = kHeaderRow;
        }
        else
        {
            // Check what row we pressed
            int32_t firstY = ShowColumnHeaders ? kColumnHeaderHeight : 0;
            int32_t row = (pos.y - firstY) / kListRowHeight;
            if (row >= 0 && row < static_cast<int32_t>(Items.size()))
            {
                result = RowColumn();
                result->Row = static_cast<int32_t>(SortedItems[row]);
            }
        }

        // Check what column we pressed if there are any
        if (result && Columns.size() > 0)
        {
            bool found = false;
            int32_t x = 0;
            for (size_t c = 0; c < Columns.size(); c++)
            {
                const auto& column = Columns[c];
                x += column.Width;
                if (column.Width != 0)
                {
                    if (pos.x < x)
                    {
                        result->Column = static_cast<int32_t>(c);
                        found = true;
                        break;
                    }
                }
            }
            if (!found)
            {
                // Past all columns
                return std::nullopt;
            }
        }
    }
    return result;
}

OpenRCT2::Widget* CustomListView::GetWidget() const
{
    size_t scrollIndex = 0;
    for (WidgetIndex widgetIndex = 0; widgetIndex < ParentWindow->widgets.size(); widgetIndex++)
    {
        auto& widget = ParentWindow->widgets[widgetIndex];
        if (widget.type == WidgetType::scroll)
        {
            if (scrollIndex == ScrollIndex)
            {
                return &widget;
            }
            scrollIndex++;
        }
    }
    return nullptr;
}

void CustomListView::Invalidate()
{
    ParentWindow->invalidate();
}

#endif
