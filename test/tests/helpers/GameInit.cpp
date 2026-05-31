/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "GameInit.hpp"

#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/ParkImporter.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/entity/EntityRegistry.h>
#include <openrct2/entity/EntityTweener.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/world/MapAnimation.h>
#include <string>

namespace TestHelpers
{
    using namespace OpenRCT2;

    void FinaliseLoadedGame(const bool retainSpatialIndices)
    {
        auto& gameState = getGameState();
        if (!retainSpatialIndices)
            gameState.entities.ResetEntitySpatialIndices();

        ResetAllSpriteQuadrantPlacements();
        LoadPalette();
        EntityTweener::Get().Reset();
        MapAnimations::MarkAllTiles();
        FixInvalidVehicleSpriteSizes();

        gGameSpeed = 1;
    }

    void LoadSavedGameFromFile(IContext& context, const std::string_view parkPath)
    {
        const auto pathStr = std::string(parkPath);
        const auto importer = ParkImporter::CreateS6(context.GetObjectRepository());
        const auto loadResult = importer->LoadSavedGame(pathStr.c_str(), false);
        context.GetObjectManager().LoadObjects(loadResult.RequiredObjects);

        MapAnimations::ClearAll();
        importer->Import(getGameState());

        FinaliseLoadedGame(false);
    }
} // namespace TestHelpers
