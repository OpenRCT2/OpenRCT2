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

// clang-format off
constexpr RideTypeDescriptor LSMLaunchedRollerCoasterRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_ROLLERCOASTER),
    SET_FIELD(EnabledTrackPieces, {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_FLAT_ROLL_BANKING, TRACK_SLOPE, TRACK_SLOPE_STEEP_UP, TRACK_SLOPE_STEEP_DOWN, TRACK_SLOPE_CURVE, TRACK_SLOPE_CURVE_STEEP, TRACK_S_BEND, TRACK_CURVE_SMALL, TRACK_CURVE, TRACK_CURVE_LARGE, TRACK_HELIX_DOWN_BANKED_HALF, TRACK_HELIX_UP_BANKED_HALF, TRACK_BRAKES, TRACK_ON_RIDE_PHOTO, TRACK_BLOCK_BRAKES, TRACK_SLOPE_ROLL_BANKING, TRACK_SLOPE_STEEP_LONG, TRACK_SLOPE_CURVE_BANKED, TRACK_SLOPE_CURVE_LARGE, TRACK_BOOSTER, TRACK_POWERED_LIFT, TRACK_SLOPE_VERTICAL, TRACK_CURVE_VERTICAL, TRACK_BARREL_ROLL, TRACK_QUARTER_LOOP, TRACK_HALF_LOOP, TRACK_HALF_LOOP_MEDIUM, TRACK_HALF_LOOP_LARGE, TRACK_VERTICAL_LOOP, TRACK_CORKSCREW, TRACK_CORKSCREW_LARGE, TRACK_ZERO_G_ROLL, TRACK_ZERO_G_ROLL_LARGE}),
    SET_FIELD(ExtraTrackPieces, {TRACK_LIFT_HILL, TRACK_LIFT_HILL_CABLE}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::EndStation),
    SET_FIELD(TrackPaintFunction, GetTrackPaintFunctionIMGTriangle),
    SET_FIELD(Flags, RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION |
                     RIDE_TYPE_FLAGS_COMMON_COASTER | RIDE_TYPE_FLAGS_COMMON_COASTER_NON_ALT |
                     RIDE_TYPE_FLAG_PEEP_CHECK_GFORCES | RIDE_TYPE_FLAG_ALLOW_MULTIPLE_CIRCUITS | RIDE_TYPE_FLAG_ALLOW_CABLE_LIFT_HILL |
                     RIDE_TYPE_FLAG_ALLOW_REVERSED_TRAINS),
    SET_FIELD(RideModes, EnumsToFlags(RideMode::ContinuousCircuit, RideMode::ContinuousCircuitBlockSectioned)),
    SET_FIELD(DefaultMode, RideMode::ContinuousCircuit),
    SET_FIELD(OperatingSettings, { 10, 27, 30, 17, 68, 1 }),
    SET_FIELD(Naming, { STR_RIDE_NAME_LSM_LAUNCH_ROLLER_COASTER, STR_RIDE_DESCRIPTION_LSM_LAUNCH_ROLLER_COASTER }),
    SET_FIELD(NameConvention, { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station }),
    SET_FIELD(EnumName, nameof(RIDE_TYPE_LSM_LAUNCH_ROLLER_COASTER)),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE)),
    SET_FIELD(Heights, { 86, 24, 9, 11, }),
    SET_FIELD(MaxMass, 31),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::LiftClassic, 5, 8 }),
    SET_FIELD(RatingsMultipliers, { 51, 32, 10 }),
    SET_FIELD(UpkeepCosts, { 10, 20, 80, 12, 3, 40 }),
    SET_FIELD(BuildCosts, { 60.00_GBP, 2.50_GBP, 55, }),
    SET_FIELD(DefaultPrices, { 20, 20 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_ACID),
    SET_FIELD(PhotoItem, ShopItem::Photo),
    SET_FIELD(BonusValue, 120),
    SET_FIELD(ColourPresets, TRACK_COLOUR_PRESETS(
        { COLOUR_LIGHT_BLUE, COLOUR_LIGHT_BLUE, COLOUR_BLACK}, // Taiga
        { COLOUR_BRIGHT_GREEN, COLOUR_BRIGHT_GREEN, COLOUR_YELLOW}, // Cheetah Hunt
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_DULL_BROWN_LIGHT}, // Maverick
        { COLOUR_BLACK, COLOUR_BLACK, COLOUR_DULL_BROWN_DARK}, // Taron
    )),
    SET_FIELD(ColourPreview, { SPR_RIDE_DESIGN_PREVIEW_GIGA_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_GIGA_COASTER_SUPPORTS }),
    SET_FIELD(ColourKey, RideColourKey::Ride),
    SET_FIELD(Name, "lsm_rc"),
    SET_FIELD(RatingsData,
    {
        RatingsCalculationType::Normal,
        { RIDE_RATING(3, 85), RIDE_RATING(0, 40), RIDE_RATING(0, 35) },
        14,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             819, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                RIDE_RATING(0, 40), RIDE_RATING(0, 05), 0 },
            { RatingsModifierType::BonusTrainLength,      0,                140434, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                51366, 85019, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,                364088, 400497, 0 },
            { RatingsModifierType::BonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::BonusGForces,          0,                36864, 30384, 49648 },
            { RatingsModifierType::BonusTurns,            0,                28235, 34767, 45749 },
            { RatingsModifierType::BonusDrops,            0,                43690, 46811, 49152 },
            { RatingsModifierType::BonusSheltered,        0,                15420, 32768, 35108 },
            { RatingsModifierType::BonusReversedTrains,   0,                2, 20, 20 },
            { RatingsModifierType::BonusProximity,        0,                20130, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                6693, 0, 0 },
            { RatingsModifierType::RequirementDropHeight, 16,               2, 2, 2 },
            { RatingsModifierType::RequirementMaxSpeed,   0xA0000,          2, 2, 2 },
            { RatingsModifierType::RequirementNegativeGs, FIXED_2DP(0, 40), 2, 2, 2 },
            { RatingsModifierType::RequirementNumDrops,   2,                2, 2, 2 },
            { RatingsModifierType::PenaltyLateralGs,      0,                36864, 30384, 49648 },
        },
    }),
};
// clang-format on
