/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TrackElemType.h"

namespace OpenRCT2
{
    bool trackTypeIsCovered(TrackElemType trackElementType)
    {
        switch (trackElementType)
        {
            case TrackElemType::flatCovered:
            case TrackElemType::up25Covered:
            case TrackElemType::up60Covered:
            case TrackElemType::flatToUp25Covered:
            case TrackElemType::up25ToUp60Covered:
            case TrackElemType::up60ToUp25Covered:
            case TrackElemType::up25ToFlatCovered:
            case TrackElemType::down25Covered:
            case TrackElemType::down60Covered:
            case TrackElemType::flatToDown25Covered:
            case TrackElemType::down25ToDown60Covered:
            case TrackElemType::down60ToDown25Covered:
            case TrackElemType::down25ToFlatCovered:
            case TrackElemType::leftQuarterTurn5TilesCovered:
            case TrackElemType::rightQuarterTurn5TilesCovered:
            case TrackElemType::sBendLeftCovered:
            case TrackElemType::sBendRightCovered:
            case TrackElemType::leftQuarterTurn3TilesCovered:
            case TrackElemType::rightQuarterTurn3TilesCovered:
                return true;
            default:
                return false;
        }
    }

    TrackElemType uncoverTrackType(TrackElemType trackElementType)
    {
        switch (trackElementType)
        {
            case TrackElemType::flatCovered:
                return TrackElemType::flat;
            case TrackElemType::up25Covered:
                return TrackElemType::up25;
            case TrackElemType::up60Covered:
                return TrackElemType::up60;
            case TrackElemType::flatToUp25Covered:
                return TrackElemType::flatToUp25;
            case TrackElemType::up25ToUp60Covered:
                return TrackElemType::up25ToUp60;
            case TrackElemType::up60ToUp25Covered:
                return TrackElemType::up60ToUp25;
            case TrackElemType::up25ToFlatCovered:
                return TrackElemType::up25ToFlat;
            case TrackElemType::down25Covered:
                return TrackElemType::down25;
            case TrackElemType::down60Covered:
                return TrackElemType::down60;
            case TrackElemType::flatToDown25Covered:
                return TrackElemType::flatToDown25;
            case TrackElemType::down25ToDown60Covered:
                return TrackElemType::down25ToDown60;
            case TrackElemType::down60ToDown25Covered:
                return TrackElemType::down60ToDown25;
            case TrackElemType::down25ToFlatCovered:
                return TrackElemType::down25ToFlat;
            case TrackElemType::leftQuarterTurn5TilesCovered:
                return TrackElemType::leftQuarterTurn5Tiles;
            case TrackElemType::rightQuarterTurn5TilesCovered:
                return TrackElemType::rightQuarterTurn5Tiles;
            case TrackElemType::sBendLeftCovered:
                return TrackElemType::sBendLeft;
            case TrackElemType::sBendRightCovered:
                return TrackElemType::sBendRight;
            case TrackElemType::leftQuarterTurn3TilesCovered:
                return TrackElemType::leftQuarterTurn3Tiles;
            case TrackElemType::rightQuarterTurn3TilesCovered:
                return TrackElemType::rightQuarterTurn3Tiles;
            default:
                return trackElementType;
        }
    }

    bool trackTypeIsStation(TrackElemType trackType)
    {
        switch (trackType)
        {
            case TrackElemType::endStation:
            case TrackElemType::beginStation:
            case TrackElemType::middleStation:
                return true;
            default:
                return false;
        }
    }

    bool trackTypeIsBrakes(TrackElemType trackType)
    {
        switch (trackType)
        {
            case TrackElemType::brakes:
            case TrackElemType::diagBrakes:
            case TrackElemType::down25Brakes:
            case TrackElemType::diagDown25Brakes:
                return true;
            default:
                return false;
        }
    }

    bool trackTypeIsBlockBrakes(TrackElemType trackType)
    {
        return (trackType == TrackElemType::blockBrakes) || (trackType == TrackElemType::diagBlockBrakes);
    }

    bool trackTypeIsBooster(TrackElemType trackType)
    {
        switch (trackType)
        {
            case TrackElemType::booster:
            case TrackElemType::diagBooster:
                return true;
            default:
                return false;
        }
    }

    bool trackTypeIsReverser(TrackElemType trackType)
    {
        return (trackType == TrackElemType::leftReverser) || (trackType == TrackElemType::rightReverser);
    }

    bool trackTypeHasSpeedSetting(TrackElemType trackType)
    {
        return trackTypeIsBooster(trackType) || trackTypeIsBrakes(trackType) || trackTypeIsBlockBrakes(trackType);
    }

    bool trackTypeIsHelix(TrackElemType trackType)
    {
        if (trackType >= TrackElemType::leftHalfBankedHelixUpSmall && trackType <= TrackElemType::rightHalfBankedHelixDownLarge)
            return true;

        if (trackType >= TrackElemType::leftQuarterBankedHelixLargeUp && trackType <= TrackElemType::rightQuarterHelixLargeDown)
            return true;

        return false;
    }
} // namespace OpenRCT2
