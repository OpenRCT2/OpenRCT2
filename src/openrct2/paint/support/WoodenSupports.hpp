/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../ride/TrackData.h"

template<OpenRCT2::TrackElemType trackType>
bool DrawSupportForSequenceA(
    PaintSession& session, const WoodenSupportType supportType, const uint8_t sequence, const Direction direction,
    const int32_t height, const ImageId imageTemplate)
{
    const auto& ted = OpenRCT2::TrackMetaData::GetTrackElementDescriptor(trackType);
    const auto& sequenceDesc = ted.sequences[sequence];
    const auto& desc = sequenceDesc.woodenSupports;

    if (desc.subType == WoodenSupportSubType::null)
        return false;

    const Direction supportRotation = (direction + sequenceDesc.extraSupportRotation) & 3;

    return WoodenASupportsPaintSetupRotated(
        session, supportType, desc.subType, supportRotation, height, imageTemplate, desc.transitionType);
}

template<OpenRCT2::TrackElemType trackType>
bool DrawSupportForSequenceB(
    PaintSession& session, const WoodenSupportType supportType, const uint8_t sequence, const Direction direction,
    const int32_t height, const ImageId imageTemplate)
{
    const auto& ted = OpenRCT2::TrackMetaData::GetTrackElementDescriptor(trackType);
    const auto& sequenceDesc = ted.sequences[sequence];
    const auto& desc = sequenceDesc.woodenSupports;

    if (desc.subType == WoodenSupportSubType::null)
        return false;

    const Direction supportRotation = (direction + sequenceDesc.extraSupportRotation) & 3;

    return WoodenBSupportsPaintSetupRotated(
        session, supportType, desc.subType, supportRotation, height, imageTemplate, desc.transitionType);
}
