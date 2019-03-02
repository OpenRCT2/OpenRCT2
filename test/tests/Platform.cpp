/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <gtest/gtest.h>
#include <openrct2/platform/platform.h>

TEST(platform, sanitise_filename)
{
#ifndef _WIN32
    ASSERT_EQ("normal-filename.png", platform_sanitise_filename("normal-filename.png"));
    ASSERT_EQ("utf🎱", platform_sanitise_filename("utf🎱"));
    ASSERT_EQ("forbidden_char", platform_sanitise_filename("forbidden/char"));
    ASSERT_EQ("forbidden_\\:\"|?*chars", platform_sanitise_filename("forbidden/\\:\"|?*chars"));
    ASSERT_EQ(" non trimmed ", platform_sanitise_filename(" non trimmed "));
#else
    ASSERT_EQ("normal-filename.png", platform_sanitise_filename("normal-filename.png"));
    ASSERT_EQ("utf🎱", platform_sanitise_filename("utf🎱"));
    ASSERT_EQ("forbidden_char", platform_sanitise_filename("forbidden/char"));
    ASSERT_EQ("forbidden_______chars", platform_sanitise_filename("forbidden/\\:\"|?*chars"));
    ASSERT_EQ("non trimmed", platform_sanitise_filename(" non trimmed "));
#endif
}
