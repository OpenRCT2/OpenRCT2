/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#include <gtest/gtest.h>
#include <openrct2/core/CircularBuffer.h>
#include <stdint.h>
#include <vector>

// CircularBuffer capacity.
constexpr size_t TEST_BUFFER_SIZE = 128;

// Amount of elements to push into buffer, will overwrite entire buffer 8 times.
constexpr size_t TEST_PUSH_COUNT = 1024;

TEST(CircularBufferTest, all)
{
    CircularBuffer<size_t, TEST_BUFFER_SIZE> buffer1;
    ASSERT_EQ(buffer1.capacity(), TEST_BUFFER_SIZE);

    // Mirror the effects of buffer1 into buffer2 to compare.
    std::vector<size_t> buffer2;

    // Create
    {
        for (size_t i = 0; i < TEST_PUSH_COUNT; i++)
        {
            buffer1.push_back(i);
            buffer2.push_back(i);

            // Mimic sliding window, always remove oldest element.
            if (buffer2.size() > buffer1.capacity())
            {
                buffer2.erase(buffer2.begin());
            }

            ASSERT_EQ(buffer1.size(), buffer2.size());
        }
    }

    ASSERT_EQ(buffer1.empty(), false);
    ASSERT_EQ(buffer1.size(), buffer2.size());

    // Compare contents.
    {
        ASSERT_EQ(buffer1.front(), buffer1[0]);
        ASSERT_EQ(buffer1.back(), buffer1[buffer1.size() - 1]);

        for (size_t i = 0; i < buffer1.size(); i++)
        {
            ASSERT_EQ(buffer1[i], buffer2[i]);
        }
    }

    // Clear
    {
        buffer1.clear();
        ASSERT_EQ(buffer1.empty(), true);
        ASSERT_EQ(buffer1.size(), size_t(0));
    }

    SUCCEED();
}
