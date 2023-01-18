/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
        ColumnSortOrder SortOrder{};
        std::string Header;
        std::string HeaderTooltip;
        std::optional<int32_t> RatioWidth{};
        std::optional<int32_t> MinWidth{};
        std::optional<int32_t> MaxWidth{};
        int32_t Width{};
    };

    struct ListViewItem
    {
        bool IsSeparator{};
        std::vector<std::string> Cells;

        ListViewItem() = default;
        explicit ListViewItem(std::string_view text)
        {
            Cells.emplace_back(text);
        }
        explicit ListViewItem(std::vector<std::string>&& cells)
            : Cells(std::move(cells))
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

        rct_window* ParentWindow{};
        size_t ScrollIndex{};
        std::vector<ListViewColumn> Columns;
        std::vector<ListViewItem> Items;
        ScrollbarType Scrollbars = ScrollbarType::Vertical;

    public:
        std::shared_ptr<Plugin> Owner;
        std::vector<size_t> SortedItems;
        std::optional<RowColumn> HighlightedCell;
        std::optional<RowColumn> LastHighlightedCell;
        std::optional<RowColumn> SelectedCell;
        std::optional<int32_t> ColumnHeaderPressed;
        bool ColumnHeaderPressedCurrentState{};
        bool ShowColumnHeaders{};
        bool IsStriped{};
        ScreenSize LastKnownSize;
        ColumnSortOrder CurrentSortOrder{};
        int32_t CurrentSortColumn{};
        bool LastIsMouseDown{};
        bool IsMouseDown{};
        bool CanSelect{};

        DukValue OnClick;
        DukValue OnHighlight;

        CustomListView(rct_window* parent, size_t scrollIndex);
        ScrollbarType GetScrollbars() const;
        void SetScrollbars(ScrollbarType value, bool initialising = false);
        const std::vector<ListViewColumn>& GetColumns() const;
        void SetColumns(const std::vector<ListViewColumn>& columns, bool initialising = false);
        const std::vector<ListViewItem>& GetItems() const;
        void SetItems(const std::vector<ListViewItem>& items, bool initialising = false);
        void SetItems(std::vector<ListViewItem>&& items, bool initialising = false);
        bool SortItem(size_t indexA, size_t indexB, int32_t column);
        void SortItems(int32_t column);
        void SortItems(int32_t column, ColumnSortOrder order);
        void Resize(const ScreenSize& size);
        ScreenSize GetSize();
        void MouseOver(const ScreenCoordsXY& pos, bool isMouseDown);
        void MouseDown(const ScreenCoordsXY& pos);
        void MouseUp(const ScreenCoordsXY& pos);
        void Paint(rct_window* w, DrawPixelInfo* dpi, const rct_scroll* scroll) const;

    private:
        void PaintHeading(
            rct_window* w, DrawPixelInfo* dpi, const ScreenCoordsXY& pos, const ScreenSize& size, const std::string& text,
            ColumnSortOrder sortOrder, bool isPressed) const;
        void PaintSeperator(DrawPixelInfo* dpi, const ScreenCoordsXY& pos, const ScreenSize& size, const char* text) const;
        void PaintCell(
            DrawPixelInfo* dpi, const ScreenCoordsXY& pos, const ScreenSize& size, const char* text,
            bool isHighlighted) const;
        std::optional<RowColumn> GetItemIndexAt(const ScreenCoordsXY& pos);
        Widget* GetWidget() const;
        void Invalidate();
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
    template<> std::vector<ListViewColumn> FromDuk(const DukValue& d);
    template<> std::vector<ListViewItem> FromDuk(const DukValue& d);
    template<> std::optional<RowColumn> FromDuk(const DukValue& d);
    template<> DukValue ToDuk(duk_context* ctx, const RowColumn& value);
    template<> DukValue ToDuk(duk_context* ctx, const ListViewColumn& value);
    template<> ScrollbarType FromDuk(const DukValue& d);
    template<> DukValue ToDuk(duk_context* ctx, const ScrollbarType& value);
} // namespace OpenRCT2::Scripting

#endif
