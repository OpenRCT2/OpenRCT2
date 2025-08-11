#pragma once

#include <algorithm>
#include <cstdint>
#include <limits>
#include <mutex>
#include <vector>

namespace OpenRCT2::Drawing
{
    class InvalidationGrid
    {
        mutable std::mutex _mutex;

        uint16_t _blockWidth{};
        uint16_t _blockHeight{};
        uint32_t _columnCount{};
        uint32_t _rowCount{};

        std::vector<uint8_t> _blocks;
        uint32_t _screenWidth{};
        uint32_t _screenHeight{};

        uint32_t _lowestRow{ std::numeric_limits<uint32_t>::max() };
        uint32_t _highestRow{ 0 };

        uint32_t _lowestColumn{ std::numeric_limits<uint32_t>::max() };
        uint32_t _highestColumn{};

    public:
        uint32_t getRowCount() const noexcept;

        uint32_t getColumnCount() const noexcept;

        uint32_t getBlockWidth() const noexcept;

        uint32_t getBlockHeight() const noexcept;

        void reset(int32_t width, int32_t height, uint32_t blockWidth, uint32_t blockHeight) noexcept;

        void invalidate(int32_t left, int32_t top, int32_t right, int32_t bottom) noexcept;

        template<typename F>
        void traverseDirtyCells(F&& func)
        {
            std::lock_guard<std::mutex> lock(_mutex);

            const uint32_t columnCount = _columnCount;
            const uint32_t rowCount = _rowCount;
            const uint32_t blockWidth = _blockWidth;
            const uint32_t blockHeight = _blockHeight;
            auto& blocks = _blocks;

            const uint32_t startRow = rowCount ? std::clamp(_lowestRow, 0u, rowCount - 1) : 0;
            const uint32_t endRow = rowCount ? std::clamp(_highestRow, 0u, rowCount - 1) : 0;
            const uint32_t startCol = columnCount ? std::clamp(_lowestColumn, 0u, columnCount - 1) : 0;
            const uint32_t endCol = columnCount ? std::clamp(_highestColumn, 0u, columnCount - 1) : 0;

            for (uint32_t currentRow = startRow; currentRow <= endRow; ++currentRow)
            {
                for (uint32_t currentCol = startCol; currentCol <= endCol;)
                {
                    const uint32_t blockIndex = currentRow * columnCount + currentCol;

                    if (blocks[blockIndex] == 0)
                    {
                        ++currentCol;
                        continue;
                    }

                    // Horizontal merge.
                    uint32_t horizontalSpan = 1;
                    while (currentCol + horizontalSpan < columnCount
                           && blocks[currentRow * columnCount + currentCol + horizontalSpan] != 0)
                    {
                        ++horizontalSpan;
                    }

                    // Vertical merge.
                    uint32_t mergedWidth = horizontalSpan;
                    uint32_t verticalSpan = 0;
                    for (uint32_t rowOffset = 1; currentRow + rowOffset <= endRow; ++rowOffset)
                    {
                        uint32_t validWidth = 0;
                        while (validWidth < mergedWidth
                               && blocks[(currentRow + rowOffset) * columnCount + currentCol + validWidth] != 0)
                        {
                            ++validWidth;
                        }
                        if (validWidth == 0)
                            break;
                        mergedWidth = validWidth;
                        ++verticalSpan;
                    }

                    const uint32_t totalRows = verticalSpan + 1;

                    // Clear blocks
                    for (uint32_t y = currentRow; y < currentRow + totalRows; ++y)
                    {
                        for (uint32_t x = currentCol; x < currentCol + mergedWidth; ++x)
                        {
                            blocks[y * columnCount + x] = 0;
                        }
                    }

                    const uint32_t coordLeft = currentCol * blockWidth;
                    const uint32_t coordTop = currentRow * blockHeight;
                    const uint32_t coordRight = (currentCol + mergedWidth) * blockWidth;
                    const uint32_t coordBottom = (currentRow + totalRows) * blockHeight;

                    const uint32_t clampedRight = std::min(coordRight, _screenWidth);
                    const uint32_t clampedBottom = std::min(coordBottom, _screenHeight);

                    if (coordLeft < _screenWidth && coordTop < _screenHeight)
                    {
                        func(coordLeft, coordTop, clampedRight, clampedBottom);
                    }

                    // Skip processed columns
                    currentCol += mergedWidth;
                }
            }

            _lowestRow = std::numeric_limits<uint32_t>::max();
            _highestRow = 0;
            _lowestColumn = std::numeric_limits<uint32_t>::max();
            _highestColumn = 0;
        }
    };

} // namespace OpenRCT2::Drawing
