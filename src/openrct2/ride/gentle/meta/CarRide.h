/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../../drawing/LightFX.h"
#include "../../../sprites.h"
#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
constexpr const RideTypeDescriptor CarRideRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_GENTLE),
    SET_FIELD(EnabledTrackPieces, {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_SLOPE, TRACK_CURVE_VERY_SMALL, TRACK_CURVE_SMALL, TRACK_SPINNING_TUNNEL}),
    SET_FIELD(ExtraTrackPieces, {TRACK_SLOPE_STEEP_UP, TRACK_SLOPE_STEEP_DOWN, TRACK_RAPIDS}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::EndStation),
    SET_FIELD(TrackPaintFunction, GetTrackPaintFunctionCarRide),
    SET_FIELD(Flags, RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAG_CAN_SYNCHRONISE_ADJACENT_STATIONS |
                     RIDE_TYPE_FLAG_HAS_DATA_LOGGING | RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS | RIDE_TYPE_FLAG_PEEP_WILL_RIDE_AGAIN |
                     RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS | RIDE_TYPE_FLAG_HAS_TRACK | RIDE_TYPE_FLAG_SUPPORTS_MULTIPLE_TRACK_COLOUR |
                     RIDE_TYPE_FLAG_ALLOW_DOORS_ON_TRACK | RIDE_TYPE_FLAG_ALLOW_MUSIC | RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT |
                     RIDE_TYPE_FLAG_ALLOW_MORE_VEHICLES_THAN_STATION_FITS | RIDE_TYPE_FLAG_SHOW_IN_TRACK_DESIGNER | 
                     RIDE_TYPE_FLAG_SLIGHTLY_INTERESTING_TO_LOOK_AT),
    SET_FIELD(RideModes, EnumsToFlags(RideMode::ContinuousCircuit)),
    SET_FIELD(DefaultMode, RideMode::ContinuousCircuit),
    SET_FIELD(OperatingSettings, { 0, 0, 0, 0, 0, 0 }),
    SET_FIELD(Naming, { STR_RIDE_NAME_CAR_RIDE, STR_RIDE_DESCRIPTION_CAR_RIDE }),
    SET_FIELD(NameConvention, { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station }),
    SET_FIELD(EnumName, nameof(RIDE_TYPE_CAR_RIDE)),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION)),
    SET_FIELD(Heights, { 6, 24, 4, 7, }),
    SET_FIELD(MaxMass, 2),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::Null, 5, 5 }),
    SET_FIELD(RatingsMultipliers, { 70, 10, 10 }),
    SET_FIELD(UpkeepCosts, { 70, 20, 0, 8, 3, 5 }),
    SET_FIELD(BuildCosts, { 12.50_GBP, 2.50_GBP, 30, }),
    SET_FIELD(DefaultPrices, { 15, 0 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_SUMMER),
    SET_FIELD(PhotoItem, ShopItem::Photo),
    SET_FIELD(BonusValue, 50),
    SET_FIELD(ColourPresets, TRACK_COLOUR_PRESETS(
        { COLOUR_SATURATED_BROWN, COLOUR_SATURATED_BROWN, COLOUR_GREY },
        { COLOUR_LIGHT_PURPLE, COLOUR_LIGHT_PURPLE, COLOUR_WHITE },
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_OLIVE_GREEN },
        { COLOUR_GREY, COLOUR_GREY, COLOUR_BLACK },
        { COLOUR_BLACK, COLOUR_BLACK, COLOUR_SATURATED_BROWN },
        { COLOUR_BRIGHT_YELLOW, COLOUR_BRIGHT_YELLOW, COLOUR_BRIGHT_RED },
        { COLOUR_LIGHT_WATER, COLOUR_LIGHT_WATER, COLOUR_GREY },
        { COLOUR_ICY_BLUE, COLOUR_ICY_BLUE, COLOUR_WHITE },
        { COLOUR_WHITE, COLOUR_WHITE, COLOUR_OLIVE_GREEN },
    )),
    SET_FIELD(ColourPreview, { SPR_RIDE_DESIGN_PREVIEW_CAR_RIDE_TRACK, SPR_RIDE_DESIGN_PREVIEW_CAR_RIDE_SUPPORTS }),
    SET_FIELD(ColourKey, RideColourKey::Ride),
    SET_FIELD(Name, "car_ride"),
    SET_FIELD(RatingsData,
    {
        SET_FIELD(Type, RatingsCalculationType::Normal),
        SET_FIELD(BaseRatings, { RIDE_RATING(2, 00), RIDE_RATING(0, 50), RIDE_RATING(0, 00) }),
        SET_FIELD(Unreliability, 12),
        SET_FIELD(RelaxRequirementsIfInversions, false),
        SET_FIELD(Modifiers, {
            { RatingsModifierType::BonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::BonusSynchronization,  0,                RIDE_RATING(0, 15), RIDE_RATING(0, 00), 0 },
            { RatingsModifierType::BonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::BonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::BonusTurns,            0,                14860, 0, 11437 },
            { RatingsModifierType::BonusDrops,            0,                8738, 0, 0 },
            { RatingsModifierType::BonusSheltered,        0,                12850, 6553, 4681 },
            { RatingsModifierType::BonusProximity,        0,                11183, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                8366, 0, 0 },
            { RatingsModifierType::RequirementLength,     0xC80000,         8, 2, 2 },
            { RatingsModifierType::PenaltyIntensity,      0,                0, 0, 0 },
        }),
    }),
    SET_FIELD(UpdateRotating, UpdateRotatingDefault),
    SET_FIELD(LightFXAddLightsMagicVehicle, LightFxAddLightsMagicVehicle_BoatHire),
};
// clang-format on
