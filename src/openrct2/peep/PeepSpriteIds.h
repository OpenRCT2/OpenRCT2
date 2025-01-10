/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

namespace OpenRCT2
{
    enum
    {
        kPeepSpriteNormalStateWalkingId = 6409,
        kPeepSpriteNormalStateCheckTimeId = 6585,
        kPeepSpriteNormalStateWatchRideId = 6621,
        kPeepSpriteNormalStateEatFoodId = 6653,
        kPeepSpriteNormalStateShakeHeadId = 6697,
        kPeepSpriteNormalStateEmptyPocketsId = 6757,
        kPeepSpriteNormalStateHoldMatId = 6561,
        kPeepSpriteNormalStateSittingIdleId = 6625,
        kPeepSpriteNormalStateSittingEatFoodId = 6629,
        kPeepSpriteNormalStateSittingLookAroundLeftId = 6637,
        kPeepSpriteNormalStateSittingLookAroundRightId = 6645,
        kPeepSpriteNormalStateHangingId = 6813,
        kPeepSpriteNormalStateWowId = 6825,
        kPeepSpriteNormalStateThrowUpId = 6845,
        kPeepSpriteNormalStateJumpId = 6909,
        kPeepSpriteNormalStateDrowningId = 6957,
        kPeepSpriteNormalStateJoyId = 6989,
        kPeepSpriteNormalStateReadMapId = 7041,
        kPeepSpriteNormalStateWaveId = 7101,
        kPeepSpriteNormalStateWave2Id = 7121,
        kPeepSpriteNormalStateTakePhotoId = 7149,
        kPeepSpriteNormalStateClapId = 7249,
        kPeepSpriteNormalStateDisgustId = 7309,
        kPeepSpriteNormalStateDrawPictureId = 7329,
        kPeepSpriteNormalStateBeingWatchedId = 7285,
        kPeepSpriteNormalStateWithdrawMoneyId = 7181,

        kHandymanSpriteStateWalkingId = 11261,
        kHandymanSpriteStateWatchRideId = 11285,
        kHandymanSpriteStateHangingId = 11289,
        kHandymanSpriteStateStaffMowerId = 11301,
        kHandymanSpriteStateStaffSweepId = 11325,
        kHandymanSpriteStateDrowningId = 11345,
        kHandymanSpriteStateStaffWateringId = 11377,
        kHandymanSpriteStateStaffEmptyBinId = 11401,

        kMechanicSpriteStateWalkingId = 11441,
        kMechanicSpriteStateWatchRideId = 11465,
        kMechanicSpriteStateHangingId = 11469,
        kMechanicSpriteStateDrowningId = 11481,
        kMechanicSpriteStateStaffAnswerCallId = 11513,
        kMechanicSpriteStateStaffCheckBoardId = 11597,
        kMechanicSpriteStateStaffFixId = 11669,
        kMechanicSpriteStateStaffFixGroundId = 11801,

        kSecuritySpriteStateWalkingId = 11881,
        kSecuritySpriteStateWatchRideId = 11905,
        kSecuritySpriteStateHangingId = 11909,
        kSecuritySpriteStateDrowningId = 11921,

        kEntertainerSpritePandaStateWalkingId = 11973,
        kEntertainerSpritePandaStateWatchRideId = 11997,
        kEntertainerSpritePandaStateWaveId = 12041,
        kEntertainerSpritePandaStateHangingId = 12001,
        kEntertainerSpritePandaStateDrowningId = 12013,
        kEntertainerSpritePandaStateJoyId = 12065,

        kEntertainerSpriteTigerStateWalkingId = 12129,
        kEntertainerSpriteTigerStateWatchRideId = 12153,
        kEntertainerSpriteTigerStateWaveId = 12197,
        kEntertainerSpriteTigerStateHangingId = 12157,
        kEntertainerSpriteTigerStateDrowningId = 12169,
        kEntertainerSpriteTigerStateJoyId = 12221,

        kEntertainerSpriteElephantStateWalkingId = 12325,
        kEntertainerSpriteElephantStateWatchRideId = 12349,
        kEntertainerSpriteElephantStateWaveId = 12365,
        kEntertainerSpriteElephantStateHangingId = 12353,
        kEntertainerSpriteElephantStateDrowningId = 12393,
        kEntertainerSpriteElephantStateJoyId = 12433,

        kEntertainerSpriteRomanStateWalkingId = 13897,
        kEntertainerSpriteRomanStateWatchRideId = 13921,
        kEntertainerSpriteRomanStateWaveId = 13937,
        kEntertainerSpriteRomanStateHangingId = 13925,
        kEntertainerSpriteRomanStateDrowningId = 13969,
        kEntertainerSpriteRomanStateJoyId = 14029,

        kEntertainerSpriteGorillaStateWalkingId = 12549,
        kEntertainerSpriteGorillaStateWatchRideId = 12545,
        kEntertainerSpriteGorillaStateWaveId = 12573,
        kEntertainerSpriteGorillaStateHangingId = 12665,
        kEntertainerSpriteGorillaStateDrowningId = 12617,

        kEntertainerSpriteSnowmanStateWalkingId = 12681,
        kEntertainerSpriteSnowmanStateWatchRideId = 12677,
        kEntertainerSpriteSnowmanStateWaveId = 12705,
        kEntertainerSpriteSnowmanStateHangingId = 12881,
        kEntertainerSpriteSnowmanStateDrowningId = 12841,

        kEntertainerSpriteKnightStateWalkingId = 12897,
        kEntertainerSpriteKnightStateWatchRideId = 12893,
        kEntertainerSpriteKnightStateWaveId = 12993,
        kEntertainerSpriteKnightStateHangingId = 12981,
        kEntertainerSpriteKnightStateDrowningId = 12921,

        kEntertainerSpriteAstronautStateWalkingId = 13769,
        kEntertainerSpriteAstronautStateWatchRideId = 13765,
        kEntertainerSpriteAstronautStateWaveId = 13857,
        kEntertainerSpriteAstronautStateHangingId = 13845,
        kEntertainerSpriteAstronautStateDrowningId = 13793,

        kEntertainerSpriteBanditStateWalkingId = 13109,
        kEntertainerSpriteBanditStateWatchRideId = 13105,
        kEntertainerSpriteBanditStateWaveId = 13209,
        kEntertainerSpriteBanditStateHangingId = 13197,
        kEntertainerSpriteBanditStateDrowningId = 13133,

        kEntertainerSpriteSheriffStateWalkingId = 13613,
        kEntertainerSpriteSheriffStateWatchRideId = 13609,
        kEntertainerSpriteSheriffStateWaveId = 13713,
        kEntertainerSpriteSheriffStateHangingId = 13701,
        kEntertainerSpriteSheriffStateDrowningId = 13637,

        kEntertainerSpritePirateStateWalkingId = 13381,
        kEntertainerSpritePirateStateWatchRideId = 13377,
        kEntertainerSpritePirateStateWaveId = 13481,
        kEntertainerSpritePirateStateHangingId = 13469,
        kEntertainerSpritePirateStateDrowningId = 13405,

        kPeepSpriteIceCreamStateWalkingId = 7505,
        kPeepSpriteIceCreamStateWatchRideId = 7501,
        kPeepSpriteIceCreamStateEatFoodId = 7581,
        kPeepSpriteIceCreamStateSittingIdleId = 7553,
        kPeepSpriteIceCreamStateSittingEatFoodId = 7557,

        kPeepSpriteChipsStateWalkingId = 7609,
        kPeepSpriteChipsStateWatchRideId = 7605,
        kPeepSpriteChipsStateEatFoodId = 7733,
        kPeepSpriteChipsStateSittingIdleId = 7681,
        kPeepSpriteChipsStateSittingEatFoodId = 7685,

        kPeepSpriteBurgerStateWalkingId = 8385,
        kPeepSpriteBurgerStateWatchRideId = 8381,
        kPeepSpriteBurgerStateEatFoodId = 8509,
        kPeepSpriteBurgerStateSittingIdleId = 8457,
        kPeepSpriteBurgerStateSittingEatFoodId = 8461,

        kPeepSpriteDrinkStateWalkingId = 8713,
        kPeepSpriteDrinkStateWatchRideId = 8709,
        kPeepSpriteDrinkStateEatFoodId = 8837,
        kPeepSpriteDrinkStateSittingIdleId = 8785,
        kPeepSpriteDrinkStateSittingEatFoodId = 8789,

        kPeepSpriteBalloonStateWalkingId = 10785,
        kPeepSpriteBalloonStateWatchRideId = 10781,
        kPeepSpriteBalloonStateSittingIdleId = 10809,
        kPeepSpriteBalloonItemStart = 10813,

        kPeepSpriteCandyflossStateWalkingId = 10849,
        kPeepSpriteCandyflossStateWatchRideId = 10845,
        kPeepSpriteCandyflossStateEatFoodId = 10973,
        kPeepSpriteCandyflossStateSittingIdleId = 10921,
        kPeepSpriteCandyflossStateSittingEatFoodId = 10925,

        kPeepSpriteUmbrellaStateWalkingId = 11197,
        kPeepSpriteUmbrellaStateWatchRideId = 11221,
        kPeepSpriteUmbrellaStateSittingIdleId = 11225,
        kPeepSpriteUmbrellaItemStart = 11229,

        kPeepSpritePizzaStateWalkingId = 7785,
        kPeepSpritePizzaStateWatchRideId = 7781,
        kPeepSpritePizzaStateEatFoodId = 7861,
        kPeepSpritePizzaStateSittingIdleId = 7833,
        kPeepSpritePizzaStateSittingEatFoodId = 7837,

        kSecurityStaffSpriteAltStateWalkingId = 11949,

        kPeepSpritePopcornStateWalkingId = 11025,
        kPeepSpritePopcornStateWatchRideId = 11021,
        kPeepSpritePopcornStateEatFoodId = 11149,
        kPeepSpritePopcornStateSittingIdleId = 11097,
        kPeepSpritePopcornStateSittingEatFoodId = 11101,

        kPeepSpriteArmsCrossedStateWalkingId = 6433,

        kPeepSpriteHeadDownStateWalkingId = 6457,

        kPeepSpriteNauseousStateWalkingId = 6481,

        kPeepSpriteVeryNauseousStateWalkingId = 6505,
        kPeepSpriteVeryNauseousStateWatchRideId = 6529,
        kPeepSpriteVeryNauseousStateSittingIdleId = 6533,
        kPeepSpriteRequireToiletStateWalkingId = 6537,

        kPeepSpriteHatStateWalkingId = 10721,
        kPeepSpriteHatStateWatchRideId = 10717,
        kPeepSpriteHatStateSittingIdleId = 10745,
        kPeepSpriteHatItemStart = 10749,

        kPeepSpriteHotDogStateWalkingId = 7889,
        kPeepSpriteHotDogStateWatchRideId = 7885,
        kPeepSpriteHotDogStateEatFoodId = 8013,
        kPeepSpriteHotDogStateSittingIdleId = 7961,
        kPeepSpriteHotDogStateSittingEatFoodId = 7965,

        kPeepSpriteTentacleStateWalkingId = 8065,
        kPeepSpriteTentacleStateWatchRideId = 8061,
        kPeepSpriteTentacleStateEatFoodId = 8285,
        kPeepSpriteTentacleStateSittingIdleId = 8185,
        kPeepSpriteTentacleStateSittingEatFoodId = 8189,

        kPeepSpriteToffeeAppleStateWalkingId = 7401,
        kPeepSpriteToffeeAppleStateWatchRideId = 7397,
        kPeepSpriteToffeeAppleStateEatFoodId = 7477,
        kPeepSpriteToffeeAppleStateSittingIdleId = 7449,
        kPeepSpriteToffeeAppleStateSittingEatFoodId = 7453,

        kPeepSpriteDonutStateWalkingId = 8561,
        kPeepSpriteDonutStateWatchRideId = 8557,
        kPeepSpriteDonutStateEatFoodId = 8661,
        kPeepSpriteDonutStateSittingIdleId = 8633,
        kPeepSpriteDonutStateSittingEatFoodId = 8637,

        kPeepSpriteCoffeeStateWalkingId = 8885,
        kPeepSpriteCoffeeStateWatchRideId = 8981,
        kPeepSpriteCoffeeStateSittingIdleId = 8957,

        kPeepSpriteChickenStateWalkingId = 9005,
        kPeepSpriteChickenStateWatchRideId = 9125,
        kPeepSpriteChickenStateSittingIdleId = 9077,

        kPeepSpriteLemonadeStateWalkingId = 9173,
        kPeepSpriteLemonadeStateWatchRideId = 9293,
        kPeepSpriteLemonadeStateSittingIdleId = 9245,

        kPeepSpriteWatchingStateWatchRideId = 7245,

        kPeepSpritePretzelStateWalkingId = 9341,
        kPeepSpritePretzelStateWatchRideId = 9461,
        kPeepSpritePretzelStateSittingIdleId = 9413,

        kPeepSpriteSunglassesStateWalkingId = 10685,
        kPeepSpriteSunglassesStateWatchRideId = 10713,
        kPeepSpriteSunglassesStateSittingIdleId = 10709,

        kPeepSpriteSujeonggwaStateWalkingId = 9509,
        kPeepSpriteSujeonggwaStateWatchRideId = 9629,
        kPeepSpriteSujeonggwaStateSittingIdleId = 9581,

        kPeepSpriteJuiceStateWalkingId = 9677,
        kPeepSpriteJuiceStateWatchRideId = 9797,
        kPeepSpriteJuiceStateSittingIdleId = 9749,

        kPeepSpriteFunnelCakeStateWalkingId = 9845,
        kPeepSpriteFunnelCakeStateWatchRideId = 9965,
        kPeepSpriteFunnelCakeStateSittingIdleId = 9917,

        kPeepSpriteNoodlesStateWalkingId = 10013,
        kPeepSpriteNoodlesStateWatchRideId = 10133,
        kPeepSpriteNoodlesStateSittingIdleId = 10085,

        kPeepSpriteSausageStateWalkingId = 10181,
        kPeepSpriteSausageStateWatchRideId = 10301,
        kPeepSpriteSausageStateSittingIdleId = 10253,

        kPeepSpriteSoupStateWalkingId = 10349,
        kPeepSpriteSoupStateWatchRideId = 10469,
        kPeepSpriteSoupStateSittingIdleId = 10421,

        kPeepSpriteSandwichStateWalkingId = 10517,
        kPeepSpriteSandwichStateWatchRideId = 10637,
        kPeepSpriteSandwichStateSittingIdleId = 10589,
    };
}
