/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../../paint/Supports.h"
#include "../../../sprites.h"
#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"
#include "../BolligerMabillardTrack.hpp"

// clang-format off
constexpr const RideTypeDescriptor VerticalDropCoasterRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_ROLLERCOASTER),
    SET_FIELD(EnabledTrackPieces, {TRACK_FLAT, TRACK_STRAIGHT, TRACK_STATION_END, TRACK_LIFT_HILL, TRACK_LIFT_HILL_STEEP, TRACK_FLAT_ROLL_BANKING, TRACK_VERTICAL_LOOP, TRACK_SLOPE, TRACK_SLOPE_STEEP_UP, TRACK_SLOPE_STEEP_DOWN, TRACK_SLOPE_LONG, TRACK_SLOPE_CURVE, TRACK_SLOPE_CURVE_STEEP, TRACK_S_BEND, TRACK_CURVE_SMALL, TRACK_CURVE, TRACK_CURVE_LARGE, TRACK_HELIX_SMALL, TRACK_BRAKES, TRACK_ON_RIDE_PHOTO, TRACK_SLOPE_VERTICAL, TRACK_SLOPE_CURVE_BANKED, TRACK_BLOCK_BRAKES, TRACK_SLOPE_ROLL_BANKING, TRACK_CURVE_VERTICAL, TRACK_HALF_LOOP_LARGE, TRACK_BRAKE_FOR_DROP}),
    SET_FIELD(ExtraTrackPieces, {TRACK_HALF_LOOP, TRACK_CORKSCREW, TRACK_BARREL_ROLL, TRACK_POWERED_LIFT, TRACK_HALF_LOOP_LARGE, TRACK_QUARTER_LOOP, TRACK_BOOSTER, TRACK_TWIST}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::EndStation),
    SET_FIELD(TrackPaintFunction, GetTrackPaintFunctionBolligerMabillard<MetalSupportType::Boxed>),
    SET_FIELD(Flags, RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION |
                     RIDE_TYPE_FLAGS_COMMON_COASTER | RIDE_TYPE_FLAGS_COMMON_COASTER_NON_ALT |
                     RIDE_TYPE_FLAG_PEEP_CHECK_GFORCES),
    SET_FIELD(RideModes, EnumsToFlags(RideMode::ContinuousCircuit, RideMode::ContinuousCircuitBlockSectioned)),
    SET_FIELD(DefaultMode, RideMode::ContinuousCircuit),
    SET_FIELD(OperatingSettings, { 10, 27, 26, 17, 68, 0 }),
    SET_FIELD(Naming, { STR_RIDE_NAME_VERTICAL_DROP_ROLLER_COASTER, STR_RIDE_DESCRIPTION_VERTICAL_DROP_ROLLER_COASTER }),
    SET_FIELD(NameConvention, { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station }),
    SET_FIELD(EnumName, nameof(RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER)),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE)),
    SET_FIELD(Heights, { 53, 24, 8, 11, }),
    SET_FIELD(MaxMass, 25),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::LiftBM, 4, 5 }),
    SET_FIELD(RatingsMultipliers, { 52, 38, 10 }),
    SET_FIELD(UpkeepCosts, { 44, 20, 80, 11, 3, 10 }),
    SET_FIELD(BuildCosts, { 60.00_GBP, 2.50_GBP, 42, }),
    SET_FIELD(DefaultPrices, { 20, 20 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_ROCK_1),
    SET_FIELD(PhotoItem, ShopItem::Photo),
    SET_FIELD(BonusValue, 95),
    SET_FIELD(ColourPresets, TRACK_COLOUR_PRESETS(
        { COLOUR_BLACK, COLOUR_GREY, COLOUR_BLACK },
        { COLOUR_DARK_GREEN, COLOUR_DARK_GREEN, COLOUR_GREY },
    )),
    SET_FIELD(ColourPreview, { SPR_RIDE_DESIGN_PREVIEW_VERTICAL_DROP_ROLLER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_VERTICAL_DROP_ROLLER_COASTER_SUPPORTS }),
    SET_FIELD(ColourKey, RideColourKey::Ride),
    SET_FIELD(Name, "vertical_drop_rc"),
    SET_FIELD(RatingsData,
    {
        SET_FIELD(Type, RatingsCalculationType::Normal),
        SET_FIELD(BaseRatings, { RIDE_RATING(3, 20), RIDE_RATING(0, 80), RIDE_RATING(0, 30) }),
        SET_FIELD(Unreliability, 16),
        SET_FIELD(RelaxRequirementsIfInversions, false),
        SET_FIELD(Modifiers, {
            { RatingsModifierType::BonusLength,           4000,             1146, 0, 0 },
            { RatingsModifierType::BonusSynchronization,  0,                RIDE_RATING(0, 40), RIDE_RATING(0, 05), 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                97418, 141699, 70849 },
            { RatingsModifierType::BonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::BonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::BonusGForces,          0,                40960, 35746, 49648 },
            { RatingsModifierType::BonusTurns,            0,                26749, 34767, 45749 },
            { RatingsModifierType::BonusDrops,            0,                58254, 46811, 49152 },
            { RatingsModifierType::BonusSheltered,        0,                15420, 32768, 35108 },
            { RatingsModifierType::BonusProximity,        0,                20130, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                6693, 0, 0 },
            { RatingsModifierType::RequirementDropHeight, 20,               2, 2, 2 },
            { RatingsModifierType::RequirementMaxSpeed,   0xA0000,          2, 2, 2 },
            { RatingsModifierType::RequirementNegativeGs, FIXED_2DP(0, 10), 2, 2, 2 },
            { RatingsModifierType::RequirementNumDrops,   1,                2, 2, 2 },
            { RatingsModifierType::PenaltyLateralGs,      0,                40960, 35746, 49648 },
            { RatingsModifierType::PenaltyIntensity,      0,                0, 0, 0 },
        }),
    }),
};
// clang-format on
