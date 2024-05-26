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
        pag[PeepActionSpriteType::None]                   = { kPeepSpriteNormalStateNoneId,                   {  8, 16,  5 }, kPeepAnimationSequenceNormalNone };
        pag[PeepActionSpriteType::CheckTime]              = { kPeepSpriteNormalStateCheckTimeId,              {  8, 16,  5 }, kPeepAnimationSequenceNormalCheckTime };
        pag[PeepActionSpriteType::WatchRide]              = { kPeepSpriteNormalStateWatchRideId,              {  8, 16,  5 }, kPeepAnimationSequenceNormalWatchRide };
        pag[PeepActionSpriteType::EatFood]                = { kPeepSpriteNormalStateEatFoodId,                {  8, 16,  5 }, kPeepAnimationSequenceNormalEatFood };
        pag[PeepActionSpriteType::ShakeHead]              = { kPeepSpriteNormalStateShakeHeadId,              {  8, 16,  5 }, kPeepAnimationSequenceNormalShakeHead };
        pag[PeepActionSpriteType::EmptyPockets]           = { kPeepSpriteNormalStateEmptyPocketsId,           {  8, 16,  5 }, kPeepAnimationSequenceNormalEmptyPockets };
        pag[PeepActionSpriteType::HoldMat]                = { kPeepSpriteNormalStateHoldMatId,                {  9, 16,  5 }, kPeepAnimationSequenceNormalHoldMat };
        pag[PeepActionSpriteType::SittingIdle]            = { kPeepSpriteNormalStateSittingIdleId,            {  9, 16,  6 }, kPeepAnimationSequenceNormalSittingIdle };
        pag[PeepActionSpriteType::SittingEatFood]         = { kPeepSpriteNormalStateSittingEatFoodId,         {  9, 16,  6 }, kPeepAnimationSequenceNormalSittingEatFood };
        pag[PeepActionSpriteType::SittingLookAroundLeft]  = { kPeepSpriteNormalStateSittingLookAroundLeftId,  {  9, 16,  6 }, kPeepAnimationSequenceNormalSittingLookAroundLeft };
        pag[PeepActionSpriteType::SittingLookAroundRight] = { kPeepSpriteNormalStateSittingLookAroundRightId, {  9, 16,  6 }, kPeepAnimationSequenceNormalSittingLookAroundRight };
        pag[PeepActionSpriteType::Ui]                     = { kPeepSpriteNormalStateUiId,                     {  8, 16,  5 }, kPeepAnimationSequenceNormalUi };
        pag[PeepActionSpriteType::Wow]                    = { kPeepSpriteNormalStateWowId,                    { 12, 22,  5 }, kPeepAnimationSequenceNormalWow };
        pag[PeepActionSpriteType::ThrowUp]                = { kPeepSpriteNormalStateThrowUpId,                {  9, 16,  7 }, kPeepAnimationSequenceNormalThrowUp };
        pag[PeepActionSpriteType::Jump]                   = { kPeepSpriteNormalStateJumpId,                   { 10, 22,  5 }, kPeepAnimationSequenceNormalJump };
        pag[PeepActionSpriteType::Drowning]               = { kPeepSpriteNormalStateDrowningId,               {  9, 15,  6 }, kPeepAnimationSequenceNormalDrowning };
        pag[PeepActionSpriteType::Joy]                    = { kPeepSpriteNormalStateJoyId,                    { 11, 24,  6 }, kPeepAnimationSequenceNormalJoy };
        pag[PeepActionSpriteType::ReadMap]                = { kPeepSpriteNormalStateReadMapId,                { 11, 16,  5 }, kPeepAnimationSequenceNormalReadMap };
        pag[PeepActionSpriteType::Wave]                   = { kPeepSpriteNormalStateWaveId,                   { 11, 16,  5 }, kPeepAnimationSequenceNormalWave };
        pag[PeepActionSpriteType::Wave2]                  = { kPeepSpriteNormalStateWave2Id,                  { 11, 16,  5 }, kPeepAnimationSequenceNormalWave2 };
        pag[PeepActionSpriteType::TakePhoto]              = { kPeepSpriteNormalStateTakePhotoId,              {  8, 16,  5 }, kPeepAnimationSequenceNormalTakePhoto };
        pag[PeepActionSpriteType::Clap]                   = { kPeepSpriteNormalStateClapId,                   {  9, 17,  6 }, kPeepAnimationSequenceNormalClap };
        pag[PeepActionSpriteType::Disgust]                = { kPeepSpriteNormalStateDisgustId,                {  9, 16,  5 }, kPeepAnimationSequenceNormalDisgust };
        pag[PeepActionSpriteType::DrawPicture]            = { kPeepSpriteNormalStateDrawPictureId,            {  9, 22,  7 }, kPeepAnimationSequenceNormalDrawPicture };
        pag[PeepActionSpriteType::BeingWatched]           = { kPeepSpriteNormalStateBeingWatchedId,           {  9, 22,  7 }, kPeepAnimationSequenceNormalBeingWatched };
        pag[PeepActionSpriteType::WithdrawMoney]          = { kPeepSpriteNormalStateWithdrawMoneyId,          {  9, 22,  7 }, kPeepAnimationSequenceNormalWithdrawMoney };
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
        pag[PeepActionSpriteType::None]          = { kHandymanSpriteStateNoneId,          { 12, 16,  6 }, kPeepAnimationSequenceHandymanNone };
        pag[PeepActionSpriteType::WatchRide]     = { kHandymanSpriteStateWatchRideId,     {  9, 16,  6 }, kPeepAnimationSequenceHandymanWatchRide };
        pag[PeepActionSpriteType::Ui]            = { kHandymanSpriteStateUiId,            { 15, 16,  5 }, kPeepAnimationSequenceHandymanUi };
        pag[PeepActionSpriteType::StaffMower]    = { kHandymanSpriteStateStaffMowerId,    { 18, 16, 11 }, kPeepAnimationSequenceHandymanStaffMower };
        pag[PeepActionSpriteType::StaffSweep]    = { kHandymanSpriteStateStaffSweepId,    { 17, 16,  9 }, kPeepAnimationSequenceHandymanStaffSweep };
        pag[PeepActionSpriteType::Drowning]      = { kHandymanSpriteStateDrowningId,      {  9, 15,  6 }, kPeepAnimationSequenceHandymanDrowning };
        pag[PeepActionSpriteType::StaffWatering] = { kHandymanSpriteStateStaffWateringId, { 17, 16,  9 }, kPeepAnimationSequenceHandymanStaffWatering };
        pag[PeepActionSpriteType::StaffEmptyBin] = { kHandymanSpriteStateStaffEmptyBinId, { 17, 16,  9 }, kPeepAnimationSequenceHandymanStaffEmptyBin };
        return pag;
    }();

    // Define animation sequences for Mechanic sprites
    static constexpr std::array<uint8_t,   6> kPeepAnimationSequenceMechanicNone             = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t,   1> kPeepAnimationSequenceMechanicWatchRide        = { 0 };
    static constexpr std::array<uint8_t,  12> kPeepAnimationSequenceMechanicUi               = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr std::array<uint8_t,  65> kPeepAnimationSequenceMechanicDrowning         = { 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 4, 5, 6, 7 };
    static constexpr std::array<uint8_t,  85> kPeepAnimationSequenceMechanicStaffAnswerCall  = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t,  59> kPeepAnimationSequenceMechanicStaffAnswerCall2 = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t,  43> kPeepAnimationSequenceMechanicStaffCheckboard  = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t,  53> kPeepAnimationSequenceMechanicStaffFix         = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t,  99> kPeepAnimationSequenceMechanicStaffFix2        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
    static constexpr std::array<uint8_t,  61> kPeepAnimationSequenceMechanicStaffFixGround   = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    static constexpr std::array<uint8_t, 110> kPeepAnimationSequenceMechanicStaffFix3        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };

    // Define animation group for Mechanic sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupMechanic = []() {
        PeepAnimationGroup pag;
        pag[PeepActionSpriteType::None]             = { kMechanicSpriteStateNoneId,            { 10, 16,  5 }, kPeepAnimationSequenceMechanicNone };
        pag[PeepActionSpriteType::WatchRide]        = { kMechanicSpriteStateWatchRideId,       { 10, 16,  5 }, kPeepAnimationSequenceMechanicWatchRide };
        pag[PeepActionSpriteType::Ui]               = { kMechanicSpriteStateUiId,              { 10, 16,  5 }, kPeepAnimationSequenceMechanicUi };
        pag[PeepActionSpriteType::Drowning]         = { kMechanicSpriteStateDrowningId,        {  9, 15,  6 }, kPeepAnimationSequenceMechanicDrowning };
        pag[PeepActionSpriteType::StaffAnswerCall]  = { kMechanicSpriteStateStaffAnswerCallId, { 13, 22,  7 }, kPeepAnimationSequenceMechanicStaffAnswerCall };
        pag[PeepActionSpriteType::StaffAnswerCall2] = { kMechanicSpriteStateStaffAnswerCallId, { 13, 22,  7 }, kPeepAnimationSequenceMechanicStaffAnswerCall2 };
        pag[PeepActionSpriteType::StaffCheckboard]  = { kMechanicSpriteStateStaffCheckboardId, { 13, 22,  7 }, kPeepAnimationSequenceMechanicStaffCheckboard };
        pag[PeepActionSpriteType::StaffFix]         = { kMechanicSpriteStateStaffFixId,        { 13, 22,  7 }, kPeepAnimationSequenceMechanicStaffFix };
        pag[PeepActionSpriteType::StaffFix2]        = { kMechanicSpriteStateStaffFixId,        { 13, 22,  7 }, kPeepAnimationSequenceMechanicStaffFix2 };
        pag[PeepActionSpriteType::StaffFixGround]   = { kMechanicSpriteStateStaffFixGroundId,  { 19, 16, 16 }, kPeepAnimationSequenceMechanicStaffFixGround };
        pag[PeepActionSpriteType::StaffFix3]        = { kMechanicSpriteStateStaffFixId,        { 13, 22,  7 }, kPeepAnimationSequenceMechanicStaffFix3 };
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
        pag[PeepActionSpriteType::None]      = { kSecuritySpriteStateNoneId,      {  8, 18,  5 }, kPeepAnimationSequenceSecurityNone };
        pag[PeepActionSpriteType::WatchRide] = { kSecuritySpriteStateWatchRideId, {  8, 17,  5 }, kPeepAnimationSequenceSecurityWatchRide };
        pag[PeepActionSpriteType::Ui]        = { kSecuritySpriteStateUiId,        { 15, 19,  6 }, kPeepAnimationSequenceSecurityUi };
        pag[PeepActionSpriteType::Drowning]  = { kSecuritySpriteStateDrowningId,  {  9, 15,  6 }, kPeepAnimationSequenceSecurityDrowning };
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
        pag[PeepActionSpriteType::None]      = { kEntertainerSpritePandaStateNoneId,      { 13, 24,  8 }, kPeepAnimationSequenceEntertainerPandaNone };
        pag[PeepActionSpriteType::WatchRide] = { kEntertainerSpritePandaStateWatchRideId, { 10, 23,  7 }, kPeepAnimationSequenceEntertainerPandaWatchRide };
        pag[PeepActionSpriteType::EatFood]   = { kEntertainerSpritePandaStateWaveId,      { 14, 24,  7 }, kPeepAnimationSequenceEntertainerPandaEatFood };
        pag[PeepActionSpriteType::Ui]        = { kEntertainerSpritePandaStateUiId,        { 19, 30,  8 }, kPeepAnimationSequenceEntertainerPandaUi };
        pag[PeepActionSpriteType::Drowning]  = { kEntertainerSpritePandaStateDrowningId,  { 13, 15,  6 }, kPeepAnimationSequenceEntertainerPandaDrowning };
        pag[PeepActionSpriteType::Joy]       = { kEntertainerSpritePandaStateJoyId,       { 14, 25,  8 }, kPeepAnimationSequenceEntertainerPandaJoy };
        pag[PeepActionSpriteType::Wave2]     = { kEntertainerSpritePandaStateWaveId,      { 14, 24,  7 }, kPeepAnimationSequenceEntertainerPandaWave2 };
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
        pag[PeepActionSpriteType::None]      = { kEntertainerSpriteTigerStateNoneId,      { 13, 24,  8 }, kPeepAnimationSequenceEntertainerTigerNone };
        pag[PeepActionSpriteType::WatchRide] = { kEntertainerSpriteTigerStateWatchRideId, { 10, 23,  7 }, kPeepAnimationSequenceEntertainerTigerWatchRide };
        pag[PeepActionSpriteType::EatFood]   = { kEntertainerSpriteTigerStateWaveId,      { 16, 24,  8 }, kPeepAnimationSequenceEntertainerTigerEatFood };
        pag[PeepActionSpriteType::Ui]        = { kEntertainerSpriteTigerStateUiId,        { 23, 30,  8 }, kPeepAnimationSequenceEntertainerTigerUi };
        pag[PeepActionSpriteType::Drowning]  = { kEntertainerSpriteTigerStateDrowningId,  { 13, 15,  6 }, kPeepAnimationSequenceEntertainerTigerDrowning };
        pag[PeepActionSpriteType::Joy]       = { kEntertainerSpriteTigerStateJoyId,       { 16, 28,  9 }, kPeepAnimationSequenceEntertainerTigerJoy };
        pag[PeepActionSpriteType::Wave2]     = { kEntertainerSpriteTigerStateWaveId,      { 16, 24,  8 }, kPeepAnimationSequenceEntertainerTigerWave2 };
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
        pag[PeepActionSpriteType::None]      = { kEntertainerSpriteElephantStateNoneId,      { 13, 24,  8 }, kPeepAnimationSequenceEntertainerElephantNone };
        pag[PeepActionSpriteType::WatchRide] = { kEntertainerSpriteElephantStateWatchRideId, { 10, 23,  7 }, kPeepAnimationSequenceEntertainerElephantWatchRide };
        pag[PeepActionSpriteType::EatFood]   = { kEntertainerSpriteElephantStateWaveId,      { 17, 24,  8 }, kPeepAnimationSequenceEntertainerElephantEatFood };
        pag[PeepActionSpriteType::Ui]        = { kEntertainerSpriteElephantStateUiId,        { 23, 30,  8 }, kPeepAnimationSequenceEntertainerElephantUi };
        pag[PeepActionSpriteType::Drowning]  = { kEntertainerSpriteElephantStateDrowningId,  { 17, 15,  6 }, kPeepAnimationSequenceEntertainerElephantDrowning };
        pag[PeepActionSpriteType::Joy]       = { kEntertainerSpriteElephantStateJoyId,       { 18, 25,  9 }, kPeepAnimationSequenceEntertainerElephantJoy };
        pag[PeepActionSpriteType::Wave2]     = { kEntertainerSpriteElephantStateWaveId,      { 17, 24,  8 }, kPeepAnimationSequenceEntertainerElephantWave2 };
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
        pag[PeepActionSpriteType::None]      = { kEntertainerSpriteRomanStateNoneId,      { 13, 24,  8 }, kPeepAnimationSequenceEntertainerRomanNone };
        pag[PeepActionSpriteType::WatchRide] = { kEntertainerSpriteRomanStateWatchRideId, { 10, 23,  7 }, kPeepAnimationSequenceEntertainerRomanWatchRide };
        pag[PeepActionSpriteType::EatFood]   = { kEntertainerSpriteRomanStateWaveId,      { 17, 24,  8 }, kPeepAnimationSequenceEntertainerRomanEatFood };
        pag[PeepActionSpriteType::Ui]        = { kEntertainerSpriteRomanStateUiId,        { 23, 30,  8 }, kPeepAnimationSequenceEntertainerRomanUi };
        pag[PeepActionSpriteType::Drowning]  = { kEntertainerSpriteRomanStateDrowningId,  { 17, 15,  6 }, kPeepAnimationSequenceEntertainerRomanDrowning };
        pag[PeepActionSpriteType::Joy]       = { kEntertainerSpriteRomanStateJoyId,       { 18, 25,  9 }, kPeepAnimationSequenceEntertainerRomanJoy };
        pag[PeepActionSpriteType::Wave2]     = { kEntertainerSpriteRomanStateWaveId,      { 17, 24,  8 }, kPeepAnimationSequenceEntertainerRomanWave2 };
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
        pag[PeepActionSpriteType::None]      = { kEntertainerSpriteGorillaStateNoneId,      { 13, 24,  8 }, kPeepAnimationSequenceEntertainerGorillaNone };
        pag[PeepActionSpriteType::WatchRide] = { kEntertainerSpriteGorillaStateWatchRideId, { 10, 23,  7 }, kPeepAnimationSequenceEntertainerGorillaWatchRide };
        pag[PeepActionSpriteType::EatFood]   = { kEntertainerSpriteGorillaStateWaveId,      { 17, 24,  8 }, kPeepAnimationSequenceEntertainerGorillaEatFood };
        pag[PeepActionSpriteType::Ui]        = { kEntertainerSpriteGorillaStateUiId,        { 23, 30,  8 }, kPeepAnimationSequenceEntertainerGorillaUi };
        pag[PeepActionSpriteType::Drowning]  = { kEntertainerSpriteGorillaStateDrowningId,  { 17, 15,  6 }, kPeepAnimationSequenceEntertainerGorillaDrowning };
        pag[PeepActionSpriteType::Joy]       = { kEntertainerSpriteGorillaStateWaveId,      { 18, 25,  9 }, kPeepAnimationSequenceEntertainerGorillaJoy };
        pag[PeepActionSpriteType::Wave2]     = { kEntertainerSpriteGorillaStateWaveId,      { 17, 24,  8 }, kPeepAnimationSequenceEntertainerGorillaWave2 };
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
        pag[PeepActionSpriteType::None]      = { kEntertainerSpriteSnowmanStateNoneId,      { 13, 24,  8 }, kPeepAnimationSequenceEntertainerSnowmanNone };
        pag[PeepActionSpriteType::WatchRide] = { kEntertainerSpriteSnowmanStateWatchRideId, { 10, 23,  7 }, kPeepAnimationSequenceEntertainerSnowmanWatchRide };
        pag[PeepActionSpriteType::EatFood]   = { kEntertainerSpriteSnowmanStateWaveId,      { 17, 28,  9 }, kPeepAnimationSequenceEntertainerSnowmanEatFood };
        pag[PeepActionSpriteType::Ui]        = { kEntertainerSpriteSnowmanStateUiId,        { 23, 30,  8 }, kPeepAnimationSequenceEntertainerSnowmanUi };
        pag[PeepActionSpriteType::Drowning]  = { kEntertainerSpriteSnowmanStateDrowningId,  { 17, 15,  9 }, kPeepAnimationSequenceEntertainerSnowmanDrowning };
        pag[PeepActionSpriteType::Joy]       = { kEntertainerSpriteSnowmanStateWaveId,      { 18, 28,  9 }, kPeepAnimationSequenceEntertainerSnowmanJoy };
        pag[PeepActionSpriteType::Wave2]     = { kEntertainerSpriteSnowmanStateWaveId,      { 17, 28,  9 }, kPeepAnimationSequenceEntertainerSnowmanWave2 };
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
        pag[PeepActionSpriteType::None]      = { kEntertainerSpriteKnightStateNoneId,      { 16, 32,  8 }, kPeepAnimationSequenceEntertainerKnightNone };
        pag[PeepActionSpriteType::WatchRide] = { kEntertainerSpriteKnightStateWatchRideId, { 10, 23,  7 }, kPeepAnimationSequenceEntertainerKnightWatchRide };
        pag[PeepActionSpriteType::EatFood]   = { kEntertainerSpriteKnightStateWaveId,      { 23, 30, 15 }, kPeepAnimationSequenceEntertainerKnightEatFood };
        pag[PeepActionSpriteType::Ui]        = { kEntertainerSpriteKnightStateUiId,        { 23, 30,  8 }, kPeepAnimationSequenceEntertainerKnightUi };
        pag[PeepActionSpriteType::Drowning]  = { kEntertainerSpriteKnightStateDrowningId,  { 21, 32,  9 }, kPeepAnimationSequenceEntertainerKnightDrowning };
        pag[PeepActionSpriteType::Joy]       = { kEntertainerSpriteKnightStateWaveId,      { 23, 30, 15 }, kPeepAnimationSequenceEntertainerKnightJoy };
        pag[PeepActionSpriteType::Wave2]     = { kEntertainerSpriteKnightStateWaveId,      { 23, 30, 15 }, kPeepAnimationSequenceEntertainerKnightWave2 };
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
        pag[PeepActionSpriteType::None]      = { kEntertainerSpriteAstronautStateNoneId,      { 16, 32,  8 }, kPeepAnimationSequenceEntertainerAstronautNone };
        pag[PeepActionSpriteType::WatchRide] = { kEntertainerSpriteAstronautStateWatchRideId, { 10, 23,  7 }, kPeepAnimationSequenceEntertainerAstronautWatchRide };
        pag[PeepActionSpriteType::EatFood]   = { kEntertainerSpriteAstronautStateWaveId,      { 23, 30, 15 }, kPeepAnimationSequenceEntertainerAstronautEatFood };
        pag[PeepActionSpriteType::Ui]        = { kEntertainerSpriteAstronautStateUiId,        { 23, 30,  8 }, kPeepAnimationSequenceEntertainerAstronautUi };
        pag[PeepActionSpriteType::Drowning]  = { kEntertainerSpriteAstronautStateDrowningId,  { 21, 32,  9 }, kPeepAnimationSequenceEntertainerAstronautDrowning };
        pag[PeepActionSpriteType::Joy]       = { kEntertainerSpriteAstronautStateWaveId,      { 23, 30, 15 }, kPeepAnimationSequenceEntertainerAstronautJoy };
        pag[PeepActionSpriteType::Wave2]     = { kEntertainerSpriteAstronautStateWaveId,      { 23, 30, 15 }, kPeepAnimationSequenceEntertainerAstronautWave2 };
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
        pag[PeepActionSpriteType::None]      = { kEntertainerSpriteBanditStateNoneId,      { 16, 32,  8 }, kPeepAnimationSequenceEntertainerBanditNone };
        pag[PeepActionSpriteType::WatchRide] = { kEntertainerSpriteBanditStateWatchRideId, { 10, 23,  7 }, kPeepAnimationSequenceEntertainerBanditWatchRide };
        pag[PeepActionSpriteType::EatFood]   = { kEntertainerSpriteBanditStateWaveId,      { 23, 30, 15 }, kPeepAnimationSequenceEntertainerBanditEatFood };
        pag[PeepActionSpriteType::Ui]        = { kEntertainerSpriteBanditStateUiId,        { 23, 30,  8 }, kPeepAnimationSequenceEntertainerBanditUi };
        pag[PeepActionSpriteType::Drowning]  = { kEntertainerSpriteBanditStateDrowningId,  { 21, 32,  9 }, kPeepAnimationSequenceEntertainerBanditDrowning };
        pag[PeepActionSpriteType::Joy]       = { kEntertainerSpriteBanditStateWaveId,      { 23, 30, 15 }, kPeepAnimationSequenceEntertainerBanditJoy };
        pag[PeepActionSpriteType::Wave2]     = { kEntertainerSpriteBanditStateWaveId,      { 23, 30, 15 }, kPeepAnimationSequenceEntertainerBanditWave2 };
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
        pag[PeepActionSpriteType::None]      = { kEntertainerSpriteSheriffStateNoneId,      { 16, 32,  8 }, kPeepAnimationSequenceEntertainerSheriffNone };
        pag[PeepActionSpriteType::WatchRide] = { kEntertainerSpriteSheriffStateWatchRideId, { 10, 23,  7 }, kPeepAnimationSequenceEntertainerSheriffWatchRide };
        pag[PeepActionSpriteType::EatFood]   = { kEntertainerSpriteSheriffStateWaveId,      { 23, 30, 15 }, kPeepAnimationSequenceEntertainerSheriffEatFood };
        pag[PeepActionSpriteType::Ui]        = { kEntertainerSpriteSheriffStateUiId,        { 23, 30,  8 }, kPeepAnimationSequenceEntertainerSheriffUi };
        pag[PeepActionSpriteType::Drowning]  = { kEntertainerSpriteSheriffStateDrowningId,  { 21, 32,  9 }, kPeepAnimationSequenceEntertainerSheriffDrowning };
        pag[PeepActionSpriteType::Joy]       = { kEntertainerSpriteSheriffStateWaveId,      { 23, 30, 15 }, kPeepAnimationSequenceEntertainerSheriffJoy };
        pag[PeepActionSpriteType::Wave2]     = { kEntertainerSpriteSheriffStateWaveId,      { 23, 30, 15 }, kPeepAnimationSequenceEntertainerSheriffWave2 };
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
        pag[PeepActionSpriteType::None]      = { kEntertainerSpritePirateStateNoneId,      { 16, 32,  8 }, kPeepAnimationSequenceEntertainerPirateNone };
        pag[PeepActionSpriteType::WatchRide] = { kEntertainerSpritePirateStateWatchRideId, { 10, 23,  7 }, kPeepAnimationSequenceEntertainerPirateWatchRide };
        pag[PeepActionSpriteType::EatFood]   = { kEntertainerSpritePirateStateWaveId,      { 23, 30, 15 }, kPeepAnimationSequenceEntertainerPirateEatFood };
        pag[PeepActionSpriteType::Ui]        = { kEntertainerSpritePirateStateUiId,        { 23, 30,  8 }, kPeepAnimationSequenceEntertainerPirateUi };
        pag[PeepActionSpriteType::Drowning]  = { kEntertainerSpritePirateStateDrowningId,  { 21, 32,  9 }, kPeepAnimationSequenceEntertainerPirateDrowning };
        pag[PeepActionSpriteType::Joy]       = { kEntertainerSpritePirateStateWaveId,      { 23, 30, 15 }, kPeepAnimationSequenceEntertainerPirateJoy };
        pag[PeepActionSpriteType::Wave2]     = { kEntertainerSpritePirateStateWaveId,      { 23, 30, 15 }, kPeepAnimationSequenceEntertainerPirateWave2 };
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
        pag[PeepActionSpriteType::None]           = { kPeepSpriteIceCreamStateNoneId,           {  8, 16,  5 }, kPeepAnimationSequenceIceCreamNone };
        pag[PeepActionSpriteType::WatchRide]      = { kPeepSpriteIceCreamStateWatchRideId,      {  8, 16,  5 }, kPeepAnimationSequenceIceCreamWatchRide };
        pag[PeepActionSpriteType::EatFood]        = { kPeepSpriteIceCreamStateEatFoodId,        {  8, 16,  5 }, kPeepAnimationSequenceIceCreamEatFood };
        pag[PeepActionSpriteType::SittingIdle]    = { kPeepSpriteIceCreamStateSittingIdleId,    {  9, 16,  6 }, kPeepAnimationSequenceIceCreamSittingIdle };
        pag[PeepActionSpriteType::SittingEatFood] = { kPeepSpriteIceCreamStateSittingEatFoodId, {  9, 16,  6 }, kPeepAnimationSequenceIceCreamSittingEatFood };
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
        pag[PeepActionSpriteType::None]           = { kPeepSpriteChipsStateNoneId,           {  8, 16,  5 }, kPeepAnimationSequenceChipsNone };
        pag[PeepActionSpriteType::WatchRide]      = { kPeepSpriteChipsStateWatchRideId,      {  8, 16,  5 }, kPeepAnimationSequenceChipsWatchRide };
        pag[PeepActionSpriteType::EatFood]        = { kPeepSpriteChipsStateEatFoodId,        {  8, 16,  5 }, kPeepAnimationSequenceChipsEatFood };
        pag[PeepActionSpriteType::SittingIdle]    = { kPeepSpriteChipsStateSittingIdleId,    {  9, 16,  6 }, kPeepAnimationSequenceChipsSittingIdle };
        pag[PeepActionSpriteType::SittingEatFood] = { kPeepSpriteChipsStateSittingEatFoodId, {  9, 16,  6 }, kPeepAnimationSequenceChipsSittingEatFood };
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
        pag[PeepActionSpriteType::None]           = { kPeepSpriteBurgerStateNoneId,           {  8, 16,  5 }, kPeepAnimationSequenceBurgerNone };
        pag[PeepActionSpriteType::WatchRide]      = { kPeepSpriteBurgerStateWatchRideId,      {  8, 16,  5 }, kPeepAnimationSequenceBurgerWatchRide };
        pag[PeepActionSpriteType::EatFood]        = { kPeepSpriteBurgerStateEatFoodId,        {  8, 16,  5 }, kPeepAnimationSequenceBurgerEatFood };
        pag[PeepActionSpriteType::SittingIdle]    = { kPeepSpriteBurgerStateSittingIdleId,    {  9, 16,  6 }, kPeepAnimationSequenceBurgerSittingIdle };
        pag[PeepActionSpriteType::SittingEatFood] = { kPeepSpriteBurgerStateSittingEatFoodId, {  9, 16,  6 }, kPeepAnimationSequenceBurgerSittingEatFood };
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
        pag[PeepActionSpriteType::None]           = { kPeepSpriteDrinkStateNoneId,           {  8, 16,  5 }, kPeepAnimationSequenceDrinkNone };
        pag[PeepActionSpriteType::WatchRide]      = { kPeepSpriteDrinkStateWatchRideId,      {  8, 16,  5 }, kPeepAnimationSequenceDrinkWatchRide };
        pag[PeepActionSpriteType::EatFood]        = { kPeepSpriteDrinkStateEatFoodId,        {  8, 16,  5 }, kPeepAnimationSequenceDrinkEatFood };
        pag[PeepActionSpriteType::SittingIdle]    = { kPeepSpriteDrinkStateSittingIdleId,    {  9, 16,  6 }, kPeepAnimationSequenceDrinkSittingIdle };
        pag[PeepActionSpriteType::SittingEatFood] = { kPeepSpriteDrinkStateSittingEatFoodId, {  9, 16,  6 }, kPeepAnimationSequenceDrinkSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Balloon sprites
    static constexpr std::array<uint8_t, 6> kPeepAnimationSequenceBalloonNone        = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceBalloonWatchRide   = { 0 };
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceBalloonSittingIdle = { 0 };

    // Define animation group for Balloon sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupBalloon = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepActionSpriteType::None]        = { kPeepSpriteBalloonStateNoneId,        { 11, 28,  5 }, kPeepAnimationSequenceBalloonNone };
        pag[PeepActionSpriteType::WatchRide]   = { kPeepSpriteBalloonStateWatchRideId,   { 11, 28,  5 }, kPeepAnimationSequenceBalloonWatchRide };
        pag[PeepActionSpriteType::SittingIdle] = { kPeepSpriteBalloonStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceBalloonSittingIdle };
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
        pag[PeepActionSpriteType::None]           = { kPeepSpriteCandyflossStateNoneId,           { 11, 19,  5 }, kPeepAnimationSequenceCandyflossNone };
        pag[PeepActionSpriteType::WatchRide]      = { kPeepSpriteCandyflossStateWatchRideId,      { 11, 19,  5 }, kPeepAnimationSequenceCandyflossWatchRide };
        pag[PeepActionSpriteType::EatFood]        = { kPeepSpriteCandyflossStateEatFoodId,        { 11, 19,  5 }, kPeepAnimationSequenceCandyflossEatFood };
        pag[PeepActionSpriteType::SittingIdle]    = { kPeepSpriteCandyflossStateSittingIdleId,    { 13, 16,  6 }, kPeepAnimationSequenceCandyflossSittingIdle };
        pag[PeepActionSpriteType::SittingEatFood] = { kPeepSpriteCandyflossStateSittingEatFoodId, { 13, 16,  6 }, kPeepAnimationSequenceCandyflossSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Umbrella sprites
    static constexpr std::array<uint8_t, 6> kPeepAnimationSequenceUmbrellaNone        = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceUmbrellaWatchRide   = { 0 };
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceUmbrellaSittingIdle = { 0 };

    // Define animation group for Umbrella sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupUmbrella = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepActionSpriteType::None]        = { kPeepSpriteUmbrellaStateNoneId,        { 14, 21,  5 }, kPeepAnimationSequenceUmbrellaNone };
        pag[PeepActionSpriteType::WatchRide]   = { kPeepSpriteUmbrellaStateWatchRideId,   { 14, 21,  5 }, kPeepAnimationSequenceUmbrellaWatchRide };
        pag[PeepActionSpriteType::SittingIdle] = { kPeepSpriteUmbrellaStateSittingIdleId, { 14, 19,  6 }, kPeepAnimationSequenceUmbrellaSittingIdle };
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
        pag[PeepActionSpriteType::None]           = { kPeepSpritePizzaStateNoneId,           {  8, 16,  5 }, kPeepAnimationSequencePizzaNone };
        pag[PeepActionSpriteType::WatchRide]      = { kPeepSpritePizzaStateWatchRideId,      {  8, 16,  5 }, kPeepAnimationSequencePizzaWatchRide };
        pag[PeepActionSpriteType::EatFood]        = { kPeepSpritePizzaStateEatFoodId,        {  8, 16,  5 }, kPeepAnimationSequencePizzaEatFood };
        pag[PeepActionSpriteType::SittingIdle]    = { kPeepSpritePizzaStateSittingIdleId,    {  9, 16,  6 }, kPeepAnimationSequencePizzaSittingIdle };
        pag[PeepActionSpriteType::SittingEatFood] = { kPeepSpritePizzaStateSittingEatFoodId, {  9, 16,  6 }, kPeepAnimationSequencePizzaSittingEatFood };
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
        pag[PeepActionSpriteType::None]      = { kSecurityStaffSpriteAltStateNoneId, {  8, 18,  5 }, kPeepAnimationSequenceSecurityAltNone };
        pag[PeepActionSpriteType::WatchRide] = { kSecuritySpriteStateWatchRideId,    {  8, 17,  5 }, kPeepAnimationSequenceSecurityAltWatchRide };
        pag[PeepActionSpriteType::Ui]        = { kSecuritySpriteStateUiId,           { 15, 19,  6 }, kPeepAnimationSequenceSecurityAltUi };
        pag[PeepActionSpriteType::Drowning]  = { kSecuritySpriteStateDrowningId,     {  9, 15,  6 }, kPeepAnimationSequenceSecurityAltDrowning };
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
        pag[PeepActionSpriteType::None]           = { kPeepSpritePopcornStateNoneId,           { 11, 19,  5 }, kPeepAnimationSequencePopcornNone };
        pag[PeepActionSpriteType::WatchRide]      = { kPeepSpritePopcornStateWatchRideId,      { 11, 19,  5 }, kPeepAnimationSequencePopcornWatchRide };
        pag[PeepActionSpriteType::EatFood]        = { kPeepSpritePopcornStateEatFoodId,        { 11, 19,  5 }, kPeepAnimationSequencePopcornEatFood };
        pag[PeepActionSpriteType::SittingIdle]    = { kPeepSpritePopcornStateSittingIdleId,    { 13, 16,  6 }, kPeepAnimationSequencePopcornSittingIdle };
        pag[PeepActionSpriteType::SittingEatFood] = { kPeepSpritePopcornStateSittingEatFoodId, { 13, 16,  6 }, kPeepAnimationSequencePopcornSittingEatFood };
        return pag;
    }();

    // Define animation sequences for ArmsCrossed sprites
    static constexpr std::array<uint8_t, 6> kPeepAnimationSequenceArmsCrossedNone = { 0, 1, 2, 3, 4, 5 };

    // Define animation group for ArmsCrossed sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupArmsCrossed = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepActionSpriteType::None] = { kPeepSpriteArmsCrossedStateNoneId, {  8, 16,  5 }, kPeepAnimationSequenceArmsCrossedNone };
        return pag;
    }();

    // Define animation sequences for HeadDown sprites
    static constexpr std::array<uint8_t, 6> kPeepAnimationSequenceHeadDownNone = { 0, 1, 2, 3, 4, 5 };

    // Define animation group for HeadDown sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupHeadDown = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepActionSpriteType::None] = { kPeepSpriteHeadDownStateNoneId, {  8, 16,  5 }, kPeepAnimationSequenceHeadDownNone };
        return pag;
    }();

    // Define animation sequences for Nauseous sprites
    static constexpr std::array<uint8_t, 6> kPeepAnimationSequenceNauseousNone = { 0, 1, 2, 3, 4, 5 };

    // Define animation group for Nauseous sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupNauseous = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepActionSpriteType::None] = { kPeepSpriteNauseousStateNoneId, {  8, 16,  5 }, kPeepAnimationSequenceNauseousNone };
        return pag;
    }();

    // Define animation sequences for VeryNauseous sprites
    static constexpr std::array<uint8_t, 6> kPeepAnimationSequenceVeryNauseousNone        = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceVeryNauseousWatchRide   = { 0 };
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceVeryNauseousSittingIdle = { 0 };

    // Define animation group for VeryNauseous sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupVeryNauseous = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepActionSpriteType::None]        = { kPeepSpriteVeryNauseousStateNoneId,        {  8, 16,  5 }, kPeepAnimationSequenceVeryNauseousNone };
        pag[PeepActionSpriteType::WatchRide]   = { kPeepSpriteVeryNauseousStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceVeryNauseousWatchRide };
        pag[PeepActionSpriteType::SittingIdle] = { kPeepSpriteVeryNauseousStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceVeryNauseousSittingIdle };
        return pag;
    }();

    // Define animation sequences for RequireToilet sprites
    static constexpr std::array<uint8_t, 6> kPeepAnimationSequenceRequireToiletNone = { 0, 1, 2, 3, 4, 5 };

    // Define animation group for RequireToilet sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupRequireToilet = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepActionSpriteType::None] = { kPeepSpriteRequireToiletStateNoneId, {  8, 16,  5 }, kPeepAnimationSequenceRequireToiletNone };
        return pag;
    }();

    // Define animation sequences for Hat sprites
    static constexpr std::array<uint8_t, 6> kPeepAnimationSequenceHatNone        = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceHatWatchRide   = { 0 };
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceHatSittingIdle = { 0 };

    // Define animation group for Hat sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupHat = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepActionSpriteType::None]        = { kPeepSpriteHatStateNoneId,        { 12, 32,  5 }, kPeepAnimationSequenceHatNone };
        pag[PeepActionSpriteType::WatchRide]   = { kPeepSpriteHatStateWatchRideId,   { 12, 32,  5 }, kPeepAnimationSequenceHatWatchRide };
        pag[PeepActionSpriteType::SittingIdle] = { kPeepSpriteHatStateSittingIdleId, { 10, 20,  6 }, kPeepAnimationSequenceHatSittingIdle };
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
        pag[PeepActionSpriteType::None]           = { kPeepSpriteHotDogStateNoneId,           {  8, 16,  5 }, kPeepAnimationSequenceHotDogNone };
        pag[PeepActionSpriteType::WatchRide]      = { kPeepSpriteHotDogStateWatchRideId,      {  8, 16,  5 }, kPeepAnimationSequenceHotDogWatchRide };
        pag[PeepActionSpriteType::EatFood]        = { kPeepSpriteHotDogStateEatFoodId,        {  8, 16,  5 }, kPeepAnimationSequenceHotDogEatFood };
        pag[PeepActionSpriteType::SittingIdle]    = { kPeepSpriteHotDogStateSittingIdleId,    {  9, 16,  6 }, kPeepAnimationSequenceHotDogSittingIdle };
        pag[PeepActionSpriteType::SittingEatFood] = { kPeepSpriteHotDogStateSittingEatFoodId, {  9, 16,  6 }, kPeepAnimationSequenceHotDogSittingEatFood };
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
        pag[PeepActionSpriteType::None]           = { kPeepSpriteTentacleStateNoneId,           {  8, 16,  5 }, kPeepAnimationSequenceTentacleNone };
        pag[PeepActionSpriteType::WatchRide]      = { kPeepSpriteTentacleStateWatchRideId,      {  8, 16,  5 }, kPeepAnimationSequenceTentacleWatchRide };
        pag[PeepActionSpriteType::EatFood]        = { kPeepSpriteTentacleStateEatFoodId,        {  8, 16,  5 }, kPeepAnimationSequenceTentacleEatFood };
        pag[PeepActionSpriteType::SittingIdle]    = { kPeepSpriteTentacleStateSittingIdleId,    {  9, 16,  6 }, kPeepAnimationSequenceTentacleSittingIdle };
        pag[PeepActionSpriteType::SittingEatFood] = { kPeepSpriteTentacleStateSittingEatFoodId, {  9, 16,  6 }, kPeepAnimationSequenceTentacleSittingEatFood };
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
        pag[PeepActionSpriteType::None]           = { kPeepSpriteToffeeAppleStateNoneId,           {  8, 16,  5 }, kPeepAnimationSequenceToffeeAppleNone };
        pag[PeepActionSpriteType::WatchRide]      = { kPeepSpriteToffeeAppleStateWatchRideId,      {  8, 16,  5 }, kPeepAnimationSequenceToffeeAppleWatchRide };
        pag[PeepActionSpriteType::EatFood]        = { kPeepSpriteToffeeAppleStateEatFoodId,        {  8, 16,  5 }, kPeepAnimationSequenceToffeeAppleEatFood };
        pag[PeepActionSpriteType::SittingIdle]    = { kPeepSpriteToffeeAppleStateSittingIdleId,    {  9, 16,  6 }, kPeepAnimationSequenceToffeeAppleSittingIdle };
        pag[PeepActionSpriteType::SittingEatFood] = { kPeepSpriteToffeeAppleStateSittingEatFoodId, {  9, 16,  6 }, kPeepAnimationSequenceToffeeAppleSittingEatFood };
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
        pag[PeepActionSpriteType::None]           = { kPeepSpriteDonutStateNoneId,           {  8, 16,  5 }, kPeepAnimationSequenceDonutNone };
        pag[PeepActionSpriteType::WatchRide]      = { kPeepSpriteDonutStateWatchRideId,      {  8, 16,  5 }, kPeepAnimationSequenceDonutWatchRide };
        pag[PeepActionSpriteType::EatFood]        = { kPeepSpriteDonutStateEatFoodId,        {  8, 16,  5 }, kPeepAnimationSequenceDonutEatFood };
        pag[PeepActionSpriteType::SittingIdle]    = { kPeepSpriteDonutStateSittingIdleId,    {  9, 16,  6 }, kPeepAnimationSequenceDonutSittingIdle };
        pag[PeepActionSpriteType::SittingEatFood] = { kPeepSpriteDonutStateSittingEatFoodId, {  9, 16,  6 }, kPeepAnimationSequenceDonutSittingEatFood };
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
        pag[PeepActionSpriteType::None]           = { kPeepSpriteCoffeeStateNoneId,        {  8, 16,  5 }, kPeepAnimationSequenceCoffeeNone };
        pag[PeepActionSpriteType::WatchRide]      = { kPeepSpriteCoffeeStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceCoffeeWatchRide };
        pag[PeepActionSpriteType::EatFood]        = { kPeepSpriteCoffeeStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceCoffeeEatFood };
        pag[PeepActionSpriteType::SittingIdle]    = { kPeepSpriteCoffeeStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceCoffeeSittingIdle };
        pag[PeepActionSpriteType::SittingEatFood] = { kPeepSpriteCoffeeStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceCoffeeSittingEatFood };
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
        pag[PeepActionSpriteType::None]           = { kPeepSpriteChickenStateNoneId,        {  8, 16,  5 }, kPeepAnimationSequenceChickenNone };
        pag[PeepActionSpriteType::WatchRide]      = { kPeepSpriteChickenStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceChickenWatchRide };
        pag[PeepActionSpriteType::EatFood]        = { kPeepSpriteChickenStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceChickenEatFood };
        pag[PeepActionSpriteType::SittingIdle]    = { kPeepSpriteChickenStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceChickenSittingIdle };
        pag[PeepActionSpriteType::SittingEatFood] = { kPeepSpriteChickenStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceChickenSittingEatFood };
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
        pag[PeepActionSpriteType::None]           = { kPeepSpriteLemonadeStateNoneId,        {  8, 16,  5 }, kPeepAnimationSequenceLemonadeNone };
        pag[PeepActionSpriteType::WatchRide]      = { kPeepSpriteLemonadeStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceLemonadeWatchRide };
        pag[PeepActionSpriteType::EatFood]        = { kPeepSpriteLemonadeStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceLemonadeEatFood };
        pag[PeepActionSpriteType::SittingIdle]    = { kPeepSpriteLemonadeStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceLemonadeSittingIdle };
        pag[PeepActionSpriteType::SittingEatFood] = { kPeepSpriteLemonadeStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceLemonadeSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Watching sprites
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceWatchingWatchRide = { 0 };

    // Define animation group for Watching sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupWatching = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepActionSpriteType::WatchRide] = { kPeepSpriteWatchingStateWatchRideId, {  8, 16,  5 }, kPeepAnimationSequenceWatchingWatchRide };
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
        pag[PeepActionSpriteType::None]           = { kPeepSpritePretzelStateNoneId,        {  8, 16,  5 }, kPeepAnimationSequencePretzelNone };
        pag[PeepActionSpriteType::WatchRide]      = { kPeepSpritePretzelStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequencePretzelWatchRide };
        pag[PeepActionSpriteType::EatFood]        = { kPeepSpritePretzelStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequencePretzelEatFood };
        pag[PeepActionSpriteType::SittingIdle]    = { kPeepSpritePretzelStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequencePretzelSittingIdle };
        pag[PeepActionSpriteType::SittingEatFood] = { kPeepSpritePretzelStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequencePretzelSittingEatFood };
        return pag;
    }();

    // Define animation sequences for Sunglasses sprites
    static constexpr std::array<uint8_t, 6> kPeepAnimationSequenceSunglassesNone        = { 0, 1, 2, 3, 4, 5 };
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceSunglassesWatchRide   = { 0 };
    static constexpr std::array<uint8_t, 1> kPeepAnimationSequenceSunglassesSittingIdle = { 0 };

    // Define animation group for Sunglasses sequences
    static constexpr PeepAnimationGroup kPeepAnimationGroupSunglasses = []() {
        PeepAnimationGroup pag = kPeepAnimationGroupNormal;
        pag[PeepActionSpriteType::None]        = { kPeepSpriteSunglassesStateNoneId,        {  8, 16,  5 }, kPeepAnimationSequenceSunglassesNone };
        pag[PeepActionSpriteType::WatchRide]   = { kPeepSpriteSunglassesStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceSunglassesWatchRide };
        pag[PeepActionSpriteType::SittingIdle] = { kPeepSpriteSunglassesStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceSunglassesSittingIdle };
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
        pag[PeepActionSpriteType::None]           = { kPeepSpriteSuJongkwaStateNoneId,        {  8, 16,  5 }, kPeepAnimationSequenceSuJongkwaNone };
        pag[PeepActionSpriteType::WatchRide]      = { kPeepSpriteSuJongkwaStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceSuJongkwaWatchRide };
        pag[PeepActionSpriteType::EatFood]        = { kPeepSpriteSuJongkwaStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceSuJongkwaEatFood };
        pag[PeepActionSpriteType::SittingIdle]    = { kPeepSpriteSuJongkwaStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceSuJongkwaSittingIdle };
        pag[PeepActionSpriteType::SittingEatFood] = { kPeepSpriteSuJongkwaStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceSuJongkwaSittingEatFood };
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
        pag[PeepActionSpriteType::None]           = { kPeepSpriteJuiceStateNoneId,        {  8, 16,  5 }, kPeepAnimationSequenceJuiceNone };
        pag[PeepActionSpriteType::WatchRide]      = { kPeepSpriteJuiceStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceJuiceWatchRide };
        pag[PeepActionSpriteType::EatFood]        = { kPeepSpriteJuiceStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceJuiceEatFood };
        pag[PeepActionSpriteType::SittingIdle]    = { kPeepSpriteJuiceStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceJuiceSittingIdle };
        pag[PeepActionSpriteType::SittingEatFood] = { kPeepSpriteJuiceStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceJuiceSittingEatFood };
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
        pag[PeepActionSpriteType::None]           = { kPeepSpriteFunnelCakeStateNoneId,        {  8, 16,  5 }, kPeepAnimationSequenceFunnelCakeNone };
        pag[PeepActionSpriteType::WatchRide]      = { kPeepSpriteFunnelCakeStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceFunnelCakeWatchRide };
        pag[PeepActionSpriteType::EatFood]        = { kPeepSpriteFunnelCakeStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceFunnelCakeEatFood };
        pag[PeepActionSpriteType::SittingIdle]    = { kPeepSpriteFunnelCakeStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceFunnelCakeSittingIdle };
        pag[PeepActionSpriteType::SittingEatFood] = { kPeepSpriteFunnelCakeStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceFunnelCakeSittingEatFood };
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
        pag[PeepActionSpriteType::None]           = { kPeepSpriteNoodlesStateNoneId,        {  8, 16,  5 }, kPeepAnimationSequenceNoodlesNone };
        pag[PeepActionSpriteType::WatchRide]      = { kPeepSpriteNoodlesStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceNoodlesWatchRide };
        pag[PeepActionSpriteType::EatFood]        = { kPeepSpriteNoodlesStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceNoodlesEatFood };
        pag[PeepActionSpriteType::SittingIdle]    = { kPeepSpriteNoodlesStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceNoodlesSittingIdle };
        pag[PeepActionSpriteType::SittingEatFood] = { kPeepSpriteNoodlesStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceNoodlesSittingEatFood };
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
        pag[PeepActionSpriteType::None]           = { kPeepSpriteSausageStateNoneId,        {  8, 16,  5 }, kPeepAnimationSequenceSausageNone };
        pag[PeepActionSpriteType::WatchRide]      = { kPeepSpriteSausageStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceSausageWatchRide };
        pag[PeepActionSpriteType::EatFood]        = { kPeepSpriteSausageStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceSausageEatFood };
        pag[PeepActionSpriteType::SittingIdle]    = { kPeepSpriteSausageStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceSausageSittingIdle };
        pag[PeepActionSpriteType::SittingEatFood] = { kPeepSpriteSausageStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceSausageSittingEatFood };
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
        pag[PeepActionSpriteType::None]           = { kPeepSpriteSoupStateNoneId,        {  8, 16,  5 }, kPeepAnimationSequenceSoupNone };
        pag[PeepActionSpriteType::WatchRide]      = { kPeepSpriteSoupStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceSoupWatchRide };
        pag[PeepActionSpriteType::EatFood]        = { kPeepSpriteSoupStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceSoupEatFood };
        pag[PeepActionSpriteType::SittingIdle]    = { kPeepSpriteSoupStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceSoupSittingIdle };
        pag[PeepActionSpriteType::SittingEatFood] = { kPeepSpriteSoupStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceSoupSittingEatFood };
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
        pag[PeepActionSpriteType::None]           = { kPeepSpriteSandwichStateNoneId,        {  8, 16,  5 }, kPeepAnimationSequenceSandwichNone };
        pag[PeepActionSpriteType::WatchRide]      = { kPeepSpriteSandwichStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceSandwichWatchRide };
        pag[PeepActionSpriteType::EatFood]        = { kPeepSpriteSandwichStateWatchRideId,   {  8, 16,  5 }, kPeepAnimationSequenceSandwichEatFood };
        pag[PeepActionSpriteType::SittingIdle]    = { kPeepSpriteSandwichStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceSandwichSittingIdle };
        pag[PeepActionSpriteType::SittingEatFood] = { kPeepSpriteSandwichStateSittingIdleId, {  9, 16,  6 }, kPeepAnimationSequenceSandwichSittingEatFood };
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

    const PeepAnimation& GetPeepAnimation(PeepSpriteType spriteType, PeepActionSpriteType actionSpriteType)
    {
        return kPeepAnimationEntries[EnumValue(spriteType)][actionSpriteType];
    }

    const SpriteBounds& GetSpriteBounds(PeepSpriteType spriteType, PeepActionSpriteType actionSpriteType)
    {
        return kPeepAnimationEntries[EnumValue(spriteType)][actionSpriteType].bounds;
    }
} // namespace OpenRCT2
