/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../localisation/StringIds.h"
#include "TrackElementDescriptor.h"

#include <cstdint>

namespace OpenRCT2::TrackMetadata
{
    using PS = PaintSegment;

    // Maze sequence blocks do not have the height marker flag set because they are handled differently
    static constexpr SequenceDescriptor kMazeSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kMazeSeq1 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq2 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq3 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq4 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq5 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq6 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq7 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq8 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq9 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq10 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq11 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq12 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq13 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq14 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq15 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    constexpr auto kTEDMaze = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 65536,
        .mirrorElement = TrackElemType::maze,
        .flags = { TrackElementFlag::onlyAboveGround },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1,
                          { kMazeSeq0, kMazeSeq1, kMazeSeq2, kMazeSeq3, kMazeSeq4, kMazeSeq5, kMazeSeq6, kMazeSeq7, kMazeSeq8,
                            kMazeSeq9, kMazeSeq10, kMazeSeq11, kMazeSeq12, kMazeSeq13, kMazeSeq14, kMazeSeq15 } },
    };
} // namespace OpenRCT2::TrackMetadata
