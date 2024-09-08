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

#ifdef DISABLE_NETWORK
#    include <picosha2.hpp>
#else
#    include "../core/Crypt.h"
#endif

#include <iostream>

static bool s_dryRun = false;

// Generic keys
static const std::string s_scenarioNameKey = "scenario_name";
static const std::string s_fullSHAKey = "sha256";
static const std::string s_coordinatesKey = "coordinates";

// Land Ownership Keys
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
    OpenRCT2::Guard::Assert(0, "Unrecognized ownership type flag");
    return {};
}

static std::vector<TileCoordsXY> getCoordinates(const json_t& parameters)
{
    if (!parameters.contains(s_coordinatesKey))
    {
        OpenRCT2::Guard::Assert(0, "Cannot have fix without coordinates array");
        return {};
    }
    else if (!parameters[s_coordinatesKey].is_array())
    {
        OpenRCT2::Guard::Assert(0, "Fix coordinates should be an array");
        return {};
    }

    auto coords = OpenRCT2::Json::AsArray(parameters[s_coordinatesKey]);
    if (coords.empty())
    {
        OpenRCT2::Guard::Assert(0, "Fix coordinates array should not be empty");
        return {};
    }

    std::vector<TileCoordsXY> parsedCoordinates;
    parsedCoordinates.reserve(coords.size());
    for (size_t i = 0; i < coords.size(); ++i)
    {
        if (!coords[i].is_array())
        {
            OpenRCT2::Guard::Assert(0, "Fix coordinates should contain only arrays");
            return {};
        }

        auto coordinatesPair = OpenRCT2::Json::AsArray(coords[i]);
        if (coordinatesPair.size() != 2)
        {
            OpenRCT2::Guard::Assert(0, "Fix coordinates sub array should have 2 elements");
            return {};
        }
        parsedCoordinates.emplace_back(
            OpenRCT2::Json::GetNumber<int32_t>(coordinatesPair[0]), OpenRCT2::Json::GetNumber<int32_t>(coordinatesPair[1]));
    }
    return parsedCoordinates;
}

static void ApplyLandOwnershipFixes(const json_t& landOwnershipFixes, int ownershipType)
{
    auto ownershipTypeKey = ToOwnershipJsonKey(ownershipType);
    if (!landOwnershipFixes.contains(ownershipTypeKey))
    {
        return;
    }

    auto ownershipParameters = landOwnershipFixes[ownershipTypeKey];
    auto coordinatesVector = getCoordinates(ownershipParameters);
    if (s_dryRun)
    {
        return;
    }
    FixLandOwnershipTilesWithOwnership(coordinatesVector, ownershipType);
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
        OpenRCT2::Guard::Assert(0, "Water fix should be an array");
        return;
    }

    auto waterFixes = OpenRCT2::Json::AsArray(scenarioPatch[s_waterFixKey]);
    if (waterFixes.empty())
    {
        OpenRCT2::Guard::Assert(0, "Water fix array should not be empty");
        return;
    }

    for (size_t i = 0; i < waterFixes.size(); ++i)
    {
        if (!waterFixes[i].contains(s_heightKey))
        {
            OpenRCT2::Guard::Assert(0, "Water fix sub-array should set a height");
            return;
        }
        if (s_dryRun)
        {
            continue;
        }

        auto waterHeight = waterFixes[i][s_heightKey];
        auto coordinatesVector = getCoordinates(waterFixes[i]);
        for (const auto& tile : coordinatesVector)
        {
            auto surfaceElement = MapGetSurfaceElementAt(tile);
            surfaceElement->SetWaterHeight(waterHeight);
        }
    }
}

static track_type_t toTrackType(const u8string_view trackTypeString)
{
    if (trackTypeString == "flat")
        return OpenRCT2::TrackElemType::Flat;
    else if (trackTypeString == "flat_covered")
        return OpenRCT2::TrackElemType::FlatCovered;
    else
    {
        OpenRCT2::Guard::Assert(0, "Unsupported track type conversion");
        return OpenRCT2::TrackElemType::None;
    }
}

static void ApplyTrackTypeFixes(const json_t& trackTilesFixes)
{
    if (!trackTilesFixes.contains(s_operationsKey))
    {
        OpenRCT2::Guard::Assert(0, "Cannot apply track tile fixes when operations array is unset");
        return;
    }

    if (!trackTilesFixes[s_operationsKey].is_array())
    {
        OpenRCT2::Guard::Assert(0, "Track tile fixes should have an operations array");
        return;
    }

    auto fixOperations = OpenRCT2::Json::AsArray(trackTilesFixes[s_operationsKey]);
    if (fixOperations.empty())
    {
        OpenRCT2::Guard::Assert(0, "Operations fix array should not be empty");
        return;
    }

    for (size_t i = 0; i < fixOperations.size(); ++i)
    {
        if (!fixOperations[i].contains(s_fromKey))
        {
            OpenRCT2::Guard::Assert(0, "Operation sub-array should contain a from key");
            return;
        }

        if (!fixOperations[i].contains(s_toKey))
        {
            OpenRCT2::Guard::Assert(0, "Operation sub-array should contain a to key");
            return;
        }

        auto fromTrackType = toTrackType(OpenRCT2::Json::GetString(fixOperations[i][s_fromKey]));
        auto destinationTrackType = toTrackType(OpenRCT2::Json::GetString(fixOperations[i][s_toKey]));
        auto coordinatesVector = getCoordinates(fixOperations[i]);

        if (s_dryRun)
        {
            continue;
        }

        for (const auto& tile : coordinatesVector)
        {
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
        OpenRCT2::Guard::Assert(0, "Unsupported tile type conversion");
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
        OpenRCT2::Guard::Assert(0, "Cannot apply tile fixes without defined type");
    }
    else
    {
        auto tileType = toTileElementType(OpenRCT2::Json::GetString(tilesFixes[s_typeKey]));
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
        OpenRCT2::Guard::Assert(0, "Invalid Ride Id for SwapRideEntranceAndExit");
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
        OpenRCT2::Guard::Assert(0, "Ride fixes should be an array of arrays");
        return;
    }

    auto rideFixes = OpenRCT2::Json::AsArray(scenarioPatch[s_ridesKey]);
    if (rideFixes.empty())
    {
        OpenRCT2::Guard::Assert(0, "Ride fixes should not be an empty array");
        return;
    }

    for (size_t i = 0; i < rideFixes.size(); ++i)
    {
        if (!rideFixes[i].contains(s_rideIdKey))
        {
            OpenRCT2::Guard::Assert(0, "Ride fixes should contain a ride id");
            return;
        }

        if (!rideFixes[i].contains(s_operationKey))
        {
            OpenRCT2::Guard::Assert(0, "Ride fixes should contain a ride operation");
            return;
        }

        RideId rideId = RideId::FromUnderlying(OpenRCT2::Json::GetNumber<uint16_t>(rideFixes[i][s_rideIdKey]));
        auto operation = OpenRCT2::Json::GetString(rideFixes[i][s_operationKey]);

        if (s_dryRun)
        {
            continue;
        }

        if (operation == "swap_entrance_exit")
        {
            SwapRideEntranceAndExit(rideId);
        }
        else
        {
            OpenRCT2::Guard::Assert(0, "Unsupported ride fix operation");
        }
    }
}

static u8string getScenarioSHA256(u8string_view scenarioPath)
{
    auto env = OpenRCT2::GetContext()->GetPlatformEnvironment();
    auto scenarioData = OpenRCT2::File::ReadAllBytes(scenarioPath);
#ifdef DISABLE_NETWORK
    auto scenarioStringHash = picosha2::hash256_hex_string(scenarioData);
#else
    auto scenarioHash = OpenRCT2::Crypt::SHA256(scenarioData.data(), scenarioData.size());
    auto scenarioStringHash = OpenRCT2::String::StringFromHex(scenarioHash);
#endif
    LOG_INFO("Fetching patch\n  Scenario: '%s'\n  SHA '%s'", scenarioPath.data(), scenarioStringHash.c_str());
    return scenarioStringHash;
}

static u8string GetPatchFileName(u8string_view scenarioHash)
{
    auto env = OpenRCT2::GetContext()->GetPlatformEnvironment();
    auto scenarioPatches = env->GetDirectoryPath(OpenRCT2::DIRBASE::OPENRCT2, OpenRCT2::DIRID::SCENARIO_PATCHES);
    auto scenarioPatchFile = OpenRCT2::Path::WithExtension(scenarioHash.substr(0, 7), ".parkpatch");
    return OpenRCT2::Path::Combine(scenarioPatches, scenarioPatchFile);
}

static bool ValidateSHA256(const json_t& scenarioPatch, u8string_view scenarioHash)
{
    if (s_dryRun)
    {
        return true;
    }

    if (!scenarioPatch.contains(s_scenarioNameKey))
    {
        OpenRCT2::Guard::Assert(0, "All .parkpatch files should contain the name of the original scenario");
        return false;
    }

    if (!scenarioPatch.contains(s_fullSHAKey))
    {
        OpenRCT2::Guard::Assert(0, "All .parkpatch files should contain the sha256 of the original scenario");
        return false;
    }

    auto scenarioName = OpenRCT2::Json::GetString(scenarioPatch[s_scenarioNameKey]);
    auto scenarioSHA = OpenRCT2::Json::GetString(scenarioPatch[s_fullSHAKey]);
    LOG_INFO(
        "\n  Scenario '%s'\n  SHA '%s'\n  SHA Valid: %d", scenarioName.c_str(), scenarioSHA.c_str(),
        (scenarioHash == scenarioSHA));

    return scenarioSHA == scenarioHash;
}

void OpenRCT2::RCT12::ApplyScenarioPatch(u8string_view scenarioPatchFile, u8string scenarioSHA)
{
    auto scenarioPatch = OpenRCT2::Json::ReadFromFile(scenarioPatchFile);
    if (!ValidateSHA256(scenarioPatch, scenarioSHA))
    {
        OpenRCT2::Guard::Assert(0, "Invalid full SHA256. Check for shortened SHA collision");
        return;
    }
    ApplyLandOwnershipFixes(scenarioPatch);
    ApplyWaterFixes(scenarioPatch);
    ApplyTileFixes(scenarioPatch);
    ApplyRideFixes(scenarioPatch);
}

void OpenRCT2::RCT12::FetchAndApplyScenarioPatch(u8string_view scenarioPath)
{
    if (scenarioPath.empty())
    {
        return;
    }

    auto scenarioSHA = getScenarioSHA256(scenarioPath);
    auto patchPath = GetPatchFileName(scenarioSHA);
    if (OpenRCT2::File::Exists(patchPath))
    {
        ApplyScenarioPatch(patchPath, scenarioSHA);
    }
}

void OpenRCT2::RCT12::SetDryRun(bool enable)
{
    s_dryRun = enable;
}
