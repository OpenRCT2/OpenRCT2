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

        std::fill(_blocks.begin(), _blocks.end(), 0);
    }

    void InvalidationGrid::invalidate(int32_t left, int32_t top, int32_t right, int32_t bottom) noexcept
    {
        left = std::max(left, 0);
        top = std::max(top, 0);
        right = std::min(right, static_cast<int32_t>(_screenWidth));
        bottom = std::min(bottom, static_cast<int32_t>(_screenHeight));

        if (left >= right)
        {
            return;
        }
        if (top >= bottom)
        {
            return;
        }

        left /= _blockWidth;
        right /= _blockWidth;

        top /= _blockHeight;
        bottom /= _blockHeight;

        // TODO: Remove this once _blocks is no longer interop wrapper.
        auto* blocks = _blocks.data();

        const auto columnSize = right - left + 1;

        for (int16_t y = top; y <= bottom; y++)
        {
            const auto yOffset = y * _columnCount;

            // Mark row by column size as invalidated.
            std::memset(blocks + yOffset + left, 0xFF, columnSize);
        }
    }

} // namespace OpenRCT2::Drawing
