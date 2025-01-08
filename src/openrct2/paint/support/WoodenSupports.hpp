/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
    PaintSession& session, WoodenSupportType supportType, uint8_t sequence, Direction direction, int32_t height,
    ImageId imageTemplate)
{
    const auto& ted = OpenRCT2::TrackMetaData::GetTrackElementDescriptor(trackType);
    const auto& desc = ted.sequences[sequence].woodenSupports;

    if (desc.subType == WoodenSupportSubType::Null)
        return false;

    return WoodenASupportsPaintSetupRotated(
        session, supportType, desc.subType, direction, height, imageTemplate, desc.transitionType);
}

template<OpenRCT2::TrackElemType trackType>
bool DrawSupportForSequenceB(
    PaintSession& session, WoodenSupportType supportType, uint8_t sequence, Direction direction, int32_t height,
    ImageId imageTemplate)
{
    const auto& ted = OpenRCT2::TrackMetaData::GetTrackElementDescriptor(trackType);
    const auto& desc = ted.sequences[sequence].woodenSupports;

    if (desc.subType == WoodenSupportSubType::Null)
        return false;

    return WoodenBSupportsPaintSetupRotated(
        session, supportType, desc.subType, direction, height, imageTemplate, desc.transitionType);
}
