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
constexpr const RideTypeDescriptor GoKartsRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_THRILL),
    SET_FIELD(EnabledTrackPieces, {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_SLOPE, TRACK_CURVE_VERY_SMALL}),
    SET_FIELD(ExtraTrackPieces, {}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::EndStation),
    SET_FIELD(TrackPaintFunction, GetTrackPaintFunctionGoKarts),
    SET_FIELD(Flags, RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_MAIN | RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_SUPPORTS | RIDE_TYPE_FLAG_NO_TEST_MODE | RIDE_TYPE_FLAG_HAS_ONE_STATION |
                     RIDE_TYPE_FLAG_TRACK_NO_WALLS | RIDE_TYPE_FLAG_PEEP_WILL_RIDE_AGAIN | RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS | RIDE_TYPE_FLAG_HAS_TRACK |
                     RIDE_TYPE_FLAG_SUPPORTS_MULTIPLE_TRACK_COLOUR | RIDE_TYPE_FLAG_ALLOW_MUSIC | RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT | RIDE_TYPE_FLAG_INTERESTING_TO_LOOK_AT),
    SET_FIELD(RideModes, EnumsToFlags(RideMode::Race, RideMode::ContinuousCircuit)),
    SET_FIELD(DefaultMode, RideMode::Race),
    SET_FIELD(OperatingSettings, { 1, 10, 0, 0, 0, 0 }),
    SET_FIELD(Naming, { STR_RIDE_NAME_GO_KARTS, STR_RIDE_DESCRIPTION_GO_KARTS }),
    SET_FIELD(NameConvention, { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station }),
    SET_FIELD(EnumName, nameof(RIDE_TYPE_GO_KARTS)),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_VEHICLE_MALFUNCTION)),
    SET_FIELD(Heights, { 7, 24, 2, 1, }),
    SET_FIELD(MaxMass, 255),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::Null, 5, 5 }),
    SET_FIELD(RatingsMultipliers, { 120, 20, 0 }),
    SET_FIELD(UpkeepCosts, { 50, 20, 0, 8, 0, 0 }),
    SET_FIELD(BuildCosts, { 31.00_GBP, 2.00_GBP, 20, }),
    SET_FIELD(DefaultPrices, { 20, 0 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_TECHNO),
    SET_FIELD(PhotoItem, ShopItem::Photo),
    SET_FIELD(BonusValue, 55),
    SET_FIELD(ColourPresets, TRACK_COLOUR_PRESETS(
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_DARK_BROWN },
        { COLOUR_YELLOW, COLOUR_YELLOW, COLOUR_BORDEAUX_RED },
        { COLOUR_DARK_GREEN, COLOUR_DARK_GREEN, COLOUR_DARK_GREEN },
        { COLOUR_DARK_BROWN, COLOUR_DARK_BROWN, COLOUR_BLACK },
    )),
    SET_FIELD(ColourPreview, { SPR_RIDE_DESIGN_PREVIEW_GO_KARTS_TRACK, SPR_RIDE_DESIGN_PREVIEW_GO_KARTS_SUPPORTS }),
    SET_FIELD(ColourKey, RideColourKey::Ride),
    SET_FIELD(Name, "go_karts"),
    SET_FIELD(RatingsData,
    {
        SET_FIELD(Type, RatingsCalculationType::Normal),
        SET_FIELD(BaseRatings, { RIDE_RATING(1, 42), RIDE_RATING(1, 73), RIDE_RATING(0, 40) }),
        SET_FIELD(Unreliability, 16),
        SET_FIELD(RelaxRequirementsIfInversions, false),
        SET_FIELD(Modifiers, {
            { RatingsModifierType::BonusLength,            700, 32768, 0, 0 },
            { RatingsModifierType::BonusGoKartRace,        4,   RIDE_RATING(1, 40), RIDE_RATING(0, 50), 0 },
            { RatingsModifierType::BonusTurns,             0,   4458, 3476, 5718 },
            { RatingsModifierType::BonusDrops,             0,   8738, 5461, 6553 },
            { RatingsModifierType::BonusSheltered,         0,   2570, 8738, 2340 },
            { RatingsModifierType::BonusProximity,         0,   11183, 0, 0 },
            { RatingsModifierType::BonusScenery,           0,   16732, 0, 0 },
            { RatingsModifierType::RequirementUnsheltered, 6,   2, 1, 1 },
            { RatingsModifierType::PenaltyIntensity,       0,   0, 0, 0 },
        }),
    }),
    SET_FIELD(UpdateRotating, UpdateRotatingDefault),
    SET_FIELD(LightFXAddLightsMagicVehicle, LightFxAddLightsMagicVehicle_BoatHire),
};
// clang-format on
