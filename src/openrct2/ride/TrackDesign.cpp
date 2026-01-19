/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TrackDesign.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../Diagnostic.h"
#include "../Game.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../TrackImporter.h"
#include "../actions/FootpathLayoutPlaceAction.h"
#include "../actions/FootpathRemoveAction.h"
#include "../actions/LargeSceneryPlaceAction.h"
#include "../actions/LargeSceneryRemoveAction.h"
#include "../actions/MazePlaceTrackAction.h"
#include "../actions/ResultWithMessage.h"
#include "../actions/RideCreateAction.h"
#include "../actions/RideDemolishAction.h"
#include "../actions/RideEntranceExitPlaceAction.h"
#include "../actions/SmallSceneryPlaceAction.h"
#include "../actions/SmallSceneryRemoveAction.h"
#include "../actions/TrackPlaceAction.h"
#include "../actions/TrackRemoveAction.h"
#include "../actions/WallPlaceAction.h"
#include "../actions/WallRemoveAction.h"
#include "../audio/Audio.h"
#include "../config/Config.h"
#include "../core/DataSerialiser.h"
#include "../core/File.h"
#include "../core/Numerics.hpp"
#include "../core/String.hpp"
#include "../core/UnitConversion.h"
#include "../drawing/X8DrawingEngine.h"
#include "../interface/Viewport.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../network/Network.h"
#include "../object/FootpathObject.h"
#include "../object/FootpathSurfaceObject.h"
#include "../object/LargeSceneryEntry.h"
#include "../object/ObjectEntryManager.h"
#include "../object/ObjectLimits.h"
#include "../object/ObjectList.h"
#include "../object/ObjectManager.h"
#include "../object/ObjectRepository.h"
#include "../object/SmallSceneryEntry.h"
#include "../object/StationObject.h"
#include "../rct2/RCT2.h"
#include "../ride/RideConstruction.h"
#include "../sawyer_coding/SawyerCoding.h"
#include "../world/Footpath.h"
#include "../world/Map.h"
#include "../world/MapSelection.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/tile_element/EntranceElement.h"
#include "../world/tile_element/PathElement.h"
#include "../world/tile_element/Slope.h"
#include "../world/tile_element/SurfaceElement.h"
#include "../world/tile_element/TrackElement.h"
#include "Ride.h"
#include "RideData.h"
#include "Track.h"
#include "TrackData.h"
#include "TrackDesign.h"
#include "TrackDesignRepository.h"
#include "Vehicle.h"

#include <iterator>
#include <memory>

using namespace OpenRCT2;
using OpenRCT2::GameActions::CommandFlag;
using OpenRCT2::GameActions::CommandFlags;

namespace OpenRCT2::TrackDesignSceneryElementFlags
{
    static constexpr uint8_t kRotationMask = 0b00000011;

    static constexpr uint8_t kQuadrantMask = 0b00001100;

    static constexpr uint8_t kEdgesMask = 0b00001111;
    static constexpr uint8_t kHasSlopeMask = 0b00010000;
    static constexpr uint8_t kSlopeDirectionMask = 0b01100000;
    static constexpr uint8_t kIsQueueMask = 0b10000000;
} // namespace OpenRCT2::TrackDesignSceneryElementFlags

using namespace TrackDesignSceneryElementFlags;

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::TrackMetaData;

constexpr TileCoordsXY TRACK_DESIGN_PREVIEW_MAP_SIZE = TileCoordsXY{ 256, 256 };

bool gTrackDesignSceneryToggle;
bool _trackDesignDrawingPreview;
bool _trackDesignPlaceStateSceneryUnavailable = false;

static bool _trackDesignPlaceStateEntranceExitPlaced{};

static void TrackDesignPreviewClearMap();

static u8string_view TrackDesignGetStationObjectIdentifier(const Ride& ride)
{
    const auto* stationObject = ride.getStationObject();
    if (stationObject == nullptr)
        return {};

    return stationObject->GetIdentifier();
}

ResultWithMessage TrackDesign::CreateTrackDesign(TrackDesignState& tds, const Ride& ride)
{
    trackAndVehicle.rtdIndex = ride.type;

    auto object = ObjectEntryGetObject(ObjectType::ride, ride.subtype);
    if (object != nullptr)
    {
        auto entry = object->GetObjectEntry();
        // Remove this check for new track design format
        if (entry.IsEmpty())
        {
            return { false, STR_VEHICLE_UNSUPPORTED_TD6 };
        }
        trackAndVehicle.vehicleObject = ObjectEntryDescriptor(entry);
    }

    operation.rideMode = ride.mode;
    appearance.vehicleColourSettings = ride.vehicleColourSettings;

    for (size_t i = 0; i < std::size(appearance.vehicleColours); i++)
    {
        appearance.vehicleColours[i] = ride.vehicleColours[i];
    }

    for (size_t i = 0; i < std::min(std::size(ride.trackColours), std::size(appearance.trackColours)); i++)
    {
        appearance.trackColours[i] = ride.trackColours[i];
    }

    operation.departFlags = ride.departFlags;
    trackAndVehicle.numberOfTrains = ride.numTrains;
    trackAndVehicle.numberOfCarsPerTrain = ride.numCarsPerTrain;
    operation.minWaitingTime = ride.minWaitingTime;
    operation.maxWaitingTime = ride.maxWaitingTime;
    operation.operationSetting = ride.operationOption;
    operation.liftHillSpeed = ride.liftHillSpeed;
    operation.numCircuits = ride.numCircuits;

    appearance.stationObjectIdentifier = TrackDesignGetStationObjectIdentifier(ride);
    statistics.maxSpeed = static_cast<int8_t>(ride.maxSpeed / 65536);
    statistics.averageSpeed = static_cast<int8_t>(ride.averageSpeed / 65536);
    statistics.rideLength = ToHumanReadableRideLength(ride.getTotalLength());
    statistics.maxPositiveVerticalG = ride.maxPositiveVerticalG;
    statistics.maxNegativeVerticalG = ride.maxNegativeVerticalG;
    statistics.maxLateralG = ride.maxLateralG;
    statistics.inversions = ride.numInversions;
    statistics.holes = ride.numHoles;
    statistics.drops = ride.numDrops;
    statistics.highestDropHeight = ride.highestDropHeight;
    statistics.totalAirTime = ride.totalAirTime;

    statistics.ratings = ride.ratings;
    statistics.upkeepCost = ride.upkeepCost;

    const auto& rtd = GetRideTypeDescriptor(trackAndVehicle.rtdIndex);

    if (rtd.DesignCreateMode == TrackDesignCreateMode::Maze)
    {
        return CreateTrackDesignMaze(tds, ride);
    }
    else
    {
        return CreateTrackDesignTrack(tds, ride);
    }
}

ResultWithMessage TrackDesign::CreateTrackDesignTrack(TrackDesignState& tds, const Ride& ride)
{
    CoordsXYE trackElement;
    if (!RideTryGetOriginElement(ride, &trackElement))
    {
        return { false, STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY };
    }

    StringId warningMessage = kStringIdNone;

    RideGetStartOfTrack(&trackElement);

    int32_t z = trackElement.element->GetBaseZ();
    auto trackType = trackElement.element->AsTrack()->GetTrackType();
    uint8_t direction = trackElement.element->GetDirection();
    _saveDirection = direction;
    auto newCoords = GetTrackElementOriginAndApplyChanges(
        { trackElement, z, direction }, trackType, 0, &trackElement.element, {});

    if (!newCoords.has_value())
    {
        return { false, STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY };
    }
    trackElement.x = newCoords->x;
    trackElement.y = newCoords->y;
    z = newCoords->z;

    const auto& ted = GetTrackElementDescriptor(trackElement.element->AsTrack()->GetTrackType());
    const TrackCoordinates* trackCoordinates = &ted.coordinates;
    // Used in the following loop to know when we have
    // completed all of the elements and are back at the
    // start.
    TileElement* initialMap = trackElement.element;

    CoordsXYZ startPos = { trackElement.x, trackElement.y, z + trackCoordinates->zBegin - ted.sequences[0].clearance.z };
    tds.origin = startPos;

    do
    {
        const auto& element = trackElement.element->AsTrack();

        // Remove this check for new track design format
        if (element->GetTrackType() > TrackElemType::highestAlias)
        {
            return { false, STR_TRACK_ELEM_UNSUPPORTED_TD6 };
        }

        TrackDesignTrackElement track{};
        track.type = element->GetTrackType();
        track.colourScheme = element->GetColourScheme();
        track.stationIndex = element->GetStationIndex();
        track.brakeBoosterSpeed = element->GetBrakeBoosterSpeed();
        track.seatRotation = element->GetSeatRotation();

        // This warning will not apply to new track design format
        if (track.type == TrackElemType::blockBrakes && element->GetBrakeBoosterSpeed() != kRCT2DefaultBlockBrakeSpeed)
        {
            warningMessage = STR_TRACK_DESIGN_BLOCK_BRAKE_SPEED_RESET;
        }

        if (element->HasChain())
            track.SetFlag(TrackDesignTrackElementFlag::hasChain);

        if (ride.getRideTypeDescriptor().HasFlag(RtdFlag::hasInvertedVariant) && element->IsInverted())
        {
            track.SetFlag(TrackDesignTrackElementFlag::isInverted);
        }

        trackElements.push_back(track);

        if (!TrackBlockGetNext(&trackElement, &trackElement, nullptr, nullptr))
        {
            break;
        }

        z = trackElement.element->GetBaseZ();
        direction = trackElement.element->GetDirection();
        trackType = trackElement.element->AsTrack()->GetTrackType();
        newCoords = GetTrackElementOriginAndApplyChanges(
            { trackElement, z, direction }, trackType, 0, &trackElement.element, {});

        if (!newCoords.has_value())
        {
            break;
        }
        trackElement.x = newCoords->x;
        trackElement.y = newCoords->y;

        if (trackElements.size() > RCT2::Limits::kTD6MaxTrackElements)
        {
            return { false, STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY };
        }
    } while (trackElement.element != initialMap);

    // First entrances, second exits
    for (int32_t i = 0; i < 2; i++)
    {
        for (const auto& station : ride.getStations())
        {
            z = station.GetBaseZ();

            TileCoordsXYZD location;
            if (i == 0)
            {
                location = station.Entrance;
            }
            else
            {
                location = station.Exit;
            }

            if (location.IsNull())
            {
                continue;
            }

            CoordsXY mapLocation = location.ToCoordsXY();

            TileElement* tileElement = MapGetFirstElementAt(mapLocation);
            if (tileElement == nullptr)
                continue;

            do
            {
                if (tileElement->GetType() != TileElementType::Entrance)
                    continue;
                if (tileElement->GetBaseZ() == z)
                    break;
            } while (!(tileElement++)->IsLastForTile());

            // Add something that stops this from walking off the end

            Direction entranceDirection = tileElement->GetDirection();
            entranceDirection -= _saveDirection;
            entranceDirection &= kTileElementDirectionMask;

            mapLocation -= tds.origin;
            // Rotate entrance coordinates backwards to the correct direction
            auto rotatedMapLocation = TileCoordsXY(mapLocation.Rotate(0 - _saveDirection));

            z -= tds.origin.z;
            z /= kCoordsZStep;

            if (z > 127 || z < -126)
            {
                return { false, STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY };
            }

            TrackDesignEntranceElement entrance{};
            entrance.location = TileCoordsXYZD(rotatedMapLocation, z, entranceDirection);

            // If this is the exit version
            if (i == 1)
            {
                entrance.isExit = true;
            }
            entranceElements.push_back(entrance);
        }
    }

    TrackDesignPreviewDrawOutlines(
        tds, *this, RideGetTemporaryForPreview(), { 4096, 4096, 0, _currentTrackPieceDirection }, false);

    // Resave global vars for scenery reasons.
    tds.origin = startPos;

    gMapSelectFlags.unset(MapSelectFlag::enableConstruct);
    gMapSelectFlags.unset(MapSelectFlag::enableArrow);
    gMapSelectFlags.unset(MapSelectFlag::green);

    statistics.spaceRequired = TileCoordsXY(tds.previewMax - tds.previewMin) + TileCoordsXY{ 1, 1 };
    return { true, warningMessage };
}

ResultWithMessage TrackDesign::CreateTrackDesignMaze(TrackDesignState& tds, const Ride& ride)
{
    auto startLoc = MazeGetFirstElement(ride);

    if (startLoc.element == nullptr)
    {
        return { false, STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY };
    }

    tds.origin = { startLoc.x, startLoc.y, startLoc.element->GetBaseZ() };

    // x is defined here as we can start the search
    // on tile start_x, start_y but then the next row
    // must restart on 0
    for (int32_t y = startLoc.y, x = startLoc.x; y < kMaximumMapSizeBig; y += kCoordsXYStep)
    {
        for (; x < kMaximumMapSizeBig; x += kCoordsXYStep)
        {
            auto tileElement = MapGetFirstElementAt(CoordsXY{ x, y });
            do
            {
                if (tileElement == nullptr)
                    break;
                if (tileElement->GetType() != TileElementType::Track)
                    continue;
                if (tileElement->AsTrack()->GetRideIndex() != ride.id)
                    continue;

                TrackDesignMazeElement maze{};

                maze.mazeEntry = tileElement->AsTrack()->GetMazeEntry();
                maze.location.x = (x - startLoc.x) / kCoordsXYStep;
                maze.location.y = (y - startLoc.y) / kCoordsXYStep;
                _saveDirection = tileElement->GetDirection();
                mazeElements.push_back(maze);

                if (mazeElements.size() >= 2000)
                {
                    return { false, STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY };
                }
            } while (!(tileElement++)->IsLastForTile());
        }
        x = 0;
    }

    auto location = ride.getStation().Entrance;
    if (location.IsNull())
    {
        return { false, STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY };
    }

    CoordsXY entranceLoc = location.ToCoordsXY();
    auto tileElement = MapGetFirstElementAt(entranceLoc);
    do
    {
        if (tileElement == nullptr)
            return { false, STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY };
        if (tileElement->GetType() != TileElementType::Entrance)
            continue;
        if (tileElement->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_RIDE_ENTRANCE)
            continue;
        if (tileElement->AsEntrance()->GetRideIndex() == ride.id)
            break;
    } while (!(tileElement++)->IsLastForTile());
    // Add something that stops this from walking off the end

    auto entranceOffset = entranceLoc - startLoc;
    TrackDesignEntranceElement mazeEntrance{};
    mazeEntrance.location = TileCoordsXYZD(CoordsXYZD(entranceOffset, 0, tileElement->GetDirection()));
    mazeEntrance.isExit = false;
    entranceElements.push_back(mazeEntrance);

    location = ride.getStation().Exit;
    if (location.IsNull())
    {
        return { false, STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY };
    }

    CoordsXY exitLoc = location.ToCoordsXY();
    tileElement = MapGetFirstElementAt(exitLoc);
    if (tileElement == nullptr)
        return { false, STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY };
    do
    {
        if (tileElement->GetType() != TileElementType::Entrance)
            continue;
        if (tileElement->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_RIDE_EXIT)
            continue;
        if (tileElement->AsEntrance()->GetRideIndex() == ride.id)
            break;
    } while (!(tileElement++)->IsLastForTile());
    // Add something that stops this from walking off the end

    auto exitOffset = exitLoc - startLoc;
    TrackDesignEntranceElement mazeExit{};
    mazeExit.location = TileCoordsXYZD(CoordsXYZD(exitOffset, 0, tileElement->GetDirection()));
    mazeExit.isExit = true;
    entranceElements.push_back(mazeExit);

    // Save global vars as they are still used by scenery????
    int32_t startZ = tds.origin.z;
    TrackDesignPreviewDrawOutlines(
        tds, *this, RideGetTemporaryForPreview(), { 4096, 4096, 0, _currentTrackPieceDirection }, false);
    tds.origin = { startLoc.x, startLoc.y, startZ };

    gMapSelectFlags.unset(MapSelectFlag::enableConstruct);
    gMapSelectFlags.unset(MapSelectFlag::enableArrow);
    gMapSelectFlags.unset(MapSelectFlag::green);

    statistics.spaceRequired = TileCoordsXY(tds.previewMax - tds.previewMin) + TileCoordsXY{ 1, 1 };

    return { true };
}

CoordsXYE TrackDesign::MazeGetFirstElement(const Ride& ride)
{
    CoordsXYE tile{};
    for (tile.y = 0; tile.y < kMaximumMapSizeBig; tile.y += kCoordsXYStep)
    {
        for (tile.x = 0; tile.x < kMaximumMapSizeBig; tile.x += kCoordsXYStep)
        {
            tile.element = MapGetFirstElementAt(CoordsXY{ tile.x, tile.y });
            do
            {
                if (tile.element == nullptr)
                    break;

                if (tile.element->GetType() != TileElementType::Track)
                    continue;
                if (tile.element->AsTrack()->GetRideIndex() == ride.id)
                {
                    return tile;
                }
            } while (!(tile.element++)->IsLastForTile());
        }
    }
    tile.element = nullptr;
    return tile;
}

ResultWithMessage TrackDesign::CreateTrackDesignScenery(TrackDesignState& tds)
{
    sceneryElements = _trackSavedTileElementsDesc;
    // Run an element loop
    for (auto& scenery : sceneryElements)
    {
        switch (scenery.sceneryObject.GetType())
        {
            case ObjectType::paths:
            {
                uint8_t slope = (scenery.getSlopeDirection() - _saveDirection) % kNumOrthogonalDirections;
                scenery.setSlopeDirection(slope);

                uint8_t edges = Numerics::ror4(scenery.getEdges(), _saveDirection);
                scenery.setEdges(edges);
                break;
            }
            case ObjectType::walls:
            {
                auto direction = (scenery.getRotation() - _saveDirection) % kNumOrthogonalDirections;
                scenery.setRotation(direction);
                break;
            }
            default:
            {
                auto direction = (scenery.getRotation() - _saveDirection) % kNumOrthogonalDirections;
                scenery.setRotation(direction);

                auto quadrant = (scenery.getQuadrant() - _saveDirection) % kNumOrthogonalDirections;
                scenery.setQuadrant(quadrant);

                break;
            }
        }

        const auto relativeMapPosition = scenery.loc - tds.origin;
        const CoordsXY rotatedRelativeMapPos = relativeMapPosition.Rotate(0 - _saveDirection);

        if (rotatedRelativeMapPos.x > 127 * kCoordsXYStep || rotatedRelativeMapPos.y > 127 * kCoordsXYStep
            || rotatedRelativeMapPos.x < -126 * kCoordsXYStep || rotatedRelativeMapPos.y < -126 * kCoordsXYStep)
        {
            return { false, STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY };
        }

        if (relativeMapPosition.z > 127 * kCoordsZStep || relativeMapPosition.z < -126 * kCoordsZStep)
        {
            return { false, STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY };
        }
        scenery.loc = CoordsXYZ(rotatedRelativeMapPos, relativeMapPosition.z);
    }

    return { true };
}

void TrackDesign::Serialise(DataSerialiser& stream)
{
    if (stream.IsLogging())
    {
        stream << DS_TAG(gameStateData.name);
        // There is too much information logged.
        // See sub actions for this information if required.
        return;
    }
    stream << DS_TAG(trackAndVehicle.rtdIndex);
    stream << DS_TAG(gameStateData.cost);
    stream << DS_TAG(operation.rideMode);
    stream << DS_TAG(gameStateData.flags);
    stream << DS_TAG(appearance.vehicleColourSettings);
    stream << DS_TAG(appearance.vehicleColours);
    stream << DS_TAG(appearance.stationObjectIdentifier);
    stream << DS_TAG(statistics.totalAirTime);
    stream << DS_TAG(operation.departFlags);
    stream << DS_TAG(trackAndVehicle.numberOfTrains);
    stream << DS_TAG(trackAndVehicle.numberOfCarsPerTrain);
    stream << DS_TAG(operation.minWaitingTime);
    stream << DS_TAG(operation.maxWaitingTime);
    stream << DS_TAG(operation.operationSetting);
    stream << DS_TAG(statistics.maxSpeed);
    stream << DS_TAG(statistics.averageSpeed);
    stream << DS_TAG(statistics.rideLength);
    stream << DS_TAG(statistics.maxPositiveVerticalG);
    stream << DS_TAG(statistics.maxNegativeVerticalG);
    stream << DS_TAG(statistics.maxLateralG);
    stream << DS_TAG(statistics.inversions);
    stream << DS_TAG(statistics.holes);
    stream << DS_TAG(statistics.drops);
    stream << DS_TAG(statistics.highestDropHeight);
    stream << DS_TAG(statistics.ratings);
    stream << DS_TAG(statistics.upkeepCost);
    stream << DS_TAG(appearance.trackColours);
    stream << DS_TAG(trackAndVehicle.vehicleObject);
    stream << DS_TAG(statistics.spaceRequired.x);
    stream << DS_TAG(statistics.spaceRequired.y);
    stream << DS_TAG(operation.liftHillSpeed);
    stream << DS_TAG(operation.numCircuits);

    stream << DS_TAG(mazeElements);
    stream << DS_TAG(trackElements);
    stream << DS_TAG(entranceElements);
    stream << DS_TAG(sceneryElements);

    stream << DS_TAG(gameStateData.name);
}

std::unique_ptr<TrackDesign> TrackDesignImport(const utf8* path)
{
    try
    {
        auto trackImporter = TrackImporter::Create(path);
        trackImporter->Load(path);
        return trackImporter->Import();
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Unable to load track design: %s", e.what());
    }
    LOG_VERBOSE("track_design_open(\"%s\")", path);
    return nullptr;
}

/**
 *
 *  rct2: 0x006ABDB0
 */
static void TrackDesignLoadSceneryObjects(const TrackDesign& td)
{
    auto& objectManager = GetContext()->GetObjectManager();
    objectManager.UnloadAllTransient();

    // Load ride object
    if (td.trackAndVehicle.vehicleObject.HasValue())
    {
        objectManager.LoadObject(td.trackAndVehicle.vehicleObject);
    }

    // Load scenery objects
    for (const auto& scenery : td.sceneryElements)
    {
        if (scenery.sceneryObject.HasValue())
        {
            objectManager.LoadObject(scenery.sceneryObject);
        }
    }
}

struct TrackSceneryEntry
{
    ObjectType Type = ObjectType::none;
    ObjectEntryIndex Index = kObjectEntryIndexNull;
    ObjectEntryIndex SecondaryIndex = kObjectEntryIndexNull; // For footpath railing
};

static ObjectEntryIndex TrackDesignGetDefaultSurfaceIndex(bool isQueue)
{
    for (ObjectEntryIndex i = 0; i < kMaxFootpathSurfaceObjects; i++)
    {
        auto footpathSurfaceObj = GetPathSurfaceEntry(i);
        if (footpathSurfaceObj != nullptr)
        {
            if (footpathSurfaceObj->Flags & FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR)
            {
                continue;
            }

            if (isQueue != ((footpathSurfaceObj->Flags & FOOTPATH_ENTRY_FLAG_IS_QUEUE) != 0))
            {
                continue;
            }

            return i;
        }
    }
    return kObjectEntryIndexNull;
}

static ObjectEntryIndex TrackDesignGetDefaultRailingIndex()
{
    for (ObjectEntryIndex i = 0; i < kMaxFootpathRailingsObjects; i++)
    {
        auto footpathRailingsObj = GetPathRailingsEntry(i);
        if (footpathRailingsObj != nullptr)
        {
            return i;
        }
    }
    return kObjectEntryIndexNull;
}

static std::optional<TrackSceneryEntry> TrackDesignPlaceSceneryElementGetEntry(const TrackDesignSceneryElement& scenery)
{
    TrackSceneryEntry result;

    auto& objectMgr = GetContext()->GetObjectManager();
    if (scenery.sceneryObject.GetType() == ObjectType::paths)
    {
        auto footpathMapping = RCT2::GetFootpathSurfaceId(scenery.sceneryObject, true, scenery.isQueue());
        if (footpathMapping == nullptr)
        {
            // Check if legacy path object is loaded
            auto obj = objectMgr.GetLoadedObject(scenery.sceneryObject);
            if (obj != nullptr)
            {
                result.Type = obj->GetObjectType();
                result.Index = objectMgr.GetLoadedObjectEntryIndex(obj);
            }
            else
            {
                result.Type = ObjectType::footpathSurface;
            }
        }
        else
        {
            result.Type = ObjectType::footpathSurface;
            result.Index = objectMgr.GetLoadedObjectEntryIndex(
                ObjectEntryDescriptor(scenery.isQueue() ? footpathMapping->QueueSurface : footpathMapping->NormalSurface));
            result.SecondaryIndex = objectMgr.GetLoadedObjectEntryIndex(ObjectEntryDescriptor(footpathMapping->Railing));
        }

        if (result.Index == kObjectEntryIndexNull)
            result.Index = TrackDesignGetDefaultSurfaceIndex(scenery.isQueue());
        if (result.SecondaryIndex == kObjectEntryIndexNull)
            result.SecondaryIndex = TrackDesignGetDefaultRailingIndex();

        if (result.Index == kObjectEntryIndexNull || result.SecondaryIndex == kObjectEntryIndexNull)
        {
            _trackDesignPlaceStateSceneryUnavailable = true;
            return {};
        }
    }
    else
    {
        auto obj = objectMgr.GetLoadedObject(scenery.sceneryObject);
        bool objectUnavailable = obj == nullptr;
        if (obj != nullptr)
        {
            result.Type = obj->GetObjectType();
            result.Index = objectMgr.GetLoadedObjectEntryIndex(obj);
            if (!getGameState().cheats.ignoreResearchStatus)
            {
                objectUnavailable = !ResearchIsInvented(result.Type, result.Index);
            }
        }
        if (objectUnavailable)
        {
            _trackDesignPlaceStateSceneryUnavailable = true;
            return {};
        }
    }
    return result;
}

/**
 *
 *  rct2: 0x006D247A
 */
static void TrackDesignMirrorScenery(TrackDesign& td)
{
    auto& objectMgr = GetContext()->GetObjectManager();
    for (auto& scenery : td.sceneryElements)
    {
        auto entryInfo = TrackDesignPlaceSceneryElementGetEntry(scenery);
        if (!entryInfo)
            continue;

        auto obj = objectMgr.GetLoadedObject(entryInfo->Type, entryInfo->Index);
        switch (obj->GetObjectType())
        {
            case ObjectType::largeScenery:
            {
                auto* sceneryEntry = reinterpret_cast<const LargeSceneryEntry*>(obj->GetLegacyData());
                int16_t x1 = 0, x2 = 0, y1 = 0, y2 = 0;
                for (auto& tile : sceneryEntry->tiles)
                {
                    if (x1 > tile.offset.x)
                    {
                        x1 = tile.offset.x;
                    }
                    if (x2 < tile.offset.x)
                    {
                        x2 = tile.offset.x;
                    }
                    if (y1 > tile.offset.y)
                    {
                        y1 = tile.offset.y;
                    }
                    if (y2 < tile.offset.y)
                    {
                        y2 = tile.offset.y;
                    }
                }

                switch (scenery.getRotation())
                {
                    case 0:
                        scenery.loc.y = -(scenery.loc.y + y1) - y2;
                        break;
                    case 1:
                        scenery.loc.x = scenery.loc.x + y2 + y1;
                        scenery.loc.y = -scenery.loc.y;
                        break;
                    case 2:
                        scenery.loc.y = -(scenery.loc.y - y2) + y1;
                        break;
                    case 3:
                        scenery.loc.x = scenery.loc.x - y2 - y1;
                        scenery.loc.y = -scenery.loc.y;
                        break;
                }
                scenery.setRotation(DirectionFlipXAxis(scenery.getRotation()));
                break;
            }
            case ObjectType::smallScenery:
            {
                auto* sceneryEntry = reinterpret_cast<const SmallSceneryEntry*>(obj->GetLegacyData());
                scenery.loc.y = -scenery.loc.y;

                if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_DIAGONAL))
                {
                    scenery.setRotation(scenery.getRotation() ^ (1 << 0));
                    if (!sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE))
                    {
                        scenery.setQuadrant(scenery.getQuadrant() ^ (1 << 0));
                    }
                    break;
                }

                scenery.setRotation(DirectionFlipXAxis(scenery.getRotation()));
                scenery.setQuadrant(scenery.getQuadrant() ^ (1 << 0));
                break;
            }
            case ObjectType::walls:
            {
                scenery.loc.y = -scenery.loc.y;
                scenery.setRotation(DirectionFlipXAxis(scenery.getRotation()));
                break;
            }
            case ObjectType::paths:
            case ObjectType::footpathSurface:
            {
                scenery.loc.y = -scenery.loc.y;

                if (scenery.hasSlope())
                {
                    scenery.setSlopeDirection(DirectionFlipXAxis(scenery.getSlopeDirection()));
                }

                uint8_t edges = scenery.getEdges();
                edges = ((edges & 0b1000) >> 2) | ((edges & 0b0010) << 2) | (edges & 0b0101);
                scenery.setEdges(edges);
                break;
            }
            default:
                break;
        }
    }
}

static void TrackDesignMirrorEntrances(TrackDesign& td)
{
    for (auto& entrance : td.entranceElements)
    {
        entrance.location.y = -entrance.location.y;
        if (entrance.location.direction & 1)
        {
            entrance.location.direction = DirectionReverse(entrance.location.direction);
        }
    }
}

/**
 *
 *  rct2: 0x006D2443
 */
static void TrackDesignMirrorRide(TrackDesign& td)
{
    for (auto& track : td.trackElements)
    {
        const auto& ted = GetTrackElementDescriptor(track.type);
        track.type = ted.mirrorElement;
    }
}

/** rct2: 0x00993EDC */
static constexpr uint8_t maze_segment_mirror_map[] = {
    5, 4, 2, 7, 1, 0, 14, 3, 13, 12, 10, 15, 9, 8, 6, 11,
};

/**
 *
 *  rct2: 0x006D25FA
 */
static void TrackDesignMirrorMaze(TrackDesign& td)
{
    for (auto& maze : td.mazeElements)
    {
        maze.location.y = -maze.location.y;

        uint32_t mazeEntry = maze.mazeEntry;
        uint16_t newEntry = 0;
        for (uint8_t position = Numerics::bitScanForward(mazeEntry); position != 0xFF;
             position = Numerics::bitScanForward(mazeEntry))
        {
            mazeEntry &= ~(1 << position);
            newEntry |= (1 << maze_segment_mirror_map[position]);
        }
        maze.mazeEntry = newEntry;
    }
}

/**
 *
 *  rct2: 0x006D2436
 */
void TrackDesignMirror(TrackDesign& td)
{
    const auto& rtd = GetRideTypeDescriptor(td.trackAndVehicle.rtdIndex);
    if (rtd.specialType == RtdSpecialType::maze)
    {
        TrackDesignMirrorMaze(td);
    }
    else
    {
        TrackDesignMirrorRide(td);
    }
    TrackDesignMirrorEntrances(td);
    TrackDesignMirrorScenery(td);
}

static void TrackDesignAddSelectedTile(const CoordsXY& coords)
{
    const auto& selectedTiles = MapSelection::getSelectedTiles();
    const auto tileIterator = std::find(selectedTiles.begin(), selectedTiles.end(), coords);
    if (tileIterator == selectedTiles.end())
    {
        MapSelection::addSelectedTile(coords);
    }
}

static void TrackDesignUpdatePreviewBounds(TrackDesignState& tds, const CoordsXYZ& coords)
{
    tds.previewMin = { std::min(tds.previewMin.x, coords.x), std::min(tds.previewMin.y, coords.y),
                       std::min(tds.previewMin.z, coords.z) };
    tds.previewMax = { std::max(tds.previewMax.x, coords.x), std::max(tds.previewMax.y, coords.y),
                       std::max(tds.previewMax.z, coords.z) };
}

static GameActions::Result TrackDesignPlaceSceneryElementRemoveGhost(
    CoordsXY mapCoord, const TrackDesignSceneryElement& scenery, uint8_t rotation, int32_t originZ)
{
    auto entryInfo = TrackDesignPlaceSceneryElementGetEntry(scenery);
    if (!entryInfo)
    {
        return GameActions::Result();
    }

    if (_trackDesignPlaceStateSceneryUnavailable)
    {
        return GameActions::Result();
    }

    int32_t z = scenery.loc.z + originZ;
    uint8_t sceneryRotation = (rotation + scenery.getRotation()) & kTileElementDirectionMask;
    const CommandFlags flags = { CommandFlag::apply, CommandFlag::allowDuringPaused, CommandFlag::noSpend, CommandFlag::ghost };
    std::unique_ptr<GameActions::GameAction> ga;
    switch (entryInfo->Type)
    {
        case ObjectType::smallScenery:
        {
            uint8_t quadrant = scenery.getQuadrant() + _currentTrackPieceDirection;
            quadrant &= 3;

            auto* sceneryEntry = ObjectManager::GetObjectEntry<SmallSceneryEntry>(entryInfo->Index);
            if (!(!sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE) && sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_DIAGONAL))
                && sceneryEntry->HasFlag(
                    SMALL_SCENERY_FLAG_DIAGONAL | SMALL_SCENERY_FLAG_HALF_SPACE | SMALL_SCENERY_FLAG_THREE_QUARTERS))
            {
                quadrant = 0;
            }

            ga = std::make_unique<GameActions::SmallSceneryRemoveAction>(
                CoordsXYZ{ mapCoord.x, mapCoord.y, z }, quadrant, entryInfo->Index);
            break;
        }
        case ObjectType::largeScenery:
            ga = std::make_unique<GameActions::LargeSceneryRemoveAction>(
                CoordsXYZD{ mapCoord.x, mapCoord.y, z, sceneryRotation }, 0);
            break;
        case ObjectType::walls:
            ga = std::make_unique<GameActions::WallRemoveAction>(CoordsXYZD{ mapCoord.x, mapCoord.y, z, sceneryRotation });
            break;
        case ObjectType::paths:
        case ObjectType::footpathSurface:
            ga = std::make_unique<GameActions::FootpathRemoveAction>(CoordsXYZ{ mapCoord.x, mapCoord.y, z });
            break;
        default:
            return GameActions::Result();
    }

    ga->SetFlags(flags);
    return GameActions::ExecuteNested(ga.get(), getGameState());
}

static bool TrackDesignPlaceSceneryElementGetPlaceZ(TrackDesignState& tds, const TrackDesignSceneryElement& scenery)
{
    int32_t z = scenery.loc.z + tds.placeZ;
    if (z < tds.placeSceneryZ)
    {
        tds.placeSceneryZ = z;
    }

    TrackDesignPlaceSceneryElementGetEntry(scenery);
    return true;
}

static GameActions::Result TrackDesignPlaceSceneryElement(
    TrackDesignState& tds, CoordsXY mapCoord, uint8_t mode, const TrackDesignSceneryElement& scenery, uint8_t rotation,
    int32_t originZ)
{
    if (tds.placeOperation == TrackPlaceOperation::drawOutlines && mode == 0)
    {
        TrackDesignAddSelectedTile(mapCoord);
        return GameActions::Result();
    }

    if (tds.placeOperation == TrackPlaceOperation::removeGhost && mode == 0)
    {
        return TrackDesignPlaceSceneryElementRemoveGhost(mapCoord, scenery, rotation, originZ);
    }

    if (tds.placeOperation == TrackPlaceOperation::getPlaceZ)
    {
        TrackDesignPlaceSceneryElementGetPlaceZ(tds, scenery);
        return GameActions::Result();
    }

    money64 cost = 0;

    if (tds.placeOperation != TrackPlaceOperation::placeQuery && tds.placeOperation != TrackPlaceOperation::place
        && tds.placeOperation != TrackPlaceOperation::placeGhost
        && tds.placeOperation != TrackPlaceOperation::placeTrackPreview)
    {
        return GameActions::Result();
    }

    auto entryInfo = TrackDesignPlaceSceneryElementGetEntry(scenery);
    if (!entryInfo)
    {
        return GameActions::Result();
    }

    int16_t z;
    CommandFlags flags;

    auto& gameState = getGameState();

    switch (entryInfo->Type)
    {
        case ObjectType::smallScenery:
        {
            if (mode != 0)
            {
                return GameActions::Result();
            }

            rotation += scenery.getRotation();
            rotation &= 3;
            z = scenery.loc.z + originZ;
            uint8_t quadrant = (scenery.getQuadrant() + _currentTrackPieceDirection) & 3;

            flags = { CommandFlag::apply, CommandFlag::trackDesign };
            if (tds.placeOperation == TrackPlaceOperation::placeTrackPreview)
            {
                flags = { CommandFlag::apply, CommandFlag::trackDesign, CommandFlag::allowDuringPaused, CommandFlag::noSpend };
            }
            else if (tds.placeOperation == TrackPlaceOperation::placeGhost)
            {
                flags = { CommandFlag::apply, CommandFlag::trackDesign, CommandFlag::allowDuringPaused, CommandFlag::ghost,
                          CommandFlag::noSpend };
            }
            else if (tds.placeOperation == TrackPlaceOperation::placeQuery)
            {
                flags = { CommandFlag::trackDesign };
            }
            if (tds.isReplay)
            {
                flags.set(CommandFlag::replay);
            }

            auto smallSceneryPlace = GameActions::SmallSceneryPlaceAction(
                { mapCoord.x, mapCoord.y, z, rotation }, quadrant, entryInfo->Index, scenery.primaryColour,
                scenery.secondaryColour, scenery.tertiaryColour);

            smallSceneryPlace.SetFlags(flags);
            auto res = flags.has(CommandFlag::apply) ? GameActions::ExecuteNested(&smallSceneryPlace, gameState)
                                                     : GameActions::QueryNested(&smallSceneryPlace, gameState);

            cost = res.error == GameActions::Status::ok ? res.cost : 0;
            break;
        }
        case ObjectType::largeScenery:
        {
            if (mode != 0)
            {
                return GameActions::Result();
            }

            rotation += scenery.getRotation();
            rotation &= 3;

            z = scenery.loc.z + originZ;

            flags = { CommandFlag::apply, CommandFlag::trackDesign };
            if (tds.placeOperation == TrackPlaceOperation::placeTrackPreview)
            {
                flags = { CommandFlag::apply, CommandFlag::trackDesign, CommandFlag::allowDuringPaused, CommandFlag::noSpend };
            }
            else if (tds.placeOperation == TrackPlaceOperation::placeGhost)
            {
                flags = { CommandFlag::apply, CommandFlag::trackDesign, CommandFlag::allowDuringPaused, CommandFlag::ghost,
                          CommandFlag::noSpend };
            }
            else if (tds.placeOperation == TrackPlaceOperation::placeQuery)
            {
                flags = { CommandFlag::trackDesign };
            }
            if (tds.isReplay)
            {
                flags.set(CommandFlag::replay);
            }
            auto sceneryPlaceAction = GameActions::LargeSceneryPlaceAction(
                { mapCoord.x, mapCoord.y, z, rotation }, entryInfo->Index, scenery.primaryColour, scenery.secondaryColour,
                scenery.tertiaryColour);
            sceneryPlaceAction.SetFlags(flags);
            auto res = flags.has(CommandFlag::apply) ? GameActions::ExecuteNested(&sceneryPlaceAction, gameState)
                                                     : GameActions::QueryNested(&sceneryPlaceAction, gameState);

            cost = res.cost;
            break;
        }
        case ObjectType::walls:
        {
            if (mode != 0)
            {
                return GameActions::Result();
            }

            z = scenery.loc.z + originZ;
            rotation += scenery.getRotation();
            rotation &= 3;

            flags = { CommandFlag::apply };
            if (tds.placeOperation == TrackPlaceOperation::placeTrackPreview)
            {
                flags = { CommandFlag::apply, CommandFlag::trackDesign, CommandFlag::allowDuringPaused, CommandFlag::noSpend };
            }
            else if (tds.placeOperation == TrackPlaceOperation::placeGhost)
            {
                flags = { CommandFlag::apply, CommandFlag::allowDuringPaused, CommandFlag::noSpend, CommandFlag::ghost };
            }
            else if (tds.placeOperation == TrackPlaceOperation::placeQuery)
            {
                flags = {};
            }
            if (tds.isReplay)
            {
                flags.set(CommandFlag::replay);
            }
            auto wallPlaceAction = GameActions::WallPlaceAction(
                entryInfo->Index, { mapCoord.x, mapCoord.y, z }, rotation, scenery.primaryColour, scenery.secondaryColour,
                scenery.tertiaryColour);
            wallPlaceAction.SetFlags(flags);
            auto res = flags.has(CommandFlag::apply) ? GameActions::ExecuteNested(&wallPlaceAction, gameState)
                                                     : GameActions::QueryNested(&wallPlaceAction, gameState);

            cost = res.cost;
            break;
        }
        case ObjectType::paths:
        case ObjectType::footpathSurface:
            z = scenery.loc.z + originZ;
            if (mode == 0)
            {
                flags = { CommandFlag::apply };
                if (tds.placeOperation == TrackPlaceOperation::placeTrackPreview)
                {
                    flags = { CommandFlag::apply, CommandFlag::allowDuringPaused, CommandFlag::noSpend };
                }
                if (tds.placeOperation == TrackPlaceOperation::placeGhost)
                {
                    flags = { CommandFlag::apply, CommandFlag::allowDuringPaused, CommandFlag::noSpend, CommandFlag::ghost };
                }
                if (tds.placeOperation == TrackPlaceOperation::placeQuery)
                {
                    flags = {};
                }
                if (tds.isReplay)
                {
                    flags.set(CommandFlag::replay);
                }
                uint8_t slopeDirection = (scenery.getSlopeDirection() + rotation) & 0x3;
                FootpathSlope slope = { FootpathSlopeType::flat, slopeDirection };
                if (scenery.hasSlope())
                    slope.type = FootpathSlopeType::sloped;
                uint8_t edges = Numerics::rol4(scenery.getEdges(), rotation);
                PathConstructFlags constructFlags = 0;
                if (scenery.isQueue())
                    constructFlags |= PathConstructFlag::IsQueue;
                if (entryInfo->Type == ObjectType::paths)
                    constructFlags |= PathConstructFlag::IsLegacyPathObject;
                auto footpathPlaceAction = GameActions::FootpathLayoutPlaceAction(
                    { mapCoord.x, mapCoord.y, z }, slope, entryInfo->Index, entryInfo->SecondaryIndex, edges, constructFlags);
                footpathPlaceAction.SetFlags(flags);
                auto res = flags.has(CommandFlag::apply) ? GameActions::ExecuteNested(&footpathPlaceAction, gameState)
                                                         : GameActions::QueryNested(&footpathPlaceAction, gameState);
                // Ignore failures
                cost = res.error == GameActions::Status::ok ? res.cost : 0;
            }
            else
            {
                if (tds.placeOperation == TrackPlaceOperation::placeQuery)
                {
                    return GameActions::Result();
                }

                auto* pathElement = MapGetPathElementAt(TileCoordsXYZ{ CoordsXYZ{ mapCoord.x, mapCoord.y, z } });
                if (pathElement == nullptr)
                {
                    return GameActions::Result();
                }

                flags = { CommandFlag::apply };
                if (tds.placeOperation == TrackPlaceOperation::placeTrackPreview)
                {
                    flags = { CommandFlag::apply, CommandFlag::allowDuringPaused, CommandFlag::noSpend };
                }
                if (tds.placeOperation == TrackPlaceOperation::placeGhost)
                {
                    flags = { CommandFlag::apply, CommandFlag::allowDuringPaused, CommandFlag::noSpend, CommandFlag::ghost };
                }
                if (tds.isReplay)
                {
                    flags.set(CommandFlag::replay);
                }

                if (tds.placeOperation == TrackPlaceOperation::placeTrackPreview
                    || tds.placeOperation == TrackPlaceOperation::place)
                {
                    if (!pathElement->IsQueue() || FootpathQueueCountConnections(mapCoord, *pathElement) < 2)
                    {
                        FootpathRemoveEdgesAt(mapCoord, reinterpret_cast<TileElement*>(pathElement));
                        FootpathConnectEdges(mapCoord, reinterpret_cast<TileElement*>(pathElement), flags);
                    }
                }

                return GameActions::Result();
            }
            break;
        default:
            _trackDesignPlaceStateSceneryUnavailable = true;
            return GameActions::Result();
    }

    auto res = GameActions::Result();
    res.cost = cost;

    return res;
}

/**
 *
 *  rct2: 0x006D0964
 */
static GameActions::Result TrackDesignPlaceAllScenery(
    TrackDesignState& tds, const std::vector<TrackDesignSceneryElement>& sceneryList, uint8_t rotation)
{
    const auto& origin = tds.origin;

    money64 cost = 0;

    for (uint8_t mode = 0; mode <= 1; mode++)
    {
        if (!sceneryList.empty())
        {
            tds.hasScenery = true;
        }

        if (!tds.placeScenery)
        {
            continue;
        }

        for (const auto& scenery : sceneryList)
        {
            auto mapCoord = CoordsXYZ{ CoordsXY(origin) + scenery.loc.Rotate(rotation), origin.z };
            TrackDesignUpdatePreviewBounds(tds, mapCoord);

            auto placementRes = TrackDesignPlaceSceneryElement(tds, mapCoord, mode, scenery, rotation, origin.z);
            if (placementRes.error != GameActions::Status::ok)
            {
                if (tds.placeOperation != TrackPlaceOperation::removeGhost)
                {
                    // Allow operation to fail when its removing ghosts.
                    return placementRes;
                }

                if (placementRes.error == GameActions::Status::noClearance)
                {
                    // Some scenery might be obstructed, don't abort the entire operation.
                    continue;
                }
            }
            cost += placementRes.cost;
        }
    }

    auto res = GameActions::Result();
    res.cost = cost;

    return res;
}

static std::optional<GameActions::Result> TrackDesignPlaceEntrances(
    TrackDesignState& tds, const TrackDesign& td, CoordsXYZ newCoords, RideId rideId, money64& totalCost)
{
    for (const auto& entrance : td.entranceElements)
    {
        auto rotation = _currentTrackPieceDirection & 3;
        CoordsXY entranceMapPos = entrance.location.ToCoordsXY();
        auto rotatedEntranceMapPos = entranceMapPos.Rotate(rotation);
        newCoords = { rotatedEntranceMapPos + tds.origin, newCoords.z };

        TrackDesignUpdatePreviewBounds(tds, newCoords);

        auto& gameState = getGameState();

        switch (tds.placeOperation)
        {
            case TrackPlaceOperation::drawOutlines:
                TrackDesignAddSelectedTile(newCoords);
                break;
            case TrackPlaceOperation::placeQuery:
            case TrackPlaceOperation::place:
            case TrackPlaceOperation::placeGhost:
            case TrackPlaceOperation::placeTrackPreview:
            {
                rotation = (rotation + entrance.location.direction) & 3;
                newCoords.z = entrance.location.z * kCoordsZStep;
                newCoords.z += tds.origin.z;

                if (tds.placeOperation != TrackPlaceOperation::placeQuery)
                {
                    auto tile = CoordsXY{ newCoords } + CoordsDirectionDelta[rotation];
                    TileElement* tile_element = MapGetFirstElementAt(tile);
                    if (tile_element == nullptr)
                    {
                        return GameActions::Result(
                            GameActions::Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_TILE_ELEMENT_NOT_FOUND);
                    }

                    do
                    {
                        if (tile_element->GetType() != TileElementType::Track)
                        {
                            continue;
                        }
                        if (tile_element->GetBaseZ() != newCoords.z)
                        {
                            continue;
                        }

                        auto stationIndex = tile_element->AsTrack()->GetStationIndex();
                        CommandFlags flags = { CommandFlag::apply };
                        if (tds.placeOperation == TrackPlaceOperation::placeTrackPreview)
                        {
                            flags = { CommandFlag::apply, CommandFlag::allowDuringPaused, CommandFlag::noSpend };
                        }
                        if (tds.placeOperation == TrackPlaceOperation::placeGhost)
                        {
                            flags = { CommandFlag::apply, CommandFlag::allowDuringPaused, CommandFlag::noSpend,
                                      CommandFlag::ghost };
                        }
                        if (tds.placeOperation == TrackPlaceOperation::placeQuery)
                        {
                            flags = {};
                        }
                        if (tds.isReplay)
                        {
                            flags.set(CommandFlag::replay);
                        }

                        auto rideEntranceExitPlaceAction = GameActions::RideEntranceExitPlaceAction(
                            newCoords, rotation, rideId, stationIndex, entrance.isExit);
                        rideEntranceExitPlaceAction.SetFlags(flags);
                        auto res = flags.has(CommandFlag::apply)
                            ? GameActions::ExecuteNested(&rideEntranceExitPlaceAction, gameState)
                            : GameActions::QueryNested(&rideEntranceExitPlaceAction, gameState);

                        if (res.error != GameActions::Status::ok)
                        {
                            return res;
                        }
                        totalCost += res.cost;
                        tds.entranceExitPlaced = true;
                        _trackDesignPlaceStateEntranceExitPlaced = true;
                        break;
                    } while (!(tile_element++)->IsLastForTile());
                }
                else
                {
                    auto res = GameActions::RideEntranceExitPlaceAction::TrackPlaceQuery(gameState, newCoords, false);
                    if (res.error != GameActions::Status::ok)
                    {
                        return res;
                    }

                    totalCost += res.cost;
                    tds.entranceExitPlaced = true;
                    _trackDesignPlaceStateEntranceExitPlaced = true;
                }
                break;
            }
            case TrackPlaceOperation::removeGhost:
            case TrackPlaceOperation::getPlaceZ:
                break;
        }
    }

    return std::nullopt;
}

static GameActions::Result TrackDesignPlaceMaze(
    TrackDesignState& tds, const TrackDesign& td, const CoordsXYZ& origin, const Ride& ride)
{
    if (tds.placeOperation == TrackPlaceOperation::drawOutlines)
    {
        MapSelection::clearSelectedTiles();
        gMapSelectArrowPosition = CoordsXYZ{ origin, TileElementHeight(origin) };
        gMapSelectArrowDirection = _currentTrackPieceDirection;
    }

    tds.placeZ = 0;
    money64 totalCost = 0;

    auto& gameState = getGameState();

    for (const auto& maze_element : td.mazeElements)
    {
        uint8_t rotation = _currentTrackPieceDirection & 3;
        CoordsXY mazeMapPos = maze_element.location.ToCoordsXY();
        auto mapCoord = mazeMapPos.Rotate(rotation);
        mapCoord += origin;

        TrackDesignUpdatePreviewBounds(tds, { mapCoord, origin.z });

        if (tds.placeOperation == TrackPlaceOperation::drawOutlines)
        {
            TrackDesignAddSelectedTile(mapCoord);
        }

        if (tds.placeOperation == TrackPlaceOperation::placeQuery || tds.placeOperation == TrackPlaceOperation::place
            || tds.placeOperation == TrackPlaceOperation::placeGhost
            || tds.placeOperation == TrackPlaceOperation::placeTrackPreview)
        {
            CommandFlags flags;
            money64 cost = 0;

            uint16_t mazeEntry = Numerics::rol16(maze_element.mazeEntry, rotation * 4);

            if (tds.placeOperation == TrackPlaceOperation::placeTrackPreview)
            {
                flags = { CommandFlag::apply, CommandFlag::allowDuringPaused, CommandFlag::noSpend };
            }
            else if (tds.placeOperation == TrackPlaceOperation::placeGhost)
            {
                flags = { CommandFlag::apply, CommandFlag::allowDuringPaused, CommandFlag::noSpend, CommandFlag::ghost };
            }
            else if (tds.placeOperation == TrackPlaceOperation::placeQuery)
            {
                flags = {};
            }
            else
            {
                flags = { CommandFlag::apply };
            }
            if (tds.isReplay)
            {
                flags.set(CommandFlag::replay);
            }

            auto mazePlace = GameActions::MazePlaceTrackAction({ mapCoord, origin.z }, ride.id, mazeEntry);
            mazePlace.SetFlags(flags);
            auto res = flags.has(CommandFlag::apply) ? GameActions::ExecuteNested(&mazePlace, gameState)
                                                     : GameActions::QueryNested(&mazePlace, gameState);
            if (res.error != GameActions::Status::ok)
            {
                return res;
            }
            cost = res.cost;

            totalCost += cost;
        }

        if (tds.placeOperation == TrackPlaceOperation::getPlaceZ)
        {
            if (!MapIsLocationValid(mapCoord))
            {
                continue;
            }

            auto surfaceElement = MapGetSurfaceElementAt(mapCoord);
            if (surfaceElement == nullptr)
                continue;
            int16_t surfaceZ = surfaceElement->GetBaseZ();
            if (surfaceElement->GetSlope() & kTileSlopeRaisedCornersMask)
            {
                surfaceZ += kLandHeightStep;
                if (surfaceElement->GetSlope() & kTileSlopeDiagonalFlag)
                {
                    surfaceZ += kLandHeightStep;
                }
            }

            int16_t waterZ = surfaceElement->GetWaterHeight();
            if (waterZ > 0 && waterZ > surfaceZ)
            {
                surfaceZ = waterZ;
            }

            int16_t temp_z = origin.z + tds.placeZ - surfaceZ;
            if (temp_z < 0)
            {
                tds.placeZ -= temp_z;
            }
        }
    }

    tds.origin = origin;

    auto result = TrackDesignPlaceEntrances(tds, td, origin, ride.id, totalCost);
    if (result.has_value())
    {
        return result.value();
    }

    if (tds.placeOperation == TrackPlaceOperation::removeGhost)
    {
        auto gameAction = GameActions::RideDemolishAction(ride.id, GameActions::RideModifyType::demolish);
        gameAction.SetFlags({ CommandFlag::allowDuringPaused, CommandFlag::noSpend, CommandFlag::ghost });
        GameActions::Execute(&gameAction, getGameState());
    }

    auto res = GameActions::Result();
    res.cost = totalCost;

    return res;
}

static GameActions::Result TrackDesignPlaceRide(
    TrackDesignState& tds, const TrackDesign& td, const CoordsXYZ& origin, Ride& ride)
{
    tds.origin = origin;
    if (tds.placeOperation == TrackPlaceOperation::drawOutlines)
    {
        MapSelection::clearSelectedTiles();
        gMapSelectArrowPosition = CoordsXYZ{ origin, TileElementHeight(origin) };
        gMapSelectArrowDirection = _currentTrackPieceDirection;
    }

    tds.placeZ = 0;
    money64 totalCost = 0;
    uint8_t rotation = _currentTrackPieceDirection;

    auto& gameState = getGameState();

    // Track elements
    auto newCoords = origin;
    for (const auto& track : td.trackElements)
    {
        auto trackType = track.type;
        const auto& ted = GetTrackElementDescriptor(trackType);

        TrackDesignUpdatePreviewBounds(tds, newCoords);

        switch (tds.placeOperation)
        {
            case TrackPlaceOperation::drawOutlines:
                for (uint8_t i = 0; i < ted.numSequences; i++)
                {
                    const auto& trackBlock = ted.sequences[i].clearance;
                    auto tile = CoordsXY{ newCoords } + CoordsXY{ trackBlock.x, trackBlock.y }.Rotate(rotation);
                    TrackDesignUpdatePreviewBounds(tds, { tile, newCoords.z });
                    TrackDesignAddSelectedTile(tile);
                }
                break;
            case TrackPlaceOperation::removeGhost:
            {
                const TrackCoordinates* trackCoordinates = &ted.coordinates;
                int32_t tempZ = newCoords.z - trackCoordinates->zBegin + ted.sequences[0].clearance.z;
                auto trackRemoveAction = GameActions::TrackRemoveAction(
                    trackType, 0, { newCoords, tempZ, static_cast<Direction>(rotation & 3) });
                trackRemoveAction.SetFlags(
                    { CommandFlag::allowDuringPaused, CommandFlag::noSpend, CommandFlag::ghost, CommandFlag::trackDesign });
                GameActions::ExecuteNested(&trackRemoveAction, gameState);
                break;
            }
            case TrackPlaceOperation::placeQuery:
            case TrackPlaceOperation::place:
            case TrackPlaceOperation::placeGhost:
            case TrackPlaceOperation::placeTrackPreview:
            {
                const TrackCoordinates* trackCoordinates = &ted.coordinates;

                // di
                int16_t tempZ = newCoords.z - trackCoordinates->zBegin;

                SelectedLiftAndInverted liftHillAndAlternativeState{};
                if (track.HasFlag(TrackDesignTrackElementFlag::hasChain))
                {
                    liftHillAndAlternativeState.set(LiftHillAndInverted::liftHill);
                }
                if (track.HasFlag(TrackDesignTrackElementFlag::isInverted))
                {
                    liftHillAndAlternativeState.set(LiftHillAndInverted::inverted);
                }

                CommandFlags flags = { CommandFlag::apply };
                if (tds.placeOperation == TrackPlaceOperation::placeTrackPreview)
                {
                    flags.set(CommandFlag::allowDuringPaused, CommandFlag::noSpend);
                }
                else if (tds.placeOperation == TrackPlaceOperation::placeGhost)
                {
                    flags.set(CommandFlag::allowDuringPaused, CommandFlag::noSpend, CommandFlag::ghost);
                }
                else if (tds.placeOperation == TrackPlaceOperation::placeQuery)
                {
                    flags = { CommandFlag::noSpend };
                }
                if (tds.isReplay)
                {
                    flags.set(CommandFlag::replay);
                }

                auto trackPlaceAction = GameActions::TrackPlaceAction(
                    ride.id, trackType, ride.type, { newCoords, tempZ, rotation }, track.brakeBoosterSpeed, track.colourScheme,
                    track.seatRotation, liftHillAndAlternativeState, true);
                trackPlaceAction.SetFlags(flags);

                auto res = flags.has(CommandFlag::apply) ? GameActions::ExecuteNested(&trackPlaceAction, gameState)
                                                         : GameActions::QueryNested(&trackPlaceAction, gameState);
                if (res.error != GameActions::Status::ok)
                {
                    return res;
                }

                totalCost += res.cost;
                break;
            }
            case TrackPlaceOperation::getPlaceZ:
            {
                int32_t tempZ = newCoords.z - ted.coordinates.zBegin;
                for (uint8_t i = 0; i < ted.numSequences; i++)
                {
                    const auto& trackBlock = ted.sequences[i].clearance;
                    auto tile = CoordsXY{ newCoords } + CoordsXY{ trackBlock.x, trackBlock.y }.Rotate(rotation);
                    if (!MapIsLocationValid(tile))
                    {
                        continue;
                    }

                    auto surfaceElement = MapGetSurfaceElementAt(tile);
                    if (surfaceElement == nullptr)
                    {
                        return GameActions::Result(
                            GameActions::Status::invalidParameters, STR_ERR_INVALID_PARAMETER,
                            STR_ERR_SURFACE_ELEMENT_NOT_FOUND);
                    }

                    int32_t surfaceZ = surfaceElement->GetBaseZ();
                    if (surfaceElement->GetSlope() & kTileSlopeRaisedCornersMask)
                    {
                        surfaceZ += kLandHeightStep;
                        if (surfaceElement->GetSlope() & kTileSlopeDiagonalFlag)
                        {
                            surfaceZ += kLandHeightStep;
                        }
                    }

                    auto waterZ = surfaceElement->GetWaterHeight();
                    if (waterZ > 0 && waterZ > surfaceZ)
                    {
                        surfaceZ = waterZ;
                    }
                    int32_t heightDifference = tempZ + tds.placeZ + trackBlock.z - surfaceZ;
                    if (heightDifference < 0)
                    {
                        tds.placeZ -= heightDifference;
                    }
                }
                break;
            }
        }

        const TrackCoordinates& track_coordinates = ted.coordinates;
        auto offsetAndRotatedTrack = CoordsXY{ newCoords }
            + CoordsXY{ track_coordinates.x, track_coordinates.y }.Rotate(rotation);

        newCoords = { offsetAndRotatedTrack, newCoords.z - track_coordinates.zBegin + track_coordinates.zEnd };
        rotation = (rotation + track_coordinates.rotationEnd - track_coordinates.rotationBegin) & 3;
        if (track_coordinates.rotationEnd & (1 << 2))
        {
            rotation |= (1 << 2);
        }
        else
        {
            newCoords += CoordsDirectionDelta[rotation];
        }
    }

    auto result = TrackDesignPlaceEntrances(tds, td, newCoords, ride.id, totalCost);
    if (result.has_value())
    {
        return result.value();
    }

    if (tds.placeOperation == TrackPlaceOperation::removeGhost)
    {
        ride.validateStations();
        ride.remove();
    }

    auto res = GameActions::Result();
    res.cost = totalCost;

    return res;
}

/**
 * Places a virtual track. This can involve highlighting the surface tiles and showing the track layout. It is also used by
 * the track preview window to place the whole track.
 * Depending on the value of bl it modifies the function.
 * bl == 0, Draw outlines on the ground
 * bl == 1,
 * bl == 2,
 * bl == 3, Returns the z value of a successful placement. Only lower 16 bits are the value, the rest may be garbage?
 * bl == 4,
 * bl == 5, Returns cost to create the track. All 32 bits are used. Places the track. (used by the preview)
 * bl == 6, Clear white outlined track.
 *  rct2: 0x006D01B3
 */
static GameActions::Result TrackDesignPlaceVirtual(
    TrackDesignState& tds, const TrackDesign& td, TrackPlaceOperation ptdOperation, bool placeScenery, Ride& ride,
    const CoordsXYZD& coords, bool isReplay = false)
{
    _trackDesignPlaceStateSceneryUnavailable = false;
    _trackDesignPlaceStateEntranceExitPlaced = false;

    tds.placeScenery = placeScenery;
    tds.entranceExitPlaced = false;
    tds.hasScenery = false;

    tds.isReplay = isReplay;
    tds.placeOperation = ptdOperation;

    tds.previewMin = coords;
    tds.previewMax = coords;
    tds.placeSceneryZ = 0;

    // NOTE: We need to save this, in networked games this would affect all clients otherwise.
    auto savedRideId = _currentRideIndex;
    auto savedTrackPieceDirection = _currentTrackPieceDirection;
    _currentRideIndex = ride.id;
    _currentTrackPieceDirection = coords.direction;

    GameActions::Result trackPlaceRes;
    const auto& rtd = GetRideTypeDescriptor(td.trackAndVehicle.rtdIndex);
    if (rtd.specialType == RtdSpecialType::maze)
    {
        trackPlaceRes = TrackDesignPlaceMaze(tds, td, coords, ride);
    }
    else
    {
        trackPlaceRes = TrackDesignPlaceRide(tds, td, coords, ride);
    }
    _currentRideIndex = savedRideId;
    _currentTrackPieceDirection = savedTrackPieceDirection;

    if (trackPlaceRes.error != GameActions::Status::ok)
    {
        return trackPlaceRes;
    }

    // Scenery elements
    auto sceneryPlaceRes = TrackDesignPlaceAllScenery(tds, td.sceneryElements, coords.direction);
    if (sceneryPlaceRes.error != GameActions::Status::ok)
    {
        return sceneryPlaceRes;
    }

    if (tds.placeOperation == TrackPlaceOperation::place || tds.placeOperation == TrackPlaceOperation::placeTrackPreview)
    {
        ride.chainQueues();
    }

    // 0x6D0FE6
    if (tds.placeOperation == TrackPlaceOperation::drawOutlines)
    {
        gMapSelectFlags.set(MapSelectFlag::enableConstruct);
        gMapSelectFlags.set(MapSelectFlag::enableArrow);
        gMapSelectFlags.unset(MapSelectFlag::green);
    }

    auto res = GameActions::Result();
    res.cost = trackPlaceRes.cost + sceneryPlaceRes.cost;

    return res;
}

GameActions::Result TrackDesignPlace(
    const TrackDesign& td, CommandFlags flags, bool placeScenery, Ride& ride, const CoordsXYZD& coords)
{
    TrackPlaceOperation ptdOperation = flags.has(CommandFlag::apply) ? TrackPlaceOperation::place
                                                                     : TrackPlaceOperation::placeQuery;
    if (flags.hasAll(CommandFlag::apply, CommandFlag::ghost))
    {
        ptdOperation = TrackPlaceOperation::placeGhost;
    }
    bool isReplay = flags.has(CommandFlag::replay);

    TrackDesignState tds{};
    return TrackDesignPlaceVirtual(tds, td, ptdOperation, placeScenery, ride, coords, isReplay);
}

void TrackDesignPreviewRemoveGhosts(const TrackDesign& td, Ride& ride, const CoordsXYZD& coords)
{
    TrackDesignState tds{};
    TrackDesignPlaceVirtual(tds, td, TrackPlaceOperation::removeGhost, true, ride, coords);
}

void TrackDesignPreviewDrawOutlines(
    TrackDesignState& tds, const TrackDesign& td, Ride& ride, const CoordsXYZD& coords, bool placeScenery)
{
    TrackDesignPlaceVirtual(tds, td, TrackPlaceOperation::drawOutlines, placeScenery, ride, coords);
}

static int32_t TrackDesignGetZPlacement(TrackDesignState& tds, const TrackDesign& td, Ride& ride, const CoordsXYZD& coords)
{
    TrackDesignPlaceVirtual(tds, td, TrackPlaceOperation::getPlaceZ, true, ride, coords);

    // Change from vanilla: originally, _trackDesignPlaceSceneryZ was not subtracted
    // from _trackDesignPlaceZ, causing bug #259.
    return tds.placeZ - tds.placeSceneryZ;
}

int32_t TrackDesignGetZPlacement(const TrackDesign& td, Ride& ride, const CoordsXYZD& coords)
{
    TrackDesignState tds{};
    return TrackDesignGetZPlacement(tds, td, ride, coords);
}

static money64 TrackDesignCreateRide(int32_t type, int32_t subType, CommandFlags flags, RideId* outRideIndex)
{
    // Don't set colours as will be set correctly later.
    auto gameAction = GameActions::RideCreateAction(
        type, subType, 0, 0, getGameState().lastEntranceStyle, Config::Get().general.defaultInspectionInterval);
    gameAction.SetFlags(flags);

    auto& gameState = getGameState();

    auto res = GameActions::ExecuteNested(&gameAction, gameState);

    // Callee's of this function expect kMoney64Undefined in case of failure.
    if (res.error != GameActions::Status::ok)
    {
        return kMoney64Undefined;
    }

    *outRideIndex = res.getData<RideId>();

    return res.cost;
}

/**
 *
 *  rct2: 0x006D2189
 * ebx = ride_id
 * cost = edi
 */
static bool TrackDesignPlacePreview(
    TrackDesignState& tds, const TrackDesign& td, Ride** outRide, TrackDesignGameStateData& gameStateData, bool placeScenery)
{
    *outRide = nullptr;
    gameStateData.flags = 0;

    auto& gameState = getGameState();
    auto& objManager = GetContext()->GetObjectManager();
    auto entry_index = objManager.GetLoadedObjectEntryIndex(td.trackAndVehicle.vehicleObject);

    RideId rideIndex;
    CommandFlags rideCreateFlags = { CommandFlag::apply, CommandFlag::allowDuringPaused, CommandFlag::noSpend };
    if (TrackDesignCreateRide(td.trackAndVehicle.rtdIndex, entry_index, rideCreateFlags, &rideIndex) == kMoney64Undefined)
    {
        return false;
    }

    auto ride = GetRide(rideIndex);
    if (ride == nullptr)
        return false;

    ride->customName = {};

    ride->entranceStyle = objManager.GetLoadedObjectEntryIndex(td.appearance.stationObjectIdentifier);
    if (ride->entranceStyle == kObjectEntryIndexNull)
    {
        ride->entranceStyle = gameState.lastEntranceStyle;
    }

    for (size_t i = 0; i < std::min(std::size(ride->trackColours), std::size(td.appearance.trackColours)); i++)
    {
        ride->trackColours[i] = td.appearance.trackColours[i];
    }

    // Flat rides need their vehicle colours loaded for display
    // in the preview window
    if (!GetRideTypeDescriptor(td.trackAndVehicle.rtdIndex).HasFlag(RtdFlag::hasTrack))
    {
        for (size_t i = 0; i < std::size(ride->vehicleColours); i++)
        {
            ride->vehicleColours[i] = td.appearance.vehicleColours[i];
        }
    }

    _trackDesignDrawingPreview = true;
    uint8_t backup_rotation = _currentTrackPieceDirection;
    uint32_t backup_park_flags = gameState.park.flags;
    gameState.park.flags &= ~PARK_FLAGS_FORBID_HIGH_CONSTRUCTION;
    auto mapSize = TileCoordsXY{ gameState.mapSize.x * 16, gameState.mapSize.y * 16 };

    _currentTrackPieceDirection = 0;
    const CoordsXYZD coords = { mapSize.x, mapSize.y, kMinimumLandZ, _currentTrackPieceDirection };
    const int32_t z = kMinimumLandZ + TrackDesignGetZPlacement(tds, td, RideGetTemporaryForPreview(), coords);

    if (tds.hasScenery)
    {
        gameStateData.setFlag(TrackDesignGameStateFlag::HasScenery, true);
    }

    if (_trackDesignPlaceStateSceneryUnavailable)
    {
        placeScenery = false;
        gameStateData.setFlag(TrackDesignGameStateFlag::SceneryUnavailable, true);
    }

    auto res = TrackDesignPlaceVirtual(
        tds, td, TrackPlaceOperation::placeTrackPreview, placeScenery, *ride,
        { mapSize.x, mapSize.y, z, _currentTrackPieceDirection });
    gameState.park.flags = backup_park_flags;

    if (res.error == GameActions::Status::ok)
    {
        if (entry_index == kObjectEntryIndexNull)
        {
            gameStateData.setFlag(TrackDesignGameStateFlag::VehicleUnavailable, true);
        }
        else if (!RideEntryIsInvented(entry_index) && !getGameState().cheats.ignoreResearchStatus)
        {
            gameStateData.setFlag(TrackDesignGameStateFlag::VehicleUnavailable, true);
        }

        _currentTrackPieceDirection = backup_rotation;
        _trackDesignDrawingPreview = false;
        gameStateData.cost = res.cost;
        *outRide = ride;
        return true;
    }

    _currentTrackPieceDirection = backup_rotation;
    ride->remove();
    _trackDesignDrawingPreview = false;
    return false;
}

Direction TrackDesignSceneryElement::getRotation() const
{
    return flags & kRotationMask;
}

void TrackDesignSceneryElement::setRotation(Direction rotation)
{
    flags &= ~kRotationMask;
    flags |= (rotation & kRotationMask);
}

// Small scenery
uint8_t TrackDesignSceneryElement::getQuadrant() const
{
    return (flags & kQuadrantMask) >> 2;
}

void TrackDesignSceneryElement::setQuadrant(uint8_t quadrant)
{
    flags &= ~kQuadrantMask;
    flags |= ((quadrant << 2) & kQuadrantMask);
}

// Path
bool TrackDesignSceneryElement::hasSlope() const
{
    return (flags & kHasSlopeMask) != 0;
}

void TrackDesignSceneryElement::setHasSlope(bool on)
{
    if (on)
        flags |= kHasSlopeMask;
    else
        flags &= ~kHasSlopeMask;
}

Direction TrackDesignSceneryElement::getSlopeDirection() const
{
    return (flags >> 5) % kNumOrthogonalDirections;
}

void TrackDesignSceneryElement::setSlopeDirection(Direction slope)
{
    flags &= ~kSlopeDirectionMask;
    flags |= ((slope << 5) & kSlopeDirectionMask);
}

uint8_t TrackDesignSceneryElement::getEdges() const
{
    return (flags & kEdgesMask);
}

void TrackDesignSceneryElement::setEdges(uint8_t edges)
{
    flags &= ~kEdgesMask;
    flags |= (edges & kEdgesMask);
}

bool TrackDesignSceneryElement::isQueue() const
{
    return (flags & kIsQueueMask) != 0;
}

void TrackDesignSceneryElement::setIsQueue(bool on)
{
    if (on)
        flags |= kIsQueueMask;
    else
        flags &= ~kIsQueueMask;
}

bool TrackDesignSceneryElement::operator==(const TrackDesignSceneryElement& rhs)
{
    return sceneryObject == rhs.sceneryObject && loc == rhs.loc && flags == rhs.flags && primaryColour == rhs.primaryColour
        && secondaryColour == rhs.secondaryColour && tertiaryColour == rhs.tertiaryColour;
}

bool TrackDesignSceneryElement::operator!=(const TrackDesignSceneryElement& rhs)
{
    return !((*this) == rhs);
}

#pragma region Track Design Preview

/**
 *
 *  rct2: 0x006D1EF0
 */
void TrackDesignDrawPreview(TrackDesign& td, uint8_t* pixels, bool placeScenery)
{
    StashMap();
    TrackDesignPreviewClearMap();

    if (gLegacyScene == LegacyScene::trackDesignsManager)
    {
        TrackDesignLoadSceneryObjects(td);
    }

    TrackDesignState tds{};

    Ride* ride;
    TrackDesignGameStateData updatedGameStateData = td.gameStateData;
    if (!TrackDesignPlacePreview(tds, td, &ride, updatedGameStateData, placeScenery))
    {
        std::fill_n(pixels, kTrackPreviewImageSize * 4, 0x00);
        UnstashMap();
        return;
    }
    td.gameStateData = updatedGameStateData;

    CoordsXYZ centre = { (tds.previewMin.x + tds.previewMax.x) / 2 + 16, (tds.previewMin.y + tds.previewMax.y) / 2 + 16,
                         (tds.previewMin.z + tds.previewMax.z) / 2 };

    int32_t size_x = tds.previewMax.x - tds.previewMin.x;
    int32_t size_y = tds.previewMax.y - tds.previewMin.y;
    int32_t size_z = tds.previewMax.z - tds.previewMin.z;

    // Special case for flat rides - Z-axis info is irrelevant
    // and must be zeroed out lest the preview be off-centre
    if (!GetRideTypeDescriptor(td.trackAndVehicle.rtdIndex).HasFlag(RtdFlag::hasTrack))
    {
        centre.z = 0;
        size_z = 0;
    }

    ZoomLevel zoom_level{ 1 };
    if (size_x < size_y)
    {
        size_x = size_y;
    }

    if (size_x > 1000 || size_z > 280)
    {
        zoom_level = ZoomLevel{ 2 };
    }

    if (size_x > 1600 || size_z > 1000)
    {
        zoom_level = ZoomLevel{ 3 };
    }

    size_x = zoom_level.ApplyTo(370);
    size_y = zoom_level.ApplyTo(217);

    Viewport view;
    view.width = 370;
    view.height = 217;
    view.pos = { 0, 0 };
    view.zoom = zoom_level;
    view.flags = VIEWPORT_FLAG_HIDE_BASE | VIEWPORT_FLAG_HIDE_ENTITIES;

    RenderTarget rt;
    rt.x = 0;
    rt.y = 0;
    rt.width = 370;
    rt.height = 217;
    rt.pitch = 0;
    rt.bits = pixels;

    auto drawingEngine = std::make_unique<X8DrawingEngine>(GetContext()->GetUiContext());
    rt.DrawingEngine = drawingEngine.get();

    drawingEngine->BeginDraw();

    const ScreenCoordsXY offset = { size_x / 2, size_y / 2 };
    for (uint8_t i = 0; i < 4; i++)
    {
        view.viewPos = Translate3DTo2DWithZ(i, centre) - offset;
        view.rotation = i;
        ViewportRender(rt, &view);

        rt.bits += kTrackPreviewImageSize;
    }

    drawingEngine->EndDraw();

    ride->remove();
    UnstashMap();
}

/**
 * Resets all the map elements to surface tiles for track preview.
 *  rct2: 0x006D1D9A
 */
static void TrackDesignPreviewClearMap()
{
    auto numTiles = kMaximumMapSizeTechnical * kMaximumMapSizeTechnical;

    auto& gameState = getGameState();
    gameState.mapSize = TRACK_DESIGN_PREVIEW_MAP_SIZE;

    // Reserve ~8 elements per tile
    std::vector<TileElement> tileElements;
    tileElements.reserve(numTiles * 8);

    for (int32_t i = 0; i < numTiles; i++)
    {
        auto* element = &tileElements.emplace_back();
        element->ClearAs(TileElementType::Surface);
        element->SetLastForTile(true);
        element->AsSurface()->SetSlope(kTileSlopeFlat);
        element->AsSurface()->SetWaterHeight(0);
        element->AsSurface()->SetSurfaceObjectIndex(0);
        element->AsSurface()->SetEdgeObjectIndex(0);
        element->AsSurface()->SetGrassLength(GRASS_LENGTH_CLEAR_0);
        element->AsSurface()->SetOwnership(OWNERSHIP_OWNED);
        element->AsSurface()->SetParkFences(0);
    }

    SetTileElements(gameState, std::move(tileElements));
}

bool TrackDesignAreEntranceAndExitPlaced()
{
    return _trackDesignPlaceStateEntranceExitPlaced;
}

#pragma endregion

bool TrackDesignGameStateData::hasFlag(TrackDesignGameStateFlag flag) const
{
    return flags & EnumToFlag(flag);
}

void TrackDesignGameStateData::setFlag(TrackDesignGameStateFlag flag, bool on)
{
    if (on)
        flags |= EnumToFlag(flag);
    else
        flags &= ~EnumToFlag(flag);
}
