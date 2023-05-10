/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../../sprites.h"
#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"
#include "../../TrackData.h"

void CondorRideUpdate(Ride& ride);

/*static constexpr const PreviewTrack TrackBlocks066[] = { { 0, 0, 0, 0, 64, { 0b1111, 0 }, 0 },
                                                         { 1, -32, -32, 0, 0, { 0b1111, 0 }, 0 },
                                                         { 2, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
                                                         { 3, -32, 32, 0, 0, { 0b1111, 0 }, 0 },
                                                         { 4, 0, -32, 0, 0, { 0b1111, 0 }, 0 },
                                                         { 5, 0, 32, 0, 0, { 0b1111, 0 }, 0 },
                                                         { 6, 32, -32, 0, 0, { 0b1111, 0 }, 0 },
                                                         { 7, 32, 32, 0, 0, { 0b1111, 0 }, 0 },
                                                         { 8, 32, 0, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
                                                         TRACK_BLOCK_END };*/

static std::vector<PreviewTrack> GetTrackBlocks9x9()
{
    std::vector<PreviewTrack> result;
    uint8_t index = 0;
    result.reserve(82);

    for (int16_t x = 0; x < 9 * 32; x += 32)
    {
        for (int16_t y = 0; y < 9 * 32; y += 32)
        {
            PreviewTrack track{ index, x, y, 0, 0, { 0b1111, 0 }, 0 };
            result.push_back(track);
            index++;
        }
    }
    result.push_back(TRACK_BLOCK_END);
    return result;
}
static auto TrackBlock9x9 = GetTrackBlocks9x9();

static std::array<uint8_t, MaxSequencesPerPiece> GetTrack9x9Sequences()
{
    std::array<uint8_t, MaxSequencesPerPiece> res;
    uint8_t index = 0;

    //do the first row
    for (int x = 0; x < 9; x++)
    {
        if (x == 0)
            res[index] = TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN;
        else if (x == 8)
            res[index] = TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1;
        else
            res[index] = TRACK_SEQUENCE_FLAG_DIRECTION_0;
        index++;
    }

    // middle rows
    for (int y = 1; y < 8; y++)
    {
        for (int x = 0; x < 9; x++)
        {
            if (x == 0)
                res[index] = TRACK_SEQUENCE_FLAG_DIRECTION_3;
            else if (x == 8)
                res[index] = TRACK_SEQUENCE_FLAG_DIRECTION_1;
            else
                res[index] = 0;
            index++;
        }
    }

    // last row
    for (int x = 0; x < 9; x++)
    {
        if (x == 0)
            res[index] = TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3;
        else if (x == 8)
            res[index] = TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2;
        else
            res[index] = TRACK_SEQUENCE_FLAG_DIRECTION_2;
        index++;
    }
    return res;
}
static auto Sequence9x9 = GetTrack9x9Sequences();

/*const uint8_t track_map_3x3[][9] = {
    { 0, 1, 2, 3, 4, 5, 6, 7, 8 },
    { 0, 3, 5, 7, 2, 8, 1, 6, 4 },
    { 0, 7, 8, 6, 5, 4, 3, 1, 2 },
    { 0, 6, 4, 1, 8, 2, 7, 3, 5 },
};*/

static std::array<std::array<uint8_t, 81>, 4> GetTrackMapSequence9x9()
{
    std::array<std::array<uint8_t, 81>, 4> res;

    std::array<std::array<uint8_t, 9>, 9> regular;
    uint32_t index = 0;
    for (int y = 0; y < 9; y++)
    {
        for (int x = 0; x < 9; x++)
        {
            regular[x][y] = index;
            index++;
        }
    }

    std::array<std::array<uint8_t, 9>, 9> transposed;
    for (int y = 0; y < 9; y++)
    {
        for (int x = 0; x < 9; x++)
        {
            transposed[x][y] = regular[y][8 - x];
        }
    }

    index = 0;
    for (int y = 0; y < 9; y++)
    {
        for (int x = 0; x < 9; x++)
        {
            index = x + y * 9;
            res[0][index] = regular[x][y];
            res[3][index] = transposed[x][y];
            index++;
        }
    }

    res[2] = res[0];
    std::reverse(res[2].begin(), res[2].end());

    res[1] = res[3];
    std::reverse(res[1].begin(), res[1].end());
    return res;
}
static auto TrackMap9x9 = GetTrackMapSequence9x9();

/*const uint8_t edges_3x3[] = {
    0, EDGE_NE | EDGE_NW, EDGE_NE, EDGE_NE | EDGE_SE, EDGE_NW, EDGE_SE, EDGE_SW | EDGE_NW, EDGE_SW | EDGE_SE, EDGE_SW,
};*/

static std::array<uint8_t, 81> GetEdges9x9()
{
    std::array<uint8_t, 81> res;

    //first row
    auto index = 0;
    for (int x = 0; x < 9; x++)
    {
        if (x == 0)
            res[index] = EDGE_NE | EDGE_NW;
        else if (x == 8)
            res[index] = EDGE_NE | EDGE_SE;
        else
            res[index] = EDGE_NE;
        index++;
    }

    // middle rows
    for (int y = 1; y < 8; y++)
    {
        for (int x = 0; x < 9; x++)
        {
            if (x == 0)
                res[index] = EDGE_NW;
            else if (x == 8)
                res[index] = EDGE_SE;
            else
                res[index] = 0;
            index++;
        }
    }

    //last row
    for (int x = 0; x < 9; x++)
    {
        if (x == 0)
            res[index] = EDGE_NW | EDGE_SW;
        else if (x == 8)
            res[index] = EDGE_SW | EDGE_SE;
        else
            res[index] = EDGE_SW;
        index++;
    }

    return res;
}
static auto Edges9x9 = GetEdges9x9();

    //static std::vector<PreviewTrack> GetTrackBlocks9x9()

// clang-format off
const TrackElementDescriptor TowerBase9x9TED =
{
    SET_FIELD(Description, STR_ENTRY_EXIT_PLATFORM),
    SET_FIELD(Coordinates, { 0, 2, 0, 0, 0, 32 } ),
    SET_FIELD(Block, TrackBlock9x9.data()),
    SET_FIELD(PieceLength, 0),
    SET_FIELD(CurveChain, { 0, 0 } ),
    SET_FIELD(AlternativeType, 0),
    SET_FIELD(PriceModifier, 0),
    SET_FIELD(MirrorElement, 0),
    SET_FIELD(HeightMarkerPositions, 0),
    SET_FIELD(Flags, 0),
    SET_FIELD(SequenceElementAllowedWallEdges, {} ),
    SET_FIELD(SequenceProperties, Sequence9x9),
    SET_FIELD(Definition, {0, 0, 0, 0, 0, 0} ),
    SET_FIELD(SpinFunction, 0),
    SET_FIELD(VerticalFactor, nullptr),
    SET_FIELD(LateralFactor, nullptr),
};

constexpr const RideTypeDescriptor CondorRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_THRILL),
    SET_FIELD(EnabledTrackPieces, {TRACK_TOWER_BASE}),
    SET_FIELD(ExtraTrackPieces, {}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::FlatTrack9x9),
    SET_FIELD(TrackPaintFunction, GetTrackPaintFunctionCondor),
    SET_FIELD(Flags, RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION | RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS |
                     RIDE_TYPE_FLAG_HAS_DATA_LOGGING | RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS | RIDE_TYPE_FLAG_TRACK_NO_WALLS |
                     RIDE_TYPE_FLAG_PEEP_WILL_RIDE_AGAIN | RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS | RIDE_TYPE_FLAG_HAS_TRACK |
                     RIDE_TYPE_FLAG_SUPPORTS_MULTIPLE_TRACK_COLOUR | RIDE_TYPE_FLAG_ALLOW_MUSIC | RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT |
                     RIDE_TYPE_FLAG_SINGLE_SESSION | RIDE_TYPE_FLAG_SHOW_IN_TRACK_DESIGNER | RIDE_TYPE_FLAG_INTERESTING_TO_LOOK_AT),
    SET_FIELD(RideModes, EnumsToFlags(RideMode::Condor)),
    SET_FIELD(DefaultMode, RideMode::Condor),

    SET_FIELD(OperatingSettings, { 0, 0, 0, 0, 0, 0 }),
    SET_FIELD(Naming, { STR_RIDE_NAME_CONDOR, STR_RIDE_DESCRIPTION_CONDOR }),
    SET_FIELD(NameConvention, { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station }),
    SET_FIELD(EnumName, nameof(RIDE_TYPE_CONDOR)),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION)),
    SET_FIELD(Heights, { 255, 32, 3, 2, }),
    SET_FIELD(MaxMass, 15),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::Null, 5, 5 }),
    SET_FIELD(RatingsCalculationFunction, RideRatingsCalculateRotoDrop),
    SET_FIELD(RatingsMultipliers, { 50, 50, 10 }),
    SET_FIELD(UpkeepCosts, { 50, 20, 0, 10, 0, 0 }),
    SET_FIELD(BuildCosts, { 22.50_GBP, 0.00_GBP, 4, }),
    SET_FIELD(DefaultPrices, { 20, 0 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_ROCK_1),
    SET_FIELD(PhotoItem, ShopItem::Photo),
    SET_FIELD(BonusValue, 45),
    SET_FIELD(ColourPresets, TRACK_COLOUR_PRESETS(
        { COLOUR_BORDEAUX_RED, COLOUR_YELLOW, COLOUR_GREY },
        { COLOUR_BLACK, COLOUR_LIGHT_BLUE, COLOUR_GREY },
    )),
    SET_FIELD(ColourPreview, { SPR_RIDE_DESIGN_PREVIEW_ROTO_DROP_TRACK, SPR_RIDE_DESIGN_PREVIEW_ROTO_DROP_SUPPORTS }),
    SET_FIELD(ColourKey, RideColourKey::Ride),
    SET_FIELD(Name, "condor"),
    SET_FIELD(UpdateRotating, UpdateRotatingDefault),
    SET_FIELD(LightFXAddLightsMagicVehicle, nullptr),
    SET_FIELD(StartRideMusic, OpenRCT2::RideAudio::DefaultStartRideMusicChannel),
    SET_FIELD(DesignCreateMode, TrackDesignCreateMode::Default),
    SET_FIELD(MusicUpdateFunction, DefaultMusicUpdate),
    SET_FIELD(Classification, RideClassification::Ride),
    SET_FIELD(UpdateLeaveEntrance, PeepUpdateRideLeaveEntranceDefault),
    SET_FIELD(SpecialElementRatingAdjustment, SpecialTrackElementRatingsAjustment_Default),
    SET_FIELD(GetGuestWaypointLocation, GetGuestWaypointLocationDefault),
    SET_FIELD(ConstructionWindowContext, RideConstructionWindowContext::Default),
    SET_FIELD(RideUpdate, CondorRideUpdate),
    SET_FIELD(UpdateMeasurementsSpecialElements, RideUpdateMeasurementsSpecialElements_Default),
    SET_FIELD(MusicTrackOffsetLength, OpenRCT2::RideAudio::RideMusicGetTrackOffsetLength_Default),
    SET_FIELD(UpdateRideApproachVehicleWaypoints, UpdateRideApproachVehicleWaypointsDefault),
    SET_FIELD(CarPlacement, CarPlacementType::Condor),
};

enum CondorRideState
{
    Waiting,
    Climbing,
    Falling
};

struct CondorRideData : public RideData
{
    CondorRideData();

    RideDataType GetType() const override;
    int32_t VehiclesZ;
    int32_t TowerTop;
    int32_t TowerBase;
    CondorRideState State;
};

class CondorVehicleData : public VehicleData
{
public:
    CondorVehicleData();
    virtual ~CondorVehicleData();

    VehicleDataType GetType() const override;
    void Serialise(DataSerialiser& stream) override;

    uint32_t VehicleIndex;
};

void CondorRideUpdate(Ride& ride);
void CondorRideUpdateWating(Ride& ride);
void CondorCreateVehicle(Vehicle* vehicle, Ride* ride, int32_t carIndex, const CoordsXYZ& carPosition, TrackElement* trackElement);
void CondorUpdateWaitingForDepart(Vehicle& vehicle);
void CondorUpdateDeparting(Vehicle& vehicle);
void CondorUpdateTravelling(Vehicle& vehicle);
void CondorUpdateMotion(Vehicle& vehicle);

// clang-format on
