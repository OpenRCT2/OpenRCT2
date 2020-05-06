/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include <cstdint>
#    include <memory>
#    include <openrct2/scripting/Duktape.hpp>
#    include <openrct2/scripting/ScriptEngine.h>
#    include <optional>
#    include <string>
#    include <vector>

namespace OpenRCT2::Ui::Windows
{
    using namespace OpenRCT2::Scripting;

    enum class ScrollbarType
    {
        None,
        Horizontal,
        Vertical,
        Both
    };

    enum class ColumnSortOrder
    {
        None,
        Ascending,
        Descending,
    };

    struct ListViewColumn
    {
        bool CanSort{};
        ColumnSortOrder SortOrder;
        std::string Header;
        std::string HeaderTooltip;
        std::optional<int32_t> RatioWidth{};
        std::optional<int32_t> MinWidth{};
        std::optional<int32_t> MaxWidth{};
        int32_t Width{};
    };

    struct ListViewItem
    {
        std::vector<std::string> Cells;

        ListViewItem() = default;
        explicit ListViewItem(const std::string_view& text)
        {
            Cells.emplace_back(text);
        }
        explicit ListViewItem(std::vector<std::string>&& cells)
            : Cells(cells)
        {
        }
    };

    struct RowColumn
    {
        int32_t Row{};
        int32_t Column{};

        RowColumn() = default;
        RowColumn(int32_t row, int32_t column)
            : Row(row)
            , Column(column)
        {
        }

        bool operator==(const RowColumn& other) const
        {
            return Row == other.Row && Column == other.Column;
        }

        bool operator!=(const RowColumn& other) const
        {
            return !(*this == other);
        }
    };

    class CustomListView
    {
    private:
        static constexpr int32_t HEADER_ROW = -1;
        std::vector<ListViewItem> Items;

    public:
        std::shared_ptr<Plugin> Owner;
        std::vector<ListViewColumn> Columns;
        std::vector<size_t> SortedItems;
        std::optional<RowColumn> HighlightedCell;
        std::optional<RowColumn> LastHighlightedCell;
        std::optional<RowColumn> SelectedCell;
        std::optional<size_t> ColumnHeaderPressed;
        bool ColumnHeaderPressedCurrentState{};
        bool ShowColumnHeaders{};
        bool IsStriped{};
        ScreenSize LastKnownSize;
        ScrollbarType Scrollbars = ScrollbarType::Vertical;
        ColumnSortOrder CurrentSortOrder{};
        size_t CurrentSortColumn{};
        bool LastIsMouseDown{};
        bool IsMouseDown{};
        bool CanSelect{};

        DukValue OnClick;
        DukValue OnHighlight;

        void SetItems(const std::vector<ListViewItem>& items);
        void SetItems(std::vector<ListViewItem>&& items);
        bool SortItem(size_t indexA, size_t indexB, size_t column);
        void SortItems(size_t column);
        void SortItems(size_t column, ColumnSortOrder order);
        void Resize(const ScreenSize& size);
        ScreenSize GetSize();
        void MouseOver(const ScreenCoordsXY& pos, bool isMouseDown);
        void MouseDown(const ScreenCoordsXY& pos);
        void MouseUp(const ScreenCoordsXY& pos);
        void Paint(rct_window* w, rct_drawpixelinfo* dpi, const rct_scroll* scroll) const;

    private:
        void PaintHeading(
            rct_window* w, rct_drawpixelinfo* dpi, const ScreenCoordsXY& pos, const ScreenSize& size, const std::string& text,
            ColumnSortOrder sortOrder, bool isPressed) const;
        void PaintCell(
            rct_drawpixelinfo* dpi, const ScreenCoordsXY& pos, const ScreenSize& size, const char* text,
            bool isHighlighted) const;
        std::optional<RowColumn> GetItemIndexAt(const ScreenCoordsXY& pos);
    };
} // namespace OpenRCT2::Ui::Windows

class DukValue;

namespace OpenRCT2::Scripting
{
    using namespace OpenRCT2::Ui::Windows;

    template<> ColumnSortOrder FromDuk(const DukValue& d);
    template<> std::optional<int32_t> FromDuk(const DukValue& d);
    template<> ListViewColumn FromDuk(const DukValue& d);
    template<> ListViewItem FromDuk(const DukValue& d);
    template<> std::optional<RowColumn> FromDuk(const DukValue& d);
    template<> DukValue ToDuk(duk_context* ctx, const RowColumn& value);
} // namespace OpenRCT2::Scripting

#endif
