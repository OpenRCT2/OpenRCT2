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
    TileCoordsXYZD location{};
    bool isExit{};
};

struct TrackDesignSceneryElement
{
    ObjectEntryDescriptor sceneryObject{};
    CoordsXYZ loc{};
    uint8_t flags{};
    colour_t primaryColour{};
    colour_t secondaryColour{};
    colour_t tertiaryColour = COLOUR_DARK_BROWN;

    Direction getRotation() const
    {
        return flags & 0x3;
    }

    void setRotation(Direction rotation)
    {
        flags &= ~0x3;
        flags |= (rotation & 0x3);
    }

    // Small scenery
    uint8_t getQuadrant() const
    {
        return (flags >> 2) & 0x3;
    }

    void setQuadrant(uint8_t quadrant)
    {
        flags &= ~0b00001100;
        flags |= (quadrant & 0x3) << 2;
    }

    // Path
    bool hasSlope() const
    {
        return (flags & 0b00010000) != 0;
    }

    void setHasSlope(bool on)
    {
        if (on)
            flags |= 0b00010000;
        else
            flags &= ~0b00010000;
    }

    Direction getSlopeDirection() const
    {
        return (flags >> 5) % NumOrthogonalDirections;
    }

    void setSlopeDirection(Direction slope)
    {
        flags &= 0x9F;
        flags |= ((slope & 3) << 5);
    }

    uint8_t getEdges() const
    {
        return (flags & 0xF);
    }

    void setEdges(uint8_t edges)
    {
        flags &= ~0xF;
        flags |= (edges & 0xF);
    }

    bool isQueue() const
    {
        return (flags & (1 << 7)) != 0;
    }

    void setIsQueue(bool on)
    {
        if (on)
            flags |= 0b10000000;
        else
            flags &= ~0b10000000;
    }

    bool operator==(const TrackDesignSceneryElement& rhs)
    {
        return sceneryObject == rhs.sceneryObject && loc == rhs.loc && flags == rhs.flags && primaryColour == rhs.primaryColour
            && secondaryColour == rhs.secondaryColour && tertiaryColour == rhs.tertiaryColour;
    }

    bool operator!=(const TrackDesignSceneryElement& rhs)
    {
        return !((*this) == rhs);
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

struct TrackDesignMazeElement
{
    TileCoordsXY location{};
    uint16_t mazeEntry{};
};

class DataSerialiser;
enum class RideMode : uint8_t;

struct TrackDesign
{
    uint8_t type;
    uint8_t vehicleType;
    money64 cost;
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
