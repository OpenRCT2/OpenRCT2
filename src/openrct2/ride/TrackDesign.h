/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Limits.h"
#include "../actions/GameActionResult.h"
#include "../core/EnumUtils.hpp"
#include "../object/Object.h"
#include "../ride/RideColour.h"
#include "../ride/Track.h"
#include "../world/Map.h"
#include "RideRatings.h"
#include "VehicleColour.h"

#include <memory>

struct Ride;
struct ResultWithMessage;
enum class ViewportInteractionItem : uint8_t;

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
    OpenRCT2::TrackElemType type = OpenRCT2::TrackElemType::Flat;
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

enum class TrackDesignGameStateFlag
{
    SceneryUnavailable,
    HasScenery,
    VehicleUnavailable,
};

struct TrackDesignTrackAndVehicleSettings
{
    ride_type_t rtdIndex{};
    ObjectEntryDescriptor vehicleObject{};
    uint8_t numberOfTrains{};
    uint8_t numberOfCarsPerTrain{};
};

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
    RatingTuple ratings{};
    int8_t maxSpeed{};
    int8_t averageSpeed{};

    // TODO: move to a struct of its own, together with rideTime, that can be repeated for multiple stations.
    uint16_t rideLength;

    fixed16_2dp maxPositiveVerticalG{};
    fixed16_2dp maxNegativeVerticalG{};
    fixed16_2dp maxLateralG{};
    uint16_t totalAirTime{};
    uint8_t drops{};
    uint8_t highestDropHeight{};
    uint8_t inversions{};
    uint8_t holes;

    money64 upkeepCost;
    TileCoordsXY spaceRequired{};
};

// Not saved in the track design, but calculated when trying to place one.
struct TrackDesignGameStateData
{
    u8string name{};
    uint8_t flags{};
    money64 cost = 0.00_GBP;

    bool hasFlag(TrackDesignGameStateFlag flag) const;
    void setFlag(TrackDesignGameStateFlag flag, bool on);
};

struct TrackDesign
{
    TrackDesignTrackAndVehicleSettings trackAndVehicle{};
    TrackDesignOperatingSettings operation{};
    TrackDesignAppearanceSettings appearance{};
    TrackDesignStatistics statistics{};

    std::vector<TrackDesignMazeElement> mazeElements;
    std::vector<TrackDesignTrackElement> trackElements;
    std::vector<TrackDesignEntranceElement> entranceElements;
    std::vector<TrackDesignSceneryElement> sceneryElements;

    TrackDesignGameStateData gameStateData{};

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

extern bool gTrackDesignSceneryToggle;

extern bool _trackDesignDrawingPreview;

extern bool _trackDesignPlaceStateSceneryUnavailable;
extern bool gTrackDesignSaveMode;
extern RideId gTrackDesignSaveRideIndex;

[[nodiscard]] std::unique_ptr<TrackDesign> TrackDesignImport(const utf8* path);

void TrackDesignMirror(TrackDesign& td);

OpenRCT2::GameActions::Result TrackDesignPlace(
    const TrackDesign& td, uint32_t flags, bool placeScenery, Ride& ride, const CoordsXYZD& coords);
void TrackDesignPreviewRemoveGhosts(const TrackDesign& td, Ride& ride, const CoordsXYZD& coords);
void TrackDesignPreviewDrawOutlines(TrackDesignState& tds, const TrackDesign& td, Ride& ride, const CoordsXYZD& coords);
int32_t TrackDesignGetZPlacement(const TrackDesign& td, Ride& ride, const CoordsXYZD& coords);

///////////////////////////////////////////////////////////////////////////////
// Track design preview
///////////////////////////////////////////////////////////////////////////////
void TrackDesignDrawPreview(TrackDesign& td, uint8_t* pixels);

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
