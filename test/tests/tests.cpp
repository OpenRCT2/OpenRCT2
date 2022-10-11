/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

// This serves as the entry point when building for MSVC which compiles gtest
// directly into the test binary.
#ifdef _MSC_VER

#    include <gtest/gtest.h>
#    include <openrct2/core/Guard.hpp>

int main(int argc, char** argv)
{
    // Abort on an assertions so the tests do not hang
    Guard::SetAssertBehaviour(ASSERT_BEHAVIOUR::ABORT);

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#endif
