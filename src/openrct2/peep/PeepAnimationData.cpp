/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "PeepAnimationData.h"

#include "PeepSpriteIds.h"

#include <array>

namespace OpenRCT2
{
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
    static constexpr std::array<uint8_t,  12> kPeepAnimationSequenceNormalUi                     = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
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
    static constexpr PeepAnimationGroup kPeepAnimationGroupNormal = []() {
        PeepAnimationGroup pag;
        pag[PeepAnimationType::None]                   = { kPeepSpriteNormalStateNoneId,                   {  8, 16,  5 }, kPeepAnimationSequenceNormalNone };
        pag[PeepAnimationType::CheckTime]              = { kPeepSpriteNormalStateCheckTimeId,              {  8, 16,  5 }, kPeepAnimationSequenceNormalCheckTime };
        pag[PeepAnimationType::WatchRide]              = { kPeepSpriteNormalStateWatchRideId,              {  8, 16,  5 }, kPeepAnimationSequenceNormalWatchRide };
        pag[PeepAnimationType::EatFood]                = { kPeepSpriteNormalStateEatFoodId,                {  8, 16,  5 }, kPeepAnimationSequenceNormalEatFood };
        pag[PeepAnimationType::ShakeHead]              = { kPeepSpriteNormalStateShakeHeadId,              {  8, 16,  5 }, kPeepAnimationSequenceNormalShakeHead };
        pag[PeepAnimationType::EmptyPockets]           = { kPeepSpriteNormalStateEmptyPocketsId,           {  8, 16,  5 }, kPeepAnimationSequenceNormalEmptyPockets };
        pag[PeepAnimationType::HoldMat]                = { kPeepSpriteNormalStateHoldMatId,                {  9, 16,  5 }, kPeepAnimationSequenceNormalHoldMat };
        pag[PeepAnimationType::SittingIdle]            = { kPeepSpriteNormalStateSittingIdleId,            {  9, 16,  6 }, kPeepAnimationSequenceNormalSittingIdle };
        pag[PeepAnimationType::SittingEatFood]         = { kPeepSpriteNormalStateSittingEatFoodId,         {  9, 16,  6 }, kPeepAnimationSequenceNormalSittingEatFood };
        pag[PeepAnimationType::SittingLookAroundLeft]  = { kPeepSpriteNormalStateSittingLookAroundLeftId,  {  9, 16,  6 }, kPeepAnimationSequenceNormalSittingLookAroundLeft };
        pag[PeepAnimationType::SittingLookAroundRight] = { kPeepSpriteNormalStateSittingLookAroundRightId, {  9, 16,  6 }, kPeepAnimationSequenceNormalSittingLookAroundRight };
        pag[PeepAnimationType::Ui]                     = { kPeepSpriteNormalStateUiId,                     {  8, 16,  5 }, kPeepAnimationSequenceNormalUi };
        pag[PeepAnimationType::Wow]                    = { kPeepSpriteNormalStateWowId,                    { 12, 22,  5 }, kPeepAnimationSequenceNormalWow };
        pag[PeepAnimationType::ThrowUp]                = { kPeepSpriteNormalStateThrowUpId,                {  9, 16,  7 }, kPeepAnimationSequenceNormalThrowUp };
        pag[PeepAnimationType::Jump]                   = { kPeepSpriteNormalStateJumpId,                   { 10, 22,  5 }, kPeepAnimationSequenceNormalJump };
        pag[PeepAnimationType::Drowning]               = { kPeepSpriteNormalStateDrowningId,               {  9, 15,  6 }, kPeepAnimationSequenceNormalDrowning };
        pag[PeepAnimationType::Joy]                    = { kPeepSpriteNormalStateJoyId,                    { 11, 24,  6 }, kPeepAnimationSequenceNormalJoy };
        pag[PeepAnimationType::ReadMap]                = { kPeepSpriteNormalStateReadMapId,                { 11, 16,  5 }, kPeepAnimationSequenceNormalReadMap };
        pag[PeepAnimationType::Wave]                   = { kPeepSpriteNormalStateWaveId,                   { 11, 16,  5 }, kPeepAnimationSequenceNormalWave };
        pag[PeepAnimationType::Wave2]                  = { kPeepSpriteNormalStateWave2Id,                  { 11, 16,  5 }, kPeepAnimationSequenceNormalWave2 };
        pag[PeepAnimationType::TakePhoto]              = { kPeepSpriteNormalStateTakePhotoId,              {  8, 16,  5 }, kPeepAnimationSequenceNormalTakePhoto };
        pag[PeepAnimationType::Clap]                   = { kPeepSpriteNormalStateClapId,                   {  9, 17,  6 }, kPeepAnimationSequenceNormalClap };
        pag[PeepAnimationType::Disgust]                = { kPeepSpriteNormalStateDisgustId,                {  9, 16,  5 }, kPeepAnimationSequenceNormalDisgust };
        pag[PeepAnimationType::DrawPicture]            = { kPeepSpriteNormalStateDrawPictureId,            {  9, 22,  7 }, kPeepAnimationSequenceNormalDrawPicture };
        pag[PeepAnimationType::BeingWatched]           = { kPeepSpriteNormalStateBeingWatchedId,           {  9, 22,  7 }, kPeepAnimationSequenceNormalBeingWatched };
        pag[PeepAnimationType::WithdrawMoney]          = { kPeepSpriteNormalStateWithdrawMoneyId,          {  9, 22,  7 }, kPeepAnimationSequenceNormalWithdrawMoney };
        return pag;
    }();

    // Define animation sequences for Handyman sprites
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceHandymanNone          = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceHandymanWatchRide     = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceHandymanUi            = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceHandymanStaffMower    = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t, 25> kPeepAnimationSequenceHandymanStaffSweep    = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
    static constexpr std::array<uint8_t, 65> kPeepAnimationSequenceHandymanDrowning      = { 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 4, 5, 6, 7 };
    static constexpr std::array<uint8_t, 18> kPeepAnimationSequenceHandymanStaffWatering = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t, 19> kPeepAnimationSequenceHandymanStaffEmptyBin = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

    // Define animation group for Handyman sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupHandyman = []() {
        PeepAnimationGroup pag;
        pag[PeepAnimationType::None]          = { kHandymanSpriteStateNoneId,          { 12, 16,  6 }, kPeepAnimationSequenceHandymanNone };
        pag[PeepAnimationType::WatchRide]     = { kHandymanSpriteStateWatchRideId,     {  9, 16,  6 }, kPeepAnimationSequenceHandymanWatchRide };
        pag[PeepAnimationType::Ui]            = { kHandymanSpriteStateUiId,            { 15, 16,  5 }, kPeepAnimationSequenceHandymanUi };
        pag[PeepAnimationType::StaffMower]    = { kHandymanSpriteStateStaffMowerId,    { 18, 16, 11 }, kPeepAnimationSequenceHandymanStaffMower };
        pag[PeepAnimationType::StaffSweep]    = { kHandymanSpriteStateStaffSweepId,    { 17, 16,  9 }, kPeepAnimationSequenceHandymanStaffSweep };
        pag[PeepAnimationType::Drowning]      = { kHandymanSpriteStateDrowningId,      {  9, 15,  6 }, kPeepAnimationSequenceHandymanDrowning };
        pag[PeepAnimationType::StaffWatering] = { kHandymanSpriteStateStaffWateringId, { 17, 16,  9 }, kPeepAnimationSequenceHandymanStaffWatering };
        pag[PeepAnimationType::StaffEmptyBin] = { kHandymanSpriteStateStaffEmptyBinId, { 17, 16,  9 }, kPeepAnimationSequenceHandymanStaffEmptyBin };
        return pag;
    }();

    // Define animation sequences for Mechanic sprites
    static constexpr std::array<uint8_t,   6> kPeepAnimationSequenceMechanicNone             = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,   1> kPeepAnimationSequenceMechanicWatchRide        = { 0 };
    static constexpr std::array<uint8_t,  12> kPeepAnimationSequenceMechanicUi               = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  65> kPeepAnimationSequenceMechanicDrowning         = { 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 4, 5, 6, 7 };
    static constexpr std::array<uint8_t,  85> kPeepAnimationSequenceMechanicStaffAnswerCall  = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t,  59> kPeepAnimationSequenceMechanicStaffAnswerCall2 = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t,  43> kPeepAnimationSequenceMechanicStaffCheckBoard  = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t,  53> kPeepAnimationSequenceMechanicStaffFix         = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t,  99> kPeepAnimationSequenceMechanicStaffFix2        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
    static constexpr std::array<uint8_t,  61> kPeepAnimationSequenceMechanicStaffFixGround   = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t, 110> kPeepAnimationSequenceMechanicStaffFix3        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };

    // Define animation group for Mechanic sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupMechanic = []() {
        PeepAnimationGroup pag;
        pag[PeepAnimationType::None]             = { kMechanicSpriteStateNoneId,            { 10, 16,  5 }, kPeepAnimationSequenceMechanicNone };
        pag[PeepAnimationType::WatchRide]        = { kMechanicSpriteStateWatchRideId,       { 10, 16,  5 }, kPeepAnimationSequenceMechanicWatchRide };
        pag[PeepAnimationType::Ui]               = { kMechanicSpriteStateUiId,              { 10, 16,  5 }, kPeepAnimationSequenceMechanicUi };
        pag[PeepAnimationType::Drowning]         = { kMechanicSpriteStateDrowningId,        {  9, 15,  6 }, kPeepAnimationSequenceMechanicDrowning };
        pag[PeepAnimationType::StaffAnswerCall]  = { kMechanicSpriteStateStaffAnswerCallId, { 13, 22,  7 }, kPeepAnimationSequenceMechanicStaffAnswerCall };
        pag[PeepAnimationType::StaffAnswerCall2] = { kMechanicSpriteStateStaffAnswerCallId, { 13, 22,  7 }, kPeepAnimationSequenceMechanicStaffAnswerCall2 };
        pag[PeepAnimationType::StaffCheckBoard]  = { kMechanicSpriteStateStaffCheckBoardId, { 13, 22,  7 }, kPeepAnimationSequenceMechanicStaffCheckBoard };
        pag[PeepAnimationType::StaffFix]         = { kMechanicSpriteStateStaffFixId,        { 13, 22,  7 }, kPeepAnimationSequenceMechanicStaffFix };
        pag[PeepAnimationType::StaffFix2]        = { kMechanicSpriteStateStaffFixId,        { 13, 22,  7 }, kPeepAnimationSequenceMechanicStaffFix2 };
        pag[PeepAnimationType::StaffFixGround]   = { kMechanicSpriteStateStaffFixGroundId,  { 19, 16, 16 }, kPeepAnimationSequenceMechanicStaffFixGround };
        pag[PeepAnimationType::StaffFix3]        = { kMechanicSpriteStateStaffFixId,        { 13, 22,  7 }, kPeepAnimationSequenceMechanicStaffFix3 };
        return pag;
    }();

    // Define animation sequences for Security sprites
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceSecurityNone      = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceSecurityWatchRide = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceSecurityUi        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t, 64> kPeepAnimationSequenceSecurityDrowning  = { 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 4, 5, 6 };

    // Define animation group for Security sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupSecurity = []() {
        PeepAnimationGroup pag;
        pag[PeepAnimationType::None]      = { kSecuritySpriteStateNoneId,      {  8, 18,  5 }, kPeepAnimationSequenceSecurityNone };
        pag[PeepAnimationType::WatchRide] = { kSecuritySpriteStateWatchRideId, {  8, 17,  5 }, kPeepAnimationSequenceSecurityWatchRide };
        pag[PeepAnimationType::Ui]        = { kSecuritySpriteStateUiId,        { 15, 19,  6 }, kPeepAnimationSequenceSecurityUi };
        pag[PeepAnimationType::Drowning]  = { kSecuritySpriteStateDrowningId,  {  9, 15,  6 }, kPeepAnimationSequenceSecurityDrowning };
        return pag;
    }();

    // Define animation sequences for EntertainerPanda sprites
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceEntertainerPandaNone      = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceEntertainerPandaWatchRide = { 0 };
    static constexpr std::array<uint8_t, 23> kPeepAnimationSequenceEntertainerPandaEatFood   = { 0, 1, 2, 3, 4, 5, 4, 3, 4, 5, 4, 3, 4, 5, 4, 3, 4, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceEntertainerPandaUi        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t, 64> kPeepAnimationSequenceEntertainerPandaDrowning  = { 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 4, 5, 6 };
    static constexpr std::array<uint8_t, 64> kPeepAnimationSequenceEntertainerPandaJoy       = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    static constexpr std::array<uint8_t, 23> kPeepAnimationSequenceEntertainerPandaWave2     = { 0, 1, 2, 3, 4, 5, 4, 3, 4, 5, 4, 3, 4, 5, 4, 3, 4, 5, 4, 3, 2, 1, 0 };

    // Define animation group for EntertainerPanda sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupEntertainerPanda = []() {
        PeepAnimationGroup pag;
        pag[PeepAnimationType::None]      = { kEntertainerSpritePandaStateNoneId,      { 13, 24,  8 }, kPeepAnimationSequenceEntertainerPandaNone };
        pag[PeepAnimationType::WatchRide] = { kEntertainerSpritePandaStateWatchRideId, { 10, 23,  7 }, kPeepAnimationSequenceEntertainerPandaWatchRide };
        pag[PeepAnimationType::EatFood]   = { kEntertainerSpritePandaStateWaveId,      { 14, 24,  7 }, kPeepAnimationSequenceEntertainerPandaEatFood };
        pag[PeepAnimationType::Ui]        = { kEntertainerSpritePandaStateUiId,        { 19, 30,  8 }, kPeepAnimationSequenceEntertainerPandaUi };
        pag[PeepAnimationType::Drowning]  = { kEntertainerSpritePandaStateDrowningId,  { 13, 15,  6 }, kPeepAnimationSequenceEntertainerPandaDrowning };
        pag[PeepAnimationType::Joy]       = { kEntertainerSpritePandaStateJoyId,       { 14, 25,  8 }, kPeepAnimationSequenceEntertainerPandaJoy };
        pag[PeepAnimationType::Wave2]     = { kEntertainerSpritePandaStateWaveId,      { 14, 24,  7 }, kPeepAnimationSequenceEntertainerPandaWave2 };
        return pag;
    }();

    // Define animation sequences for EntertainerTiger sprites
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceEntertainerTigerNone      = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceEntertainerTigerWatchRide = { 0 };
    static constexpr std::array<uint8_t, 23> kPeepAnimationSequenceEntertainerTigerEatFood   = { 0, 1, 2, 3, 4, 5, 4, 3, 4, 5, 4, 3, 4, 5, 4, 3, 4, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceEntertainerTigerUi        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t, 64> kPeepAnimationSequenceEntertainerTigerDrowning  = { 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 4, 5, 6 };
    static constexpr std::array<uint8_t, 78> kPeepAnimationSequenceEntertainerTigerJoy       = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 25, 25, 25, 25 };
    static constexpr std::array<uint8_t, 23> kPeepAnimationSequenceEntertainerTigerWave2     = { 0, 1, 2, 3, 4, 5, 4, 3, 4, 5, 4, 3, 4, 5, 4, 3, 4, 5, 4, 3, 2, 1, 0 };

    // Define animation group for EntertainerTiger sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupEntertainerTiger = []() {
        PeepAnimationGroup pag;
        pag[PeepAnimationType::None]      = { kEntertainerSpriteTigerStateNoneId,      { 13, 24,  8 }, kPeepAnimationSequenceEntertainerTigerNone };
        pag[PeepAnimationType::WatchRide] = { kEntertainerSpriteTigerStateWatchRideId, { 10, 23,  7 }, kPeepAnimationSequenceEntertainerTigerWatchRide };
        pag[PeepAnimationType::EatFood]   = { kEntertainerSpriteTigerStateWaveId,      { 16, 24,  8 }, kPeepAnimationSequenceEntertainerTigerEatFood };
        pag[PeepAnimationType::Ui]        = { kEntertainerSpriteTigerStateUiId,        { 23, 30,  8 }, kPeepAnimationSequenceEntertainerTigerUi };
        pag[PeepAnimationType::Drowning]  = { kEntertainerSpriteTigerStateDrowningId,  { 13, 15,  6 }, kPeepAnimationSequenceEntertainerTigerDrowning };
        pag[PeepAnimationType::Joy]       = { kEntertainerSpriteTigerStateJoyId,       { 16, 28,  9 }, kPeepAnimationSequenceEntertainerTigerJoy };
        pag[PeepAnimationType::Wave2]     = { kEntertainerSpriteTigerStateWaveId,      { 16, 24,  8 }, kPeepAnimationSequenceEntertainerTigerWave2 };
        return pag;
    }();

    // Define animation sequences for EntertainerElephant sprites
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceEntertainerElephantNone      = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceEntertainerElephantWatchRide = { 0 };
    static constexpr std::array<uint8_t, 25> kPeepAnimationSequenceEntertainerElephantEatFood   = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 3, 4, 5, 6, 5, 4, 3, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceEntertainerElephantUi        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t, 67> kPeepAnimationSequenceEntertainerElephantDrowning  = { 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 4, 5, 6, 7, 8, 9 };
    static constexpr std::array<uint8_t, 79> kPeepAnimationSequenceEntertainerElephantJoy       = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27 };
    static constexpr std::array<uint8_t, 25> kPeepAnimationSequenceEntertainerElephantWave2     = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 3, 4, 5, 6, 5, 4, 3, 4, 5, 6, 5, 4, 3, 2, 1, 0 };

    // Define animation group for EntertainerElephant sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupEntertainerElephant = []() {
        PeepAnimationGroup pag;
        pag[PeepAnimationType::None]      = { kEntertainerSpriteElephantStateNoneId,      { 13, 24,  8 }, kPeepAnimationSequenceEntertainerElephantNone };
        pag[PeepAnimationType::WatchRide] = { kEntertainerSpriteElephantStateWatchRideId, { 10, 23,  7 }, kPeepAnimationSequenceEntertainerElephantWatchRide };
        pag[PeepAnimationType::EatFood]   = { kEntertainerSpriteElephantStateWaveId,      { 17, 24,  8 }, kPeepAnimationSequenceEntertainerElephantEatFood };
        pag[PeepAnimationType::Ui]        = { kEntertainerSpriteElephantStateUiId,        { 23, 30,  8 }, kPeepAnimationSequenceEntertainerElephantUi };
        pag[PeepAnimationType::Drowning]  = { kEntertainerSpriteElephantStateDrowningId,  { 17, 15,  6 }, kPeepAnimationSequenceEntertainerElephantDrowning };
        pag[PeepAnimationType::Joy]       = { kEntertainerSpriteElephantStateJoyId,       { 18, 25,  9 }, kPeepAnimationSequenceEntertainerElephantJoy };
        pag[PeepAnimationType::Wave2]     = { kEntertainerSpriteElephantStateWaveId,      { 17, 24,  8 }, kPeepAnimationSequenceEntertainerElephantWave2 };
        return pag;
    }();

    // Define animation sequences for EntertainerRoman sprites
    static constexpr std::array<uint8_t,   6> kPeepAnimationSequenceEntertainerRomanNone      = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,   1> kPeepAnimationSequenceEntertainerRomanWatchRide = { 0 };
    static constexpr std::array<uint8_t,  21> kPeepAnimationSequenceEntertainerRomanEatFood   = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 6, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t,  12> kPeepAnimationSequenceEntertainerRomanUi        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  65> kPeepAnimationSequenceEntertainerRomanDrowning  = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
    static constexpr std::array<uint8_t, 144> kPeepAnimationSequenceEntertainerRomanJoy       = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 14, 13, 12, 11, 10, 11, 12, 13, 14, 15, 14, 13, 12, 11, 10, 11, 12, 13, 14, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 4, 4, 4, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 14, 13, 12, 11, 10, 11, 12, 13, 14, 15, 14, 13, 12, 11, 10, 11, 12, 13, 14, 15, 14, 13, 12, 11, 10, 11, 12, 13, 14, 15, 16, 16, 16, 16, 16, 17, 18, 19, 20, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 24, 25, 26, 27, 28, 28, 28, 29, 30, 31, 32, 32, 32, 32, 32, 33, 34, 35, 36, 37, 38 };
    static constexpr std::array<uint8_t,  21> kPeepAnimationSequenceEntertainerRomanWave2     = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 6, 5, 4, 3, 2, 1, 0 };

    // Define animation group for EntertainerRoman sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupEntertainerRoman = []() {
        PeepAnimationGroup pag;
        pag[PeepAnimationType::None]      = { kEntertainerSpriteRomanStateNoneId,      { 13, 24,  8 }, kPeepAnimationSequenceEntertainerRomanNone };
        pag[PeepAnimationType::WatchRide] = { kEntertainerSpriteRomanStateWatchRideId, { 10, 23,  7 }, kPeepAnimationSequenceEntertainerRomanWatchRide };
        pag[PeepAnimationType::EatFood]   = { kEntertainerSpriteRomanStateWaveId,      { 17, 24,  8 }, kPeepAnimationSequenceEntertainerRomanEatFood };
        pag[PeepAnimationType::Ui]        = { kEntertainerSpriteRomanStateUiId,        { 23, 30,  8 }, kPeepAnimationSequenceEntertainerRomanUi };
        pag[PeepAnimationType::Drowning]  = { kEntertainerSpriteRomanStateDrowningId,  { 17, 15,  6 }, kPeepAnimationSequenceEntertainerRomanDrowning };
        pag[PeepAnimationType::Joy]       = { kEntertainerSpriteRomanStateJoyId,       { 18, 25,  9 }, kPeepAnimationSequenceEntertainerRomanJoy };
        pag[PeepAnimationType::Wave2]     = { kEntertainerSpriteRomanStateWaveId,      { 17, 24,  8 }, kPeepAnimationSequenceEntertainerRomanWave2 };
        return pag;
    }();

    // Define animation sequences for EntertainerGorilla sprites
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceEntertainerGorillaNone      = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceEntertainerGorillaWatchRide = { 0 };
    static constexpr std::array<uint8_t, 39> kPeepAnimationSequenceEntertainerGorillaEatFood   = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 9, 8, 7, 8, 9, 10, 9, 8, 7, 8, 9, 10, 9, 8, 7, 8, 9, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceEntertainerGorillaUi        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t, 66> kPeepAnimationSequenceEntertainerGorillaDrowning  = { 0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t, 39> kPeepAnimationSequenceEntertainerGorillaJoy       = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 9, 8, 7, 8, 9, 10, 9, 8, 7, 8, 9, 10, 9, 8, 7, 8, 9, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t, 39> kPeepAnimationSequenceEntertainerGorillaWave2     = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 9, 8, 7, 8, 9, 10, 9, 8, 7, 8, 9, 10, 9, 8, 7, 8, 9, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

    // Define animation group for EntertainerGorilla sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupEntertainerGorilla = []() {
        PeepAnimationGroup pag;
        pag[PeepAnimationType::None]      = { kEntertainerSpriteGorillaStateNoneId,      { 13, 24,  8 }, kPeepAnimationSequenceEntertainerGorillaNone };
        pag[PeepAnimationType::WatchRide] = { kEntertainerSpriteGorillaStateWatchRideId, { 10, 23,  7 }, kPeepAnimationSequenceEntertainerGorillaWatchRide };
        pag[PeepAnimationType::EatFood]   = { kEntertainerSpriteGorillaStateWaveId,      { 17, 24,  8 }, kPeepAnimationSequenceEntertainerGorillaEatFood };
        pag[PeepAnimationType::Ui]        = { kEntertainerSpriteGorillaStateUiId,        { 23, 30,  8 }, kPeepAnimationSequenceEntertainerGorillaUi };
        pag[PeepAnimationType::Drowning]  = { kEntertainerSpriteGorillaStateDrowningId,  { 17, 15,  6 }, kPeepAnimationSequenceEntertainerGorillaDrowning };
        pag[PeepAnimationType::Joy]       = { kEntertainerSpriteGorillaStateWaveId,      { 18, 25,  9 }, kPeepAnimationSequenceEntertainerGorillaJoy };
        pag[PeepAnimationType::Wave2]     = { kEntertainerSpriteGorillaStateWaveId,      { 17, 24,  8 }, kPeepAnimationSequenceEntertainerGorillaWave2 };
        return pag;
    }();

    // Define animation sequences for EntertainerSnowman sprites
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceEntertainerSnowmanNone      = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceEntertainerSnowmanWatchRide = { 0 };
    static constexpr std::array<uint8_t, 65> kPeepAnimationSequenceEntertainerSnowmanEatFood   = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 31, 31, 31, 32, 33 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceEntertainerSnowmanUi        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t, 62> kPeepAnimationSequenceEntertainerSnowmanDrowning  = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7, 8, 9 };
    static constexpr std::array<uint8_t, 65> kPeepAnimationSequenceEntertainerSnowmanJoy       = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 31, 31, 31, 32, 33 };
    static constexpr std::array<uint8_t, 65> kPeepAnimationSequenceEntertainerSnowmanWave2     = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 31, 31, 31, 32, 33 };

    // Define animation group for EntertainerSnowman sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupEntertainerSnowman = []() {
        PeepAnimationGroup pag;
        pag[PeepAnimationType::None]      = { kEntertainerSpriteSnowmanStateNoneId,      { 13, 24,  8 }, kPeepAnimationSequenceEntertainerSnowmanNone };
        pag[PeepAnimationType::WatchRide] = { kEntertainerSpriteSnowmanStateWatchRideId, { 10, 23,  7 }, kPeepAnimationSequenceEntertainerSnowmanWatchRide };
        pag[PeepAnimationType::EatFood]   = { kEntertainerSpriteSnowmanStateWaveId,      { 17, 28,  9 }, kPeepAnimationSequenceEntertainerSnowmanEatFood };
        pag[PeepAnimationType::Ui]        = { kEntertainerSpriteSnowmanStateUiId,        { 23, 30,  8 }, kPeepAnimationSequenceEntertainerSnowmanUi };
        pag[PeepAnimationType::Drowning]  = { kEntertainerSpriteSnowmanStateDrowningId,  { 17, 15,  9 }, kPeepAnimationSequenceEntertainerSnowmanDrowning };
        pag[PeepAnimationType::Joy]       = { kEntertainerSpriteSnowmanStateWaveId,      { 18, 28,  9 }, kPeepAnimationSequenceEntertainerSnowmanJoy };
        pag[PeepAnimationType::Wave2]     = { kEntertainerSpriteSnowmanStateWaveId,      { 17, 28,  9 }, kPeepAnimationSequenceEntertainerSnowmanWave2 };
        return pag;
    }();

    // Define animation sequences for EntertainerKnight sprites
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceEntertainerKnightNone      = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceEntertainerKnightWatchRide = { 0 };
    static constexpr std::array<uint8_t, 47> kPeepAnimationSequenceEntertainerKnightEatFood   = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 7, 8, 9, 10, 11, 12, 12, 12, 12, 12, 13, 14, 15, 16, 17, 17, 17, 17, 17, 18, 19, 20, 21, 22, 23, 23, 23, 23, 23, 24, 25, 26, 27 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceEntertainerKnightUi        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t, 66> kPeepAnimationSequenceEntertainerKnightDrowning  = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
    static constexpr std::array<uint8_t, 47> kPeepAnimationSequenceEntertainerKnightJoy       = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 7, 8, 9, 10, 11, 12, 12, 12, 12, 12, 13, 14, 15, 16, 17, 17, 17, 17, 17, 18, 19, 20, 21, 22, 23, 23, 23, 23, 23, 24, 25, 26, 27 };
    static constexpr std::array<uint8_t, 47> kPeepAnimationSequenceEntertainerKnightWave2     = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 7, 8, 9, 10, 11, 12, 12, 12, 12, 12, 13, 14, 15, 16, 17, 17, 17, 17, 17, 18, 19, 20, 21, 22, 23, 23, 23, 23, 23, 24, 25, 26, 27 };

    // Define animation group for EntertainerKnight sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupEntertainerKnight = []() {
        PeepAnimationGroup pag;
        pag[PeepAnimationType::None]      = { kEntertainerSpriteKnightStateNoneId,      { 16, 32,  8 }, kPeepAnimationSequenceEntertainerKnightNone };
        pag[PeepAnimationType::WatchRide] = { kEntertainerSpriteKnightStateWatchRideId, { 10, 23,  7 }, kPeepAnimationSequenceEntertainerKnightWatchRide };
        pag[PeepAnimationType::EatFood]   = { kEntertainerSpriteKnightStateWaveId,      { 23, 30, 15 }, kPeepAnimationSequenceEntertainerKnightEatFood };
        pag[PeepAnimationType::Ui]        = { kEntertainerSpriteKnightStateUiId,        { 23, 30,  8 }, kPeepAnimationSequenceEntertainerKnightUi };
        pag[PeepAnimationType::Drowning]  = { kEntertainerSpriteKnightStateDrowningId,  { 21, 32,  9 }, kPeepAnimationSequenceEntertainerKnightDrowning };
        pag[PeepAnimationType::Joy]       = { kEntertainerSpriteKnightStateWaveId,      { 23, 30, 15 }, kPeepAnimationSequenceEntertainerKnightJoy };
        pag[PeepAnimationType::Wave2]     = { kEntertainerSpriteKnightStateWaveId,      { 23, 30, 15 }, kPeepAnimationSequenceEntertainerKnightWave2 };
        return pag;
    }();

    // Define animation sequences for EntertainerAstronaut sprites
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceEntertainerAstronautNone      = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceEntertainerAstronautWatchRide = { 0 };
    static constexpr std::array<uint8_t, 19> kPeepAnimationSequenceEntertainerAstronautEatFood   = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 7, 8, 9, 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceEntertainerAstronautUi        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t, 63> kPeepAnimationSequenceEntertainerAstronautDrowning  = { 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
    static constexpr std::array<uint8_t, 19> kPeepAnimationSequenceEntertainerAstronautJoy       = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 7, 8, 9, 0 };
    static constexpr std::array<uint8_t, 19> kPeepAnimationSequenceEntertainerAstronautWave2     = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 7, 8, 9, 0 };

    // Define animation group for EntertainerAstronaut sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupEntertainerAstronaut = []() {
        PeepAnimationGroup pag;
        pag[PeepAnimationType::None]      = { kEntertainerSpriteAstronautStateNoneId,      { 16, 32,  8 }, kPeepAnimationSequenceEntertainerAstronautNone };
        pag[PeepAnimationType::WatchRide] = { kEntertainerSpriteAstronautStateWatchRideId, { 10, 23,  7 }, kPeepAnimationSequenceEntertainerAstronautWatchRide };
        pag[PeepAnimationType::EatFood]   = { kEntertainerSpriteAstronautStateWaveId,      { 23, 30, 15 }, kPeepAnimationSequenceEntertainerAstronautEatFood };
        pag[PeepAnimationType::Ui]        = { kEntertainerSpriteAstronautStateUiId,        { 23, 30,  8 }, kPeepAnimationSequenceEntertainerAstronautUi };
        pag[PeepAnimationType::Drowning]  = { kEntertainerSpriteAstronautStateDrowningId,  { 21, 32,  9 }, kPeepAnimationSequenceEntertainerAstronautDrowning };
        pag[PeepAnimationType::Joy]       = { kEntertainerSpriteAstronautStateWaveId,      { 23, 30, 15 }, kPeepAnimationSequenceEntertainerAstronautJoy };
        pag[PeepAnimationType::Wave2]     = { kEntertainerSpriteAstronautStateWaveId,      { 23, 30, 15 }, kPeepAnimationSequenceEntertainerAstronautWave2 };
        return pag;
    }();

    // Define animation sequences for EntertainerBandit sprites
    static constexpr std::array<uint8_t,   6> kPeepAnimationSequenceEntertainerBanditNone      = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,   1> kPeepAnimationSequenceEntertainerBanditWatchRide = { 0 };
    static constexpr std::array<uint8_t, 111> kPeepAnimationSequenceEntertainerBanditEatFood   = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 8, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 12, 13, 13, 13, 13, 13, 14, 15, 16, 17, 17, 17, 17, 17, 18, 19, 20, 21, 22, 21, 20, 19, 20, 21, 22, 21, 20, 19, 20, 21, 22, 21, 20, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41 };
    static constexpr std::array<uint8_t,  12> kPeepAnimationSequenceEntertainerBanditUi        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  64> kPeepAnimationSequenceEntertainerBanditDrowning  = { 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    static constexpr std::array<uint8_t, 111> kPeepAnimationSequenceEntertainerBanditJoy       = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 8, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 12, 13, 13, 13, 13, 13, 14, 15, 16, 17, 17, 17, 17, 17, 18, 19, 20, 21, 22, 21, 20, 19, 20, 21, 22, 21, 20, 19, 20, 21, 22, 21, 20, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41 };
    static constexpr std::array<uint8_t, 111> kPeepAnimationSequenceEntertainerBanditWave2     = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 8, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 12, 13, 13, 13, 13, 13, 14, 15, 16, 17, 17, 17, 17, 17, 18, 19, 20, 21, 22, 21, 20, 19, 20, 21, 22, 21, 20, 19, 20, 21, 22, 21, 20, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41 };

    // Define animation group for EntertainerBandit sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupEntertainerBandit = []() {
        PeepAnimationGroup pag;
        pag[PeepAnimationType::None]      = { kEntertainerSpriteBanditStateNoneId,      { 16, 32,  8 }, kPeepAnimationSequenceEntertainerBanditNone };
        pag[PeepAnimationType::WatchRide] = { kEntertainerSpriteBanditStateWatchRideId, { 10, 23,  7 }, kPeepAnimationSequenceEntertainerBanditWatchRide };
        pag[PeepAnimationType::EatFood]   = { kEntertainerSpriteBanditStateWaveId,      { 23, 30, 15 }, kPeepAnimationSequenceEntertainerBanditEatFood };
        pag[PeepAnimationType::Ui]        = { kEntertainerSpriteBanditStateUiId,        { 23, 30,  8 }, kPeepAnimationSequenceEntertainerBanditUi };
        pag[PeepAnimationType::Drowning]  = { kEntertainerSpriteBanditStateDrowningId,  { 21, 32,  9 }, kPeepAnimationSequenceEntertainerBanditDrowning };
        pag[PeepAnimationType::Joy]       = { kEntertainerSpriteBanditStateWaveId,      { 23, 30, 15 }, kPeepAnimationSequenceEntertainerBanditJoy };
        pag[PeepAnimationType::Wave2]     = { kEntertainerSpriteBanditStateWaveId,      { 23, 30, 15 }, kPeepAnimationSequenceEntertainerBanditWave2 };
        return pag;
    }();

    // Define animation sequences for EntertainerSheriff sprites
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceEntertainerSheriffNone      = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceEntertainerSheriffWatchRide = { 0 };
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequenceEntertainerSheriffEatFood   = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 8, 9, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceEntertainerSheriffUi        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t, 64> kPeepAnimationSequenceEntertainerSheriffDrowning  = { 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequenceEntertainerSheriffJoy       = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 8, 9, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequenceEntertainerSheriffWave2     = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 8, 9, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

    // Define animation group for EntertainerSheriff sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupEntertainerSheriff = []() {
        PeepAnimationGroup pag;
        pag[PeepAnimationType::None]      = { kEntertainerSpriteSheriffStateNoneId,      { 16, 32,  8 }, kPeepAnimationSequenceEntertainerSheriffNone };
        pag[PeepAnimationType::WatchRide] = { kEntertainerSpriteSheriffStateWatchRideId, { 10, 23,  7 }, kPeepAnimationSequenceEntertainerSheriffWatchRide };
        pag[PeepAnimationType::EatFood]   = { kEntertainerSpriteSheriffStateWaveId,      { 23, 30, 15 }, kPeepAnimationSequenceEntertainerSheriffEatFood };
        pag[PeepAnimationType::Ui]        = { kEntertainerSpriteSheriffStateUiId,        { 23, 30,  8 }, kPeepAnimationSequenceEntertainerSheriffUi };
        pag[PeepAnimationType::Drowning]  = { kEntertainerSpriteSheriffStateDrowningId,  { 21, 32,  9 }, kPeepAnimationSequenceEntertainerSheriffDrowning };
        pag[PeepAnimationType::Joy]       = { kEntertainerSpriteSheriffStateWaveId,      { 23, 30, 15 }, kPeepAnimationSequenceEntertainerSheriffJoy };
        pag[PeepAnimationType::Wave2]     = { kEntertainerSpriteSheriffStateWaveId,      { 23, 30, 15 }, kPeepAnimationSequenceEntertainerSheriffWave2 };
        return pag;
    }();

    // Define animation sequences for EntertainerPirate sprites
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceEntertainerPirateNone      = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceEntertainerPirateWatchRide = { 0 };
    static constexpr std::array<uint8_t, 67> kPeepAnimationSequenceEntertainerPirateEatFood   = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 9, 10, 11, 12, 13, 14, 15, 16, 9, 10, 11, 12, 13, 14, 15, 16, 18, 19, 20, 21, 21, 21, 21, 21, 21, 22, 23, 23, 23, 23, 24, 25, 26, 27, 27, 27, 27, 28, 29, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 31 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceEntertainerPirateUi        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t, 72> kPeepAnimationSequenceEntertainerPirateDrowning  = { 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    static constexpr std::array<uint8_t, 67> kPeepAnimationSequenceEntertainerPirateJoy       = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 9, 10, 11, 12, 13, 14, 15, 16, 9, 10, 11, 12, 13, 14, 15, 16, 18, 19, 20, 21, 21, 21, 21, 21, 21, 22, 23, 23, 23, 23, 24, 25, 26, 27, 27, 27, 27, 28, 29, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 31 };
    static constexpr std::array<uint8_t, 67> kPeepAnimationSequenceEntertainerPirateWave2     = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 9, 10, 11, 12, 13, 14, 15, 16, 9, 10, 11, 12, 13, 14, 15, 16, 18, 19, 20, 21, 21, 21, 21, 21, 21, 22, 23, 23, 23, 23, 24, 25, 26, 27, 27, 27, 27, 28, 29, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 31 };

    // Define animation group for EntertainerPirate sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupEntertainerPirate = []() {
        PeepAnimationGroup pag;
        pag[PeepAnimationType::None]      = { kEntertainerSpritePirateStateNoneId,      { 16, 32,  8 }, kPeepAnimationSequenceEntertainerPirateNone };
        pag[PeepAnimationType::WatchRide] = { kEntertainerSpritePirateStateWatchRideId, { 10, 23,  7 }, kPeepAnimationSequenceEntertainerPirateWatchRide };
        pag[PeepAnimationType::EatFood]   = { kEntertainerSpritePirateStateWaveId,      { 23, 30, 15 }, kPeepAnimationSequenceEntertainerPirateEatFood };
        pag[PeepAnimationType::Ui]        = { kEntertainerSpritePirateStateUiId,        { 23, 30,  8 }, kPeepAnimationSequenceEntertainerPirateUi };
        pag[PeepAnimationType::Drowning]  = { kEntertainerSpritePirateStateDrowningId,  { 21, 32,  9 }, kPeepAnimationSequenceEntertainerPirateDrowning };
        pag[PeepAnimationType::Joy]       = { kEntertainerSpritePirateStateWaveId,      { 23, 30, 15 }, kPeepAnimationSequenceEntertainerPirateJoy };
        pag[PeepAnimationType::Wave2]     = { kEntertainerSpritePirateStateWaveId,      { 23, 30, 15 }, kPeepAnimationSequenceEntertainerPirateWave2 };
        return pag;
    }();

    // Define animation sequences for IceCream sprites
    static constexpr std::array<uint8_t, 42> kPeepAnimationSequenceIceCreamNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceIceCreamWatchRide      = { 0 };
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceIceCreamEatFood        = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceIceCreamSittingIdle    = { 0 };
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceIceCreamSittingEatFood = { 0, 1, 2, 3, 4, 5 };

    // Define animation group for IceCream sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupIceCream = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteIceCreamStateNoneId,           {  8, 16,  5 }, kPeepAnimationSequenceIceCreamNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteIceCreamStateWatchRideId,      {  8, 16,  5 }, kPeepAnimationSequenceIceCreamWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteIceCreamStateEatFoodId,        {  8, 16,  5 }, kPeepAnimationSequenceIceCreamEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteIceCreamStateSittingIdleId,    {  9, 16,  6 }, kPeepAnimationSequenceIceCreamSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteIceCreamStateSittingEatFoodId, {  9, 16,  6 }, kPeepAnimationSequenceIceCreamSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Chips sprites
    static constexpr std::array<uint8_t, 36> kPeepAnimationSequenceChipsNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceChipsWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceChipsEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceChipsSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceChipsSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for Chips sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupChips = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteChipsStateNoneId,           {  8, 16,  5 }, kPeepAnimationSequenceChipsNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteChipsStateWatchRideId,      {  8, 16,  5 }, kPeepAnimationSequenceChipsWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteChipsStateEatFoodId,        {  8, 16,  5 }, kPeepAnimationSequenceChipsEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteChipsStateSittingIdleId,    {  9, 16,  6 }, kPeepAnimationSequenceChipsSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteChipsStateSittingEatFoodId, {  9, 16,  6 }, kPeepAnimationSequenceChipsSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Burger sprites
    static constexpr std::array<uint8_t, 36> kPeepAnimationSequenceBurgerNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceBurgerWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceBurgerEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceBurgerSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceBurgerSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for Burger sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupBurger = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteBurgerStateNoneId,           {  8, 16,  5 }, kPeepAnimationSequenceBurgerNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteBurgerStateWatchRideId,      {  8, 16,  5 }, kPeepAnimationSequenceBurgerWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteBurgerStateEatFoodId,        {  8, 16,  5 }, kPeepAnimationSequenceBurgerEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteBurgerStateSittingIdleId,    {  9, 16,  6 }, kPeepAnimationSequenceBurgerSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteBurgerStateSittingEatFoodId, {  9, 16,  6 }, kPeepAnimationSequenceBurgerSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Drink sprites
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequenceDrinkNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceDrinkWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceDrinkEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceDrinkSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceDrinkSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for Drink sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupDrink = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteDrinkStateNoneId,           {  8, 16,  5 }, kPeepAnimationSequenceDrinkNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteDrinkStateWatchRideId,      {  8, 16,  5 }, kPeepAnimationSequenceDrinkWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteDrinkStateEatFoodId,        {  8, 16,  5 }, kPeepAnimationSequenceDrinkEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteDrinkStateSittingIdleId,    {  9, 16,  6 }, kPeepAnimationSequenceDrinkSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteDrinkStateSittingEatFoodId, {  9, 16,  6 }, kPeepAnimationSequenceDrinkSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Balloon sprites
    static constexpr std::array<uint8_t, 6> kPeepAnimationSequenceBalloonNone        = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceBalloonWatchRide   = { 0 };
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceBalloonSittingIdle = { 0 };

    // Define animation group for Balloon sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupBalloon = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::None]        = { kPeepSpriteBalloonStateNoneId,        { 11, 28,  5 }, kPeepAnimationSequenceBalloonNone };
        pag[PeepAnimationType::WatchRide]   = { kPeepSpriteBalloonStateWatchRideId,   { 11, 28,  5 }, kPeepAnimationSequenceBalloonWatchRide };
        pag[PeepAnimationType::SittingIdle] = { kPeepSpriteBalloonStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceBalloonSittingIdle };
        return pag;
    }();

    // Define animation sequences for Candyfloss sprites
    static constexpr std::array<uint8_t, 36> kPeepAnimationSequenceCandyflossNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceCandyflossWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceCandyflossEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceCandyflossSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceCandyflossSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for Candyfloss sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupCandyfloss = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteCandyflossStateNoneId,           { 11, 19,  5 }, kPeepAnimationSequenceCandyflossNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteCandyflossStateWatchRideId,      { 11, 19,  5 }, kPeepAnimationSequenceCandyflossWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteCandyflossStateEatFoodId,        { 11, 19,  5 }, kPeepAnimationSequenceCandyflossEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteCandyflossStateSittingIdleId,    { 13, 16,  6 }, kPeepAnimationSequenceCandyflossSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteCandyflossStateSittingEatFoodId, { 13, 16,  6 }, kPeepAnimationSequenceCandyflossSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Umbrella sprites
    static constexpr std::array<uint8_t, 6> kPeepAnimationSequenceUmbrellaNone        = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceUmbrellaWatchRide   = { 0 };
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceUmbrellaSittingIdle = { 0 };

    // Define animation group for Umbrella sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupUmbrella = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::None]        = { kPeepSpriteUmbrellaStateNoneId,        { 14, 21,  5 }, kPeepAnimationSequenceUmbrellaNone };
        pag[PeepAnimationType::WatchRide]   = { kPeepSpriteUmbrellaStateWatchRideId,   { 14, 21,  5 }, kPeepAnimationSequenceUmbrellaWatchRide };
        pag[PeepAnimationType::SittingIdle] = { kPeepSpriteUmbrellaStateSittingIdleId, { 14, 19,  6 }, kPeepAnimationSequenceUmbrellaSittingIdle };
        return pag;
    }();

    // Define animation sequences for Pizza sprites
    static constexpr std::array<uint8_t, 30> kPeepAnimationSequencePizzaNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequencePizzaWatchRide      = { 0 };
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequencePizzaEatFood        = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequencePizzaSittingIdle    = { 0 };
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequencePizzaSittingEatFood = { 0, 1, 2, 3, 4, 5 };

    // Define animation group for Pizza sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupPizza = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::None]           = { kPeepSpritePizzaStateNoneId,           {  8, 16,  5 }, kPeepAnimationSequencePizzaNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpritePizzaStateWatchRideId,      {  8, 16,  5 }, kPeepAnimationSequencePizzaWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpritePizzaStateEatFoodId,        {  8, 16,  5 }, kPeepAnimationSequencePizzaEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpritePizzaStateSittingIdleId,    {  9, 16,  6 }, kPeepAnimationSequencePizzaSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpritePizzaStateSittingEatFoodId, {  9, 16,  6 }, kPeepAnimationSequencePizzaSittingEatFood };
        return pag;
    }();

    // Define animation sequences for SecurityAlt sprites
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceSecurityAltNone      = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceSecurityAltWatchRide = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceSecurityAltUi        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t, 64> kPeepAnimationSequenceSecurityAltDrowning  = { 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 4, 5, 6 };

    // Define animation group for SecurityAlt sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupSecurityAlt = []() {
        PeepAnimationGroup pag;
        pag[PeepAnimationType::None]      = { kSecurityStaffSpriteAltStateNoneId, {  8, 18,  5 }, kPeepAnimationSequenceSecurityAltNone };
        pag[PeepAnimationType::WatchRide] = { kSecuritySpriteStateWatchRideId,    {  8, 17,  5 }, kPeepAnimationSequenceSecurityAltWatchRide };
        pag[PeepAnimationType::Ui]        = { kSecuritySpriteStateUiId,           { 15, 19,  6 }, kPeepAnimationSequenceSecurityAltUi };
        pag[PeepAnimationType::Drowning]  = { kSecuritySpriteStateDrowningId,     {  9, 15,  6 }, kPeepAnimationSequenceSecurityAltDrowning };
        return pag;
    }();

    // Define animation sequences for Popcorn sprites
    static constexpr std::array<uint8_t, 36> kPeepAnimationSequencePopcornNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequencePopcornWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequencePopcornEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequencePopcornSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequencePopcornSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for Popcorn sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupPopcorn = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::None]           = { kPeepSpritePopcornStateNoneId,           { 11, 19,  5 }, kPeepAnimationSequencePopcornNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpritePopcornStateWatchRideId,      { 11, 19,  5 }, kPeepAnimationSequencePopcornWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpritePopcornStateEatFoodId,        { 11, 19,  5 }, kPeepAnimationSequencePopcornEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpritePopcornStateSittingIdleId,    { 13, 16,  6 }, kPeepAnimationSequencePopcornSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpritePopcornStateSittingEatFoodId, { 13, 16,  6 }, kPeepAnimationSequencePopcornSittingEatFood };
        return pag;
    }();

    // Define animation sequences for ArmsCrossed sprites
    static constexpr std::array<uint8_t, 6> kPeepAnimationSequenceArmsCrossedNone = { 0, 1, 2, 3, 4, 5 };

    // Define animation group for ArmsCrossed sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupArmsCrossed = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::None] = { kPeepSpriteArmsCrossedStateNoneId, {  8, 16,  5 }, kPeepAnimationSequenceArmsCrossedNone };
        return pag;
    }();

    // Define animation sequences for HeadDown sprites
    static constexpr std::array<uint8_t, 6> kPeepAnimationSequenceHeadDownNone = { 0, 1, 2, 3, 4, 5 };

    // Define animation group for HeadDown sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupHeadDown = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::None] = { kPeepSpriteHeadDownStateNoneId, {  8, 16,  5 }, kPeepAnimationSequenceHeadDownNone };
        return pag;
    }();

    // Define animation sequences for Nauseous sprites
    static constexpr std::array<uint8_t, 6> kPeepAnimationSequenceNauseousNone = { 0, 1, 2, 3, 4, 5 };

    // Define animation group for Nauseous sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupNauseous = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::None] = { kPeepSpriteNauseousStateNoneId, {  8, 16,  5 }, kPeepAnimationSequenceNauseousNone };
        return pag;
    }();

    // Define animation sequences for VeryNauseous sprites
    static constexpr std::array<uint8_t, 6> kPeepAnimationSequenceVeryNauseousNone        = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceVeryNauseousWatchRide   = { 0 };
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceVeryNauseousSittingIdle = { 0 };

    // Define animation group for VeryNauseous sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupVeryNauseous = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::None]        = { kPeepSpriteVeryNauseousStateNoneId,        {  8, 16,  5 }, kPeepAnimationSequenceVeryNauseousNone };
        pag[PeepAnimationType::WatchRide]   = { kPeepSpriteVeryNauseousStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceVeryNauseousWatchRide };
        pag[PeepAnimationType::SittingIdle] = { kPeepSpriteVeryNauseousStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceVeryNauseousSittingIdle };
        return pag;
    }();

    // Define animation sequences for RequireToilet sprites
    static constexpr std::array<uint8_t, 6> kPeepAnimationSequenceRequireToiletNone = { 0, 1, 2, 3, 4, 5 };

    // Define animation group for RequireToilet sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupRequireToilet = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::None] = { kPeepSpriteRequireToiletStateNoneId, {  8, 16,  5 }, kPeepAnimationSequenceRequireToiletNone };
        return pag;
    }();

    // Define animation sequences for Hat sprites
    static constexpr std::array<uint8_t, 6> kPeepAnimationSequenceHatNone        = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceHatWatchRide   = { 0 };
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceHatSittingIdle = { 0 };

    // Define animation group for Hat sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupHat = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::None]        = { kPeepSpriteHatStateNoneId,        { 12, 32,  5 }, kPeepAnimationSequenceHatNone };
        pag[PeepAnimationType::WatchRide]   = { kPeepSpriteHatStateWatchRideId,   { 12, 32,  5 }, kPeepAnimationSequenceHatWatchRide };
        pag[PeepAnimationType::SittingIdle] = { kPeepSpriteHatStateSittingIdleId, { 10, 20,  6 }, kPeepAnimationSequenceHatSittingIdle };
        return pag;
    }();

    // Define animation sequences for HotDog sprites
    static constexpr std::array<uint8_t, 36> kPeepAnimationSequenceHotDogNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceHotDogWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceHotDogEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceHotDogSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceHotDogSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for HotDog sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupHotDog = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteHotDogStateNoneId,           {  8, 16,  5 }, kPeepAnimationSequenceHotDogNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteHotDogStateWatchRideId,      {  8, 16,  5 }, kPeepAnimationSequenceHotDogWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteHotDogStateEatFoodId,        {  8, 16,  5 }, kPeepAnimationSequenceHotDogEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteHotDogStateSittingIdleId,    {  9, 16,  6 }, kPeepAnimationSequenceHotDogSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteHotDogStateSittingEatFoodId, {  9, 16,  6 }, kPeepAnimationSequenceHotDogSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Tentacle sprites
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequenceTentacleNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceTentacleWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 24> kPeepAnimationSequenceTentacleEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceTentacleSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 24> kPeepAnimationSequenceTentacleSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23 };

    // Define animation group for Tentacle sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupTentacle = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteTentacleStateNoneId,           {  8, 16,  5 }, kPeepAnimationSequenceTentacleNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteTentacleStateWatchRideId,      {  8, 16,  5 }, kPeepAnimationSequenceTentacleWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteTentacleStateEatFoodId,        {  8, 16,  5 }, kPeepAnimationSequenceTentacleEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteTentacleStateSittingIdleId,    {  9, 16,  6 }, kPeepAnimationSequenceTentacleSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteTentacleStateSittingEatFoodId, {  9, 16,  6 }, kPeepAnimationSequenceTentacleSittingEatFood };
        return pag;
    }();

    // Define animation sequences for ToffeeApple sprites
    static constexpr std::array<uint8_t, 42> kPeepAnimationSequenceToffeeAppleNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceToffeeAppleWatchRide      = { 0 };
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceToffeeAppleEatFood        = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceToffeeAppleSittingIdle    = { 0 };
    static constexpr std::array<uint8_t,  6> kPeepAnimationSequenceToffeeAppleSittingEatFood = { 0, 1, 2, 3, 4, 5 };

    // Define animation group for ToffeeApple sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupToffeeApple = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteToffeeAppleStateNoneId,           {  8, 16,  5 }, kPeepAnimationSequenceToffeeAppleNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteToffeeAppleStateWatchRideId,      {  8, 16,  5 }, kPeepAnimationSequenceToffeeAppleWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteToffeeAppleStateEatFoodId,        {  8, 16,  5 }, kPeepAnimationSequenceToffeeAppleEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteToffeeAppleStateSittingIdleId,    {  9, 16,  6 }, kPeepAnimationSequenceToffeeAppleSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteToffeeAppleStateSittingEatFoodId, {  9, 16,  6 }, kPeepAnimationSequenceToffeeAppleSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Donut sprites
    static constexpr std::array<uint8_t, 36> kPeepAnimationSequenceDonutNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceDonutWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceDonutEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceDonutSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 11> kPeepAnimationSequenceDonutSittingEatFood = { 0, 1, 2, 3, 3, 3, 3, 3, 3, 4, 5 };

    // Define animation group for Donut sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupDonut = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteDonutStateNoneId,           {  8, 16,  5 }, kPeepAnimationSequenceDonutNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteDonutStateWatchRideId,      {  8, 16,  5 }, kPeepAnimationSequenceDonutWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteDonutStateEatFoodId,        {  8, 16,  5 }, kPeepAnimationSequenceDonutEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteDonutStateSittingIdleId,    {  9, 16,  6 }, kPeepAnimationSequenceDonutSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteDonutStateSittingEatFoodId, {  9, 16,  6 }, kPeepAnimationSequenceDonutSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Coffee sprites
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequenceCoffeeNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceCoffeeWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceCoffeeEatFood        = { 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 2, 1 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceCoffeeSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceCoffeeSittingEatFood = { 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 2, 1 };

    // Define animation group for Coffee sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupCoffee = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteCoffeeStateNoneId,        {  8, 16,  5 }, kPeepAnimationSequenceCoffeeNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteCoffeeStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceCoffeeWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteCoffeeStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceCoffeeEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteCoffeeStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceCoffeeSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteCoffeeStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceCoffeeSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Chicken sprites
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequenceChickenNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceChickenWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceChickenEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceChickenSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceChickenSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for Chicken sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupChicken = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteChickenStateNoneId,        {  8, 16,  5 }, kPeepAnimationSequenceChickenNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteChickenStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceChickenWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteChickenStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceChickenEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteChickenStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceChickenSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteChickenStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceChickenSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Lemonade sprites
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequenceLemonadeNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceLemonadeWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceLemonadeEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceLemonadeSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceLemonadeSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for Lemonade sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupLemonade = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteLemonadeStateNoneId,        {  8, 16,  5 }, kPeepAnimationSequenceLemonadeNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteLemonadeStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceLemonadeWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteLemonadeStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceLemonadeEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteLemonadeStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceLemonadeSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteLemonadeStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceLemonadeSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Watching sprites
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceWatchingWatchRide = { 0 };

    // Define animation group for Watching sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupWatching = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::WatchRide] = { kPeepSpriteWatchingStateWatchRideId, {  8, 16,  5 }, kPeepAnimationSequenceWatchingWatchRide };
        return pag;
    }();

    // Define animation sequences for Pretzel sprites
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequencePretzelNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequencePretzelWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequencePretzelEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequencePretzelSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequencePretzelSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for Pretzel sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupPretzel = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::None]           = { kPeepSpritePretzelStateNoneId,        {  8, 16,  5 }, kPeepAnimationSequencePretzelNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpritePretzelStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequencePretzelWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpritePretzelStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequencePretzelEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpritePretzelStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequencePretzelSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpritePretzelStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequencePretzelSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Sunglasses sprites
    static constexpr std::array<uint8_t, 6> kPeepAnimationSequenceSunglassesNone        = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceSunglassesWatchRide   = { 0 };
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceSunglassesSittingIdle = { 0 };

    // Define animation group for Sunglasses sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupSunglasses = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::None]        = { kPeepSpriteSunglassesStateNoneId,        {  8, 16,  5 }, kPeepAnimationSequenceSunglassesNone };
        pag[PeepAnimationType::WatchRide]   = { kPeepSpriteSunglassesStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceSunglassesWatchRide };
        pag[PeepAnimationType::SittingIdle] = { kPeepSpriteSunglassesStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceSunglassesSittingIdle };
        return pag;
    }();

    // Define animation sequences for SuJongkwa sprites
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequenceSuJongkwaNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceSuJongkwaWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceSuJongkwaEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceSuJongkwaSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceSuJongkwaSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for SuJongkwa sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupSuJongkwa = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteSuJongkwaStateNoneId,        {  8, 16,  5 }, kPeepAnimationSequenceSuJongkwaNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteSuJongkwaStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceSuJongkwaWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteSuJongkwaStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceSuJongkwaEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteSuJongkwaStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceSuJongkwaSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteSuJongkwaStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceSuJongkwaSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Juice sprites
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequenceJuiceNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceJuiceWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceJuiceEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceJuiceSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceJuiceSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for Juice sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupJuice = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteJuiceStateNoneId,        {  8, 16,  5 }, kPeepAnimationSequenceJuiceNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteJuiceStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceJuiceWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteJuiceStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceJuiceEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteJuiceStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceJuiceSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteJuiceStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceJuiceSittingEatFood };
        return pag;
    }();

    // Define animation sequences for FunnelCake sprites
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequenceFunnelCakeNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceFunnelCakeWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceFunnelCakeEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceFunnelCakeSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceFunnelCakeSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for FunnelCake sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupFunnelCake = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteFunnelCakeStateNoneId,        {  8, 16,  5 }, kPeepAnimationSequenceFunnelCakeNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteFunnelCakeStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceFunnelCakeWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteFunnelCakeStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceFunnelCakeEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteFunnelCakeStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceFunnelCakeSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteFunnelCakeStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceFunnelCakeSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Noodles sprites
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequenceNoodlesNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceNoodlesWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceNoodlesEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceNoodlesSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceNoodlesSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for Noodles sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupNoodles = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteNoodlesStateNoneId,        {  8, 16,  5 }, kPeepAnimationSequenceNoodlesNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteNoodlesStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceNoodlesWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteNoodlesStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceNoodlesEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteNoodlesStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceNoodlesSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteNoodlesStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceNoodlesSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Sausage sprites
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequenceSausageNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceSausageWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceSausageEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceSausageSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceSausageSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for Sausage sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupSausage = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteSausageStateNoneId,        {  8, 16,  5 }, kPeepAnimationSequenceSausageNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteSausageStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceSausageWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteSausageStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceSausageEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteSausageStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceSausageSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteSausageStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceSausageSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Soup sprites
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequenceSoupNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceSoupWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceSoupEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceSoupSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceSoupSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for Soup sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupSoup = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteSoupStateNoneId,        {  8, 16,  5 }, kPeepAnimationSequenceSoupNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteSoupStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceSoupWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteSoupStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceSoupEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteSoupStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceSoupSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteSoupStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceSoupSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Sandwich sprites
    static constexpr std::array<uint8_t, 48> kPeepAnimationSequenceSandwichNone           = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceSandwichWatchRide      = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceSandwichEatFood        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  1> kPeepAnimationSequenceSandwichSittingIdle    = { 0 };
    static constexpr std::array<uint8_t, 12> kPeepAnimationSequenceSandwichSittingEatFood = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    // Define animation group for Sandwich sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupSandwich = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepAnimationType::None]           = { kPeepSpriteSandwichStateNoneId,        {  8, 16,  5 }, kPeepAnimationSequenceSandwichNone };
        pag[PeepAnimationType::WatchRide]      = { kPeepSpriteSandwichStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceSandwichWatchRide };
        pag[PeepAnimationType::EatFood]        = { kPeepSpriteSandwichStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceSandwichEatFood };
        pag[PeepAnimationType::SittingIdle]    = { kPeepSpriteSandwichStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceSandwichSittingIdle };
        pag[PeepAnimationType::SittingEatFood] = { kPeepSpriteSandwichStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceSandwichSittingEatFood };
        return pag;
    }();

    // clang-format on
    // rct2: 0x00982708
    static std::array kPeepAnimationEntries = {
        kPeepAnimationGroupNormal,
        kPeepAnimationGroupHandyman,
        kPeepAnimationGroupMechanic,
        kPeepAnimationGroupSecurity,
        kPeepAnimationGroupEntertainerPanda,
        kPeepAnimationGroupEntertainerTiger,
        kPeepAnimationGroupEntertainerElephant,
        kPeepAnimationGroupEntertainerRoman,
        kPeepAnimationGroupEntertainerGorilla,
        kPeepAnimationGroupEntertainerSnowman,
        kPeepAnimationGroupEntertainerKnight,
        kPeepAnimationGroupEntertainerAstronaut,
        kPeepAnimationGroupEntertainerBandit,
        kPeepAnimationGroupEntertainerSheriff,
        kPeepAnimationGroupEntertainerPirate,
        kPeepAnimationGroupIceCream,
        kPeepAnimationGroupChips,
        kPeepAnimationGroupBurger,
        kPeepAnimationGroupDrink,
        kPeepAnimationGroupBalloon,
        kPeepAnimationGroupCandyfloss,
        kPeepAnimationGroupUmbrella,
        kPeepAnimationGroupPizza,
        kPeepAnimationGroupSecurityAlt,
        kPeepAnimationGroupPopcorn,
        kPeepAnimationGroupArmsCrossed,
        kPeepAnimationGroupHeadDown,
        kPeepAnimationGroupNauseous,
        kPeepAnimationGroupVeryNauseous,
        kPeepAnimationGroupRequireToilet,
        kPeepAnimationGroupHat,
        kPeepAnimationGroupHotDog,
        kPeepAnimationGroupTentacle,
        kPeepAnimationGroupToffeeApple,
        kPeepAnimationGroupDonut,
        kPeepAnimationGroupCoffee,
        kPeepAnimationGroupChicken,
        kPeepAnimationGroupLemonade,
        kPeepAnimationGroupWatching,
        kPeepAnimationGroupPretzel,
        kPeepAnimationGroupSunglasses,
        kPeepAnimationGroupSuJongkwa,
        kPeepAnimationGroupJuice,
        kPeepAnimationGroupFunnelCake,
        kPeepAnimationGroupNoodles,
        kPeepAnimationGroupSausage,
        kPeepAnimationGroupSoup,
        kPeepAnimationGroupSandwich,
    };

    const PeepAnimation& GetPeepAnimation(PeepSpriteType spriteType, PeepAnimationType actionSpriteType)
    {
        return kPeepAnimationEntries[EnumValue(spriteType)][actionSpriteType];
    }

    const SpriteBounds& GetSpriteBounds(PeepSpriteType spriteType, PeepAnimationType actionSpriteType)
    {
        return kPeepAnimationEntries[EnumValue(spriteType)][actionSpriteType].bounds;
    }
} // namespace OpenRCT2
