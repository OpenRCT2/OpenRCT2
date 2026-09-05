/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <optional>
#include <string_view>

namespace OpenRCT2::RCT1
{
    /**
     * RollerCoaster Tycoon: Deluxe bundles several hundred fan-made track designs from the original
     * RCT1 Ride Exchange, but ships them under anonymous placeholder filenames such as "u(0010)"
     * rather than their real submitted names.
     *
     * The names below were recovered by cross-referencing the bundled track design files against:
     *  - byte-identical copies preserved in RCT1 fan track-design archives (exact SHA-256 match), and
     *  - the archived official Ride Exchange database (rollercoastertycoon.com, via the Wayback
     *    Machine), which stores each submission under the same numeric id as the "u(####)" filename.
     *
     * A small number of ids (the "u(Z###)" designs) could not be matched against either source and
     * are intentionally left out of this table; they keep their raw placeholder filename.
     *
     * Returns the recovered display name for a known RCT Deluxe track design, keyed by its raw
     * filename with the extension removed (e.g. "u(0010)"), or std::nullopt if rawName is not one of
     * these known designs.
     */
    std::optional<std::string_view> GetDeluxeTrackDesignName(std::string_view rawName);
} // namespace OpenRCT2::RCT1
