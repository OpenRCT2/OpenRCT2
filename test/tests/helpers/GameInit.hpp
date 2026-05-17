/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2/Context.h>
#include <string_view>

namespace TestHelpers
{
    // Run the post-Import initialisation steps used by every test that loads a save: reset
    // entity spatial indices (unless `retainSpatialIndices` is true), reset sprite quadrant
    // placements, reload the palette, reset the entity tweener, mark every tile for animation,
    // fix invalid vehicle sprite sizes, and set game speed to 1
    void FinaliseLoadedGame(bool retainSpatialIndices = false);

    // Load a saved S6 game from disk into the given context's GameState and run FinaliseLoadedGame
    void LoadSavedGameFromFile(OpenRCT2::IContext& context, std::string_view parkPath);
} // namespace TestHelpers
