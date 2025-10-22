/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideMode.h"

using namespace OpenRCT2;

// Convenience methods
RideMode RideModes::FromID(RideModeID index)
{
    return RideModes::kOperatingModes[static_cast<uint8_t>(index)];
}

RideMode RideModes::FromIndex(uint8_t index)
{
    return RideModes::kOperatingModes[index];
}

uint8_t RideModes::ToIndex(RideMode mode)
{
    return static_cast<uint8_t>(mode.Index);
}

uint64_t RideModes::ToFlags(std::vector<RideMode> modes)
{
    uint64_t result = 0;
    for (uint8_t i = 0; i < static_cast<uint8_t>(modes.size()); i++)
    {
        result |= ToIndex(modes.at(i));
    }
    return result;
}

RideMode RideModes::GetBlockSectionedCounterpart(RideMode originalMode)
{
    switch (originalMode.Index)
    {
        case RideModeID::CONTINUOUS_CIRCUIT:
            return RideModes::continuousCircuitBlockSectioned;
        case RideModeID::POWERED_LAUNCH:
        case RideModeID::POWERED_LAUNCH_PASSTHROUGH:
            return RideModes::poweredLaunchBlockSectioned;
        default:
            return originalMode;
    }
}

RideMode RideModes::GetNonBlockSectionedCounterpart(RideMode originalMode, RideModeID defaultMode)
{
    switch (originalMode.Index)
    {
        case RideModeID::CONTINUOUS_CIRCUIT_BLOCK_SECTIONED:
            return RideModes::continuousCircuit;
        case RideModeID::POWERED_LAUNCH_BLOCK_SECTIONED:
            return FromID(defaultMode);
        default:
            return originalMode;
    }
}
