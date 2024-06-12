/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TrackDesign.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../Game.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../TrackImporter.h"
#include "../actions/FootpathLayoutPlaceAction.h"
#include "../actions/FootpathRemoveAction.h"
#include "../actions/LargeSceneryPlaceAction.h"
#include "../actions/LargeSceneryRemoveAction.h"
#include "../actions/MazePlaceTrackAction.h"
#include "../actions/RideCreateAction.h"
#include "../actions/RideDemolishAction.h"
#include "../actions/RideEntranceExitPlaceAction.h"
#include "../actions/SmallSceneryPlaceAction.h"
#include "../actions/SmallSceneryRemoveAction.h"
#include "../actions/TrackPlaceAction.h"
#include "../actions/TrackRemoveAction.h"
#include "../actions/WallPlaceAction.h"
#include "../actions/WallRemoveAction.h"
#include "../audio/audio.h"
#include "../core/DataSerialiser.h"
#include "../core/File.h"
#include "../core/Numerics.hpp"
#include "../core/String.hpp"
#include "../drawing/X8DrawingEngine.h"
#include "../interface/Viewport.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../network/network.h"
#include "../object/FootpathObject.h"
#include "../object/FootpathSurfaceObject.h"
#include "../object/LargeSceneryEntry.h"
#include "../object/ObjectEntryManager.h"
#include "../object/ObjectList.h"
#include "../object/ObjectManager.h"
#include "../object/ObjectRepository.h"
#include "../object/SmallSceneryEntry.h"
#include "../object/StationObject.h"
#include "../rct2/RCT2.h"
#include "../ride/RideConstruction.h"
#include "../util/SawyerCoding.h"
#include "../util/Util.h"
#include "../world/Footpath.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/Surface.h"
#include "../world/Wall.h"
#include "../world/tile_element/Slope.h"
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
    const auto* stationObject = ride.GetStationObject();
    if (stationObject == nullptr)
        return "";

    return stationObject->GetIdentifier();
}

ResultWithMessage TrackDesign::CreateTrackDesign(TrackDesignState& tds, const Ride& ride)
{
    type = ride.type;

    auto object = ObjectEntryGetObject(ObjectType::Ride, ride.subtype);
    if (object != nullptr)
    {
        auto entry = object->GetObjectEntry();
        // Remove this check for new track design format
        if (entry.IsEmpty())
        {
            return { false, STR_VEHICLE_UNSUPPORTED_TD6 };
        }
        vehicleObject = ObjectEntryDescriptor(entry);
    }

    rideMode = ride.mode;
    colourScheme = ride.colour_scheme_type & 3;

    for (size_t i = 0; i < std::size(vehicleColours); i++)
    {
        vehicleColours[i] = ride.vehicle_colours[i];
    }

    for (int32_t i = 0; i < OpenRCT2::Limits::kNumColourSchemes; i++)
    {
        trackSpineColour[i] = ride.track_colour[i].main;
        trackRailColour[i] = ride.track_colour[i].additional;
        trackSupportColour[i] = ride.track_colour[i].supports;
    }

    departFlags = ride.depart_flags;
    numberOfTrains = ride.NumTrains;
    numberOfCarsPerTrain = ride.num_cars_per_train;
    minWaitingTime = ride.min_waiting_time;
    maxWaitingTime = ride.max_waiting_time;
    operationSetting = ride.operation_option;
    liftHillSpeed = ride.lift_hill_speed;
    numCircuits = ride.num_circuits;

    stationObjectIdentifier = TrackDesignGetStationObjectIdentifier(ride);
    maxSpeed = static_cast<int8_t>(ride.max_speed / 65536);
    averageSpeed = static_cast<int8_t>(ride.average_speed / 65536);
    rideLength = ride.GetTotalLength() / 65536;
    maxPositiveVerticalG = ride.max_positive_vertical_g / 32;
    maxNegativeVerticalG = ride.max_negative_vertical_g / 32;
    maxLateralG = ride.max_lateral_g / 32;
    holes = ride.holes & 0x1F;
    inversions = ride.inversions & 0x1F;
    inversions |= (ride.sheltered_eighths << 5);
    drops = ride.drops;
    highestDropHeight = ride.highest_drop_height;

    uint16_t _totalAirTime = (ride.total_air_time * 123) / 1024;
    if (_totalAirTime > 255)
    {
        _totalAirTime = 0;
    }
    totalAirTime = static_cast<uint8_t>(_totalAirTime);

    excitement = ride.ratings.Excitement / 10;
    intensity = ride.ratings.Intensity / 10;
    nausea = ride.ratings.Nausea / 10;

    upkeepCost = ride.upkeep_cost;

    const auto& rtd = GetRideTypeDescriptor(type);

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

    StringId warningMessage = STR_NONE;

    RideGetStartOfTrack(&trackElement);

    int32_t z = trackElement.element->GetBaseZ();
    auto trackType = trackElement.element->AsTrack()->GetTrackType();
    uint8_t direction = trackElement.element->GetDirection();
    _saveDirection = direction;
    auto newCoords = GetTrackElementOriginAndApplyChanges(
        { trackElement, z, direction }, trackType, 0, &trackElement.element, 0);

    if (!newCoords.has_value())
    {
        return { false, STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY };
    }
    trackElement.x = newCoords->x;
    trackElement.y = newCoords->y;
    z = newCoords->z;

    const auto& ted = GetTrackElementDescriptor(trackElement.element->AsTrack()->GetTrackType());
    const TrackCoordinates* trackCoordinates = &ted.Coordinates;
    const auto* trackBlock = ted.Block;
    // Used in the following loop to know when we have
    // completed all of the elements and are back at the
    // start.
    TileElement* initialMap = trackElement.element;

    CoordsXYZ startPos = { trackElement.x, trackElement.y, z + trackCoordinates->z_begin - trackBlock->z };
    tds.Origin = startPos;

    do
    {
        const auto& element = trackElement.element->AsTrack();

        // Remove this check for new track design format
        if (element->GetTrackType() > TrackElemType::HighestAlias)
        {
            return { false, STR_TRACK_ELEM_UNSUPPORTED_TD6 };
        }

        TrackDesignTrackElement track{};
        track.Type = element->GetTrackType();
        track.ColourScheme = element->GetColourScheme();
        track.StationIndex = element->GetStationIndex();
        track.BrakeBoosterSpeed = element->GetBrakeBoosterSpeed();
        track.SeatRotation = element->GetSeatRotation();

        // This warning will not apply to new track design format
        if (track.Type == TrackElemType::BlockBrakes && element->GetBrakeBoosterSpeed() != kRCT2DefaultBlockBrakeSpeed)
        {
            warningMessage = STR_TRACK_DESIGN_BLOCK_BRAKE_SPEED_RESET;
        }

        if (element->HasChain())
            track.SetFlag(TrackDesignTrackElementFlag::HasChain);

        if (ride.GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_ALTERNATIVE_TRACK_TYPE) && element->IsInverted())
        {
            track.SetFlag(TrackDesignTrackElementFlag::IsInverted);
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
            { trackElement, z, direction }, trackType, 0, &trackElement.element, 0);

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
        for (const auto& station : ride.GetStations())
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

            mapLocation -= tds.Origin;
            // Rotate entrance coordinates backwards to the correct direction
            auto rotatedMapLocation = TileCoordsXY(mapLocation.Rotate(0 - _saveDirection));

            z -= tds.Origin.z;
            z /= COORDS_Z_STEP;

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

    TrackDesignPreviewDrawOutlines(tds, this, RideGetTemporaryForPreview(), { 4096, 4096, 0, _currentTrackPieceDirection });

    // Resave global vars for scenery reasons.
    tds.Origin = startPos;

    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_GREEN;

    spaceRequiredX = ((tds.PreviewMax.x - tds.PreviewMin.x) / 32) + 1;
    spaceRequiredY = ((tds.PreviewMax.y - tds.PreviewMin.y) / 32) + 1;
    return { true, warningMessage };
}

ResultWithMessage TrackDesign::CreateTrackDesignMaze(TrackDesignState& tds, const Ride& ride)
{
    auto startLoc = MazeGetFirstElement(ride);

    if (startLoc.element == nullptr)
    {
        return { false, STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY };
    }

    tds.Origin = { startLoc.x, startLoc.y, startLoc.element->GetBaseZ() };

    // x is defined here as we can start the search
    // on tile start_x, start_y but then the next row
    // must restart on 0
    for (int32_t y = startLoc.y, x = startLoc.x; y < MAXIMUM_MAP_SIZE_BIG; y += COORDS_XY_STEP)
    {
        for (; x < MAXIMUM_MAP_SIZE_BIG; x += COORDS_XY_STEP)
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
                maze.location.x = (x - startLoc.x) / COORDS_XY_STEP;
                maze.location.y = (y - startLoc.y) / COORDS_XY_STEP;
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

    auto location = ride.GetStation().Entrance;
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

    location = ride.GetStation().Exit;
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
    int32_t startZ = tds.Origin.z;
    TrackDesignPreviewDrawOutlines(tds, this, RideGetTemporaryForPreview(), { 4096, 4096, 0, _currentTrackPieceDirection });
    tds.Origin = { startLoc.x, startLoc.y, startZ };

    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_GREEN;

    spaceRequiredX = ((tds.PreviewMax.x - tds.PreviewMin.x) / 32) + 1;
    spaceRequiredY = ((tds.PreviewMax.y - tds.PreviewMin.y) / 32) + 1;
    return { true };
}

CoordsXYE TrackDesign::MazeGetFirstElement(const Ride& ride)
{
    CoordsXYE tile{};
    for (tile.y = 0; tile.y < MAXIMUM_MAP_SIZE_BIG; tile.y += COORDS_XY_STEP)
    {
        for (tile.x = 0; tile.x < MAXIMUM_MAP_SIZE_BIG; tile.x += COORDS_XY_STEP)
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
            case ObjectType::Paths:
            {
                uint8_t slope = (scenery.getSlopeDirection() - _saveDirection) % NumOrthogonalDirections;
                scenery.setSlopeDirection(slope);

                uint8_t edges = Numerics::ror4(scenery.getEdges(), _saveDirection);
                scenery.setEdges(edges);
                break;
            }
            case ObjectType::Walls:
            {
                auto direction = (scenery.getRotation() - _saveDirection) % NumOrthogonalDirections;
                scenery.setRotation(direction);
                break;
            }
            default:
            {
                auto direction = (scenery.getRotation() - _saveDirection) % NumOrthogonalDirections;
                scenery.setRotation(direction);

                auto quadrant = (scenery.getQuadrant() - _saveDirection) % NumOrthogonalDirections;
                scenery.setQuadrant(quadrant);

                break;
            }
        }

        const auto relativeMapPosition = scenery.loc - tds.Origin;
        const CoordsXY rotatedRelativeMapPos = relativeMapPosition.Rotate(0 - _saveDirection);

        if (rotatedRelativeMapPos.x > 127 * COORDS_XY_STEP || rotatedRelativeMapPos.y > 127 * COORDS_XY_STEP
            || rotatedRelativeMapPos.x < -126 * COORDS_XY_STEP || rotatedRelativeMapPos.y < -126 * COORDS_XY_STEP)
        {
            return { false, STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY };
        }

        if (relativeMapPosition.z > 127 * COORDS_Z_STEP || relativeMapPosition.z < -126 * COORDS_Z_STEP)
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
        stream << DS_TAG(name);
        // There is too much information logged.
        // See sub actions for this information if required.
        return;
    }
    stream << DS_TAG(type);
    stream << DS_TAG(vehicleType);
    stream << DS_TAG(cost);
    stream << DS_TAG(rideMode);
    stream << DS_TAG(trackFlags);
    stream << DS_TAG(colourScheme);
    stream << DS_TAG(vehicleColours);
    stream << DS_TAG(stationObjectIdentifier);
    stream << DS_TAG(totalAirTime);
    stream << DS_TAG(departFlags);
    stream << DS_TAG(numberOfTrains);
    stream << DS_TAG(numberOfCarsPerTrain);
    stream << DS_TAG(minWaitingTime);
    stream << DS_TAG(maxWaitingTime);
    stream << DS_TAG(operationSetting);
    stream << DS_TAG(maxSpeed);
    stream << DS_TAG(averageSpeed);
    stream << DS_TAG(rideLength);
    stream << DS_TAG(maxPositiveVerticalG);
    stream << DS_TAG(maxNegativeVerticalG);
    stream << DS_TAG(maxLateralG);
    stream << DS_TAG(inversions);
    stream << DS_TAG(holes);
    stream << DS_TAG(drops);
    stream << DS_TAG(highestDropHeight);
    stream << DS_TAG(excitement);
    stream << DS_TAG(intensity);
    stream << DS_TAG(nausea);
    stream << DS_TAG(upkeepCost);
    stream << DS_TAG(trackSpineColour);
    stream << DS_TAG(trackRailColour);
    stream << DS_TAG(trackSupportColour);
    stream << DS_TAG(vehicleObject);
    stream << DS_TAG(spaceRequiredX);
    stream << DS_TAG(spaceRequiredY);
    stream << DS_TAG(liftHillSpeed);
    stream << DS_TAG(numCircuits);

    stream << DS_TAG(mazeElements);
    stream << DS_TAG(trackElements);
    stream << DS_TAG(entranceElements);
    stream << DS_TAG(sceneryElements);

    stream << DS_TAG(name);
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
static void TrackDesignLoadSceneryObjects(TrackDesign* td6)
{
    auto& objectManager = OpenRCT2::GetContext()->GetObjectManager();
    objectManager.UnloadAllTransient();

    // Load ride object
    if (td6->vehicleObject.HasValue())
    {
        objectManager.LoadObject(td6->vehicleObject);
    }

    // Load scenery objects
    for (const auto& scenery : td6->sceneryElements)
    {
        if (scenery.sceneryObject.HasValue())
        {
            objectManager.LoadObject(scenery.sceneryObject);
        }
    }
}

struct TrackSceneryEntry
{
    ObjectType Type = ObjectType::None;
    ObjectEntryIndex Index = OBJECT_ENTRY_INDEX_NULL;
    ObjectEntryIndex SecondaryIndex = OBJECT_ENTRY_INDEX_NULL; // For footpath railing
};

static ObjectEntryIndex TrackDesignGetDefaultSurfaceIndex(bool isQueue)
{
    for (ObjectEntryIndex i = 0; i < MAX_FOOTPATH_SURFACE_OBJECTS; i++)
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
    return OBJECT_ENTRY_INDEX_NULL;
}

static ObjectEntryIndex TrackDesignGetDefaultRailingIndex()
{
    for (ObjectEntryIndex i = 0; i < MAX_FOOTPATH_RAILINGS_OBJECTS; i++)
    {
        auto footpathRailingsObj = GetPathRailingsEntry(i);
        if (footpathRailingsObj != nullptr)
        {
            return i;
        }
    }
    return OBJECT_ENTRY_INDEX_NULL;
}

static std::optional<TrackSceneryEntry> TrackDesignPlaceSceneryElementGetEntry(const TrackDesignSceneryElement& scenery)
{
    TrackSceneryEntry result;

    auto& objectMgr = OpenRCT2::GetContext()->GetObjectManager();
    if (scenery.sceneryObject.GetType() == ObjectType::Paths)
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
                result.Type = ObjectType::FootpathSurface;
            }
        }
        else
        {
            result.Type = ObjectType::FootpathSurface;
            result.Index = objectMgr.GetLoadedObjectEntryIndex(
                ObjectEntryDescriptor(scenery.isQueue() ? footpathMapping->QueueSurface : footpathMapping->NormalSurface));
            result.SecondaryIndex = objectMgr.GetLoadedObjectEntryIndex(ObjectEntryDescriptor(footpathMapping->Railing));
        }

        if (result.Index == OBJECT_ENTRY_INDEX_NULL)
            result.Index = TrackDesignGetDefaultSurfaceIndex(scenery.isQueue());
        if (result.SecondaryIndex == OBJECT_ENTRY_INDEX_NULL)
            result.SecondaryIndex = TrackDesignGetDefaultRailingIndex();

        if (result.Index == OBJECT_ENTRY_INDEX_NULL || result.SecondaryIndex == OBJECT_ENTRY_INDEX_NULL)
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
            if (!GetGameState().Cheats.IgnoreResearchStatus)
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
static void TrackDesignMirrorScenery(TrackDesign* td6)
{
    auto& objectMgr = OpenRCT2::GetContext()->GetObjectManager();
    for (auto& scenery : td6->sceneryElements)
    {
        auto entryInfo = TrackDesignPlaceSceneryElementGetEntry(scenery);
        if (!entryInfo)
            continue;

        auto obj = objectMgr.GetLoadedObject(entryInfo->Type, entryInfo->Index);
        switch (obj->GetObjectType())
        {
            case ObjectType::LargeScenery:
            {
                auto* sceneryEntry = reinterpret_cast<const LargeSceneryEntry*>(obj->GetLegacyData());
                int16_t x1 = 0, x2 = 0, y1 = 0, y2 = 0;
                for (LargeSceneryTile* tile = sceneryEntry->tiles; tile->x_offset != -1; tile++)
                {
                    if (x1 > tile->x_offset)
                    {
                        x1 = tile->x_offset;
                    }
                    if (x2 < tile->x_offset)
                    {
                        x2 = tile->x_offset;
                    }
                    if (y1 > tile->y_offset)
                    {
                        y1 = tile->y_offset;
                    }
                    if (y2 < tile->y_offset)
                    {
                        y2 = tile->y_offset;
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
            case ObjectType::SmallScenery:
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
            case ObjectType::Walls:
            {
                scenery.loc.y = -scenery.loc.y;
                scenery.setRotation(DirectionFlipXAxis(scenery.getRotation()));
                break;
            }
            case ObjectType::Paths:
            case ObjectType::FootpathSurface:
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
static void TrackDesignMirrorRide(TrackDesign* td6)
{
    for (auto& track : td6->trackElements)
    {
        const auto& ted = GetTrackElementDescriptor(track.Type);
        track.Type = ted.MirrorElement;
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
static void TrackDesignMirrorMaze(TrackDesign* td6)
{
    for (auto& maze : td6->mazeElements)
    {
        maze.location.y = -maze.location.y;

        auto mazeEntry = maze.mazeEntry;
        uint16_t newEntry = 0;
        for (uint8_t position = UtilBitScanForward(mazeEntry); position != 0xFF; position = UtilBitScanForward(mazeEntry))
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
void TrackDesignMirror(TrackDesign* td6)
{
    const auto& rtd = GetRideTypeDescriptor(td6->type);
    if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_MAZE))
    {
        TrackDesignMirrorMaze(td6);
    }
    else
    {
        TrackDesignMirrorRide(td6);
    }
    TrackDesignMirrorEntrances(*td6);
    TrackDesignMirrorScenery(td6);
}

static void TrackDesignAddSelectedTile(const CoordsXY& coords)
{
    auto tileIterator = std::find(gMapSelectionTiles.begin(), gMapSelectionTiles.end(), coords);
    if (tileIterator == gMapSelectionTiles.end())
    {
        gMapSelectionTiles.push_back(coords);
    }
}

static void TrackDesignUpdatePreviewBounds(TrackDesignState& tds, const CoordsXYZ& coords)
{
    tds.PreviewMin = { std::min(tds.PreviewMin.x, coords.x), std::min(tds.PreviewMin.y, coords.y),
                       std::min(tds.PreviewMin.z, coords.z) };
    tds.PreviewMax = { std::max(tds.PreviewMax.x, coords.x), std::max(tds.PreviewMax.y, coords.y),
                       std::max(tds.PreviewMax.z, coords.z) };
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
    const uint32_t flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND
        | GAME_COMMAND_FLAG_GHOST;
    std::unique_ptr<GameAction> ga;
    switch (entryInfo->Type)
    {
        case ObjectType::SmallScenery:
        {
            uint8_t quadrant = scenery.getQuadrant() + _currentTrackPieceDirection;
            quadrant &= 3;

            auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<SmallSceneryEntry>(entryInfo->Index);
            if (!(!sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE) && sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_DIAGONAL))
                && sceneryEntry->HasFlag(
                    SMALL_SCENERY_FLAG_DIAGONAL | SMALL_SCENERY_FLAG_HALF_SPACE | SMALL_SCENERY_FLAG_THREE_QUARTERS))
            {
                quadrant = 0;
            }

            ga = std::make_unique<SmallSceneryRemoveAction>(CoordsXYZ{ mapCoord.x, mapCoord.y, z }, quadrant, entryInfo->Index);
            break;
        }
        case ObjectType::LargeScenery:
            ga = std::make_unique<LargeSceneryRemoveAction>(CoordsXYZD{ mapCoord.x, mapCoord.y, z, sceneryRotation }, 0);
            break;
        case ObjectType::Walls:
            ga = std::make_unique<WallRemoveAction>(CoordsXYZD{ mapCoord.x, mapCoord.y, z, sceneryRotation });
            break;
        case ObjectType::Paths:
        case ObjectType::FootpathSurface:
            ga = std::make_unique<FootpathRemoveAction>(CoordsXYZ{ mapCoord.x, mapCoord.y, z });
            break;
        default:
            return GameActions::Result();
    }

    ga->SetFlags(flags);
    return GameActions::ExecuteNested(ga.get());
}

static bool TrackDesignPlaceSceneryElementGetPlaceZ(TrackDesignState& tds, const TrackDesignSceneryElement& scenery)
{
    int32_t z = scenery.loc.z + tds.PlaceZ;
    if (z < tds.PlaceSceneryZ)
    {
        tds.PlaceSceneryZ = z;
    }

    TrackDesignPlaceSceneryElementGetEntry(scenery);
    return true;
}

static GameActions::Result TrackDesignPlaceSceneryElement(
    TrackDesignState& tds, CoordsXY mapCoord, uint8_t mode, const TrackDesignSceneryElement& scenery, uint8_t rotation,
    int32_t originZ)
{
    if (tds.PlaceOperation == PTD_OPERATION_DRAW_OUTLINES && mode == 0)
    {
        TrackDesignAddSelectedTile(mapCoord);
        return GameActions::Result();
    }

    if (tds.PlaceOperation == PTD_OPERATION_REMOVE_GHOST && mode == 0)
    {
        return TrackDesignPlaceSceneryElementRemoveGhost(mapCoord, scenery, rotation, originZ);
    }

    if (tds.PlaceOperation == PTD_OPERATION_GET_PLACE_Z)
    {
        TrackDesignPlaceSceneryElementGetPlaceZ(tds, scenery);
        return GameActions::Result();
    }

    money64 cost = 0;

    if (tds.PlaceOperation != PTD_OPERATION_PLACE_QUERY && tds.PlaceOperation != PTD_OPERATION_PLACE
        && tds.PlaceOperation != PTD_OPERATION_PLACE_GHOST && tds.PlaceOperation != PTD_OPERATION_PLACE_TRACK_PREVIEW)
    {
        return GameActions::Result();
    }

    auto entryInfo = TrackDesignPlaceSceneryElementGetEntry(scenery);
    if (!entryInfo)
    {
        return GameActions::Result();
    }

    int16_t z;
    uint8_t flags;

    switch (entryInfo->Type)
    {
        case ObjectType::SmallScenery:
        {
            if (mode != 0)
            {
                return GameActions::Result();
            }

            rotation += scenery.getRotation();
            rotation &= 3;
            z = scenery.loc.z + originZ;
            uint8_t quadrant = (scenery.getQuadrant() + _currentTrackPieceDirection) & 3;

            flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_TRACK_DESIGN;
            if (tds.PlaceOperation == PTD_OPERATION_PLACE_TRACK_PREVIEW)
            {
                flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_TRACK_DESIGN | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED
                    | GAME_COMMAND_FLAG_NO_SPEND;
            }
            else if (tds.PlaceOperation == PTD_OPERATION_PLACE_GHOST)
            {
                flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_TRACK_DESIGN | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED
                    | GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_NO_SPEND;
            }
            else if (tds.PlaceOperation == PTD_OPERATION_PLACE_QUERY)
            {
                flags = GAME_COMMAND_FLAG_TRACK_DESIGN;
            }
            if (tds.IsReplay)
            {
                flags |= GAME_COMMAND_FLAG_REPLAY;
            }

            auto smallSceneryPlace = SmallSceneryPlaceAction(
                { mapCoord.x, mapCoord.y, z, rotation }, quadrant, entryInfo->Index, scenery.primaryColour,
                scenery.secondaryColour, scenery.tertiaryColour);

            smallSceneryPlace.SetFlags(flags);
            auto res = flags & GAME_COMMAND_FLAG_APPLY ? GameActions::ExecuteNested(&smallSceneryPlace)
                                                       : GameActions::QueryNested(&smallSceneryPlace);

            cost = res.Error == GameActions::Status::Ok ? res.Cost : 0;
            break;
        }
        case ObjectType::LargeScenery:
        {
            if (mode != 0)
            {
                return GameActions::Result();
            }

            rotation += scenery.getRotation();
            rotation &= 3;

            z = scenery.loc.z + originZ;

            flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_TRACK_DESIGN;
            if (tds.PlaceOperation == PTD_OPERATION_PLACE_TRACK_PREVIEW)
            {
                flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_TRACK_DESIGN | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED
                    | GAME_COMMAND_FLAG_NO_SPEND;
            }
            else if (tds.PlaceOperation == PTD_OPERATION_PLACE_GHOST)
            {
                flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_TRACK_DESIGN | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED
                    | GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_NO_SPEND;
            }
            else if (tds.PlaceOperation == PTD_OPERATION_PLACE_QUERY)
            {
                flags = GAME_COMMAND_FLAG_TRACK_DESIGN;
            }
            if (tds.IsReplay)
            {
                flags |= GAME_COMMAND_FLAG_REPLAY;
            }
            auto sceneryPlaceAction = LargeSceneryPlaceAction(
                { mapCoord.x, mapCoord.y, z, rotation }, entryInfo->Index, scenery.primaryColour, scenery.secondaryColour,
                scenery.tertiaryColour);
            sceneryPlaceAction.SetFlags(flags);
            auto res = flags & GAME_COMMAND_FLAG_APPLY ? GameActions::ExecuteNested(&sceneryPlaceAction)
                                                       : GameActions::QueryNested(&sceneryPlaceAction);

            cost = res.Cost;
            break;
        }
        case ObjectType::Walls:
        {
            if (mode != 0)
            {
                return GameActions::Result();
            }

            z = scenery.loc.z + originZ;
            rotation += scenery.getRotation();
            rotation &= 3;

            flags = GAME_COMMAND_FLAG_APPLY;
            if (tds.PlaceOperation == PTD_OPERATION_PLACE_TRACK_PREVIEW)
            {
                flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_TRACK_DESIGN | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED
                    | GAME_COMMAND_FLAG_NO_SPEND;
            }
            else if (tds.PlaceOperation == PTD_OPERATION_PLACE_GHOST)
            {
                flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND
                    | GAME_COMMAND_FLAG_GHOST;
            }
            else if (tds.PlaceOperation == PTD_OPERATION_PLACE_QUERY)
            {
                flags = 0;
            }
            if (tds.IsReplay)
            {
                flags |= GAME_COMMAND_FLAG_REPLAY;
            }
            auto wallPlaceAction = WallPlaceAction(
                entryInfo->Index, { mapCoord.x, mapCoord.y, z }, rotation, scenery.primaryColour, scenery.secondaryColour,
                scenery.tertiaryColour);
            wallPlaceAction.SetFlags(flags);
            auto res = flags & GAME_COMMAND_FLAG_APPLY ? GameActions::ExecuteNested(&wallPlaceAction)
                                                       : GameActions::QueryNested(&wallPlaceAction);

            cost = res.Cost;
            break;
        }
        case ObjectType::Paths:
        case ObjectType::FootpathSurface:
            z = scenery.loc.z + originZ;
            if (mode == 0)
            {
                flags = GAME_COMMAND_FLAG_APPLY;
                if (tds.PlaceOperation == PTD_OPERATION_PLACE_TRACK_PREVIEW)
                {
                    flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND;
                }
                if (tds.PlaceOperation == PTD_OPERATION_PLACE_GHOST)
                {
                    flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND
                        | GAME_COMMAND_FLAG_GHOST;
                }
                if (tds.PlaceOperation == PTD_OPERATION_PLACE_QUERY)
                {
                    flags = 0;
                }
                if (tds.IsReplay)
                {
                    flags |= GAME_COMMAND_FLAG_REPLAY;
                }
                uint8_t slope = (scenery.getSlopeDirection() + rotation) & 0x3;
                if (scenery.hasSlope())
                    slope |= FOOTPATH_PROPERTIES_FLAG_IS_SLOPED;
                uint8_t edges = Numerics::rol4(scenery.getEdges(), rotation);
                PathConstructFlags constructFlags = 0;
                if (scenery.isQueue())
                    constructFlags |= PathConstructFlag::IsQueue;
                if (entryInfo->Type == ObjectType::Paths)
                    constructFlags |= PathConstructFlag::IsLegacyPathObject;
                auto footpathPlaceAction = FootpathLayoutPlaceAction(
                    { mapCoord.x, mapCoord.y, z }, slope, entryInfo->Index, entryInfo->SecondaryIndex, edges, constructFlags);
                footpathPlaceAction.SetFlags(flags);
                auto res = flags & GAME_COMMAND_FLAG_APPLY ? GameActions::ExecuteNested(&footpathPlaceAction)
                                                           : GameActions::QueryNested(&footpathPlaceAction);
                // Ignore failures
                cost = res.Error == GameActions::Status::Ok ? res.Cost : 0;
            }
            else
            {
                if (tds.PlaceOperation == PTD_OPERATION_PLACE_QUERY)
                {
                    return GameActions::Result();
                }

                auto* pathElement = MapGetPathElementAt(TileCoordsXYZ{ CoordsXYZ{ mapCoord.x, mapCoord.y, z } });
                if (pathElement == nullptr)
                {
                    return GameActions::Result();
                }

                if (tds.PlaceOperation == PTD_OPERATION_PLACE)
                {
                    FootpathQueueChainReset();
                    FootpathRemoveEdgesAt(mapCoord, reinterpret_cast<TileElement*>(pathElement));
                }

                flags = GAME_COMMAND_FLAG_APPLY;
                if (tds.PlaceOperation == PTD_OPERATION_PLACE_TRACK_PREVIEW)
                {
                    flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND;
                }
                if (tds.PlaceOperation == PTD_OPERATION_PLACE_GHOST)
                {
                    flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND
                        | GAME_COMMAND_FLAG_GHOST;
                }
                if (tds.IsReplay)
                {
                    flags |= GAME_COMMAND_FLAG_REPLAY;
                }

                if (tds.PlaceOperation == PTD_OPERATION_PLACE)
                {
                    FootpathConnectEdges(mapCoord, reinterpret_cast<TileElement*>(pathElement), flags);
                    FootpathUpdateQueueChains();
                }

                return GameActions::Result();
            }
            break;
        default:
            _trackDesignPlaceStateSceneryUnavailable = true;
            return GameActions::Result();
    }

    auto res = GameActions::Result();
    res.Cost = cost;

    return res;
}

/**
 *
 *  rct2: 0x006D0964
 */
static GameActions::Result TrackDesignPlaceAllScenery(
    TrackDesignState& tds, const std::vector<TrackDesignSceneryElement>& sceneryList, uint8_t rotation)
{
    const auto& origin = tds.Origin;

    money64 cost = 0;

    for (uint8_t mode = 0; mode <= 1; mode++)
    {
        if (!sceneryList.empty())
        {
            tds.HasScenery = true;
        }

        if (!tds.PlaceScenery)
        {
            continue;
        }

        for (const auto& scenery : sceneryList)
        {
            auto mapCoord = CoordsXYZ{ CoordsXY(origin) + scenery.loc.Rotate(rotation), origin.z };
            TrackDesignUpdatePreviewBounds(tds, mapCoord);

            auto placementRes = TrackDesignPlaceSceneryElement(tds, mapCoord, mode, scenery, rotation, origin.z);
            if (placementRes.Error != GameActions::Status::Ok)
            {
                // Allow operation to fail when its removing ghosts.
                if (tds.PlaceOperation != PTD_OPERATION_REMOVE_GHOST)
                {
                    return placementRes;
                }
            }
            cost += placementRes.Cost;
        }
    }

    auto res = GameActions::Result();
    res.Cost = cost;

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
        newCoords = { rotatedEntranceMapPos + tds.Origin, newCoords.z };

        TrackDesignUpdatePreviewBounds(tds, newCoords);

        switch (tds.PlaceOperation)
        {
            case PTD_OPERATION_DRAW_OUTLINES:
                TrackDesignAddSelectedTile(newCoords);
                break;
            case PTD_OPERATION_PLACE_QUERY:
            case PTD_OPERATION_PLACE:
            case PTD_OPERATION_PLACE_GHOST:
            case PTD_OPERATION_PLACE_TRACK_PREVIEW:
            {
                rotation = (rotation + entrance.location.direction) & 3;
                newCoords.z = entrance.location.z * COORDS_Z_STEP;
                newCoords.z += tds.Origin.z;

                if (tds.PlaceOperation != PTD_OPERATION_PLACE_QUERY)
                {
                    auto tile = CoordsXY{ newCoords } + CoordsDirectionDelta[rotation];
                    TileElement* tile_element = MapGetFirstElementAt(tile);
                    if (tile_element == nullptr)
                    {
                        return GameActions::Result(
                            GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_TILE_ELEMENT_NOT_FOUND);
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
                        uint8_t flags = GAME_COMMAND_FLAG_APPLY;
                        if (tds.PlaceOperation == PTD_OPERATION_PLACE_TRACK_PREVIEW)
                        {
                            flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED
                                | GAME_COMMAND_FLAG_NO_SPEND;
                        }
                        if (tds.PlaceOperation == PTD_OPERATION_PLACE_GHOST)
                        {
                            flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND
                                | GAME_COMMAND_FLAG_GHOST;
                        }
                        if (tds.PlaceOperation == PTD_OPERATION_PLACE_QUERY)
                        {
                            flags = 0;
                        }
                        if (tds.IsReplay)
                        {
                            flags |= GAME_COMMAND_FLAG_REPLAY;
                        }

                        auto rideEntranceExitPlaceAction = RideEntranceExitPlaceAction(
                            newCoords, rotation, rideId, stationIndex, entrance.isExit);
                        rideEntranceExitPlaceAction.SetFlags(flags);
                        auto res = flags & GAME_COMMAND_FLAG_APPLY ? GameActions::ExecuteNested(&rideEntranceExitPlaceAction)
                                                                   : GameActions::QueryNested(&rideEntranceExitPlaceAction);

                        if (res.Error != GameActions::Status::Ok)
                        {
                            return res;
                        }
                        totalCost += res.Cost;
                        tds.EntranceExitPlaced = true;
                        _trackDesignPlaceStateEntranceExitPlaced = true;
                        break;
                    } while (!(tile_element++)->IsLastForTile());
                }
                else
                {
                    auto res = RideEntranceExitPlaceAction::TrackPlaceQuery(newCoords, false);
                    if (res.Error != GameActions::Status::Ok)
                    {
                        return res;
                    }

                    totalCost += res.Cost;
                    tds.EntranceExitPlaced = true;
                    _trackDesignPlaceStateEntranceExitPlaced = true;
                }
                break;
            }
        }
    }

    return std::nullopt;
}

static GameActions::Result TrackDesignPlaceMaze(
    TrackDesignState& tds, TrackDesign& td, const CoordsXYZ& origin, const Ride& ride)
{
    if (tds.PlaceOperation == PTD_OPERATION_DRAW_OUTLINES)
    {
        gMapSelectionTiles.clear();
        gMapSelectArrowPosition = CoordsXYZ{ origin, TileElementHeight(origin) };
        gMapSelectArrowDirection = _currentTrackPieceDirection;
    }

    tds.PlaceZ = 0;
    money64 totalCost = 0;

    for (const auto& maze_element : td.mazeElements)
    {
        uint8_t rotation = _currentTrackPieceDirection & 3;
        CoordsXY mazeMapPos = maze_element.location.ToCoordsXY();
        auto mapCoord = mazeMapPos.Rotate(rotation);
        mapCoord += origin;

        TrackDesignUpdatePreviewBounds(tds, { mapCoord, origin.z });

        if (tds.PlaceOperation == PTD_OPERATION_DRAW_OUTLINES)
        {
            TrackDesignAddSelectedTile(mapCoord);
        }

        if (tds.PlaceOperation == PTD_OPERATION_PLACE_QUERY || tds.PlaceOperation == PTD_OPERATION_PLACE
            || tds.PlaceOperation == PTD_OPERATION_PLACE_GHOST || tds.PlaceOperation == PTD_OPERATION_PLACE_TRACK_PREVIEW)
        {
            uint8_t flags;
            money64 cost = 0;

            uint16_t mazeEntry = Numerics::rol16(maze_element.mazeEntry, rotation * 4);

            if (tds.PlaceOperation == PTD_OPERATION_PLACE_TRACK_PREVIEW)
            {
                flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND;
            }
            else if (tds.PlaceOperation == PTD_OPERATION_PLACE_GHOST)
            {
                flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND
                    | GAME_COMMAND_FLAG_GHOST;
            }
            else if (tds.PlaceOperation == PTD_OPERATION_PLACE_QUERY)
            {
                flags = 0;
            }
            else
            {
                flags = GAME_COMMAND_FLAG_APPLY;
            }
            if (tds.IsReplay)
            {
                flags |= GAME_COMMAND_FLAG_REPLAY;
            }

            auto mazePlace = MazePlaceTrackAction({ mapCoord, origin.z }, ride.id, mazeEntry);
            mazePlace.SetFlags(flags);
            auto res = flags & GAME_COMMAND_FLAG_APPLY ? GameActions::ExecuteNested(&mazePlace)
                                                       : GameActions::QueryNested(&mazePlace);
            if (res.Error != GameActions::Status::Ok)
            {
                return res;
            }
            cost = res.Cost;

            totalCost += cost;
        }

        if (tds.PlaceOperation == PTD_OPERATION_GET_PLACE_Z)
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
                surfaceZ += LAND_HEIGHT_STEP;
                if (surfaceElement->GetSlope() & kTileSlopeDiagonalFlag)
                {
                    surfaceZ += LAND_HEIGHT_STEP;
                }
            }

            int16_t waterZ = surfaceElement->GetWaterHeight();
            if (waterZ > 0 && waterZ > surfaceZ)
            {
                surfaceZ = waterZ;
            }

            int16_t temp_z = origin.z + tds.PlaceZ - surfaceZ;
            if (temp_z < 0)
            {
                tds.PlaceZ -= temp_z;
            }
        }
    }

    tds.Origin = origin;

    auto result = TrackDesignPlaceEntrances(tds, td, origin, ride.id, totalCost);
    if (result.has_value())
    {
        return result.value();
    }

    if (tds.PlaceOperation == PTD_OPERATION_REMOVE_GHOST)
    {
        auto gameAction = RideDemolishAction(ride.id, RIDE_MODIFY_DEMOLISH);
        gameAction.SetFlags(GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND | GAME_COMMAND_FLAG_GHOST);
        GameActions::Execute(&gameAction);
    }

    auto res = GameActions::Result();
    res.Cost = totalCost;

    return res;
}

static GameActions::Result TrackDesignPlaceRide(TrackDesignState& tds, TrackDesign* td6, const CoordsXYZ& origin, Ride& ride)
{
    tds.Origin = origin;
    if (tds.PlaceOperation == PTD_OPERATION_DRAW_OUTLINES)
    {
        gMapSelectionTiles.clear();
        gMapSelectArrowPosition = CoordsXYZ{ origin, TileElementHeight(origin) };
        gMapSelectArrowDirection = _currentTrackPieceDirection;
    }

    tds.PlaceZ = 0;
    money64 totalCost = 0;
    uint8_t rotation = _currentTrackPieceDirection;

    // Track elements
    auto newCoords = origin;
    for (const auto& track : td6->trackElements)
    {
        auto trackType = track.Type;
        const auto& ted = GetTrackElementDescriptor(trackType);

        TrackDesignUpdatePreviewBounds(tds, newCoords);

        switch (tds.PlaceOperation)
        {
            case PTD_OPERATION_DRAW_OUTLINES:
                for (const PreviewTrack* trackBlock = ted.Block; trackBlock->index != 0xFF; trackBlock++)
                {
                    auto tile = CoordsXY{ newCoords } + CoordsXY{ trackBlock->x, trackBlock->y }.Rotate(rotation);
                    TrackDesignUpdatePreviewBounds(tds, { tile, newCoords.z });
                    TrackDesignAddSelectedTile(tile);
                }
                break;
            case PTD_OPERATION_REMOVE_GHOST:
            {
                const TrackCoordinates* trackCoordinates = &ted.Coordinates;
                const PreviewTrack* trackBlock = ted.Block;
                int32_t tempZ = newCoords.z - trackCoordinates->z_begin + trackBlock->z;
                auto trackRemoveAction = TrackRemoveAction(
                    trackType, 0, { newCoords, tempZ, static_cast<Direction>(rotation & 3) });
                trackRemoveAction.SetFlags(
                    GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND | GAME_COMMAND_FLAG_GHOST
                    | GAME_COMMAND_FLAG_TRACK_DESIGN);
                GameActions::ExecuteNested(&trackRemoveAction);
                break;
            }
            case PTD_OPERATION_PLACE_QUERY:
            case PTD_OPERATION_PLACE:
            case PTD_OPERATION_PLACE_GHOST:
            case PTD_OPERATION_PLACE_TRACK_PREVIEW:
            {
                const TrackCoordinates* trackCoordinates = &ted.Coordinates;

                // di
                int16_t tempZ = newCoords.z - trackCoordinates->z_begin;

                int32_t liftHillAndAlternativeState = 0;
                if (track.HasFlag(TrackDesignTrackElementFlag::HasChain))
                {
                    liftHillAndAlternativeState |= 1;
                }
                if (track.HasFlag(TrackDesignTrackElementFlag::IsInverted))
                {
                    liftHillAndAlternativeState |= 2;
                }

                uint8_t flags = GAME_COMMAND_FLAG_APPLY;
                if (tds.PlaceOperation == PTD_OPERATION_PLACE_TRACK_PREVIEW)
                {
                    flags |= GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED;
                    flags |= GAME_COMMAND_FLAG_NO_SPEND;
                }
                else if (tds.PlaceOperation == PTD_OPERATION_PLACE_GHOST)
                {
                    flags |= GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED;
                    flags |= GAME_COMMAND_FLAG_NO_SPEND;
                    flags |= GAME_COMMAND_FLAG_GHOST;
                }
                else if (tds.PlaceOperation == PTD_OPERATION_PLACE_QUERY)
                {
                    flags = GAME_COMMAND_FLAG_NO_SPEND;
                }
                if (tds.IsReplay)
                {
                    flags |= GAME_COMMAND_FLAG_REPLAY;
                }

                auto trackPlaceAction = TrackPlaceAction(
                    ride.id, trackType, ride.type, { newCoords, tempZ, static_cast<uint8_t>(rotation) },
                    track.BrakeBoosterSpeed, track.ColourScheme, track.SeatRotation, liftHillAndAlternativeState, true);
                trackPlaceAction.SetFlags(flags);

                auto res = flags & GAME_COMMAND_FLAG_APPLY ? GameActions::ExecuteNested(&trackPlaceAction)
                                                           : GameActions::QueryNested(&trackPlaceAction);
                if (res.Error != GameActions::Status::Ok)
                {
                    return res;
                }

                totalCost += res.Cost;
                break;
            }
            case PTD_OPERATION_GET_PLACE_Z:
            {
                int32_t tempZ = newCoords.z - ted.Coordinates.z_begin;
                for (const PreviewTrack* trackBlock = ted.Block; trackBlock->index != 0xFF; trackBlock++)
                {
                    auto tile = CoordsXY{ newCoords } + CoordsXY{ trackBlock->x, trackBlock->y }.Rotate(rotation);
                    if (!MapIsLocationValid(tile))
                    {
                        continue;
                    }

                    auto surfaceElement = MapGetSurfaceElementAt(tile);
                    if (surfaceElement == nullptr)
                    {
                        return GameActions::Result(
                            GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER,
                            STR_ERR_SURFACE_ELEMENT_NOT_FOUND);
                    }

                    int32_t surfaceZ = surfaceElement->GetBaseZ();
                    if (surfaceElement->GetSlope() & kTileSlopeRaisedCornersMask)
                    {
                        surfaceZ += LAND_HEIGHT_STEP;
                        if (surfaceElement->GetSlope() & kTileSlopeDiagonalFlag)
                        {
                            surfaceZ += LAND_HEIGHT_STEP;
                        }
                    }

                    auto waterZ = surfaceElement->GetWaterHeight();
                    if (waterZ > 0 && waterZ > surfaceZ)
                    {
                        surfaceZ = waterZ;
                    }
                    int32_t heightDifference = tempZ + tds.PlaceZ + trackBlock->z - surfaceZ;
                    if (heightDifference < 0)
                    {
                        tds.PlaceZ -= heightDifference;
                    }
                }
                break;
            }
        }

        const TrackCoordinates& track_coordinates = ted.Coordinates;
        auto offsetAndRotatedTrack = CoordsXY{ newCoords }
            + CoordsXY{ track_coordinates.x, track_coordinates.y }.Rotate(rotation);

        newCoords = { offsetAndRotatedTrack, newCoords.z - track_coordinates.z_begin + track_coordinates.z_end };
        rotation = (rotation + track_coordinates.rotation_end - track_coordinates.rotation_begin) & 3;
        if (track_coordinates.rotation_end & (1 << 2))
        {
            rotation |= (1 << 2);
        }
        else
        {
            newCoords += CoordsDirectionDelta[rotation];
        }
    }

    auto result = TrackDesignPlaceEntrances(tds, *td6, newCoords, ride.id, totalCost);
    if (result.has_value())
    {
        return result.value();
    }

    if (tds.PlaceOperation == PTD_OPERATION_REMOVE_GHOST)
    {
        ride.ValidateStations();
        ride.Delete();
    }

    auto res = GameActions::Result();
    res.Cost = totalCost;

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
    TrackDesignState& tds, TrackDesign* td6, uint8_t ptdOperation, bool placeScenery, Ride& ride, const CoordsXYZD& coords)
{
    _trackDesignPlaceStateSceneryUnavailable = false;
    _trackDesignPlaceStateEntranceExitPlaced = false;

    tds.PlaceScenery = placeScenery;
    tds.EntranceExitPlaced = false;
    tds.HasScenery = false;

    tds.IsReplay = ptdOperation & PTD_OPERATION_FLAG_IS_REPLAY;
    ptdOperation &= ~PTD_OPERATION_FLAG_IS_REPLAY;
    tds.PlaceOperation = ptdOperation;

    tds.PreviewMin = coords;
    tds.PreviewMax = coords;
    tds.PlaceSceneryZ = 0;

    if (gTrackDesignSceneryToggle)
    {
        tds.PlaceScenery = false;
    }

    // NOTE: We need to save this, in networked games this would affect all clients otherwise.
    auto savedRideId = _currentRideIndex;
    auto savedTrackPieceDirection = _currentTrackPieceDirection;
    _currentRideIndex = ride.id;
    _currentTrackPieceDirection = coords.direction;

    GameActions::Result trackPlaceRes;
    const auto& rtd = GetRideTypeDescriptor(td6->type);
    if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_MAZE))
    {
        trackPlaceRes = TrackDesignPlaceMaze(tds, *td6, coords, ride);
    }
    else
    {
        trackPlaceRes = TrackDesignPlaceRide(tds, td6, coords, ride);
    }
    _currentRideIndex = savedRideId;
    _currentTrackPieceDirection = savedTrackPieceDirection;

    if (trackPlaceRes.Error != GameActions::Status::Ok)
    {
        return trackPlaceRes;
    }

    // Scenery elements
    auto sceneryPlaceRes = TrackDesignPlaceAllScenery(tds, td6->sceneryElements, coords.direction);
    if (sceneryPlaceRes.Error != GameActions::Status::Ok)
    {
        return sceneryPlaceRes;
    }

    // 0x6D0FE6
    if (tds.PlaceOperation == PTD_OPERATION_DRAW_OUTLINES)
    {
        gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
        gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_ARROW;
        gMapSelectFlags &= ~MAP_SELECT_FLAG_GREEN;
        MapInvalidateMapSelectionTiles();
    }

    auto res = GameActions::Result();
    res.Cost = trackPlaceRes.Cost + sceneryPlaceRes.Cost;

    return res;
}

GameActions::Result TrackDesignPlace(TrackDesign* td6, uint32_t flags, bool placeScenery, Ride& ride, const CoordsXYZD& coords)
{
    uint32_t ptdOperation = (flags & GAME_COMMAND_FLAG_APPLY) != 0 ? PTD_OPERATION_PLACE : PTD_OPERATION_PLACE_QUERY;
    if ((flags & GAME_COMMAND_FLAG_APPLY) != 0 && (flags & GAME_COMMAND_FLAG_GHOST) != 0)
    {
        ptdOperation = PTD_OPERATION_PLACE_GHOST;
    }
    if (flags & GAME_COMMAND_FLAG_REPLAY)
        ptdOperation |= PTD_OPERATION_FLAG_IS_REPLAY;

    TrackDesignState tds{};
    return TrackDesignPlaceVirtual(tds, td6, ptdOperation, placeScenery, ride, coords);
}

void TrackDesignPreviewRemoveGhosts(TrackDesign* td6, Ride& ride, const CoordsXYZD& coords)
{
    TrackDesignState tds{};
    TrackDesignPlaceVirtual(tds, td6, PTD_OPERATION_REMOVE_GHOST, true, ride, coords);
}

void TrackDesignPreviewDrawOutlines(TrackDesignState& tds, TrackDesign* td6, Ride& ride, const CoordsXYZD& coords)
{
    TrackDesignPlaceVirtual(tds, td6, PTD_OPERATION_DRAW_OUTLINES, true, ride, coords);
}

static int32_t TrackDesignGetZPlacement(TrackDesignState& tds, TrackDesign* td6, Ride& ride, const CoordsXYZD& coords)
{
    TrackDesignPlaceVirtual(tds, td6, PTD_OPERATION_GET_PLACE_Z, true, ride, coords);

    // Change from vanilla: originally, _trackDesignPlaceSceneryZ was not subtracted
    // from _trackDesignPlaceZ, causing bug #259.
    return tds.PlaceZ - tds.PlaceSceneryZ;
}

int32_t TrackDesignGetZPlacement(TrackDesign* td6, Ride& ride, const CoordsXYZD& coords)
{
    TrackDesignState tds{};
    return TrackDesignGetZPlacement(tds, td6, ride, coords);
}

static money64 TrackDesignCreateRide(int32_t type, int32_t subType, int32_t flags, RideId* outRideIndex)
{
    // Don't set colours as will be set correctly later.
    auto gameAction = RideCreateAction(type, subType, 0, 0, GetGameState().LastEntranceStyle);
    gameAction.SetFlags(flags);

    auto res = GameActions::ExecuteNested(&gameAction);

    // Callee's of this function expect kMoney64Undefined in case of failure.
    if (res.Error != GameActions::Status::Ok)
    {
        return kMoney64Undefined;
    }

    *outRideIndex = res.GetData<RideId>();

    return res.Cost;
}

/**
 *
 *  rct2: 0x006D2189
 * ebx = ride_id
 * cost = edi
 */
static bool TrackDesignPlacePreview(TrackDesignState& tds, TrackDesign* td6, money64* cost, Ride** outRide, uint8_t* flags)
{
    *outRide = nullptr;
    *flags = 0;

    auto& gameState = GetGameState();
    auto& objManager = GetContext()->GetObjectManager();
    auto entry_index = objManager.GetLoadedObjectEntryIndex(td6->vehicleObject);

    RideId rideIndex;
    uint8_t rideCreateFlags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND;
    if (TrackDesignCreateRide(td6->type, entry_index, rideCreateFlags, &rideIndex) == kMoney64Undefined)
    {
        return false;
    }

    auto ride = GetRide(rideIndex);
    if (ride == nullptr)
        return false;

    ride->custom_name = {};

    ride->entrance_style = objManager.GetLoadedObjectEntryIndex(td6->stationObjectIdentifier);
    if (ride->entrance_style == OBJECT_ENTRY_INDEX_NULL)
    {
        ride->entrance_style = gameState.LastEntranceStyle;
    }

    for (int32_t i = 0; i < OpenRCT2::Limits::kNumColourSchemes; i++)
    {
        ride->track_colour[i].main = td6->trackSpineColour[i];
        ride->track_colour[i].additional = td6->trackRailColour[i];
        ride->track_colour[i].supports = td6->trackSupportColour[i];
    }

    // Flat rides need their vehicle colours loaded for display
    // in the preview window
    if (!GetRideTypeDescriptor(td6->type).HasFlag(RIDE_TYPE_FLAG_HAS_TRACK))
    {
        for (size_t i = 0; i < std::size(ride->vehicle_colours); i++)
        {
            ride->vehicle_colours[i] = td6->vehicleColours[i];
        }
    }

    _trackDesignDrawingPreview = true;
    uint8_t backup_rotation = _currentTrackPieceDirection;
    uint32_t backup_park_flags = gameState.Park.Flags;
    gameState.Park.Flags &= ~PARK_FLAGS_FORBID_HIGH_CONSTRUCTION;
    auto mapSize = TileCoordsXY{ gameState.MapSize.x * 16, gameState.MapSize.y * 16 };

    _currentTrackPieceDirection = 0;
    int32_t z = TrackDesignGetZPlacement(
        tds, td6, RideGetTemporaryForPreview(), { mapSize.x, mapSize.y, 16, _currentTrackPieceDirection });

    if (tds.HasScenery)
    {
        *flags |= TRACK_DESIGN_FLAG_HAS_SCENERY;
    }

    z += 16 - tds.PlaceSceneryZ;

    bool placeScenery = true;
    if (_trackDesignPlaceStateSceneryUnavailable)
    {
        placeScenery = false;
        *flags |= TRACK_DESIGN_FLAG_SCENERY_UNAVAILABLE;
    }

    auto res = TrackDesignPlaceVirtual(
        tds, td6, PTD_OPERATION_PLACE_TRACK_PREVIEW, placeScenery, *ride,
        { mapSize.x, mapSize.y, z, _currentTrackPieceDirection });
    gameState.Park.Flags = backup_park_flags;

    if (res.Error == GameActions::Status::Ok)
    {
        if (entry_index == OBJECT_ENTRY_INDEX_NULL)
        {
            *flags |= TRACK_DESIGN_FLAG_VEHICLE_UNAVAILABLE;
        }
        else if (!RideEntryIsInvented(entry_index) && !GetGameState().Cheats.IgnoreResearchStatus)
        {
            *flags |= TRACK_DESIGN_FLAG_VEHICLE_UNAVAILABLE;
        }

        _currentTrackPieceDirection = backup_rotation;
        _trackDesignDrawingPreview = false;
        *cost = res.Cost;
        *outRide = ride;
        return true;
    }

    _currentTrackPieceDirection = backup_rotation;
    ride->Delete();
    _trackDesignDrawingPreview = false;
    return false;
}

#pragma region Track Design Preview

/**
 *
 *  rct2: 0x006D1EF0
 */
void TrackDesignDrawPreview(TrackDesign* td6, uint8_t* pixels)
{
    StashMap();
    TrackDesignPreviewClearMap();

    if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
    {
        TrackDesignLoadSceneryObjects(td6);
    }

    TrackDesignState tds{};

    money64 cost;
    Ride* ride;
    uint8_t flags;
    if (!TrackDesignPlacePreview(tds, td6, &cost, &ride, &flags))
    {
        std::fill_n(pixels, kTrackPreviewImageSize * 4, 0x00);
        UnstashMap();
        return;
    }
    td6->cost = cost;
    td6->trackFlags = flags & 7;

    CoordsXYZ centre = { (tds.PreviewMin.x + tds.PreviewMax.x) / 2 + 16, (tds.PreviewMin.y + tds.PreviewMax.y) / 2 + 16,
                         (tds.PreviewMin.z + tds.PreviewMax.z) / 2 };

    int32_t size_x = tds.PreviewMax.x - tds.PreviewMin.x;
    int32_t size_y = tds.PreviewMax.y - tds.PreviewMin.y;
    int32_t size_z = tds.PreviewMax.z - tds.PreviewMin.z;

    // Special case for flat rides - Z-axis info is irrelevant
    // and must be zeroed out lest the preview be off-centre
    if (!GetRideTypeDescriptor(td6->type).HasFlag(RIDE_TYPE_FLAG_HAS_TRACK))
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
    view.view_width = size_x;
    view.view_height = size_y;
    view.pos = { 0, 0 };
    view.zoom = zoom_level;
    view.flags = VIEWPORT_FLAG_HIDE_BASE | VIEWPORT_FLAG_HIDE_ENTITIES;

    DrawPixelInfo dpi;
    dpi.zoom_level = zoom_level;
    dpi.x = 0;
    dpi.y = 0;
    dpi.width = 370;
    dpi.height = 217;
    dpi.pitch = 0;
    dpi.bits = pixels;

    auto drawingEngine = std::make_unique<X8DrawingEngine>(GetContext()->GetUiContext());
    dpi.DrawingEngine = drawingEngine.get();

    const ScreenCoordsXY offset = { size_x / 2, size_y / 2 };
    for (uint8_t i = 0; i < 4; i++)
    {
        view.viewPos = Translate3DTo2DWithZ(i, centre) - offset;
        view.rotation = i;
        ViewportRender(dpi, &view, { {}, ScreenCoordsXY{ size_x, size_y } });

        dpi.bits += kTrackPreviewImageSize;
    }

    ride->Delete();
    UnstashMap();
}

/**
 * Resets all the map elements to surface tiles for track preview.
 *  rct2: 0x006D1D9A
 */
static void TrackDesignPreviewClearMap()
{
    auto numTiles = kMaximumMapSizeTechnical * kMaximumMapSizeTechnical;

    GetGameState().MapSize = TRACK_DESIGN_PREVIEW_MAP_SIZE;

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
    SetTileElements(std::move(tileElements));
}

bool TrackDesignAreEntranceAndExitPlaced()
{
    return _trackDesignPlaceStateEntranceExitPlaced;
}

#pragma endregion
