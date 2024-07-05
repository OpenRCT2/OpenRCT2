/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>
#include <exception>

class UnsupportedVersionException : public std::exception
{
public:
    uint32_t const MinVersion;
    uint32_t const TargetVersion;

    explicit UnsupportedVersionException(uint32_t minVersion, uint32_t targetVersion)
        : MinVersion(minVersion)
        , TargetVersion(targetVersion)
    {
    }
};
