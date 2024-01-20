/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ScenarioPatcher.h"

#include "../Context.h"
#include "../Game.h"
#include "../PlatformEnvironment.h"
#include "../core/File.h"
#include "../core/Guard.hpp"
#include "../core/Json.hpp"
#include "../core/Path.hpp"
#include "../entity/EntityList.h"
#include "../entity/Guest.h"
#include "../entity/Peep.h"
#include "../ride/Ride.h"
#include "../ride/Track.h"
#include "../world/Footpath.h"
#include "../world/Location.hpp"
#include "../world/Map.h"
#include "../world/Surface.h"
#include "../world/tile_element/TileElementType.h"

#include <iostream>

static const std::string s_coordinatesKey = "coordinates";

// Land Ownership Keys
static const std::string s_cannotDowngradeKey = "cannot_downgrade";
static const std::string s_landOwnershipKey = "land_ownership";

// Water fix keys
static const std::string s_waterFixKey = "water";
static const std::string s_heightKey = "height";

// Tile fix keys
static const std::string s_operationsKey = "operations";
static const std::string s_fromKey = "from";
static const std::string s_toKey = "to";
static const std::string s_tilesKey = "tiles";
static const std::string s_typeKey = "type";

// Ride fix keys
static const std::string s_ridesKey = "rides";
static const std::string s_rideIdKey = "id";
static const std::string s_operationKey = "operation";

static u8string ToOwnershipJsonKey(int ownershipType)
{
    switch (ownershipType)
    {
        case OWNERSHIP_UNOWNED:
            return "unowned";
        case OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED:
            return "construction_rights_owned";
        case OWNERSHIP_OWNED:
            return "owned";
        case OWNERSHIP_CONSTRUCTION_RIGHTS_AVAILABLE:
            return "construction_rights_available";
        case OWNERSHIP_AVAILABLE:
            return "available";
    }
    Guard::Assert(0, "Unrecognized ownership type flag");
    return {};
}

static void ApplyLandOwnershipFixes(const json_t& landOwnershipFixes, int ownershipType)
{
    auto ownershipTypeKey = ToOwnershipJsonKey(ownershipType);
    if (!landOwnershipFixes.contains(ownershipTypeKey))
    {
        return;
    }

    auto ownershipParameters = landOwnershipFixes[ownershipTypeKey];
    if (!ownershipParameters.contains(s_coordinatesKey))
    {
        Guard::Assert(0, "Cannot have ownership fix without coordinates array");
        return;
    }
    else if (!ownershipParameters[s_coordinatesKey].is_array())
    {
        Guard::Assert(0, "Ownership fix coordinates should be an array");
        return;
    }

    auto ownershipCoords = Json::AsArray(ownershipParameters[s_coordinatesKey]);
    if (ownershipCoords.empty())
    {
        Guard::Assert(0, "Ownership fix coordinates array should not be empty");
        return;
    }

    const bool cannotDowngrade = ownershipParameters.contains(s_cannotDowngradeKey)
        ? Json::GetBoolean(ownershipParameters[s_cannotDowngradeKey], false)
        : false;
    for (size_t i = 0; i < ownershipCoords.size(); ++i)
    {
        if (!ownershipCoords[i].is_array())
        {
            Guard::Assert(0, "Ownership fix coordinates should contain only arrays");
            return;
        }

        auto coordinatesPair = Json::AsArray(ownershipCoords[i]);
        if (coordinatesPair.size() != 2)
        {
            Guard::Assert(0, "Ownership fix coordinates sub array should have 2 elements");
            return;
        }
        FixLandOwnershipTilesWithOwnership(
            { { Json::GetNumber<int32_t>(coordinatesPair[0]), Json::GetNumber<int32_t>(coordinatesPair[1]) } }, ownershipType,
            cannotDowngrade);
    }
}

static void ApplyLandOwnershipFixes(const json_t& scenarioPatch)
{
    if (!scenarioPatch.contains(s_landOwnershipKey))
    {
        return;
    }

    auto landOwnershipFixes = scenarioPatch[s_landOwnershipKey];
    for (const auto& ownershipType : { OWNERSHIP_UNOWNED, OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED, OWNERSHIP_OWNED,
                                       OWNERSHIP_CONSTRUCTION_RIGHTS_AVAILABLE, OWNERSHIP_AVAILABLE })
    {
        ApplyLandOwnershipFixes(landOwnershipFixes, ownershipType);
    }
}

static void ApplyWaterFixes(const json_t& scenarioPatch)
{
    if (!scenarioPatch.contains(s_waterFixKey))
    {
        return;
    }

    if (!scenarioPatch[s_waterFixKey].is_array())
    {
        Guard::Assert(0, "Water fix should be an array");
        return;
    }

    auto waterFixes = Json::AsArray(scenarioPatch[s_waterFixKey]);
    if (waterFixes.empty())
    {
        Guard::Assert(0, "Water fix array should not be empty");
        return;
    }

    for (size_t i = 0; i < waterFixes.size(); ++i)
    {
        if (!waterFixes[i].contains(s_heightKey))
        {
            Guard::Assert(0, "Water fix sub-array should set a height");
            return;
        }

        auto waterHeight = waterFixes[i][s_heightKey];

        if (!waterFixes[i].contains(s_coordinatesKey))
        {
            Guard::Assert(0, "Water fix sub-array should contain coordinates");
            return;
        }

        if (!waterFixes[i][s_coordinatesKey].is_array())
        {
            Guard::Assert(0, "Water fix coordinates sub-array should be an array");
            return;
        }

        auto coordinatesPairs = Json::AsArray(waterFixes[i][s_coordinatesKey]);
        if (coordinatesPairs.empty())
        {
            Guard::Assert(0, "Water fix coordinates sub-array should not be empty");
            return;
        }

        for (size_t j = 0; j < coordinatesPairs.size(); ++j)
        {
            if (!coordinatesPairs[j].is_array())
            {
                Guard::Assert(0, "Water fix coordinates should contain only arrays");
                return;
            }

            auto coordinatesPair = Json::AsArray(coordinatesPairs[j]);
            if (coordinatesPair.size() != 2)
            {
                Guard::Assert(0, "Water fix coordinates sub array should have 2 elements");
                return;
            }
            auto surfaceElement = MapGetSurfaceElementAt(
                TileCoordsXY{ Json::GetNumber<int32_t>(coordinatesPair[0]), Json::GetNumber<int32_t>(coordinatesPair[1]) });

            surfaceElement->SetWaterHeight(waterHeight);
        }
    }
}

static track_type_t toTrackType(const u8string_view trackTypeString)
{
    if (trackTypeString == "flat")
        return TrackElemType::Flat;
    else if (trackTypeString == "flat_covered")
        return TrackElemType::FlatCovered;
    else
    {
        Guard::Assert(0, "Unsupported track type conversion");
        return TrackElemType::None;
    }
}

static void ApplyTrackTypeFixes(const json_t& trackTilesFixes)
{
    if (!trackTilesFixes.contains(s_operationsKey))
    {
        Guard::Assert(0, "Cannot apply track tile fixes when operations array is unset");
        return;
    }

    if (!trackTilesFixes[s_operationsKey].is_array())
    {
        Guard::Assert(0, "Track tile fixes should have an operations array");
        return;
    }

    auto fixOperations = Json::AsArray(trackTilesFixes[s_operationsKey]);
    if (fixOperations.empty())
    {
        Guard::Assert(0, "Operations fix array should not be empty");
        return;
    }

    for (size_t i = 0; i < fixOperations.size(); ++i)
    {
        if (!fixOperations[i].contains(s_fromKey))
        {
            Guard::Assert(0, "Operation sub-array should contain a from key");
            return;
        }

        if (!fixOperations[i].contains(s_toKey))
        {
            Guard::Assert(0, "Operation sub-array should contain a to key");
            return;
        }

        auto fromTrackType = toTrackType(Json::GetString(fixOperations[i][s_fromKey]));
        auto destinationTrackType = toTrackType(Json::GetString(fixOperations[i][s_toKey]));

        if (!fixOperations[i].contains(s_coordinatesKey))
        {
            Guard::Assert(0, "Operations fix sub-array should contain coordinates");
            return;
        }

        if (!fixOperations[i][s_coordinatesKey].is_array())
        {
            Guard::Assert(0, "Operations fix coordinates sub-array should be an array");
            return;
        }

        auto coordinatesPairs = Json::AsArray(fixOperations[i][s_coordinatesKey]);
        if (coordinatesPairs.empty())
        {
            Guard::Assert(0, "Operations fix coordinates sub-array should not be empty");
            return;
        }

        for (size_t j = 0; j < coordinatesPairs.size(); ++j)
        {
            if (!coordinatesPairs[j].is_array())
            {
                Guard::Assert(0, "Operations fix coordinates should contain only arrays");
                return;
            }

            auto coordinatesPair = Json::AsArray(coordinatesPairs[j]);
            if (coordinatesPair.size() != 2)
            {
                Guard::Assert(0, "Operations fix coordinates sub array should have 2 elements");
                return;
            }

            TileCoordsXY tile{ Json::GetNumber<int32_t>(coordinatesPair[0]), Json::GetNumber<int32_t>(coordinatesPair[1]) };
            auto* tileElement = MapGetFirstElementAt(tile);
            if (tileElement == nullptr)
                continue;

            do
            {
                if (tileElement->GetType() != TileElementType::Track)
                    continue;

                auto* trackElement = tileElement->AsTrack();
                if (trackElement->GetTrackType() != fromTrackType)
                    continue;

                trackElement->SetTrackType(destinationTrackType);
            } while (!(tileElement++)->IsLastForTile());
        }
    }
}

static TileElementType toTileElementType(const u8string_view tileTypeString)
{
    if (tileTypeString == "track")
        return TileElementType::Track;
    else
    {
        Guard::Assert(0, "Unsupported tile type conversion");
        return TileElementType::Track;
    }
}

static void ApplyTileFixes(const json_t& scenarioPatch)
{
    if (!scenarioPatch.contains(s_tilesKey))
    {
        return;
    }

    auto tilesFixes = scenarioPatch[s_tilesKey];
    if (!tilesFixes.contains(s_typeKey))
    {
        Guard::Assert(0, "Cannot apply tile fixes without defined type");
    }
    else
    {
        auto tileType = toTileElementType(Json::GetString(tilesFixes[s_typeKey]));
        if (tileType == TileElementType::Track)
        {
            ApplyTrackTypeFixes(tilesFixes);
        }
    }
}

static void SwapRideEntranceAndExit(RideId rideId)
{
    auto ride = GetRide(rideId);
    if (ride == nullptr)
    {
        Guard::Assert(0, "Invalid Ride Id for SwapRideEntranceAndExit");
        return;
    }

    // First, make the queuing peep exit
    for (auto peep : EntityList<Guest>())
    {
        if (peep->State == PeepState::QueuingFront && peep->CurrentRide == rideId)
        {
            peep->RemoveFromQueue();
            peep->SetState(PeepState::Falling);
            break;
        }
    }

    // Now, swap the entrance and exit.
    if (ride != nullptr)
    {
        auto& station = ride->GetStation();
        auto entranceCoords = station.Exit;
        auto exitCoords = station.Entrance;
        station.Entrance = entranceCoords;
        station.Exit = exitCoords;

        auto entranceElement = MapGetRideExitElementAt(entranceCoords.ToCoordsXYZD(), false);
        entranceElement->SetEntranceType(ENTRANCE_TYPE_RIDE_ENTRANCE);
        auto exitElement = MapGetRideEntranceElementAt(exitCoords.ToCoordsXYZD(), false);
        exitElement->SetEntranceType(ENTRANCE_TYPE_RIDE_EXIT);

        // Trigger footpath update
        FootpathQueueChainReset();
        FootpathConnectEdges(
            entranceCoords.ToCoordsXY(), reinterpret_cast<TileElement*>(entranceElement),
            GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED);
        FootpathUpdateQueueChains();
    }
}

static void ApplyRideFixes(const json_t& scenarioPatch)
{
    if (!scenarioPatch.contains(s_ridesKey))
    {
        return;
    }

    if (!scenarioPatch[s_ridesKey].is_array())
    {
        Guard::Assert(0, "Ride fixes should be an array of arrays");
        return;
    }

    auto rideFixes = Json::AsArray(scenarioPatch[s_ridesKey]);
    if (rideFixes.empty())
    {
        Guard::Assert(0, "Ride fixes should not be an empty array");
        return;
    }

    for (size_t i = 0; i < rideFixes.size(); ++i)
    {
        if (!rideFixes[i].contains(s_rideIdKey))
        {
            Guard::Assert(0, "Ride fixes should contain a ride id");
            return;
        }

        if (!rideFixes[i].contains(s_operationKey))
        {
            Guard::Assert(0, "Ride fixes should contain a ride operation");
            return;
        }

        RideId rideId = RideId::FromUnderlying(Json::GetNumber<uint16_t>(rideFixes[i][s_rideIdKey]));
        auto operation = Json::GetString(rideFixes[i][s_operationKey]);
        if (operation == "swap_entrance_exit")
        {
            SwapRideEntranceAndExit(rideId);
        }
        else
        {
            Guard::Assert(0, "Unsupported ride fix operation");
        }
    }
}

static u8string GetPatchFileName(u8string_view scenarioName)
{
    auto env = OpenRCT2::GetContext()->GetPlatformEnvironment();
    auto scenarioPatches = env->GetDirectoryPath(OpenRCT2::DIRBASE::OPENRCT2, OpenRCT2::DIRID::SCENARIO_PATCHES);
    auto scenarioPatchFile = Path::WithExtension(Path::GetFileNameWithoutExtension(scenarioName), ".json");
    return Path::Combine(scenarioPatches, scenarioPatchFile);
}

void RCT12::FetchAndApplyScenarioPatch(u8string_view scenarioName, bool isScenario)
{
    auto patchPath = GetPatchFileName(scenarioName);
    std::cout << "Path is: " << patchPath << std::endl;
    // TODO: Check if case sensitive, some scenario names have all lowercase variations
    if (File::Exists(patchPath))
    {
        auto scenarioPatch = Json::ReadFromFile(patchPath);
        // TODO: Land ownership is applied even when loading saved scenario. Should it?
        ApplyLandOwnershipFixes(scenarioPatch);
        if (isScenario)
        {
            ApplyWaterFixes(scenarioPatch);
            ApplyTileFixes(scenarioPatch);
            ApplyRideFixes(scenarioPatch);
        }
    }
}
