/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include "../actions/FootpathPlaceAction.h"
#include "../actions/GameActionResult.h"
#include "../core/File.h"
#include "../core/Guard.hpp"
#include "../core/Json.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../entity/EntityList.h"
#include "../entity/Guest.h"
#include "../entity/Peep.h"
#include "../object/ObjectManager.h"
#include "../ride/Ride.h"
#include "../ride/Track.h"
#include "../world/Footpath.h"
#include "../world/Location.hpp"
#include "../world/Map.h"
#include "../world/tile_element/EntranceElement.h"
#include "../world/tile_element/PathElement.h"
#include "../world/tile_element/Slope.h"
#include "../world/tile_element/SurfaceElement.h"
#include "../world/tile_element/TileElement.h"
#include "../world/tile_element/TileElementType.h"
#include "../world/tile_element/TrackElement.h"

#ifdef DISABLE_NETWORK
    #include <picosha2.hpp>
#else
    #include "../core/Crypt.h"
#endif

#include <iostream>

using namespace OpenRCT2;
using OpenRCT2::GameActions::CommandFlag;

static bool _dryRun = false;

// Generic keys
static const std::string _scenarioNameKey = "scenario_name";
static const std::string _fullSHAKey = "sha256";
static const std::string _coordinatesKey = "coordinates";

// Land Ownership Keys
static const std::string _landOwnershipKey = "land_ownership";

// Water fix keys
static const std::string _waterFixKey = "water";
static const std::string _heightKey = "height";

// Tile fix keys
static const std::string _operationsKey = "operations";
static const std::string _fromKey = "from";
static const std::string _toKey = "to";
static const std::string _tilesKey = "tiles";
static const std::string _typeKey = "type";

// Surface fix keys
static const std::string _surfacesKey = "surfaces";
static const std::string _destinationSurface = "to_surface";

// Elements to be removed keys
static const std::string _elementsToDelete = "elements_to_delete";
static const std::string _element_index = "element_index";

// Ride fix keys
static const std::string _ridesKey = "rides";
static const std::string _rideIdKey = "id";
static const std::string _operationKey = "operation";

// Path fix keys
static const std::string _pathsKey = "paths";
static const std::string _railingsKey = "railings";
static const std::string _surfaceKey = "surface";
static const std::string _directionKey = "slope_direction";
static const std::string _isQueue = "queue";

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
    Guard::Assert(false, "Unrecognized ownership type flag");
    return {};
}

static void readCoordinate(std::vector<TileCoordsXY>& out, const json_t& coordinatesArray)
{
    if (coordinatesArray.size() != 2)
    {
        Guard::Assert(false, "Fix coordinates sub array should have 2 elements");
        return;
    }

    out.emplace_back(Json::GetNumber<int32_t>(coordinatesArray[0]), Json::GetNumber<int32_t>(coordinatesArray[1]));
}

static void readCoordinate(std::vector<TileCoordsXYZ>& out, const json_t& coordinatesArray)
{
    if (coordinatesArray.size() != 3)
    {
        Guard::Assert(false, "Fix coordinates sub array should have 3 elements");
        return;
    }

    out.emplace_back(
        Json::GetNumber<int32_t>(coordinatesArray[0]), Json::GetNumber<int32_t>(coordinatesArray[1]),
        Json::GetNumber<int32_t>(coordinatesArray[2]));
}

template<typename TTileCoords = TileCoordsXY>
static std::vector<TTileCoords> getCoordinates(const json_t& parameters)
{
    if (!parameters.contains(_coordinatesKey))
    {
        Guard::Assert(false, "Cannot have fix without coordinates array");
        return {};
    }
    else if (!parameters[_coordinatesKey].is_array())
    {
        Guard::Assert(false, "Fix coordinates should be an array");
        return {};
    }

    auto coords = Json::AsArray(parameters[_coordinatesKey]);
    if (coords.empty())
    {
        Guard::Assert(false, "Fix coordinates array should not be empty");
        return {};
    }

    std::vector<TTileCoords> parsedCoordinates;
    parsedCoordinates.reserve(coords.size());
    for (size_t i = 0; i < coords.size(); ++i)
    {
        if (!coords[i].is_array())
        {
            Guard::Assert(false, "Fix coordinates should contain only arrays");
            return {};
        }

        auto coordinatesArray = Json::AsArray(coords[i]);
        readCoordinate(parsedCoordinates, coordinatesArray);
    }
    return parsedCoordinates;
}

static Direction GetDirection(const json_t& parameters)
{
    if (!parameters.contains(_directionKey))
    {
        return kInvalidDirection;
    }
    else if (!parameters[_directionKey].is_number())
    {
        Guard::Assert(false, "Fix direction must be a number");
        return kInvalidDirection;
    }

    Direction direction = Json::GetNumber<Direction>(parameters[_directionKey]);

    if (direction > 3)
    {
        Guard::Assert(false, "Direction must be between 0 and 3");
        return kInvalidDirection;
    }

    return direction;
}

static bool IsQueue(const json_t& parameters)
{
    if (!parameters.contains(_isQueue))
    {
        return false;
    }
    else if (!parameters[_isQueue].is_boolean())
    {
        Guard::Assert(false, "queue must be a boolean");
        return false;
    }
    else
    {
        return Json::GetBoolean(parameters[_isQueue]);
    }
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
    if (_dryRun)
    {
        return;
    }
    FixLandOwnershipTilesWithOwnership(coordinatesVector, ownershipType);
}

static void ApplyLandOwnershipFixes(const json_t& scenarioPatch)
{
    if (!scenarioPatch.contains(_landOwnershipKey))
    {
        return;
    }

    auto landOwnershipFixes = scenarioPatch[_landOwnershipKey];
    for (const auto& ownershipType : { OWNERSHIP_UNOWNED, OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED, OWNERSHIP_OWNED,
                                       OWNERSHIP_CONSTRUCTION_RIGHTS_AVAILABLE, OWNERSHIP_AVAILABLE })
    {
        ApplyLandOwnershipFixes(landOwnershipFixes, ownershipType);
    }
}

static void ApplyWaterFixes(const json_t& scenarioPatch)
{
    if (!scenarioPatch.contains(_waterFixKey))
    {
        return;
    }

    if (!scenarioPatch[_waterFixKey].is_array())
    {
        Guard::Assert(false, "Water fix should be an array");
        return;
    }

    auto waterFixes = Json::AsArray(scenarioPatch[_waterFixKey]);
    if (waterFixes.empty())
    {
        Guard::Assert(false, "Water fix array should not be empty");
        return;
    }

    for (size_t i = 0; i < waterFixes.size(); ++i)
    {
        if (!waterFixes[i].contains(_heightKey))
        {
            Guard::Assert(false, "Water fix sub-array should set a height");
            return;
        }
        if (_dryRun)
        {
            continue;
        }

        auto waterHeight = waterFixes[i][_heightKey];
        auto coordinatesVector = getCoordinates(waterFixes[i]);
        for (const auto& tile : coordinatesVector)
        {
            auto surfaceElement = MapGetSurfaceElementAt(tile);
            surfaceElement->SetWaterHeight(waterHeight);
        }
    }
}

static TrackElemType toTrackType(const u8string_view trackTypeString)
{
    if (trackTypeString == "flat")
        return TrackElemType::flat;
    else if (trackTypeString == "flat_covered")
        return TrackElemType::flatCovered;
    else
    {
        Guard::Assert(false, "Unsupported track type conversion");
        return TrackElemType::none;
    }
}

static void ApplyTrackTypeFixes(const json_t& trackTilesFixes)
{
    if (!trackTilesFixes.contains(_operationsKey))
    {
        Guard::Assert(false, "Cannot apply track tile fixes when operations array is unset");
        return;
    }

    if (!trackTilesFixes[_operationsKey].is_array())
    {
        Guard::Assert(false, "Track tile fixes should have an operations array");
        return;
    }

    auto fixOperations = Json::AsArray(trackTilesFixes[_operationsKey]);
    if (fixOperations.empty())
    {
        Guard::Assert(false, "Operations fix array should not be empty");
        return;
    }

    for (size_t i = 0; i < fixOperations.size(); ++i)
    {
        if (!fixOperations[i].contains(_fromKey))
        {
            Guard::Assert(false, "Operation sub-array should contain a from key");
            return;
        }

        if (!fixOperations[i].contains(_toKey))
        {
            Guard::Assert(false, "Operation sub-array should contain a to key");
            return;
        }

        auto fromTrackType = toTrackType(Json::GetString(fixOperations[i][_fromKey]));
        auto destinationTrackType = toTrackType(Json::GetString(fixOperations[i][_toKey]));
        auto coordinatesVector = getCoordinates(fixOperations[i]);

        if (_dryRun)
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
        Guard::Assert(false, "Unsupported tile type conversion");
        return TileElementType::Track;
    }
}

static void ApplyTileFixes(const json_t& scenarioPatch)
{
    if (!scenarioPatch.contains(_tilesKey))
    {
        return;
    }

    auto tilesFixes = scenarioPatch[_tilesKey];
    if (!tilesFixes.contains(_typeKey))
    {
        Guard::Assert(false, "Cannot apply tile fixes without defined type");
    }
    else
    {
        auto tileType = toTileElementType(Json::GetString(tilesFixes[_typeKey]));
        if (tileType == TileElementType::Track)
        {
            ApplyTrackTypeFixes(tilesFixes);
        }
    }
}

static void ApplySurfaceFixes(const json_t& scenarioPatch)
{
    if (!scenarioPatch.contains(_surfacesKey))
    {
        return;
    }

    if (!scenarioPatch[_surfacesKey].is_array())
    {
        Guard::Assert(false, "Surface fix should be an array");
        return;
    }

    auto surfaceFixes = Json::AsArray(scenarioPatch[_surfacesKey]);
    if (surfaceFixes.empty())
    {
        Guard::Assert(false, "Surface fix array should not be empty");
        return;
    }

    for (size_t i = 0; i < surfaceFixes.size(); ++i)
    {
        if (!surfaceFixes[i].contains(_destinationSurface))
        {
            Guard::Assert(false, "Surface fix sub-array should set a destination surface");
            return;
        }
        if (_dryRun)
        {
            continue;
        }

        auto destinationSurface = Json::GetString(surfaceFixes[i][_destinationSurface]);
        auto& objectManager = GetContext()->GetObjectManager();
        auto surfaceObj = objectManager.GetLoadedObject(ObjectEntryDescriptor::Parse(destinationSurface));
        if (surfaceObj == nullptr)
        {
            Guard::Assert(false, "Surface object not found");
            return;
        }

        auto surfaceObjIndex = objectManager.GetLoadedObjectEntryIndex(surfaceObj);
        auto coordinatesVector = getCoordinates(surfaceFixes[i]);
        for (const auto& tile : coordinatesVector)
        {
            auto surfaceElement = MapGetSurfaceElementAt(tile);
            surfaceElement->SetSurfaceObjectIndex(surfaceObjIndex);
        }
    }
}

static void RemoveTileElements(const json_t& scenarioPatch)
{
    if (!scenarioPatch.contains(_elementsToDelete))
    {
        return;
    }

    if (!scenarioPatch[_elementsToDelete].is_array())
    {
        Guard::Assert(false, "Elements to delete should be an array");
        return;
    }

    auto elementsToDelete = Json::AsArray(scenarioPatch[_elementsToDelete]);
    if (elementsToDelete.empty())
    {
        Guard::Assert(false, "Elements to delete should not be empty");
        return;
    }

    for (size_t i = 0; i < elementsToDelete.size(); ++i)
    {
        if (!elementsToDelete[i].contains(_element_index))
        {
            Guard::Assert(false, "Elements to delete sub-array should set an element_index");
            return;
        }
        auto elementIndex = elementsToDelete[i][_element_index];
        auto coordinatesVector = getCoordinates(elementsToDelete[i]);
        if (_dryRun)
        {
            return;
        }

        for (const auto& tile : coordinatesVector)
        {
            auto tileElement = MapGetNthElementAt(tile.ToCoordsXY(), elementIndex);
            if (tileElement == nullptr)
            {
                Guard::Assert(false, "Invalid Nth element at tile");
                return;
            }
            else
            {
                ClearElementAt(tile.ToCoordsXY(), &tileElement);
            }
        }
    }
}

static void SwapRideEntranceAndExit(RideId rideId)
{
    auto ride = GetRide(rideId);
    if (ride == nullptr)
    {
        Guard::Assert(false, "Invalid Ride Id for SwapRideEntranceAndExit");
        return;
    }

    // First, make the queuing peep exit
    for (auto peep : EntityList<Guest>())
    {
        if (peep->State == PeepState::queuingFront && peep->CurrentRide == rideId)
        {
            peep->RemoveFromQueue();
            peep->SetState(PeepState::falling);
            break;
        }
    }

    // Now, swap the entrance and exit.
    if (ride != nullptr)
    {
        auto& station = ride->getStation();
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
            { CommandFlag::apply, CommandFlag::allowDuringPaused });
        FootpathUpdateQueueChains();
    }
}

static void ApplyRideFixes(const json_t& scenarioPatch)
{
    if (!scenarioPatch.contains(_ridesKey))
    {
        return;
    }

    if (!scenarioPatch[_ridesKey].is_array())
    {
        Guard::Assert(false, "Ride fixes should be an array of arrays");
        return;
    }

    auto rideFixes = Json::AsArray(scenarioPatch[_ridesKey]);
    if (rideFixes.empty())
    {
        Guard::Assert(false, "Ride fixes should not be an empty array");
        return;
    }

    for (size_t i = 0; i < rideFixes.size(); ++i)
    {
        if (!rideFixes[i].contains(_rideIdKey))
        {
            Guard::Assert(false, "Ride fixes should contain a ride id");
            return;
        }

        if (!rideFixes[i].contains(_operationKey))
        {
            Guard::Assert(false, "Ride fixes should contain a ride operation");
            return;
        }

        RideId rideId = RideId::FromUnderlying(Json::GetNumber<uint16_t>(rideFixes[i][_rideIdKey]));
        auto operation = Json::GetString(rideFixes[i][_operationKey]);

        if (_dryRun)
        {
            continue;
        }

        if (operation == "swap_entrance_exit")
        {
            SwapRideEntranceAndExit(rideId);
        }
        else
        {
            Guard::Assert(false, "Unsupported ride fix operation");
        }
    }
}

static void ApplyPathFixes(const json_t& scenarioPatch)
{
    if (!scenarioPatch.contains(_pathsKey))
    {
        return;
    }

    if (!scenarioPatch[_pathsKey].is_array())
    {
        Guard::Assert(false, "Path fixes should be an array of arrays");
        return;
    }

    auto pathFixes = Json::AsArray(scenarioPatch[_pathsKey]);
    if (pathFixes.empty())
    {
        Guard::Assert(false, "Path fixes should not be an empty array");
        return;
    }

    for (size_t i = 0; i < pathFixes.size(); ++i)
    {
        auto pathFix = pathFixes[i];

        if (!pathFix.contains(_railingsKey))
        {
            Guard::Assert(false, "Path fixes should have railings");
            return;
        }

        if (!pathFix.contains(_surfaceKey))
        {
            Guard::Assert(false, "Path fixes should have a surface");
            return;
        }

        auto railings = Json::GetString(pathFix[_railingsKey]);
        auto surface = Json::GetString(pathFix[_surfaceKey]);

        if (_dryRun)
        {
            continue;
        }

        auto& objectManager = GetContext()->GetObjectManager();
        auto railingsObjIndex = objectManager.GetLoadedObjectEntryIndex(railings);
        auto surfaceObjIndex = objectManager.GetLoadedObjectEntryIndex(surface);

        if (railingsObjIndex == kObjectEntryIndexNull)
        {
            Guard::Assert(false, "Railings object not found");
            return;
        }

        if (surfaceObjIndex == kObjectEntryIndexNull)
        {
            Guard::Assert(false, "Surface object not found");
            return;
        }

        auto coordinates = getCoordinates<TileCoordsXYZ>(pathFix);
        Direction direction = GetDirection(pathFix);
        PathConstructFlags constructionFlags = IsQueue(pathFix) ? PathConstructFlag::IsQueue : 0;

        for (auto coordinate : coordinates)
        {
            auto slope = FootpathSlope(FootpathSlopeType::flat, 0);
            if (direction != kInvalidDirection)
                slope = { FootpathSlopeType::sloped, direction };
            auto footpathPlaceAction = GameActions::FootpathPlaceAction(
                coordinate.ToCoordsXYZ(), slope, surfaceObjIndex, railingsObjIndex, direction, constructionFlags);
            auto& gameState = getGameState();
            auto result = footpathPlaceAction.Execute(gameState);
            if (result.error != GameActions::Status::ok)
            {
                Guard::Assert(false, "Could not patch path");
            }
        }
    }
}

static u8string getScenarioSHA256(u8string_view scenarioPath)
{
    auto scenarioData = File::ReadAllBytes(scenarioPath);
#ifdef DISABLE_NETWORK
    auto scenarioStringHash = picosha2::hash256_hex_string(scenarioData);
#else
    auto scenarioHash = Crypt::SHA256(scenarioData.data(), scenarioData.size());
    auto scenarioStringHash = String::StringFromHex(scenarioHash);
#endif
    LOG_VERBOSE("Fetching patch\n  Scenario: '%s'\n  SHA '%s'", scenarioPath.data(), scenarioStringHash.c_str());
    return scenarioStringHash;
}

static u8string GetPatchFileName(u8string_view scenarioHash)
{
    auto& env = GetContext()->GetPlatformEnvironment();
    auto scenarioPatches = env.GetDirectoryPath(DirBase::openrct2, DirId::scenarioPatches);
    auto scenarioPatchFile = Path::WithExtension(scenarioHash.substr(0, 7), ".parkpatch");
    return Path::Combine(scenarioPatches, scenarioPatchFile);
}

static bool ValidateSHA256(const json_t& scenarioPatch, u8string_view scenarioHash)
{
    if (_dryRun)
    {
        return true;
    }

    if (!scenarioPatch.contains(_scenarioNameKey))
    {
        Guard::Assert(false, "All .parkpatch files should contain the name of the original scenario");
        return false;
    }

    if (!scenarioPatch.contains(_fullSHAKey))
    {
        Guard::Assert(false, "All .parkpatch files should contain the sha256 of the original scenario");
        return false;
    }

    auto scenarioName = Json::GetString(scenarioPatch[_scenarioNameKey]);
    auto scenarioSHA = Json::GetString(scenarioPatch[_fullSHAKey]);
    LOG_VERBOSE(
        "\n  Scenario '%s'\n  SHA '%s'\n  SHA Valid: %d", scenarioName.c_str(), scenarioSHA.c_str(),
        (scenarioHash == scenarioSHA));

    return scenarioSHA == scenarioHash;
}

void RCT12::ApplyScenarioPatch(u8string_view scenarioPatchFile, u8string scenarioSHA)
{
    auto scenarioPatch = Json::ReadFromFile(scenarioPatchFile);
    if (!ValidateSHA256(scenarioPatch, scenarioSHA))
    {
        Guard::Assert(false, "Invalid full SHA256. Check for shortened SHA collision");
        return;
    }
    ApplyLandOwnershipFixes(scenarioPatch);
    ApplyWaterFixes(scenarioPatch);
    ApplyTileFixes(scenarioPatch);
    ApplySurfaceFixes(scenarioPatch);
    RemoveTileElements(scenarioPatch);
    ApplyRideFixes(scenarioPatch);
    ApplyPathFixes(scenarioPatch);
}

void RCT12::FetchAndApplyScenarioPatch(u8string_view scenarioPath)
{
    if (scenarioPath.empty())
    {
        return;
    }

    auto scenarioSHA = getScenarioSHA256(scenarioPath);
    auto patchPath = GetPatchFileName(scenarioSHA);
    if (File::Exists(patchPath))
    {
        ApplyScenarioPatch(patchPath, scenarioSHA);
    }
}

void RCT12::SetDryRun(bool enable)
{
    _dryRun = enable;
}
