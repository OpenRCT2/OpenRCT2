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
#include "../ride/RideColour.h"
#include "../world/Map.h"
#include "VehicleColour.h"

#include <memory>

struct Ride;
struct ResultWithMessage;

constexpr uint32_t kTrackPreviewImageSize = 370 * 217;

enum class TrackPlaceOperation : uint8_t
{
    drawOutlines,
    placeQuery,
    place,
    getPlaceZ,
    placeGhost,
    placeTrackPreview,
    removeGhost,
};

struct TrackDesignState
{
    CoordsXYZ previewMin;
    CoordsXYZ previewMax;
    CoordsXYZ origin;
    TrackPlaceOperation placeOperation{};
    int16_t placeZ{};
    int16_t placeSceneryZ{};
    bool entranceExitPlaced{};
    bool hasScenery{};
    bool placeScenery{};
    bool isReplay{};
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

    Direction getRotation() const;
    void setRotation(Direction rotation);

    // Small scenery
    uint8_t getQuadrant() const;
    void setQuadrant(uint8_t quadrant);

    // Path
    bool hasSlope() const;
    void setHasSlope(bool on);

    Direction getSlopeDirection() const;
    void setSlopeDirection(Direction slope);

    uint8_t getEdges() const;
    void setEdges(uint8_t edges);

    bool isQueue() const;
    void setIsQueue(bool on);

    bool operator==(const TrackDesignSceneryElement& rhs);
    bool operator!=(const TrackDesignSceneryElement& rhs);
};

enum class TrackDesignTrackElementFlag : uint8_t
{
    hasChain = (1 << 0),
    isInverted = (1 << 1),
    isCovered = (1 << 2), // Reserved
};

struct TrackDesignTrackElement
{
    track_type_t type = 0;
    uint8_t flags = 0;
    uint8_t colourScheme = 0;
    ::StationIndex stationIndex = StationIndex::FromUnderlying(0);
    uint8_t brakeBoosterSpeed = 0;
    uint8_t seatRotation = 4;

    constexpr bool HasFlag(const TrackDesignTrackElementFlag flag) const
    {
        return flags & EnumValue(flag);
    }

    constexpr void SetFlag(const TrackDesignTrackElementFlag flag)
    {
        flags |= EnumValue(flag);
    }

    constexpr void ClearFlag(const TrackDesignTrackElementFlag flag)
    {
        flags &= ~EnumValue(flag);
    }
};

struct TrackDesignMazeElement
{
    TileCoordsXY location{};
    uint16_t mazeEntry{};
};

class DataSerialiser;
enum class RideMode : uint8_t;

struct TrackDesignOperatingSettings
{
    RideMode rideMode{};
    uint8_t liftHillSpeed{};
    uint8_t numCircuits{};
    uint8_t operationSetting{};
    uint8_t departFlags{};
    uint8_t minWaitingTime{};
    uint8_t maxWaitingTime{};
};

struct TrackDesignAppearanceSettings
{
    std::array<TrackColour, kNumRideColourSchemes> trackColours{};
    u8string stationObjectIdentifier{};
    VehicleColourSettings vehicleColourSettings{};
    std::array<VehicleColour, OpenRCT2::Limits::kMaxVehicleColours> vehicleColours{};
};

struct TrackDesignStatistics
{
    uint8_t excitement{};
    uint8_t intensity{};
    uint8_t nausea{};
    int8_t maxSpeed{};
    int8_t averageSpeed{};

    // TODO: move to a struct of its own, together with rideTime, that can be repeated for multiple stations.
    uint16_t rideLength;

    uint8_t maxPositiveVerticalG{};
    int8_t maxNegativeVerticalG{};
    uint8_t maxLateralG{};
    uint8_t totalAirTime{};
    uint8_t drops{};
    uint8_t highestDropHeight{};
    union
    {
        uint8_t inversions{};
        uint8_t holes;
    };

    money64 upkeepCost;
    TileCoordsXY spaceRequired{};
};

struct TrackDesign
{
    uint8_t type;
    uint8_t vehicleType;
    money64 cost;
    uint8_t trackFlags;
    uint8_t numberOfTrains;
    uint8_t numberOfCarsPerTrain;
    ObjectEntryDescriptor vehicleObject;

    TrackDesignOperatingSettings operation{};
    TrackDesignAppearanceSettings appearance{};
    TrackDesignStatistics statistics{};

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
