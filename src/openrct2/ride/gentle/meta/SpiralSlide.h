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
constexpr const RideTypeDescriptor SpiralSlideRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_GENTLE),
    SET_FIELD(EnabledTrackPieces, {}),
    SET_FIELD(ExtraTrackPieces, {}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::FlatTrack2x2),
    SET_FIELD(TrackPaintFunction, GetTrackPaintFunctionSpiralSlide),
    SET_FIELD(Flags, RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION | RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS |
                     RIDE_TYPE_FLAG_NO_TEST_MODE | RIDE_TYPE_FLAG_NO_VEHICLES | RIDE_TYPE_FLAG_HAS_NO_TRACK | RIDE_TYPE_FLAG_TRACK_NO_WALLS |
                     RIDE_TYPE_FLAG_FLAT_RIDE | RIDE_TYPE_FLAG_ALLOW_MUSIC | RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT | RIDE_TYPE_FLAG_INTERESTING_TO_LOOK_AT | 
                     RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY | RIDE_TYPE_FLAG_IS_SPIRAL_SLIDE),
    SET_FIELD(RideModes, EnumsToFlags(RideMode::SingleRidePerAdmission, RideMode::UnlimitedRidesPerAdmission)),
    SET_FIELD(DefaultMode, RideMode::SingleRidePerAdmission),
    SET_FIELD(OperatingSettings, { 1, 5, 0, 0, 0, 0 }),
    SET_FIELD(Naming, { STR_RIDE_NAME_SPIRAL_SLIDE, STR_RIDE_DESCRIPTION_SPIRAL_SLIDE }),
    SET_FIELD(NameConvention, { RideComponentType::Train, RideComponentType::Building, RideComponentType::Station }),
    SET_FIELD(EnumName, nameof(RIDE_TYPE_SPIRAL_SLIDE)),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_SAFETY_CUT_OUT)),
    SET_FIELD(Heights, { 15, 128, 0, 2, }),
    SET_FIELD(MaxMass, 255),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::Null, 5, 5 }),
    SET_FIELD(RatingsMultipliers, { 50, 10, 0 }),
    SET_FIELD(UpkeepCosts, { 50, 1, 0, 0, 0, 0 }),
    SET_FIELD(BuildCosts, { 82.50_GBP, 1.00_GBP, 1, }),
    SET_FIELD(DefaultPrices, { 15, 0 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_SUMMER),
    SET_FIELD(PhotoItem, ShopItem::Photo),
    SET_FIELD(BonusValue, 40),
    SET_FIELD(ColourPresets, TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_RED, COLOUR_YELLOW, COLOUR_DARK_BROWN },
        { COLOUR_LIGHT_BLUE, COLOUR_BRIGHT_PINK, COLOUR_LIGHT_PURPLE },
        { COLOUR_WHITE, COLOUR_BORDEAUX_RED, COLOUR_GREY },
        { COLOUR_DULL_BROWN_LIGHT, COLOUR_SATURATED_RED, COLOUR_GREY },
    )),
    SET_FIELD(ColourPreview, { SPR_RIDE_DESIGN_PREVIEW_SPIRAL_SLIDE_TRACK, 0 }),
    SET_FIELD(ColourKey, RideColourKey::Ride),
    SET_FIELD(Name, "spiral_slide"),
    SET_FIELD(RatingsData,
    {
        SET_FIELD(Type, RatingsCalculationType::FlatRide),
        SET_FIELD(BaseRatings, { RIDE_RATING(1, 50), RIDE_RATING(1, 40), RIDE_RATING(0, 90) }),
        SET_FIELD(Unreliability, 8),
        SET_FIELD(RideShelter, 2),
        SET_FIELD(RelaxRequirementsIfInversions, false),
        SET_FIELD(Modifiers, {
            { RatingsModifierType::BonusSlideUnlimitedRides, 0, RIDE_RATING(0, 40), RIDE_RATING(0, 20), RIDE_RATING(0, 25), 0 },
            { RatingsModifierType::BonusScenery,             0, 25098, 0, 0 },
        }),
    }),
    SET_FIELD(UpdateRotating, UpdateRotatingDefault),
    SET_FIELD(LightFXAddLightsMagicVehicle, nullptr),
    SET_FIELD(StartRideMusic, OpenRCT2::RideAudio::DefaultStartRideMusicChannel),
    SET_FIELD(DesignCreateMode, TrackDesignCreateMode::Default),
    SET_FIELD(MusicUpdateFunction, DefaultMusicUpdate),
    SET_FIELD(Classification, RideClassification::Ride),
    SET_FIELD(UpdateLeaveEntrance, PeepUpdateRideLeaveEntranceSpiralSlide),
    SET_FIELD(SpecialElementRatingAdjustment, SpecialTrackElementRatingsAjustment_Default),
    SET_FIELD(GetGuestWaypointLocation, GetGuestWaypointLocationDefault),
    SET_FIELD(ConstructionWindowContext, RideConstructionWindowContext::Default),
    SET_FIELD(RideUpdate, UpdateSpiralSlide),
};
// clang-format on
