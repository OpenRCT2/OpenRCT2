/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
    OpenRCT2::Guard::Assert(0, "Unrecognized ownership type flag");
    return {};
}

static void readCoordinate(std::vector<TileCoordsXY>& out, const json_t& coordinatesArray)
{
    if (coordinatesArray.size() != 2)
    {
        OpenRCT2::Guard::Assert(0, "Fix coordinates sub array should have 2 elements");
        return;
    }

    out.emplace_back(
        OpenRCT2::Json::GetNumber<int32_t>(coordinatesArray[0]), OpenRCT2::Json::GetNumber<int32_t>(coordinatesArray[1]));
}

static void readCoordinate(std::vector<TileCoordsXYZ>& out, const json_t& coordinatesArray)
{
    if (coordinatesArray.size() != 3)
    {
        OpenRCT2::Guard::Assert(0, "Fix coordinates sub array should have 3 elements");
        return;
    }

    out.emplace_back(
        OpenRCT2::Json::GetNumber<int32_t>(coordinatesArray[0]), OpenRCT2::Json::GetNumber<int32_t>(coordinatesArray[1]),
        OpenRCT2::Json::GetNumber<int32_t>(coordinatesArray[2]));
}

template<typename TTileCoords = TileCoordsXY>
static std::vector<TTileCoords> getCoordinates(const json_t& parameters)
{
    if (!parameters.contains(_coordinatesKey))
    {
        OpenRCT2::Guard::Assert(0, "Cannot have fix without coordinates array");
        return {};
    }
    else if (!parameters[_coordinatesKey].is_array())
    {
        OpenRCT2::Guard::Assert(0, "Fix coordinates should be an array");
        return {};
    }

    auto coords = OpenRCT2::Json::AsArray(parameters[_coordinatesKey]);
    if (coords.empty())
    {
        OpenRCT2::Guard::Assert(0, "Fix coordinates array should not be empty");
        return {};
    }

    std::vector<TTileCoords> parsedCoordinates;
    parsedCoordinates.reserve(coords.size());
    for (size_t i = 0; i < coords.size(); ++i)
    {
        if (!coords[i].is_array())
        {
            OpenRCT2::Guard::Assert(0, "Fix coordinates should contain only arrays");
            return {};
        }

        auto coordinatesArray = OpenRCT2::Json::AsArray(coords[i]);
        readCoordinate(parsedCoordinates, coordinatesArray);
    }
    return parsedCoordinates;
}

static Direction GetDirection(const json_t& parameters)
{
    if (!parameters.contains(_directionKey))
    {
        return INVALID_DIRECTION;
    }
    else if (!parameters[_directionKey].is_number())
    {
        OpenRCT2::Guard::Assert(0, "Fix direction must be a number");
        return INVALID_DIRECTION;
    }

    Direction direction = OpenRCT2::Json::GetNumber<Direction>(parameters[_directionKey]);

    if (direction > 3)
    {
        OpenRCT2::Guard::Assert(0, "Direction must be between 0 and 3");
        return INVALID_DIRECTION;
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
        OpenRCT2::Guard::Assert(0, "queue must be a boolean");
        return false;
    }
    else
    {
        return OpenRCT2::Json::GetBoolean(parameters[_isQueue]);
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
        OpenRCT2::Guard::Assert(0, "Water fix should be an array");
        return;
    }

    auto waterFixes = OpenRCT2::Json::AsArray(scenarioPatch[_waterFixKey]);
    if (waterFixes.empty())
    {
        OpenRCT2::Guard::Assert(0, "Water fix array should not be empty");
        return;
    }

    for (size_t i = 0; i < waterFixes.size(); ++i)
    {
        if (!waterFixes[i].contains(_heightKey))
        {
            OpenRCT2::Guard::Assert(0, "Water fix sub-array should set a height");
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

static OpenRCT2::TrackElemType toTrackType(const u8string_view trackTypeString)
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
    if (!trackTilesFixes.contains(_operationsKey))
    {
        OpenRCT2::Guard::Assert(0, "Cannot apply track tile fixes when operations array is unset");
        return;
    }

    if (!trackTilesFixes[_operationsKey].is_array())
    {
        OpenRCT2::Guard::Assert(0, "Track tile fixes should have an operations array");
        return;
    }

    auto fixOperations = OpenRCT2::Json::AsArray(trackTilesFixes[_operationsKey]);
    if (fixOperations.empty())
    {
        OpenRCT2::Guard::Assert(0, "Operations fix array should not be empty");
        return;
    }

    for (size_t i = 0; i < fixOperations.size(); ++i)
    {
        if (!fixOperations[i].contains(_fromKey))
        {
            OpenRCT2::Guard::Assert(0, "Operation sub-array should contain a from key");
            return;
        }

        if (!fixOperations[i].contains(_toKey))
        {
            OpenRCT2::Guard::Assert(0, "Operation sub-array should contain a to key");
            return;
        }

        auto fromTrackType = toTrackType(OpenRCT2::Json::GetString(fixOperations[i][_fromKey]));
        auto destinationTrackType = toTrackType(OpenRCT2::Json::GetString(fixOperations[i][_toKey]));
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
        OpenRCT2::Guard::Assert(0, "Unsupported tile type conversion");
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
        OpenRCT2::Guard::Assert(0, "Cannot apply tile fixes without defined type");
    }
    else
    {
        auto tileType = toTileElementType(OpenRCT2::Json::GetString(tilesFixes[_typeKey]));
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
        OpenRCT2::Guard::Assert(0, "Surface fix should be an array");
        return;
    }

    auto surfaceFixes = OpenRCT2::Json::AsArray(scenarioPatch[_surfacesKey]);
    if (surfaceFixes.empty())
    {
        OpenRCT2::Guard::Assert(0, "Surface fix array should not be empty");
        return;
    }

    for (size_t i = 0; i < surfaceFixes.size(); ++i)
    {
        if (!surfaceFixes[i].contains(_destinationSurface))
        {
            OpenRCT2::Guard::Assert(0, "Surface fix sub-array should set a destination surface");
            return;
        }
        if (_dryRun)
        {
            continue;
        }

        auto destinationSurface = OpenRCT2::Json::GetString(surfaceFixes[i][_destinationSurface]);
        auto& objectManager = OpenRCT2::GetContext()->GetObjectManager();
        auto surfaceObj = objectManager.GetLoadedObject(ObjectEntryDescriptor::Parse(destinationSurface));
        if (surfaceObj == nullptr)
        {
            OpenRCT2::Guard::Assert(0, "Surface object not found");
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
        OpenRCT2::Guard::Assert(0, "Elements to delete should be an array");
        return;
    }

    auto elementsToDelete = OpenRCT2::Json::AsArray(scenarioPatch[_elementsToDelete]);
    if (elementsToDelete.empty())
    {
        OpenRCT2::Guard::Assert(0, "Elements to delete should not be empty");
        return;
    }

    for (size_t i = 0; i < elementsToDelete.size(); ++i)
    {
        if (!elementsToDelete[i].contains(_element_index))
        {
            OpenRCT2::Guard::Assert(0, "Elements to delete sub-array should set an element_index");
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
                OpenRCT2::Guard::Assert(0, "Invalid Nth element at tile");
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
    if (!scenarioPatch.contains(_ridesKey))
    {
        return;
    }

    if (!scenarioPatch[_ridesKey].is_array())
    {
        OpenRCT2::Guard::Assert(0, "Ride fixes should be an array of arrays");
        return;
    }

    auto rideFixes = OpenRCT2::Json::AsArray(scenarioPatch[_ridesKey]);
    if (rideFixes.empty())
    {
        OpenRCT2::Guard::Assert(0, "Ride fixes should not be an empty array");
        return;
    }

    for (size_t i = 0; i < rideFixes.size(); ++i)
    {
        if (!rideFixes[i].contains(_rideIdKey))
        {
            OpenRCT2::Guard::Assert(0, "Ride fixes should contain a ride id");
            return;
        }

        if (!rideFixes[i].contains(_operationKey))
        {
            OpenRCT2::Guard::Assert(0, "Ride fixes should contain a ride operation");
            return;
        }

        RideId rideId = RideId::FromUnderlying(OpenRCT2::Json::GetNumber<uint16_t>(rideFixes[i][_rideIdKey]));
        auto operation = OpenRCT2::Json::GetString(rideFixes[i][_operationKey]);

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
            OpenRCT2::Guard::Assert(0, "Unsupported ride fix operation");
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
        OpenRCT2::Guard::Assert(0, "Path fixes should be an array of arrays");
        return;
    }

    auto pathFixes = OpenRCT2::Json::AsArray(scenarioPatch[_pathsKey]);
    if (pathFixes.empty())
    {
        OpenRCT2::Guard::Assert(0, "Path fixes should not be an empty array");
        return;
    }

    for (size_t i = 0; i < pathFixes.size(); ++i)
    {
        auto pathFix = pathFixes[i];

        if (!pathFix.contains(_railingsKey))
        {
            OpenRCT2::Guard::Assert(0, "Path fixes should have railings");
            return;
        }

        if (!pathFix.contains(_surfaceKey))
        {
            OpenRCT2::Guard::Assert(0, "Path fixes should have a surface");
            return;
        }

        auto railings = OpenRCT2::Json::GetString(pathFix[_railingsKey]);
        auto surface = OpenRCT2::Json::GetString(pathFix[_surfaceKey]);

        if (_dryRun)
        {
            continue;
        }

        auto& objectManager = OpenRCT2::GetContext()->GetObjectManager();
        auto railingsObjIndex = objectManager.GetLoadedObjectEntryIndex(railings);
        auto surfaceObjIndex = objectManager.GetLoadedObjectEntryIndex(surface);

        if (railingsObjIndex == kObjectEntryIndexNull)
        {
            OpenRCT2::Guard::Assert(0, "Railings object not found");
            return;
        }

        if (surfaceObjIndex == kObjectEntryIndexNull)
        {
            OpenRCT2::Guard::Assert(0, "Surface object not found");
            return;
        }

        auto coordinates = getCoordinates<TileCoordsXYZ>(pathFix);
        Direction direction = GetDirection(pathFix);
        PathConstructFlags constructionFlags = IsQueue(pathFix) ? OpenRCT2::PathConstructFlag::IsQueue : 0;

        for (auto coordinate : coordinates)
        {
            auto slope = direction != INVALID_DIRECTION ? direction + 4 : 0;
            auto footpathPlaceAction = FootpathPlaceAction(
                coordinate.ToCoordsXYZ(), slope, surfaceObjIndex, railingsObjIndex, direction, constructionFlags);
            auto result = footpathPlaceAction.Execute();
            if (result.Error != OpenRCT2::GameActions::Status::Ok)
            {
                OpenRCT2::Guard::Assert(0, "Could not patch path");
            }
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
    if (_dryRun)
    {
        return true;
    }

    if (!scenarioPatch.contains(_scenarioNameKey))
    {
        OpenRCT2::Guard::Assert(0, "All .parkpatch files should contain the name of the original scenario");
        return false;
    }

    if (!scenarioPatch.contains(_fullSHAKey))
    {
        OpenRCT2::Guard::Assert(0, "All .parkpatch files should contain the sha256 of the original scenario");
        return false;
    }

    auto scenarioName = OpenRCT2::Json::GetString(scenarioPatch[_scenarioNameKey]);
    auto scenarioSHA = OpenRCT2::Json::GetString(scenarioPatch[_fullSHAKey]);
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
    ApplySurfaceFixes(scenarioPatch);
    RemoveTileElements(scenarioPatch);
    ApplyRideFixes(scenarioPatch);
    ApplyPathFixes(scenarioPatch);
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
    _dryRun = enable;
}
