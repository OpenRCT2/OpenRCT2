/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TestData.h"

#include <openrct2/core/Path.hpp>

namespace TestData
{
    std::string GetBasePath()
    {
        return "testdata";
    }

    std::string GetParkPath(std::string name)
    {
        std::string path = Path::Combine(GetBasePath(), u8"parks", name);
        return path;
    }
} // namespace TestData
