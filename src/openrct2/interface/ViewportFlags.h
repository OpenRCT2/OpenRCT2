/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

 #pragma once

#include "../core/FlagHolder.hpp"

namespace OpenRCT2
{
    // Flags must currenly retain their values to avoid breaking plugins.
    // Values can be changed when plugins move to using named constants.
    enum class ViewportFlag : uint32_t
    {
        gridlines = 7,
        undergroundInside = 0,
        hideBase = 12,
        hideVertical = 13,

        soundOn = 10,
        landOwnership = 8,
        constructionRights = 9,
        hideEntities = 14,
        clipView = 15,
        clipViewSeeThrough = 17,
        highlightPathIssues = 18,
        transparentBackground = 19,

        landHeights = 4,
        trackHeights = 5,
        pathHeights = 6,

        hideRides = 1,
        hideVehicles = 20,
        hideVegetation = 21,
        hideScenery = 2,
        hidePaths = 16,
        hideSupports = 3,
        hideGuests = 11,
        hideStaff = 23,

        invisibleRides = 24,
        invisibleVehicles = 25,
        invisibleVegetation = 26,
        invisibleScenery = 27,
        invisiblePaths = 28,
        invisibleSupports = 29,

        independentRotation = 30,
        renderingInhibited = 31,
    };
    using ViewportFlags = FlagHolder<uint32_t, ViewportFlag>;
}