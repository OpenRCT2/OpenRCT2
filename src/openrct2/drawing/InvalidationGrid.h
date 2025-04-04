#pragma once

#include <algorithm>
#include <cstdint>

namespace OpenRCT2::Drawing
{
    class InvalidationGrid
    {
        uint16_t _blockWidth{};
        uint16_t _blockHeight{};
        uint32_t _columnCount{};
        uint32_t _rowCount{};

        uint8_t _blocks[7500]{};
        uint32_t _screenWidth{};
        uint32_t _screenHeight{};

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
            const auto columnCount = _columnCount;
            const auto rowCount = _rowCount;
            const auto blockWidth = _blockWidth;
            const auto blockHeight = _blockHeight;
            auto& blocks = _blocks;

            for (uint32_t column = 0; column < columnCount; column++)
            {
                for (uint32_t row = 0; row < rowCount; row++)
                {
                    const auto rowStartOffset = row * columnCount;
                    if (blocks[rowStartOffset + column] != 0)
                    {
                        uint32_t rowEndOffset = rowStartOffset;
                        uint32_t numRowsDirty = 0;

                        // Count amount of dirty rows at current column.
                        while (true)
                        {
                            if (row + numRowsDirty + 1 >= rowCount || blocks[rowEndOffset + column + columnCount] == 0)
                            {
                                break;
                            }

                            numRowsDirty++;
                            rowEndOffset += columnCount;
                        }

                        // Clear rows at the current column.
                        for (auto rowOffset = rowStartOffset; rowOffset <= rowEndOffset; rowOffset += columnCount)
                        {
                            blocks[rowOffset + column] = 0;
                        }

                        // Convert to pixel coordinates.
                        const auto left = column * blockWidth;
                        const auto top = row * blockHeight;
                        const auto right = (column + 1) * blockWidth;
                        const auto bottom = (row + numRowsDirty + 1) * blockHeight;

                        if (left < _screenWidth && top < _screenHeight)
                        {
                            func(left, top, std::min(right, _screenWidth), std::min(bottom, _screenHeight));
                        }
                    }
                }
            }
        }
    };

} // namespace OpenRCT2::Drawing
