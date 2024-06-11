/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Limits.h"
#include "../actions/GameActionResult.h"
#include "../common.h"
#include "../object/Object.h"
#include "../world/Map.h"
#include "VehicleColour.h"

#include <memory>

struct Ride;
struct ResultWithMessage;

constexpr uint32_t kTrackPreviewImageSize = 370 * 217;

struct TrackDesignState
{
    CoordsXYZ PreviewMin;
    CoordsXYZ PreviewMax;
    CoordsXYZ Origin;
    uint8_t PlaceOperation{};
    int16_t PlaceZ{};
    int16_t PlaceSceneryZ{};
    bool EntranceExitPlaced{};
    bool HasScenery{};
    bool PlaceScenery{};
    bool IsReplay{};
};

/* Track Entrance entry */
struct TrackDesignEntranceElement
{
    TileCoordsXYZD Location{};
    bool IsExit{};
};

struct TrackDesignSceneryElement
{
    ObjectEntryDescriptor scenery_object;
    CoordsXYZ loc;
    uint8_t flags;
    uint8_t primary_colour;
    uint8_t secondary_colour;

    bool IsQueue() const
    {
        return (flags & (1 << 7)) != 0;
    }
};

enum class TrackDesignTrackElementFlag : uint8_t
{
    HasChain = (1 << 0),
    IsInverted = (1 << 1),
    IsCovered = (1 << 2), // Reserved
};

struct TrackDesignTrackElement
{
    track_type_t Type = 0;
    uint8_t Flags = 0;
    uint8_t ColourScheme = 0;
    ::StationIndex StationIndex = StationIndex::FromUnderlying(0);
    uint8_t BrakeBoosterSpeed = 0;
    uint8_t SeatRotation = 4;

    constexpr bool HasFlag(const TrackDesignTrackElementFlag flag) const
    {
        return Flags & EnumValue(flag);
    }

    constexpr void SetFlag(const TrackDesignTrackElementFlag flag)
    {
        Flags |= EnumValue(flag);
    }

    constexpr void ClearFlag(const TrackDesignTrackElementFlag flag)
    {
        Flags &= ~EnumValue(flag);
    }
};

/* Maze Element entry   size: 0x04 */
struct TrackDesignMazeElement
{
    union
    {
        uint32_t all;
        struct
        {
            int8_t x;
            int8_t y;
            union
            {
                uint16_t maze_entry;
                struct
                {
                    uint8_t direction;
                    uint8_t type;
                };
            };
        };
    };
};

class DataSerialiser;
enum class RideMode : uint8_t;

struct TrackDesign
{
    uint8_t type;
    uint8_t vehicleType;
    money64 cost;
    uint32_t flags;
    RideMode rideMode;
    uint8_t trackFlags;
    uint8_t colourScheme;
    std::array<VehicleColour, OpenRCT2::Limits::kMaxVehicleColours> vehicleColours;
    u8string stationObjectIdentifier{};
    uint8_t totalAirTime;
    uint8_t departFlags;
    uint8_t numberOfTrains;
    uint8_t numberOfCarsPerTrain;
    uint8_t minWaitingTime;
    uint8_t maxWaitingTime;
    uint8_t operationSetting;
    int8_t maxSpeed;
    int8_t averageSpeed;
    uint16_t rideLength;
    uint8_t maxPositiveVerticalG;
    int8_t maxNegativeVerticalG;
    uint8_t maxLateralG;
    uint8_t inversions;
    uint8_t holes;
    uint8_t drops;
    uint8_t highestDropHeight;
    uint8_t excitement;
    uint8_t intensity;
    uint8_t nausea;
    money64 upkeepCost;
    uint8_t trackSpineColour[OpenRCT2::Limits::kNumColourSchemes];
    uint8_t trackRailColour[OpenRCT2::Limits::kNumColourSchemes];
    uint8_t trackSupportColour[OpenRCT2::Limits::kNumColourSchemes];
    uint32_t flags2;
    ObjectEntryDescriptor vehicleObject;
    uint8_t spaceRequiredX;
    uint8_t spaceRequiredY;
    uint8_t liftHillSpeed;
    uint8_t numCircuits;

    std::vector<TrackDesignMazeElement> mazeElements;
    std::vector<TrackDesignTrackElement> trackElements;
    std::vector<TrackDesignEntranceElement> entranceElements;
    std::vector<TrackDesignSceneryElement> sceneryElements;

    std::string name;

public:
    ResultWithMessage CreateTrackDesign(TrackDesignState& tds, const Ride& ride);
    ResultWithMessage CreateTrackDesignScenery(TrackDesignState& tds);
    void Serialise(DataSerialiser& stream);

private:
    uint8_t _saveDirection;
    ResultWithMessage CreateTrackDesignTrack(TrackDesignState& tds, const Ride& ride);
    ResultWithMessage CreateTrackDesignMaze(TrackDesignState& tds, const Ride& ride);
    CoordsXYE MazeGetFirstElement(const Ride& ride);
};

enum
{
    TDPF_PLACE_SCENERY = 1 << 0,
};

enum
{
    TRACK_DESIGN_FLAG_SCENERY_UNAVAILABLE = (1 << 0),
    TRACK_DESIGN_FLAG_HAS_SCENERY = (1 << 1),
    TRACK_DESIGN_FLAG_VEHICLE_UNAVAILABLE = (1 << 2),
};

enum
{
    PTD_OPERATION_DRAW_OUTLINES,
    PTD_OPERATION_PLACE_QUERY,
    PTD_OPERATION_PLACE,
    PTD_OPERATION_GET_PLACE_Z,
    PTD_OPERATION_PLACE_GHOST,
    PTD_OPERATION_PLACE_TRACK_PREVIEW,
    PTD_OPERATION_REMOVE_GHOST,
};

static constexpr uint8_t PTD_OPERATION_FLAG_IS_REPLAY = (1 << 7);

extern bool gTrackDesignSceneryToggle;

extern bool _trackDesignDrawingPreview;

extern bool _trackDesignPlaceStateSceneryUnavailable;
extern bool gTrackDesignSaveMode;
extern RideId gTrackDesignSaveRideIndex;

[[nodiscard]] std::unique_ptr<TrackDesign> TrackDesignImport(const utf8* path);

void TrackDesignMirror(TrackDesign* td6);

GameActions::Result TrackDesignPlace(TrackDesign* td6, uint32_t flags, bool placeScenery, Ride& ride, const CoordsXYZD& coords);
void TrackDesignPreviewRemoveGhosts(TrackDesign* td6, Ride& ride, const CoordsXYZD& coords);
void TrackDesignPreviewDrawOutlines(TrackDesignState& tds, TrackDesign* td6, Ride& ride, const CoordsXYZD& coords);
int32_t TrackDesignGetZPlacement(TrackDesign* td6, Ride& ride, const CoordsXYZD& coords);

///////////////////////////////////////////////////////////////////////////////
// Track design preview
///////////////////////////////////////////////////////////////////////////////
void TrackDesignDrawPreview(TrackDesign* td6, uint8_t* pixels);

///////////////////////////////////////////////////////////////////////////////
// Track design saving
///////////////////////////////////////////////////////////////////////////////
void TrackDesignSaveInit();
void TrackDesignSaveResetScenery();
bool TrackDesignSaveContainsTileElement(const TileElement* tileElement);
void TrackDesignSaveSelectNearbyScenery(RideId rideIndex);
void TrackDesignSaveSelectTileElement(
    ViewportInteractionItem interactionType, const CoordsXY& loc, TileElement* tileElement, bool collect);

bool TrackDesignAreEntranceAndExitPlaced();

extern std::vector<TrackDesignSceneryElement> _trackSavedTileElementsDesc;
extern std::vector<const TileElement*> _trackSavedTileElements;
