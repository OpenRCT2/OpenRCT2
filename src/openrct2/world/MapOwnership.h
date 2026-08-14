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

struct CoordsXYZ;

namespace OpenRCT2
{
    enum class OwnershipFlag : uint8_t
    {
        constructionRightsOwned,
        landOwned,
        constructionRightsForSale,
        landForSale,
    };
    using OwnershipFlags = FlagHolder<uint8_t, OwnershipFlag>;

    constexpr OwnershipFlags kUnowned = {};

    OwnershipFlags CheckMaxAllowableLandRightsForTile(const CoordsXYZ& tileMapPos);
} // namespace OpenRCT2
