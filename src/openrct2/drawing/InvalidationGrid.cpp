#include "InvalidationGrid.h"

#include <algorithm>
#include <cstring>

namespace OpenRCT2::Drawing
{
    uint32_t InvalidationGrid::getRowCount() const noexcept
    {
        return _rowCount;
    }

    uint32_t InvalidationGrid::getColumnCount() const noexcept
    {
        return _columnCount;
    }

    uint32_t InvalidationGrid::getBlockWidth() const noexcept
    {
        return _blockWidth;
    }

    uint32_t InvalidationGrid::getBlockHeight() const noexcept
    {
        return _blockHeight;
    }

    void InvalidationGrid::reset(int32_t width, int32_t height, uint32_t blockWidth, uint32_t blockHeight) noexcept
    {
        _blockWidth = blockWidth;
        _blockHeight = blockHeight;
        _columnCount = (width / blockWidth) + 1;
        _rowCount = (height / blockHeight) + 1;
        _screenWidth = width;
        _screenHeight = height;

        _blocks.resize(_columnCount * _rowCount);
        _blocks.shrink_to_fit();

        _lowestRow = std::numeric_limits<uint32_t>::max();
        _highestRow = 0;
        _lowestColumn = std::numeric_limits<uint32_t>::max();
        _highestColumn = 0;

        std::fill(_blocks.begin(), _blocks.end(), 0);
    }

    void InvalidationGrid::invalidate(int32_t left, int32_t top, int32_t right, int32_t bottom) noexcept
    {
        // Clamp to screen bounds
        left = std::max(left, 0);
        top = std::max(top, 0);
        right = std::min(right, static_cast<int32_t>(_screenWidth));
        bottom = std::min(bottom, static_cast<int32_t>(_screenHeight));

        if (left >= right || top >= bottom)
            return;

        // Calculate block indices
        const int32_t blockWidth = static_cast<int32_t>(_blockWidth);
        const int32_t blockHeight = static_cast<int32_t>(_blockHeight);

        const int32_t startCol = left / blockWidth;
        const int32_t endCol = (right - 1) / blockWidth;
        const int32_t startRow = top / blockHeight;
        const int32_t endRow = (bottom - 1) / blockHeight;

        // Clamp to grid dimensions
        const int32_t clampedStartCol = std::max(startCol, 0);
        const int32_t clampedEndCol = std::min(endCol, static_cast<int32_t>(_columnCount) - 1);
        const int32_t clampedStartRow = std::max(startRow, 0);
        const int32_t clampedEndRow = std::min(endRow, static_cast<int32_t>(_rowCount) - 1);

        if (clampedStartCol > clampedEndCol || clampedStartRow > clampedEndRow)
            return;

        // Update tracked regions
        _lowestRow = std::min(_lowestRow, static_cast<uint32_t>(clampedStartRow));
        _highestRow = std::max(_highestRow, static_cast<uint32_t>(clampedEndRow));
        _lowestColumn = std::min(_lowestColumn, static_cast<uint32_t>(clampedStartCol));
        _highestColumn = std::max(_highestColumn, static_cast<uint32_t>(clampedEndCol));

        // Mark blocks
        const size_t columnsToMark = clampedEndCol - clampedStartCol + 1;
        for (int32_t row = clampedStartRow; row <= clampedEndRow; ++row)
        {
            std::memset(
                &_blocks[row * _columnCount + clampedStartCol], 0xFF, columnsToMark * sizeof(decltype(_blocks)::value_type));
        }
    }

} // namespace OpenRCT2::Drawing
