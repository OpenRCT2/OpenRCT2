/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "PeepAnimationData.h"

#include "../drawing/Drawing.h"
#include "PeepSpriteIds.h"

#include <algorithm>
#include <array>

namespace OpenRCT2
{
    // Adapted from CarEntry.cpp
    static SpriteBounds inferMaxAnimationDimensions(const PeepAnimation& anim)
    {
        constexpr uint8_t kWidth = 200;
        constexpr uint8_t kHeight = 200;
        constexpr uint8_t kCentreX = kWidth / 2;
        constexpr uint8_t kCentreY = kHeight / 2;

        uint8_t bitmap[kHeight][kWidth] = { 0 };

        DrawPixelInfo dpi = {
            .bits = reinterpret_cast<uint8_t*>(bitmap),
            .x = -(kWidth / 2),
            .y = -(kHeight / 2),
            .width = kWidth,
            .height = kHeight,
            .pitch = 0,
            .zoom_level = ZoomLevel{ 0 },
        };

        const auto numImages = *(std::max_element(anim.frame_offsets.begin(), anim.frame_offsets.end())) + 1;
        for (int32_t i = 0; i < numImages; ++i)
        {
            GfxDrawSpriteSoftware(dpi, ImageId(anim.base_image + i), { 0, 0 });
        }

        int32_t spriteWidth = -1;
        for (int32_t i = kCentreX - 1; i != 0; --i)
        {
            for (int32_t j = 0; j < kWidth; j++)
            {
                if (bitmap[j][kCentreX - i] != 0)
                {
                    spriteWidth = i;
                    break;
                }
            }

            if (spriteWidth != -1)
                break;

            for (int32_t j = 0; j < kWidth; j++)
            {
                if (bitmap[j][kCentreX + i] != 0)
                {
                    spriteWidth = i;
                    break;
                }
            }

            if (spriteWidth != -1)
                break;
        }
        spriteWidth++;

        int32_t spriteHeightNegative = -1;
        for (int32_t i = kCentreY - 1; i != 0; --i)
        {
            for (int32_t j = 0; j < kWidth; j++)
            {
                if (bitmap[kCentreY - i][j] != 0)
                {
                    spriteHeightNegative = i;
                    break;
                }
            }

            if (spriteHeightNegative != -1)
                break;
        }
        spriteHeightNegative++;

        int32_t spriteHeightPositive = -1;
        for (int32_t i = kCentreY - 1; i != 0; --i)
        {
            for (int32_t j = 0; j < kWidth; j++)
            {
                if (bitmap[kCentreY + i][j] != 0)
                {
                    spriteHeightPositive = i;
                    break;
                }
            }

            if (spriteHeightPositive != -1)
                break;
        }
        spriteHeightPositive++;

        return {
            .sprite_width = static_cast<uint8_t>(spriteWidth),
            .sprite_height_negative = static_cast<uint8_t>(spriteHeightNegative),
            .sprite_height_positive = static_cast<uint8_t>(spriteHeightPositive),
        };
    }

    // clang-format off

    // Define animation sequences for Normal sprites
    static constexpr std::array<uint8_t,   6> kPeepAnimationSequenceNormalNone                   = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,  16> kPeepAnimationSequenceNormalCheckTime              = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
    static constexpr std::array<uint8_t,   1> kPeepAnimationSequenceNormalWatchRide              = { 0 };
    static constexpr std::array<uint8_t,  24> kPeepAnimationSequenceNormalEatFood                = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10 };
    static constexpr std::array<uint8_t,  20> kPeepAnimationSequenceNormalShakeHead              = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
    static constexpr std::array<uint8_t,  14> kPeepAnimationSequenceNormalEmptyPockets           = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
    static constexpr std::array<uint8_t,   6> kPeepAnimationSequenceNormalHoldMat                = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,   1> kPeepAnimationSequenceNormalSittingIdle            = { 0 };
    static constexpr std::array<uint8_t,  26> kPeepAnimationSequenceNormalSittingEatFood         = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
    static constexpr std::array<uint8_t,  16> kPeepAnimationSequenceNormalSittingLookAroundLeft  = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
    static constexpr std::array<uint8_t,  16> kPeepAnimationSequenceNormalSittingLookAroundRight = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
    static constexpr std::array<uint8_t,  12> kPeepAnimationSequenceNormalHanging                = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  10> kPeepAnimationSequenceNormalWow                    = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t,  33> kPeepAnimationSequenceNormalThrowUp                = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
    static constexpr std::array<uint8_t,  24> kPeepAnimationSequenceNormalJump                   = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
    static constexpr std::array<uint8_t,  68> kPeepAnimationSequenceNormalDrowning               = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
    static constexpr std::array<uint8_t,  13> kPeepAnimationSequenceNormalJoy                    = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
    static constexpr std::array<uint8_t,  41> kPeepAnimationSequenceNormalReadMap                = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t,  20> kPeepAnimationSequenceNormalWave                   = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t,  25> kPeepAnimationSequenceNormalWave2                  = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t,  23> kPeepAnimationSequenceNormalTakePhoto              = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t, 172> kPeepAnimationSequenceNormalClap                   = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t,  16> kPeepAnimationSequenceNormalDisgust                = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t,  70> kPeepAnimationSequenceNormalDrawPicture            = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t,  13> kPeepAnimationSequenceNormalBeingWatched           = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
    static constexpr std::array<uint8_t,  30> kPeepAnimationSequenceNormalWithdrawMoney          = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

    // Define animation group for Normal sequences
    static PeepAnimations kPeepAnimationsNormal = []() {
        PeepAnimations pag;
        pag[PeepAnimationType::None]                   = { kPeepSpriteNormalStateNoneId,                   kPeepAnimationSequenceNormalNone };
        pag[PeepAnimationType::CheckTime]              = { kPeepSpriteNormalStateCheckTimeId,              kPeepAnimationSequenceNormalCheckTime };
        pag[PeepAnimationType::WatchRide]              = { kPeepSpriteNormalStateWatchRideId,              kPeepAnimationSequenceNormalWatchRide };
        pag[PeepAnimationType::EatFood]                = { kPeepSpriteNormalStateEatFoodId,                kPeepAnimationSequenceNormalEatFood };
        pag[PeepAnimationType::ShakeHead]              = { kPeepSpriteNormalStateShakeHeadId,              kPeepAnimationSequenceNormalShakeHead };
        pag[PeepAnimationType::EmptyPockets]           = { kPeepSpriteNormalStateEmptyPocketsId,           kPeepAnimationSequenceNormalEmptyPockets };
        pag[PeepAnimationType::HoldMat]                = { kPeepSpriteNormalStateHoldMatId,                kPeepAnimationSequenceNormalHoldMat };
        pag[PeepAnimationType::SittingIdle]            = { kPeepSpriteNormalStateSittingIdleId,            kPeepAnimationSequenceNormalSittingIdle };
        pag[PeepAnimationType::SittingEatFood]         = { kPeepSpriteNormalStateSittingEatFoodId,         kPeepAnimationSequenceNormalSittingEatFood };
        pag[PeepAnimationType::SittingLookAroundLeft]  = { kPeepSpriteNormalStateSittingLookAroundLeftId,  kPeepAnimationSequenceNormalSittingLookAroundLeft };
        pag[PeepAnimationType::SittingLookAroundRight] = { kPeepSpriteNormalStateSittingLookAroundRightId, kPeepAnimationSequenceNormalSittingLookAroundRight };
        pag[PeepAnimationType::Hanging]                = { kPeepSpriteNormalStateHangingId,                kPeepAnimationSequenceNormalHanging };
        pag[PeepAnimationType::Wow]                    = { kPeepSpriteNormalStateWowId,                    kPeepAnimationSequenceNormalWow };
        pag[PeepAnimationType::ThrowUp]                = { kPeepSpriteNormalStateThrowUpId,                kPeepAnimationSequenceNormalThrowUp };
        pag[PeepAnimationType::Jump]                   = { kPeepSpriteNormalStateJumpId,                   kPeepAnimationSequenceNormalJump };
        pag[PeepAnimationType::Drowning]               = { kPeepSpriteNormalStateDrowningId,               kPeepAnimationSequenceNormalDrowning };
        pag[PeepAnimationType::Joy]                    = { kPeepSpriteNormalStateJoyId,                    kPeepAnimationSequenceNormalJoy };
        pag[PeepAnimationType::ReadMap]                = { kPeepSpriteNormalStateReadMapId,                kPeepAnimationSequenceNormalReadMap };
        pag[PeepAnimationType::Wave]                   = { kPeepSpriteNormalStateWaveId,                   kPeepAnimationSequenceNormalWave };
        pag[PeepAnimationType::Wave2]                  = { kPeepSpriteNormalStateWave2Id,                  kPeepAnimationSequenceNormalWave2 };
        pag[PeepAnimationType::TakePhoto]              = { kPeepSpriteNormalStateTakePhotoId,              kPeepAnimationSequenceNormalTakePhoto };
        pag[PeepAnimationType::Clap]                   = { kPeepSpriteNormalStateClapId,                   kPeepAnimationSequenceNormalClap };
        pag[PeepAnimationType::Disgust]                = { kPeepSpriteNormalStateDisgustId,                kPeepAnimationSequenceNormalDisgust };
        pag[PeepAnimationType::DrawPicture]            = { kPeepSpriteNormalStateDrawPictureId,            kPeepAnimationSequenceNormalDrawPicture };
        pag[PeepAnimationType::BeingWatched]           = { kPeepSpriteNormalStateBeingWatchedId,           kPeepAnimationSequenceNormalBeingWatched };
        pag[PeepAnimationType::WithdrawMoney]          = { kPeepSpriteNormalStateWithdrawMoneyId,          kPeepAnimationSequenceNormalWithdrawMoney };
        return pag;
    }();

    // Define animation sequences for Handyman sprites
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceHandymanNone          = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceHandymanWatchRide     = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceHandymanHanging       = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceHandymanStaffMower    = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t, 25> kPeepAnimationSequenceHandymanStaffSweep    = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
    static constexpr std::array<uint8_t, 65> kPeepAnimationSequenceHandymanDrowning      = { 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 4, 5, 6, 7 };
    static constexpr std::array<uint8_t, 18> kPeepAnimationSequenceHandymanStaffWatering = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t, 19> kPeepAnimationSequenceHandymanStaffEmptyBin = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

    // Define animation group for Handyman sequences
    static PeepAnimations kPeepAnimationsHandyman = []() {
        PeepAnimations pag;
        pag[PeepAnimationType::None]          = { kHandymanSpriteStateNoneId,          kPeepAnimationSequenceHandymanNone };
        pag[PeepAnimationType::WatchRide]     = { kHandymanSpriteStateWatchRideId,     kPeepAnimationSequenceHandymanWatchRide };
        pag[PeepAnimationType::Hanging]       = { kHandymanSpriteStateHangingId,       kPeepAnimationSequenceHandymanHanging };
        pag[PeepAnimationType::StaffMower]    = { kHandymanSpriteStateStaffMowerId,    kPeepAnimationSequenceHandymanStaffMower };
        pag[PeepAnimationType::StaffSweep]    = { kHandymanSpriteStateStaffSweepId,    kPeepAnimationSequenceHandymanStaffSweep };
        pag[PeepAnimationType::Drowning]      = { kHandymanSpriteStateDrowningId,      kPeepAnimationSequenceHandymanDrowning };
        pag[PeepAnimationType::StaffWatering] = { kHandymanSpriteStateStaffWateringId, kPeepAnimationSequenceHandymanStaffWatering };
        pag[PeepAnimationType::StaffEmptyBin] = { kHandymanSpriteStateStaffEmptyBinId, kPeepAnimationSequenceHandymanStaffEmptyBin };
        return pag;
    }();

    // Define animation sequences for Mechanic sprites
    static constexpr std::array<uint8_t,   6> kPeepAnimationSequenceMechanicNone             = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,   1> kPeepAnimationSequenceMechanicWatchRide        = { 0 };
    static constexpr std::array<uint8_t,  12> kPeepAnimationSequenceMechanicHanging          = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  65> kPeepAnimationSequenceMechanicDrowning         = { 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 4, 5, 6, 7 };
    static constexpr std::array<uint8_t,  85> kPeepAnimationSequenceMechanicStaffAnswerCall  = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t,  59> kPeepAnimationSequenceMechanicStaffAnswerCall2 = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t,  43> kPeepAnimationSequenceMechanicStaffCheckBoard  = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t,  53> kPeepAnimationSequenceMechanicStaffFix         = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t,  99> kPeepAnimationSequenceMechanicStaffFix2        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
    static constexpr std::array<uint8_t,  61> kPeepAnimationSequenceMechanicStaffFixGround   = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t, 110> kPeepAnimationSequenceMechanicStaffFix3        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };

    // Define animation group for Mechanic sequences
    static PeepAnimations kPeepAnimationsMechanic = []() {
        PeepAnimations pag;
        pag[PeepAnimationType::None]             = { kMechanicSpriteStateNoneId,            kPeepAnimationSequenceMechanicNone };
        pag[PeepAnimationType::WatchRide]        = { kMechanicSpriteStateWatchRideId,       kPeepAnimationSequenceMechanicWatchRide };
        pag[PeepAnimationType::Hanging]          = { kMechanicSpriteStateHangingId,         kPeepAnimationSequenceMechanicHanging };
        pag[PeepAnimationType::Drowning]         = { kMechanicSpriteStateDrowningId,        kPeepAnimationSequenceMechanicDrowning };
        pag[PeepAnimationType::StaffAnswerCall]  = { kMechanicSpriteStateStaffAnswerCallId, kPeepAnimationSequenceMechanicStaffAnswerCall };
        pag[PeepAnimationType::StaffAnswerCall2] = { kMechanicSpriteStateStaffAnswerCallId, kPeepAnimationSequenceMechanicStaffAnswerCall2 };
        pag[PeepAnimationType::StaffCheckBoard]  = { kMechanicSpriteStateStaffCheckBoardId, kPeepAnimationSequenceMechanicStaffCheckBoard };
        pag[PeepAnimationType::StaffFix]         = { kMechanicSpriteStateStaffFixId,        kPeepAnimationSequenceMechanicStaffFix };
        pag[PeepAnimationType::StaffFix2]        = { kMechanicSpriteStateStaffFixId,        kPeepAnimationSequenceMechanicStaffFix2 };
        pag[PeepAnimationType::StaffFixGround]   = { kMechanicSpriteStateStaffFixGroundId,  kPeepAnimationSequenceMechanicStaffFixGround };
        pag[PeepAnimationType::StaffFix3]        = { kMechanicSpriteStateStaffFixId,        kPeepAnimationSequenceMechanicStaffFix3 };
        return pag;
    }();

    // Define animation sequences for Security sprites
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceSecurityNone      = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceSecurityWatchRide = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceSecurityHanging   = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t, 64> kPeepAnimationSequenceSecurityDrowning  = { 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 4, 5, 6 };

    // Define animation group for Security sequences
    static PeepAnimations kPeepAnimationsSecurity = []() {
        PeepAnimations pag;
        pag[PeepAnimationType::None]      = { kSecuritySpriteStateNoneId,      kPeepAnimationSequenceSecurityNone };
        pag[PeepAnimationType::WatchRide] = { kSecuritySpriteStateWatchRideId, kPeepAnimationSequenceSecurityWatchRide };
        pag[PeepAnimationType::Hanging]   = { kSecuritySpriteStateHangingId,   kPeepAnimationSequenceSecurityHanging };
        pag[PeepAnimationType::Drowning]  = { kSecuritySpriteStateDrowningId,  kPeepAnimationSequenceSecurityDrowning };
        return pag;
    }();

    // Define animation sequences for EntertainerPanda sprites
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceEntertainerPandaNone      = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceEntertainerPandaWatchRide = { 0 };
    static constexpr std::array<uint8_t, 23> kPeepAnimationSequenceEntertainerPandaEatFood   = { 0, 1, 2, 3, 4, 5, 4, 3, 4, 5, 4, 3, 4, 5, 4, 3, 4, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceEntertainerPandaHanging   = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t, 64> kPeepAnimationSequenceEntertainerPandaDrowning  = { 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 4, 5, 6 };
    static constexpr std::array<uint8_t, 64> kPeepAnimationSequenceEntertainerPandaJoy       = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    static constexpr std::array<uint8_t, 23> kPeepAnimationSequenceEntertainerPandaWave2     = { 0, 1, 2, 3, 4, 5, 4, 3, 4, 5, 4, 3, 4, 5, 4, 3, 4, 5, 4, 3, 2, 1, 0 };

    // Define animation group for EntertainerPanda sequences
    static PeepAnimations kPeepAnimationsEntertainerPanda = []() {
        PeepAnimations pag;
        pag[PeepAnimationType::None]      = { kEntertainerSpritePandaStateNoneId,      kPeepAnimationSequenceEntertainerPandaNone };
        pag[PeepAnimationType::WatchRide] = { kEntertainerSpritePandaStateWatchRideId, kPeepAnimationSequenceEntertainerPandaWatchRide };
        pag[PeepAnimationType::EatFood]   = { kEntertainerSpritePandaStateWaveId,      kPeepAnimationSequenceEntertainerPandaEatFood };
        pag[PeepAnimationType::Hanging]   = { kEntertainerSpritePandaStateHangingId,   kPeepAnimationSequenceEntertainerPandaHanging };
        pag[PeepAnimationType::Drowning]  = { kEntertainerSpritePandaStateDrowningId,  kPeepAnimationSequenceEntertainerPandaDrowning };
        pag[PeepAnimationType::Joy]       = { kEntertainerSpritePandaStateJoyId,       kPeepAnimationSequenceEntertainerPandaJoy };
        pag[PeepAnimationType::Wave2]     = { kEntertainerSpritePandaStateWaveId,      kPeepAnimationSequenceEntertainerPandaWave2 };
        return pag;
    }();

    // Define animation sequences for EntertainerTiger sprites
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceEntertainerTigerNone      = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceEntertainerTigerWatchRide = { 0 };
    static constexpr std::array<uint8_t, 23> kPeepAnimationSequenceEntertainerTigerEatFood   = { 0, 1, 2, 3, 4, 5, 4, 3, 4, 5, 4, 3, 4, 5, 4, 3, 4, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceEntertainerTigerHanging   = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t, 64> kPeepAnimationSequenceEntertainerTigerDrowning  = { 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 4, 5, 6 };
    static constexpr std::array<uint8_t, 78> kPeepAnimationSequenceEntertainerTigerJoy       = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 25, 25, 25, 25 };
    static constexpr std::array<uint8_t, 23> kPeepAnimationSequenceEntertainerTigerWave2     = { 0, 1, 2, 3, 4, 5, 4, 3, 4, 5, 4, 3, 4, 5, 4, 3, 4, 5, 4, 3, 2, 1, 0 };

    // Define animation group for EntertainerTiger sequences
    static PeepAnimations kPeepAnimationsEntertainerTiger = []() {
        PeepAnimations pag;
        pag[PeepAnimationType::None]      = { kEntertainerSpriteTigerStateNoneId,      kPeepAnimationSequenceEntertainerTigerNone };
        pag[PeepAnimationType::WatchRide] = { kEntertainerSpriteTigerStateWatchRideId, kPeepAnimationSequenceEntertainerTigerWatchRide };
        pag[PeepAnimationType::EatFood]   = { kEntertainerSpriteTigerStateWaveId,      kPeepAnimationSequenceEntertainerTigerEatFood };
        pag[PeepAnimationType::Hanging]   = { kEntertainerSpriteTigerStateHangingId,   kPeepAnimationSequenceEntertainerTigerHanging };
        pag[PeepAnimationType::Drowning]  = { kEntertainerSpriteTigerStateDrowningId,  kPeepAnimationSequenceEntertainerTigerDrowning };
        pag[PeepAnimationType::Joy]       = { kEntertainerSpriteTigerStateJoyId,       kPeepAnimationSequenceEntertainerTigerJoy };
        pag[PeepAnimationType::Wave2]     = { kEntertainerSpriteTigerStateWaveId,      kPeepAnimationSequenceEntertainerTigerWave2 };
        return pag;
    }();

    // Define animation sequences for EntertainerElephant sprites
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceEntertainerElephantNone      = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceEntertainerElephantWatchRide = { 0 };
    static constexpr std::array<uint8_t, 25> kPeepAnimationSequenceEntertainerElephantEatFood   = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 3, 4, 5, 6, 5, 4, 3, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceEntertainerElephantHanging   = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t, 67> kPeepAnimationSequenceEntertainerElephantDrowning  = { 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 4, 5, 6, 7, 8, 9 };
    static constexpr std::array<uint8_t, 79> kPeepAnimationSequenceEntertainerElephantJoy       = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27 };
    static constexpr std::array<uint8_t, 25> kPeepAnimationSequenceEntertainerElephantWave2     = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 3, 4, 5, 6, 5, 4, 3, 4, 5, 6, 5, 4, 3, 2, 1, 0 };

    // Define animation group for EntertainerElephant sequences
    static PeepAnimations kPeepAnimationsEntertainerElephant = []() {
        PeepAnimations pag;
        pag[PeepAnimationType::None]      = { kEntertainerSpriteElephantStateNoneId,      kPeepAnimationSequenceEntertainerElephantNone };
        pag[PeepAnimationType::WatchRide] = { kEntertainerSpriteElephantStateWatchRideId, kPeepAnimationSequenceEntertainerElephantWatchRide };
        pag[PeepAnimationType::EatFood]   = { kEntertainerSpriteElephantStateWaveId,      kPeepAnimationSequenceEntertainerElephantEatFood };
        pag[PeepAnimationType::Hanging]   = { kEntertainerSpriteElephantStateHangingId,   kPeepAnimationSequenceEntertainerElephantHanging };
        pag[PeepAnimationType::Drowning]  = { kEntertainerSpriteElephantStateDrowningId,  kPeepAnimationSequenceEntertainerElephantDrowning };
        pag[PeepAnimationType::Joy]       = { kEntertainerSpriteElephantStateJoyId,       kPeepAnimationSequenceEntertainerElephantJoy };
        pag[PeepAnimationType::Wave2]     = { kEntertainerSpriteElephantStateWaveId,      kPeepAnimationSequenceEntertainerElephantWave2 };
        return pag;
    }();

    // Define animation sequences for EntertainerRoman sprites
    static constexpr std::array<uint8_t,   6> kPeepAnimationSequenceEntertainerRomanNone      = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,   1> kPeepAnimationSequenceEntertainerRomanWatchRide = { 0 };
    static constexpr std::array<uint8_t,  21> kPeepAnimationSequenceEntertainerRomanEatFood   = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 6, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t,  12> kPeepAnimationSequenceEntertainerRomanHanging   = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  65> kPeepAnimationSequenceEntertainerRomanDrowning  = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
    static constexpr std::array<uint8_t, 144> kPeepAnimationSequenceEntertainerRomanJoy       = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 14, 13, 12, 11, 10, 11, 12, 13, 14, 15, 14, 13, 12, 11, 10, 11, 12, 13, 14, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 4, 4, 4, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 14, 13, 12, 11, 10, 11, 12, 13, 14, 15, 14, 13, 12, 11, 10, 11, 12, 13, 14, 15, 14, 13, 12, 11, 10, 11, 12, 13, 14, 15, 16, 16, 16, 16, 16, 17, 18, 19, 20, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 24, 25, 26, 27, 28, 28, 28, 29, 30, 31, 32, 32, 32, 32, 32, 33, 34, 35, 36, 37, 38 };
    static constexpr std::array<uint8_t,  21> kPeepAnimationSequenceEntertainerRomanWave2     = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 6, 5, 4, 3, 2, 1, 0 };

    // Define animation group for EntertainerRoman sequences
    static PeepAnimations kPeepAnimationsEntertainerRoman = []() {
        PeepAnimations pag;
        pag[PeepAnimationType::None]      = { kEntertainerSpriteRomanStateNoneId,      kPeepAnimationSequenceEntertainerRomanNone };
        pag[PeepAnimationType::WatchRide] = { kEntertainerSpriteRomanStateWatchRideId, kPeepAnimationSequenceEntertainerRomanWatchRide };
        pag[PeepAnimationType::EatFood]   = { kEntertainerSpriteRomanStateWaveId,      kPeepAnimationSequenceEntertainerRomanEatFood };
        pag[PeepAnimationType::Hanging]   = { kEntertainerSpriteRomanStateHangingId,   kPeepAnimationSequenceEntertainerRomanHanging };
        pag[PeepAnimationType::Drowning]  = { kEntertainerSpriteRomanStateDrowningId,  kPeepAnimationSequenceEntertainerRomanDrowning };
        pag[PeepAnimationType::Joy]       = { kEntertainerSpriteRomanStateJoyId,       kPeepAnimationSequenceEntertainerRomanJoy };
        pag[PeepAnimationType::Wave2]     = { kEntertainerSpriteRomanStateWaveId,      kPeepAnimationSequenceEntertainerRomanWave2 };
        return pag;
    }();

    // Define animation sequences for EntertainerGorilla sprites
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceEntertainerGorillaNone      = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceEntertainerGorillaWatchRide = { 0 };
    static constexpr std::array<uint8_t, 39> kPeepAnimationSequenceEntertainerGorillaEatFood   = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 9, 8, 7, 8, 9, 10, 9, 8, 7, 8, 9, 10, 9, 8, 7, 8, 9, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceEntertainerGorillaHanging   = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t, 66> kPeepAnimationSequenceEntertainerGorillaDrowning  = { 0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t, 39> kPeepAnimationSequenceEntertainerGorillaJoy       = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 9, 8, 7, 8, 9, 10, 9, 8, 7, 8, 9, 10, 9, 8, 7, 8, 9, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t, 39> kPeepAnimationSequenceEntertainerGorillaWave2     = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 9, 8, 7, 8, 9, 10, 9, 8, 7, 8, 9, 10, 9, 8, 7, 8, 9, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

    // Define animation group for EntertainerGorilla sequences
    static PeepAnimations kPeepAnimationsEntertainerGorilla = []() {
        PeepAnimations pag;
        pag[PeepAnimationType::None]      = { kEntertainerSpriteGorillaStateNoneId,      kPeepAnimationSequenceEntertainerGorillaNone };
        pag[PeepAnimationType::WatchRide] = { kEntertainerSpriteGorillaStateWatchRideId, kPeepAnimationSequenceEntertainerGorillaWatchRide };
        pag[PeepAnimationType::EatFood]   = { kEntertainerSpriteGorillaStateWaveId,      kPeepAnimationSequenceEntertainerGorillaEatFood };
        pag[PeepAnimationType::Hanging]   = { kEntertainerSpriteGorillaStateHangingId,   kPeepAnimationSequenceEntertainerGorillaHanging };
        pag[PeepAnimationType::Drowning]  = { kEntertainerSpriteGorillaStateDrowningId,  kPeepAnimationSequenceEntertainerGorillaDrowning };
        pag[PeepAnimationType::Joy]       = { kEntertainerSpriteGorillaStateWaveId,      kPeepAnimationSequenceEntertainerGorillaJoy };
        pag[PeepAnimationType::Wave2]     = { kEntertainerSpriteGorillaStateWaveId,      kPeepAnimationSequenceEntertainerGorillaWave2 };
        return pag;
    }();

    // Define animation sequences for EntertainerSnowman sprites
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceEntertainerSnowmanNone      = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceEntertainerSnowmanWatchRide = { 0 };
    static constexpr std::array<uint8_t, 65> kPeepAnimationSequenceEntertainerSnowmanEatFood   = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 31, 31, 31, 32, 33 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceEntertainerSnowmanHanging   = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t, 62> kPeepAnimationSequenceEntertainerSnowmanDrowning  = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7, 8, 9 };
    static constexpr std::array<uint8_t, 65> kPeepAnimationSequenceEntertainerSnowmanJoy       = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 31, 31, 31, 32, 33 };
    static constexpr std::array<uint8_t, 65> kPeepAnimationSequenceEntertainerSnowmanWave2     = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 31, 31, 31, 32, 33 };

    // Define animation group for EntertainerSnowman sequences
    static PeepAnimations kPeepAnimationsEntertainerSnowman = []() {
        PeepAnimations pag;
        pag[PeepAnimationType::None]      = { kEntertainerSpriteSnowmanStateNoneId,      kPeepAnimationSequenceEntertainerSnowmanNone };
        pag[PeepAnimationType::WatchRide] = { kEntertainerSpriteSnowmanStateWatchRideId, kPeepAnimationSequenceEntertainerSnowmanWatchRide };
        pag[PeepAnimationType::EatFood]   = { kEntertainerSpriteSnowmanStateWaveId,      kPeepAnimationSequenceEntertainerSnowmanEatFood };
        pag[PeepAnimationType::Hanging]   = { kEntertainerSpriteSnowmanStateHangingId,   kPeepAnimationSequenceEntertainerSnowmanHanging };
        pag[PeepAnimationType::Drowning]  = { kEntertainerSpriteSnowmanStateDrowningId,  kPeepAnimationSequenceEntertainerSnowmanDrowning };
        pag[PeepAnimationType::Joy]       = { kEntertainerSpriteSnowmanStateWaveId,      kPeepAnimationSequenceEntertainerSnowmanJoy };
        pag[PeepAnimationType::Wave2]     = { kEntertainerSpriteSnowmanStateWaveId,      kPeepAnimationSequenceEntertainerSnowmanWave2 };
        return pag;
    }();

    // Define animation sequences for EntertainerKnight sprites
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceEntertainerKnightNone      = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceEntertainerKnightWatchRide = { 0 };
    static constexpr std::array<uint8_t, 47> kPeepAnimationSequenceEntertainerKnightEatFood   = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 7, 8, 9, 10, 11, 12, 12, 12, 12, 12, 13, 14, 15, 16, 17, 17, 17, 17, 17, 18, 19, 20, 21, 22, 23, 23, 23, 23, 23, 24, 25, 26, 27 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceEntertainerKnightHanging   = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t, 66> kPeepAnimationSequenceEntertainerKnightDrowning  = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
    static constexpr std::array<uint8_t, 47> kPeepAnimationSequenceEntertainerKnightJoy       = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 7, 8, 9, 10, 11, 12, 12, 12, 12, 12, 13, 14, 15, 16, 17, 17, 17, 17, 17, 18, 19, 20, 21, 22, 23, 23, 23, 23, 23, 24, 25, 26, 27 };
    static constexpr std::array<uint8_t, 47> kPeepAnimationSequenceEntertainerKnightWave2     = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 7, 8, 9, 10, 11, 12, 12, 12, 12, 12, 13, 14, 15, 16, 17, 17, 17, 17, 17, 18, 19, 20, 21, 22, 23, 23, 23, 23, 23, 24, 25, 26, 27 };

    // Define animation group for EntertainerKnight sequences
    static PeepAnimations kPeepAnimationsEntertainerKnight = []() {
        PeepAnimations pag;
        pag[PeepAnimationType::None]      = { kEntertainerSpriteKnightStateNoneId,      kPeepAnimationSequenceEntertainerKnightNone };
        pag[PeepAnimationType::WatchRide] = { kEntertainerSpriteKnightStateWatchRideId, kPeepAnimationSequenceEntertainerKnightWatchRide };
        pag[PeepAnimationType::EatFood]   = { kEntertainerSpriteKnightStateWaveId,      kPeepAnimationSequenceEntertainerKnightEatFood };
        pag[PeepAnimationType::Hanging]   = { kEntertainerSpriteKnightStateHangingId,   kPeepAnimationSequenceEntertainerKnightHanging };
        pag[PeepAnimationType::Drowning]  = { kEntertainerSpriteKnightStateDrowningId,  kPeepAnimationSequenceEntertainerKnightDrowning };
        pag[PeepAnimationType::Joy]       = { kEntertainerSpriteKnightStateWaveId,      kPeepAnimationSequenceEntertainerKnightJoy };
        pag[PeepAnimationType::Wave2]     = { kEntertainerSpriteKnightStateWaveId,      kPeepAnimationSequenceEntertainerKnightWave2 };
        return pag;
    }();

    // Define animation sequences for EntertainerAstronaut sprites
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceEntertainerAstronautNone      = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceEntertainerAstronautWatchRide = { 0 };
    static constexpr std::array<uint8_t, 19> kPeepAnimationSequenceEntertainerAstronautEatFood   = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 7, 8, 9, 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceEntertainerAstronautHanging   = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t, 63> kPeepAnimationSequenceEntertainerAstronautDrowning  = { 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
    static constexpr std::array<uint8_t, 19> kPeepAnimationSequenceEntertainerAstronautJoy       = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 7, 8, 9, 0 };
    static constexpr std::array<uint8_t, 19> kPeepAnimationSequenceEntertainerAstronautWave2     = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 7, 8, 9, 0 };

    // Define animation group for EntertainerAstronaut sequences
    static PeepAnimations kPeepAnimationsEntertainerAstronaut = []() {
        PeepAnimations pag;
        pag[PeepAnimationType::None]      = { kEntertainerSpriteAstronautStateNoneId,      kPeepAnimationSequenceEntertainerAstronautNone };
        pag[PeepAnimationType::WatchRide] = { kEntertainerSpriteAstronautStateWatchRideId, kPeepAnimationSequenceEntertainerAstronautWatchRide };
        pag[PeepAnimationType::EatFood]   = { kEntertainerSpriteAstronautStateWaveId,      kPeepAnimationSequenceEntertainerAstronautEatFood };
        pag[PeepAnimationType::Hanging]   = { kEntertainerSpriteAstronautStateHangingId,   kPeepAnimationSequenceEntertainerAstronautHanging };
        pag[PeepAnimationType::Drowning]  = { kEntertainerSpriteAstronautStateDrowningId,  kPeepAnimationSequenceEntertainerAstronautDrowning };
        pag[PeepAnimationType::Joy]       = { kEntertainerSpriteAstronautStateWaveId,      kPeepAnimationSequenceEntertainerAstronautJoy };
        pag[PeepAnimationType::Wave2]     = { kEntertainerSpriteAstronautStateWaveId,      kPeepAnimationSequenceEntertainerAstronautWave2 };
        return pag;
    }();

    // Define animation sequences for EntertainerBandit sprites
    static constexpr std::array<uint8_t,   6> kPeepAnimationSequenceEntertainerBanditNone      = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,   1> kPeepAnimationSequenceEntertainerBanditWatchRide = { 0 };
    static constexpr std::array<uint8_t, 111> kPeepAnimationSequenceEntertainerBanditEatFood   = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 8, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 12, 13, 13, 13, 13, 13, 14, 15, 16, 17, 17, 17, 17, 17, 18, 19, 20, 21, 22, 21, 20, 19, 20, 21, 22, 21, 20, 19, 20, 21, 22, 21, 20, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41 };
    static constexpr std::array<uint8_t,  12> kPeepAnimationSequenceEntertainerBanditHanging   = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  64> kPeepAnimationSequenceEntertainerBanditDrowning  = { 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    static constexpr std::array<uint8_t, 111> kPeepAnimationSequenceEntertainerBanditJoy       = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 8, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 12, 13, 13, 13, 13, 13, 14, 15, 16, 17, 17, 17, 17, 17, 18, 19, 20, 21, 22, 21, 20, 19, 20, 21, 22, 21, 20, 19, 20, 21, 22, 21, 20, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41 };
    static constexpr std::array<uint8_t, 111> kPeepAnimationSequenceEntertainerBanditWave2     = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 8, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 12, 13, 13, 13, 13, 13, 14, 15, 16, 17, 17, 17, 17, 17, 18, 19, 20, 21, 22, 21, 20, 19, 20, 21, 22, 21, 20, 19, 20, 21, 22, 21, 20, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41 };

    // Define animation group for EntertainerBandit sequences
    static PeepAnimations kPeepAnimationsEntertainerBandit = []() {
        PeepAnimations pag;
        pag[PeepAnimationType::None]      = { kEntertainerSpriteBanditStateNoneId,      kPeepAnimationSequenceEntertainerBanditNone };
        pag[PeepAnimationType::WatchRide] = { kEntertainerSpriteBanditStateWatchRideId, kPeepAnimationSequenceEntertainerBanditWatchRide };
        pag[PeepAnimationType::EatFood]   = { kEntertainerSpriteBanditStateWaveId,      kPeepAnimationSequenceEntertainerBanditEatFood };
        pag[PeepAnimationType::Hanging]   = { kEntertainerSpriteBanditStateHangingId,   kPeepAnimationSequenceEntertainerBanditHanging };
        pag[PeepAnimationType::Drowning]  = { kEntertainerSpriteBanditStateDrowningId,  kPeepAnimationSequenceEntertainerBanditDrowning };
        pag[PeepAnimationType::Joy]       = { kEntertainerSpriteBanditStateWaveId,      kPeepAnimationSequenceEntertainerBanditJoy };
        pag[PeepAnimationType::Wave2]     = { kEntertainerSpriteBanditStateWaveId,      kPeepAnimationSequenceEntertainerBanditWave2 };
        return pag;
    }();

    // Define animation sequences for EntertainerSheriff sprites
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceEntertainerSheriffNone      = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceEntertainerSheriffWatchRide = { 0 };
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequenceEntertainerSheriffEatFood   = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 8, 9, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceEntertainerSheriffHanging   = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t, 64> kPeepAnimationSequenceEntertainerSheriffDrowning  = { 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequenceEntertainerSheriffJoy       = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 8, 9, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequenceEntertainerSheriffWave2     = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 8, 9, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

    // Define animation group for EntertainerSheriff sequences
    static PeepAnimations kPeepAnimationsEntertainerSheriff = []() {
        PeepAnimations pag;
        pag[PeepAnimationType::None]      = { kEntertainerSpriteSheriffStateNoneId,      kPeepAnimationSequenceEntertainerSheriffNone };
        pag[PeepAnimationType::WatchRide] = { kEntertainerSpriteSheriffStateWatchRideId, kPeepAnimationSequenceEntertainerSheriffWatchRide };
        pag[PeepAnimationType::EatFood]   = { kEntertainerSpriteSheriffStateWaveId,      kPeepAnimationSequenceEntertainerSheriffEatFood };
        pag[PeepAnimationType::Hanging]   = { kEntertainerSpriteSheriffStateHangingId,   kPeepAnimationSequenceEntertainerSheriffHanging };
        pag[PeepAnimationType::Drowning]  = { kEntertainerSpriteSheriffStateDrowningId,  kPeepAnimationSequenceEntertainerSheriffDrowning };
        pag[PeepAnimationType::Joy]       = { kEntertainerSpriteSheriffStateWaveId,      kPeepAnimationSequenceEntertainerSheriffJoy };
        pag[PeepAnimationType::Wave2]     = { kEntertainerSpriteSheriffStateWaveId,      kPeepAnimationSequenceEntertainerSheriffWave2 };
        return pag;
    }();

    // Define animation sequences for EntertainerPirate sprites
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceEntertainerPirateNone      = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceEntertainerPirateWatchRide = { 0 };
    static constexpr std::array<uint8_t, 67> kPeepAnimationSequenceEntertainerPirateEatFood   = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 9, 10, 11, 12, 13, 14, 15, 16, 9, 10, 11, 12, 13, 14, 15, 16, 18, 19, 20, 21, 21, 21, 21, 21, 21, 22, 23, 23, 23, 23, 24, 25, 26, 27, 27, 27, 27, 28, 29, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 31 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceEntertainerPirateHanging   = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t, 72> kPeepAnimationSequenceEntertainerPirateDrowning  = { 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    static constexpr std::array<uint8_t, 67> kPeepAnimationSequenceEntertainerPirateJoy       = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 9, 10, 11, 12, 13, 14, 15, 16, 9, 10, 11, 12, 13, 14, 15, 16, 18, 19, 20, 21, 21, 21, 21, 21, 21, 22, 23, 23, 23, 23, 24, 25, 26, 27, 27, 27, 27, 28, 29, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 31 };
    static constexpr std::array<uint8_t, 67> kPeepAnimationSequenceEntertainerPirateWave2     = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 9, 10, 11, 12, 13, 14, 15, 16, 9, 10, 11, 12, 13, 14, 15, 16, 18, 19, 20, 21, 21, 21, 21, 21, 21, 22, 23, 23, 23, 23, 24, 25, 26, 27, 27, 27, 27, 28, 29, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 31 };

    // Define animation group for EntertainerPirate sequences
    static PeepAnimations kPeepAnimationsEntertainerPirate = []() {
        PeepAnimations pag;
        pag[PeepAnimationType::None]      = { kEntertainerSpritePirateStateNoneId,      kPeepAnimationSequenceEntertainerPirateNone };
        pag[PeepAnimationType::WatchRide] = { kEntertainerSpritePirateStateWatchRideId, kPeepAnimationSequenceEntertainerPirateWatchRide };
        pag[PeepAnimationType::EatFood]   = { kEntertainerSpritePirateStateWaveId,      kPeepAnimationSequenceEntertainerPirateEatFood };
        pag[PeepAnimationType::Hanging]   = { kEntertainerSpritePirateStateHangingId,   kPeepAnimationSequenceEntertainerPirateHanging };
        pag[PeepAnimationType::Drowning]  = { kEntertainerSpritePirateStateDrowningId,  kPeepAnimationSequenceEntertainerPirateDrowning };
        pag[PeepAnimationType::Joy]       = { kEntertainerSpritePirateStateWaveId,      kPeepAnimationSequenceEntertainerPirateJoy };
        pag[PeepAnimationType::Wave2]     = { kEntertainerSpritePirateStateWaveId,      kPeepAnimationSequenceEntertainerPirateWave2 };
        return pag;
    }();

    // Define animation sequences for IceCream sprites
    static constexpr std::array<uint8_t, 42> kPeepAnimationSequenceIceCreamNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceIceCreamWatchRide      = { 0 };
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceIceCreamEatFood        = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceIceCreamSittingIdle    = { 0 };
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceIceCreamSittingEatFood = { 0, 1, 2, 3, 4, 5 };

    // Define animation group for IceCream sequences
    static PeepAnimations kPeepAnimationsIceCream = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteIceCreamStateNoneId,           kPeepAnimationSequenceIceCreamNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteIceCreamStateWatchRideId,      kPeepAnimationSequenceIceCreamWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteIceCreamStateEatFoodId,        kPeepAnimationSequenceIceCreamEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteIceCreamStateSittingIdleId,    kPeepAnimationSequenceIceCreamSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteIceCreamStateSittingEatFoodId, kPeepAnimationSequenceIceCreamSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Chips sprites
    static constexpr std::array<uint8_t, 36> kPeepAnimationSequenceChipsNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceChipsWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceChipsEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceChipsSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceChipsSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for Chips sequences
    static PeepAnimations kPeepAnimationsChips = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteChipsStateNoneId,           kPeepAnimationSequenceChipsNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteChipsStateWatchRideId,      kPeepAnimationSequenceChipsWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteChipsStateEatFoodId,        kPeepAnimationSequenceChipsEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteChipsStateSittingIdleId,    kPeepAnimationSequenceChipsSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteChipsStateSittingEatFoodId, kPeepAnimationSequenceChipsSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Burger sprites
    static constexpr std::array<uint8_t, 36> kPeepAnimationSequenceBurgerNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceBurgerWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceBurgerEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceBurgerSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceBurgerSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for Burger sequences
    static PeepAnimations kPeepAnimationsBurger = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteBurgerStateNoneId,           kPeepAnimationSequenceBurgerNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteBurgerStateWatchRideId,      kPeepAnimationSequenceBurgerWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteBurgerStateEatFoodId,        kPeepAnimationSequenceBurgerEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteBurgerStateSittingIdleId,    kPeepAnimationSequenceBurgerSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteBurgerStateSittingEatFoodId, kPeepAnimationSequenceBurgerSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Drink sprites
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequenceDrinkNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceDrinkWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceDrinkEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceDrinkSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceDrinkSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for Drink sequences
    static PeepAnimations kPeepAnimationsDrink = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteDrinkStateNoneId,           kPeepAnimationSequenceDrinkNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteDrinkStateWatchRideId,      kPeepAnimationSequenceDrinkWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteDrinkStateEatFoodId,        kPeepAnimationSequenceDrinkEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteDrinkStateSittingIdleId,    kPeepAnimationSequenceDrinkSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteDrinkStateSittingEatFoodId, kPeepAnimationSequenceDrinkSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Balloon sprites
    static constexpr std::array<uint8_t, 6> kPeepAnimationSequenceBalloonNone        = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceBalloonWatchRide   = { 0 };
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceBalloonSittingIdle = { 0 };

    // Define animation group for Balloon sequences
    static PeepAnimations kPeepAnimationsBalloon = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::None]        = { kPeepSpriteBalloonStateNoneId,        kPeepAnimationSequenceBalloonNone };
        pag[PeepAnimationType::WatchRide]   = { kPeepSpriteBalloonStateWatchRideId,   kPeepAnimationSequenceBalloonWatchRide };
        pag[PeepAnimationType::SittingIdle] = { kPeepSpriteBalloonStateSittingIdleId, kPeepAnimationSequenceBalloonSittingIdle };
        return pag;
    }();

    // Define animation sequences for Candyfloss sprites
    static constexpr std::array<uint8_t, 36> kPeepAnimationSequenceCandyflossNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceCandyflossWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceCandyflossEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceCandyflossSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceCandyflossSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for Candyfloss sequences
    static PeepAnimations kPeepAnimationsCandyfloss = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteCandyflossStateNoneId,           kPeepAnimationSequenceCandyflossNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteCandyflossStateWatchRideId,      kPeepAnimationSequenceCandyflossWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteCandyflossStateEatFoodId,        kPeepAnimationSequenceCandyflossEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteCandyflossStateSittingIdleId,    kPeepAnimationSequenceCandyflossSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteCandyflossStateSittingEatFoodId, kPeepAnimationSequenceCandyflossSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Umbrella sprites
    static constexpr std::array<uint8_t, 6> kPeepAnimationSequenceUmbrellaNone        = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceUmbrellaWatchRide   = { 0 };
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceUmbrellaSittingIdle = { 0 };

    // Define animation group for Umbrella sequences
    static PeepAnimations kPeepAnimationsUmbrella = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::None]        = { kPeepSpriteUmbrellaStateNoneId,        kPeepAnimationSequenceUmbrellaNone };
        pag[PeepAnimationType::WatchRide]   = { kPeepSpriteUmbrellaStateWatchRideId,   kPeepAnimationSequenceUmbrellaWatchRide };
        pag[PeepAnimationType::SittingIdle] = { kPeepSpriteUmbrellaStateSittingIdleId, kPeepAnimationSequenceUmbrellaSittingIdle };
        return pag;
    }();

    // Define animation sequences for Pizza sprites
    static constexpr std::array<uint8_t, 30> kPeepAnimationSequencePizzaNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequencePizzaWatchRide      = { 0 };
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequencePizzaEatFood        = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequencePizzaSittingIdle    = { 0 };
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequencePizzaSittingEatFood = { 0, 1, 2, 3, 4, 5 };

    // Define animation group for Pizza sequences
    static PeepAnimations kPeepAnimationsPizza = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::None]           = { kPeepSpritePizzaStateNoneId,           kPeepAnimationSequencePizzaNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpritePizzaStateWatchRideId,      kPeepAnimationSequencePizzaWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpritePizzaStateEatFoodId,        kPeepAnimationSequencePizzaEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpritePizzaStateSittingIdleId,    kPeepAnimationSequencePizzaSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpritePizzaStateSittingEatFoodId, kPeepAnimationSequencePizzaSittingEatFood };
        return pag;
    }();

    // Define animation sequences for SecurityAlt sprites
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceSecurityAltNone      = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceSecurityAltWatchRide = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceSecurityAltHanging   = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t, 64> kPeepAnimationSequenceSecurityAltDrowning  = { 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 4, 5, 6 };

    // Define animation group for SecurityAlt sequences
    static PeepAnimations kPeepAnimationsSecurityAlt = []() {
        PeepAnimations pag;
        pag[PeepAnimationType::None]      = { kSecurityStaffSpriteAltStateNoneId, kPeepAnimationSequenceSecurityAltNone };
        pag[PeepAnimationType::WatchRide] = { kSecuritySpriteStateWatchRideId,    kPeepAnimationSequenceSecurityAltWatchRide };
        pag[PeepAnimationType::Hanging]   = { kSecuritySpriteStateHangingId,      kPeepAnimationSequenceSecurityAltHanging };
        pag[PeepAnimationType::Drowning]  = { kSecuritySpriteStateDrowningId,     kPeepAnimationSequenceSecurityAltDrowning };
        return pag;
    }();

    // Define animation sequences for Popcorn sprites
    static constexpr std::array<uint8_t, 36> kPeepAnimationSequencePopcornNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequencePopcornWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequencePopcornEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequencePopcornSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequencePopcornSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for Popcorn sequences
    static PeepAnimations kPeepAnimationsPopcorn = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::None]           = { kPeepSpritePopcornStateNoneId,           kPeepAnimationSequencePopcornNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpritePopcornStateWatchRideId,      kPeepAnimationSequencePopcornWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpritePopcornStateEatFoodId,        kPeepAnimationSequencePopcornEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpritePopcornStateSittingIdleId,    kPeepAnimationSequencePopcornSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpritePopcornStateSittingEatFoodId, kPeepAnimationSequencePopcornSittingEatFood };
        return pag;
    }();

    // Define animation sequences for ArmsCrossed sprites
    static constexpr std::array<uint8_t, 6> kPeepAnimationSequenceArmsCrossedNone = { 0, 1, 2, 3, 4, 5 };

    // Define animation group for ArmsCrossed sequences
    static PeepAnimations kPeepAnimationsArmsCrossed = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::None] = { kPeepSpriteArmsCrossedStateNoneId, kPeepAnimationSequenceArmsCrossedNone };
        return pag;
    }();

    // Define animation sequences for HeadDown sprites
    static constexpr std::array<uint8_t, 6> kPeepAnimationSequenceHeadDownNone = { 0, 1, 2, 3, 4, 5 };

    // Define animation group for HeadDown sequences
    static PeepAnimations kPeepAnimationsHeadDown = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::None] = { kPeepSpriteHeadDownStateNoneId, kPeepAnimationSequenceHeadDownNone };
        return pag;
    }();

    // Define animation sequences for Nauseous sprites
    static constexpr std::array<uint8_t, 6> kPeepAnimationSequenceNauseousNone = { 0, 1, 2, 3, 4, 5 };

    // Define animation group for Nauseous sequences
    static PeepAnimations kPeepAnimationsNauseous = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::None] = { kPeepSpriteNauseousStateNoneId, kPeepAnimationSequenceNauseousNone };
        return pag;
    }();

    // Define animation sequences for VeryNauseous sprites
    static constexpr std::array<uint8_t, 6> kPeepAnimationSequenceVeryNauseousNone        = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceVeryNauseousWatchRide   = { 0 };
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceVeryNauseousSittingIdle = { 0 };

    // Define animation group for VeryNauseous sequences
    static PeepAnimations kPeepAnimationsVeryNauseous = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::None]        = { kPeepSpriteVeryNauseousStateNoneId,        kPeepAnimationSequenceVeryNauseousNone };
        pag[PeepAnimationType::WatchRide]   = { kPeepSpriteVeryNauseousStateWatchRideId,   kPeepAnimationSequenceVeryNauseousWatchRide };
        pag[PeepAnimationType::SittingIdle] = { kPeepSpriteVeryNauseousStateSittingIdleId, kPeepAnimationSequenceVeryNauseousSittingIdle };
        return pag;
    }();

    // Define animation sequences for RequireToilet sprites
    static constexpr std::array<uint8_t, 6> kPeepAnimationSequenceRequireToiletNone = { 0, 1, 2, 3, 4, 5 };

    // Define animation group for RequireToilet sequences
    static PeepAnimations kPeepAnimationsRequireToilet = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::None] = { kPeepSpriteRequireToiletStateNoneId, kPeepAnimationSequenceRequireToiletNone };
        return pag;
    }();

    // Define animation sequences for Hat sprites
    static constexpr std::array<uint8_t, 6> kPeepAnimationSequenceHatNone        = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceHatWatchRide   = { 0 };
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceHatSittingIdle = { 0 };

    // Define animation group for Hat sequences
    static PeepAnimations kPeepAnimationsHat = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::None]        = { kPeepSpriteHatStateNoneId,        kPeepAnimationSequenceHatNone };
        pag[PeepAnimationType::WatchRide]   = { kPeepSpriteHatStateWatchRideId,   kPeepAnimationSequenceHatWatchRide };
        pag[PeepAnimationType::SittingIdle] = { kPeepSpriteHatStateSittingIdleId, kPeepAnimationSequenceHatSittingIdle };
        return pag;
    }();

    // Define animation sequences for HotDog sprites
    static constexpr std::array<uint8_t, 36> kPeepAnimationSequenceHotDogNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceHotDogWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceHotDogEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceHotDogSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceHotDogSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for HotDog sequences
    static PeepAnimations kPeepAnimationsHotDog = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteHotDogStateNoneId,           kPeepAnimationSequenceHotDogNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteHotDogStateWatchRideId,      kPeepAnimationSequenceHotDogWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteHotDogStateEatFoodId,        kPeepAnimationSequenceHotDogEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteHotDogStateSittingIdleId,    kPeepAnimationSequenceHotDogSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteHotDogStateSittingEatFoodId, kPeepAnimationSequenceHotDogSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Tentacle sprites
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequenceTentacleNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceTentacleWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 24> kPeepAnimationSequenceTentacleEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceTentacleSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 24> kPeepAnimationSequenceTentacleSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23 };

    // Define animation group for Tentacle sequences
    static PeepAnimations kPeepAnimationsTentacle = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteTentacleStateNoneId,           kPeepAnimationSequenceTentacleNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteTentacleStateWatchRideId,      kPeepAnimationSequenceTentacleWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteTentacleStateEatFoodId,        kPeepAnimationSequenceTentacleEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteTentacleStateSittingIdleId,    kPeepAnimationSequenceTentacleSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteTentacleStateSittingEatFoodId, kPeepAnimationSequenceTentacleSittingEatFood };
        return pag;
    }();

    // Define animation sequences for ToffeeApple sprites
    static constexpr std::array<uint8_t, 42> kPeepAnimationSequenceToffeeAppleNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceToffeeAppleWatchRide      = { 0 };
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceToffeeAppleEatFood        = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceToffeeAppleSittingIdle    = { 0 };
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceToffeeAppleSittingEatFood = { 0, 1, 2, 3, 4, 5 };

    // Define animation group for ToffeeApple sequences
    static PeepAnimations kPeepAnimationsToffeeApple = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteToffeeAppleStateNoneId,           kPeepAnimationSequenceToffeeAppleNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteToffeeAppleStateWatchRideId,      kPeepAnimationSequenceToffeeAppleWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteToffeeAppleStateEatFoodId,        kPeepAnimationSequenceToffeeAppleEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteToffeeAppleStateSittingIdleId,    kPeepAnimationSequenceToffeeAppleSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteToffeeAppleStateSittingEatFoodId, kPeepAnimationSequenceToffeeAppleSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Donut sprites
    static constexpr std::array<uint8_t, 36> kPeepAnimationSequenceDonutNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceDonutWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceDonutEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceDonutSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 11> kPeepAnimationSequenceDonutSittingEatFood = { 0, 1, 2, 3, 3, 3, 3, 3, 3, 4, 5 };

    // Define animation group for Donut sequences
    static PeepAnimations kPeepAnimationsDonut = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteDonutStateNoneId,           kPeepAnimationSequenceDonutNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteDonutStateWatchRideId,      kPeepAnimationSequenceDonutWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteDonutStateEatFoodId,        kPeepAnimationSequenceDonutEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteDonutStateSittingIdleId,    kPeepAnimationSequenceDonutSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteDonutStateSittingEatFoodId, kPeepAnimationSequenceDonutSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Coffee sprites
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequenceCoffeeNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceCoffeeWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceCoffeeEatFood        = { 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 2, 1 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceCoffeeSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceCoffeeSittingEatFood = { 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 2, 1 };

    // Define animation group for Coffee sequences
    static PeepAnimations kPeepAnimationsCoffee = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteCoffeeStateNoneId,        kPeepAnimationSequenceCoffeeNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteCoffeeStateWatchRideId,   kPeepAnimationSequenceCoffeeWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteCoffeeStateWatchRideId,   kPeepAnimationSequenceCoffeeEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteCoffeeStateSittingIdleId, kPeepAnimationSequenceCoffeeSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteCoffeeStateSittingIdleId, kPeepAnimationSequenceCoffeeSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Chicken sprites
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequenceChickenNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceChickenWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceChickenEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceChickenSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceChickenSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for Chicken sequences
    static PeepAnimations kPeepAnimationsChicken = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteChickenStateNoneId,        kPeepAnimationSequenceChickenNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteChickenStateWatchRideId,   kPeepAnimationSequenceChickenWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteChickenStateWatchRideId,   kPeepAnimationSequenceChickenEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteChickenStateSittingIdleId, kPeepAnimationSequenceChickenSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteChickenStateSittingIdleId, kPeepAnimationSequenceChickenSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Lemonade sprites
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequenceLemonadeNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceLemonadeWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceLemonadeEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceLemonadeSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceLemonadeSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for Lemonade sequences
    static PeepAnimations kPeepAnimationsLemonade = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteLemonadeStateNoneId,        kPeepAnimationSequenceLemonadeNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteLemonadeStateWatchRideId,   kPeepAnimationSequenceLemonadeWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteLemonadeStateWatchRideId,   kPeepAnimationSequenceLemonadeEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteLemonadeStateSittingIdleId, kPeepAnimationSequenceLemonadeSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteLemonadeStateSittingIdleId, kPeepAnimationSequenceLemonadeSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Watching sprites
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceWatchingWatchRide = { 0 };

    // Define animation group for Watching sequences
    static PeepAnimations kPeepAnimationsWatching = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::WatchRide] = { kPeepSpriteWatchingStateWatchRideId, kPeepAnimationSequenceWatchingWatchRide };
        return pag;
    }();

    // Define animation sequences for Pretzel sprites
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequencePretzelNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequencePretzelWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequencePretzelEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequencePretzelSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequencePretzelSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for Pretzel sequences
    static PeepAnimations kPeepAnimationsPretzel = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::None]           = { kPeepSpritePretzelStateNoneId,        kPeepAnimationSequencePretzelNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpritePretzelStateWatchRideId,   kPeepAnimationSequencePretzelWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpritePretzelStateWatchRideId,   kPeepAnimationSequencePretzelEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpritePretzelStateSittingIdleId, kPeepAnimationSequencePretzelSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpritePretzelStateSittingIdleId, kPeepAnimationSequencePretzelSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Sunglasses sprites
    static constexpr std::array<uint8_t, 6> kPeepAnimationSequenceSunglassesNone        = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceSunglassesWatchRide   = { 0 };
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceSunglassesSittingIdle = { 0 };

    // Define animation group for Sunglasses sequences
    static PeepAnimations kPeepAnimationsSunglasses = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::None]        = { kPeepSpriteSunglassesStateNoneId,        kPeepAnimationSequenceSunglassesNone };
        pag[PeepAnimationType::WatchRide]   = { kPeepSpriteSunglassesStateWatchRideId,   kPeepAnimationSequenceSunglassesWatchRide };
        pag[PeepAnimationType::SittingIdle] = { kPeepSpriteSunglassesStateSittingIdleId, kPeepAnimationSequenceSunglassesSittingIdle };
        return pag;
    }();

    // Define animation sequences for SuJongkwa sprites
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequenceSuJongkwaNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceSuJongkwaWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceSuJongkwaEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceSuJongkwaSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceSuJongkwaSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for SuJongkwa sequences
    static PeepAnimations kPeepAnimationsSuJongkwa = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteSuJongkwaStateNoneId,        kPeepAnimationSequenceSuJongkwaNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteSuJongkwaStateWatchRideId,   kPeepAnimationSequenceSuJongkwaWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteSuJongkwaStateWatchRideId,   kPeepAnimationSequenceSuJongkwaEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteSuJongkwaStateSittingIdleId, kPeepAnimationSequenceSuJongkwaSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteSuJongkwaStateSittingIdleId, kPeepAnimationSequenceSuJongkwaSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Juice sprites
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequenceJuiceNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceJuiceWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceJuiceEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceJuiceSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceJuiceSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for Juice sequences
    static PeepAnimations kPeepAnimationsJuice = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteJuiceStateNoneId,        kPeepAnimationSequenceJuiceNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteJuiceStateWatchRideId,   kPeepAnimationSequenceJuiceWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteJuiceStateWatchRideId,   kPeepAnimationSequenceJuiceEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteJuiceStateSittingIdleId, kPeepAnimationSequenceJuiceSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteJuiceStateSittingIdleId, kPeepAnimationSequenceJuiceSittingEatFood };
        return pag;
    }();

    // Define animation sequences for FunnelCake sprites
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequenceFunnelCakeNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceFunnelCakeWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceFunnelCakeEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceFunnelCakeSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceFunnelCakeSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for FunnelCake sequences
    static PeepAnimations kPeepAnimationsFunnelCake = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteFunnelCakeStateNoneId,        kPeepAnimationSequenceFunnelCakeNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteFunnelCakeStateWatchRideId,   kPeepAnimationSequenceFunnelCakeWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteFunnelCakeStateWatchRideId,   kPeepAnimationSequenceFunnelCakeEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteFunnelCakeStateSittingIdleId, kPeepAnimationSequenceFunnelCakeSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteFunnelCakeStateSittingIdleId, kPeepAnimationSequenceFunnelCakeSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Noodles sprites
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequenceNoodlesNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceNoodlesWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceNoodlesEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceNoodlesSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceNoodlesSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for Noodles sequences
    static PeepAnimations kPeepAnimationsNoodles = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteNoodlesStateNoneId,        kPeepAnimationSequenceNoodlesNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteNoodlesStateWatchRideId,   kPeepAnimationSequenceNoodlesWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteNoodlesStateWatchRideId,   kPeepAnimationSequenceNoodlesEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteNoodlesStateSittingIdleId, kPeepAnimationSequenceNoodlesSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteNoodlesStateSittingIdleId, kPeepAnimationSequenceNoodlesSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Sausage sprites
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequenceSausageNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceSausageWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceSausageEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceSausageSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceSausageSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for Sausage sequences
    static PeepAnimations kPeepAnimationsSausage = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteSausageStateNoneId,        kPeepAnimationSequenceSausageNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteSausageStateWatchRideId,   kPeepAnimationSequenceSausageWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteSausageStateWatchRideId,   kPeepAnimationSequenceSausageEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteSausageStateSittingIdleId, kPeepAnimationSequenceSausageSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteSausageStateSittingIdleId, kPeepAnimationSequenceSausageSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Soup sprites
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequenceSoupNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceSoupWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceSoupEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceSoupSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceSoupSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for Soup sequences
    static PeepAnimations kPeepAnimationsSoup = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteSoupStateNoneId,        kPeepAnimationSequenceSoupNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteSoupStateWatchRideId,   kPeepAnimationSequenceSoupWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteSoupStateWatchRideId,   kPeepAnimationSequenceSoupEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteSoupStateSittingIdleId, kPeepAnimationSequenceSoupSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteSoupStateSittingIdleId, kPeepAnimationSequenceSoupSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Sandwich sprites
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequenceSandwichNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceSandwichWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceSandwichEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceSandwichSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceSandwichSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for Sandwich sequences
    static PeepAnimations kPeepAnimationsSandwich = []() {
        PeepAnimations pag = kPeepAnimationsNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteSandwichStateNoneId,        kPeepAnimationSequenceSandwichNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteSandwichStateWatchRideId,   kPeepAnimationSequenceSandwichWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteSandwichStateWatchRideId,   kPeepAnimationSequenceSandwichEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteSandwichStateSittingIdleId, kPeepAnimationSequenceSandwichSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteSandwichStateSittingIdleId, kPeepAnimationSequenceSandwichSittingEatFood };
        return pag;
    }();

    // clang-format on
    // rct2: 0x00982708
    static std::array kPeepAnimationEntries = {
        kPeepAnimationsNormal,
        kPeepAnimationsHandyman,
        kPeepAnimationsMechanic,
        kPeepAnimationsSecurity,
        kPeepAnimationsEntertainerPanda,
        kPeepAnimationsEntertainerTiger,
        kPeepAnimationsEntertainerElephant,
        kPeepAnimationsEntertainerRoman,
        kPeepAnimationsEntertainerGorilla,
        kPeepAnimationsEntertainerSnowman,
        kPeepAnimationsEntertainerKnight,
        kPeepAnimationsEntertainerAstronaut,
        kPeepAnimationsEntertainerBandit,
        kPeepAnimationsEntertainerSheriff,
        kPeepAnimationsEntertainerPirate,
        kPeepAnimationsIceCream,
        kPeepAnimationsChips,
        kPeepAnimationsBurger,
        kPeepAnimationsDrink,
        kPeepAnimationsBalloon,
        kPeepAnimationsCandyfloss,
        kPeepAnimationsUmbrella,
        kPeepAnimationsPizza,
        kPeepAnimationsSecurityAlt,
        kPeepAnimationsPopcorn,
        kPeepAnimationsArmsCrossed,
        kPeepAnimationsHeadDown,
        kPeepAnimationsNauseous,
        kPeepAnimationsVeryNauseous,
        kPeepAnimationsRequireToilet,
        kPeepAnimationsHat,
        kPeepAnimationsHotDog,
        kPeepAnimationsTentacle,
        kPeepAnimationsToffeeApple,
        kPeepAnimationsDonut,
        kPeepAnimationsCoffee,
        kPeepAnimationsChicken,
        kPeepAnimationsLemonade,
        kPeepAnimationsWatching,
        kPeepAnimationsPretzel,
        kPeepAnimationsSunglasses,
        kPeepAnimationsSuJongkwa,
        kPeepAnimationsJuice,
        kPeepAnimationsFunnelCake,
        kPeepAnimationsNoodles,
        kPeepAnimationsSausage,
        kPeepAnimationsSoup,
        kPeepAnimationsSandwich,
    };

    const PeepAnimation& GetPeepAnimation(PeepAnimationGroup spriteType, PeepAnimationType actionAnimationGroup)
    {
        return kPeepAnimationEntries[EnumValue(spriteType)][actionAnimationGroup];
    }

    const SpriteBounds& GetSpriteBounds(PeepAnimationGroup spriteType, PeepAnimationType actionAnimationGroup)
    {
        return kPeepAnimationEntries[EnumValue(spriteType)][actionAnimationGroup].bounds;
    }

    void inferMaxPeepSpriteDimensions()
    {
        for (auto groupKey = EnumValue(PeepAnimationGroup::Normal); groupKey < EnumValue(PeepAnimationGroup::Count); groupKey++)
        {
            auto& group = kPeepAnimationEntries[groupKey];

            for (auto type = EnumValue(PeepAnimationType::None); type <= EnumValue(PeepAnimationType::WithdrawMoney); type++)
            {
                auto& anim = group[PeepAnimationType(type)];
                if (anim.frame_offsets.empty())
                    continue;

                anim.bounds = inferMaxAnimationDimensions(anim);
            }
        }
    }

} // namespace OpenRCT2
