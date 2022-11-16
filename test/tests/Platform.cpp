/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <gtest/gtest.h>
#include <openrct2/platform/Platform.h>

TEST(platform, sanitise_filename)
{
    ASSERT_EQ("normal-filename.png", Platform::SanitiseFilename("normal-filename.png"));
    ASSERT_EQ("utf🎱", Platform::SanitiseFilename("utf🎱"));
    ASSERT_EQ("forbidden_char", Platform::SanitiseFilename("forbidden/char"));
    ASSERT_EQ("non trimmed", Platform::SanitiseFilename(" non trimmed "));
#ifndef _WIN32
    ASSERT_EQ("forbidden_\\:\"|?*chars", Platform::SanitiseFilename("forbidden/\\:\"|?*chars"));
#else
    ASSERT_EQ("forbidden_______chars", Platform::SanitiseFilename("forbidden/\\:\"|?*chars"));
#endif
}
