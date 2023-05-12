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

template<size_t NumElements> static void MakeCenterElementLast(std::array<PreviewTrack, NumElements>& elements)
{
    auto middleIndex = (elements.size() - 1) / 2;
    auto middleElement = elements.begin() + middleIndex;

    auto lastElement = elements.end() - 2;
    auto lastIndex = lastElement->index;
    lastElement->index = middleElement->index;
    middleElement->index = lastIndex;
    std::iter_swap(middleElement, lastElement);
}

template<size_t NumElements> static void MakeCenterElementLast(std::array<uint8_t, NumElements>& elements)
{
    auto middleIndex = (elements.size()) / 2;
    auto middleElement = elements.begin() + middleIndex;

    auto lastElement = elements.end() - 1;
    std::iter_swap(middleElement, lastElement);
}

template<size_t NumElements>
static void MakeCenterTileLast(std::array<uint8_t, NumElements>& elements)
{
    //find the center element
    auto centerElement = std::find(elements.begin(), elements.end(), NumElements / 2);

    //find the last element i.e. max index
    auto lastElement = std::find(elements.begin(), elements.end(), NumElements-1);

    //swap
    std::iter_swap(centerElement, lastElement);
}

    //static std::vector<PreviewTrack> GetTrackBlocks9x9()

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
