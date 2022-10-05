/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../config/Config.h"
#include "../../entity/EntityRegistry.h"
#include "../../entity/Guest.h"
#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../../world/Map.h"
#include "../../world/Surface.h"
#include "../RideData.h"
#include "../TrackData.h"
#include "../TrackPaint.h"
#include "../Vehicle.h"
#include "../VehiclePaint.h"

#include <iterator>

enum
{
    SprMiniGolfFlatSwNe = 14404,
    SprMiniGolfFlatNwSe = 14405,
    SprMiniGolfFlatFenceBackSwNe = 14406,
    SprMiniGolfFlatFenceBackNwSe = 14407,
    SprMiniGolfFlatFenceFrontSwNe = 14408,
    SprMiniGolfFlatFenceFrontNwSe = 14409,
    SprMiniGolfStationFloor = 14410,
    SprMiniGolfQuarterTurn1TileSwNw = 14411,
    SprMiniGolfQuarterTurn1TileNwNe = 14412,
    SprMiniGolfQuarterTurn1TileNeSe = 14413,
    SprMiniGolfQuarterTurn1TileSeSw = 14414,
    SprMiniGolfQuarterTurn1TileFenceBackSwNw = 14415,
    SprMiniGolfQuarterTurn1TileFenceBackNwNe = 14416,
    SprMiniGolfQuarterTurn1TileFenceBackNeSe = 14417,
    SprMiniGolfQuarterTurn1TileFenceBackSeSw = 14418,
    SprMiniGolfQuarterTurn1TileFenceFrontSwNw = 14419,
    SprMiniGolfQuarterTurn1TileFenceFrontNwNe = 14420,
    SprMiniGolfQuarterTurn1TileFenceFrontNeSe = 14421,
    SprMiniGolfQuarterTurn1TileFenceFrontSeSw = 14422,
    SprMiniGolfQuarterTurn1TileFenceInsideSwNw = 14423,
    SprMiniGolfQuarterTurn1TileFenceInsideNeSe = 14424,
    SprMiniGolfFlatTo25DegUpSwNe = 14425,
    SprMiniGolfFlatTo25DegUpNwSe = 14426,
    SprMiniGolfFlatTo25DegUpNeSw = 14427,
    SprMiniGolfFlatTo25DegUpSeNw = 14428,
    SprMiniGolf25DegUpToFlatSwNe = 14429,
    SprMiniGolf25DegUpToFlatNwSe = 14430,
    SprMiniGolf25DegUpToFlatNeSw = 14431,
    SprMiniGolf25DegUpToFlatSeNw = 14432,
    SprMiniGolf25DegUpSwNe = 14433,
    SprMiniGolf25DegUpNwSe = 14434,
    SprMiniGolf25DegUpNeSw = 14435,
    SprMiniGolf25DegUpSeNw = 14436,
    SprMiniGolfFlatTo25DegUpFenceBackSwNe = 14437,
    SprMiniGolfFlatTo25DegUpFenceBackNwSe = 14438,
    SprMiniGolfFlatTo25DegUpFenceBackNeSw = 14439,
    SprMiniGolfFlatTo25DegUpFenceBackSeNw = 14440,
    SprMiniGolf25DegUpToFlatFenceBackSwNe = 14441,
    SprMiniGolf25DegUpToFlatFenceBackNwSe = 14442,
    SprMiniGolf25DegUpToFlatFenceBackNeSw = 14443,
    SprMiniGolf25DegUpToFlatFenceBackSeNw = 14444,
    SprMiniGolfFlatTo25DegUpFenceFrontSwNe = 14445,
    SprMiniGolfFlatTo25DegUpFenceFrontNwSe = 14446,
    SprMiniGolfFlatTo25DegUpFenceFrontNeSw = 14447,
    SprMiniGolfFlatTo25DegUpFenceFrontSeNw = 14448,
    SprMiniGolf25DegUpToFlatFenceFrontSwNe = 14449,
    SprMiniGolf25DegUpToFlatFenceFrontNwSe = 14450,
    SprMiniGolf25DegUpToFlatFenceFrontNeSw = 14451,
    SprMiniGolf25DegUpToFlatFenceFrontSeNw = 14452,
    SprMiniGolf25DegUpFenceBackSwNe = 14453,
    SprMiniGolf25DegUpFenceBackNwSe = 14454,
    SprMiniGolf25DegUpFenceBackNeSw = 14455,
    SprMiniGolf25DegUpFenceBackSeNw = 14456,
    SprMiniGolf25DegUpFenceFrontSwNe = 14457,
    SprMiniGolf25DegUpFenceFrontNwSe = 14458,
    SprMiniGolf25DegUpFenceFrontNeSw = 14459,
    SprMiniGolf25DegUpFenceFrontSeNw = 14460,

    SprMiniGolfHoleABasePart1SwNe = 14461,
    SprMiniGolfHoleABasePart2SwNe = 14462,
    SprMiniGolfHoleABasePart1NwSe = 14463,
    SprMiniGolfHoleABasePart2NwSe = 14464,
    SprMiniGolfHoleABasePart1NeSw = 14465,
    SprMiniGolfHoleABasePart2NeSw = 14466,
    SprMiniGolfHoleABasePart1SeNw = 14467,
    SprMiniGolfHoleABasePart2SeNw = 14468,
    SprMiniGolfHoleATrimPart1SwNe = 14469,
    SprMiniGolfHoleATrimPart2SwNe = 14470,
    SprMiniGolfHoleATrimPart1NwSe = 14471,
    SprMiniGolfHoleATrimPart2NwSe = 14472,
    SprMiniGolfHoleATrimPart1NeSw = 14473,
    SprMiniGolfHoleATrimPart2NeSw = 14474,
    SprMiniGolfHoleATrimPart1SeNw = 14475,
    SprMiniGolfHoleATrimPart2SeNw = 14476,

    SprMiniGolfHoleBBasePart1SwNe = 14477,
    SprMiniGolfHoleBBasePart2SwNe = 14478,
    SprMiniGolfHoleBBasePart1NwSe = 14479,
    SprMiniGolfHoleBBasePart2NwSe = 14480,
    SprMiniGolfHoleBBasePart1NeSw = 14481,
    SprMiniGolfHoleBBasePart2NeSw = 14482,
    SprMiniGolfHoleBBasePart1SeNw = 14483,
    SprMiniGolfHoleBBasePart2SeNw = 14484,
    SprMiniGolfHoleBTrimPart1SwNe = 14485,
    SprMiniGolfHoleBTrimPart2SwNe = 14486,
    SprMiniGolfHoleBTrimPart1NwSe = 14487,
    SprMiniGolfHoleBTrimPart2NwSe = 14488,
    SprMiniGolfHoleBTrimPart1NeSw = 14489,
    SprMiniGolfHoleBTrimPart2NeSw = 14490,
    SprMiniGolfHoleBTrimPart1SeNw = 14491,
    SprMiniGolfHoleBTrimPart2SeNw = 14492,

    SprMiniGolfHoleCBasePart1SwNe = 14493,
    SprMiniGolfHoleCBasePart2SwNe = 14494,
    SprMiniGolfHoleCBasePart1NwSe = 14495,
    SprMiniGolfHoleCBasePart2NwSe = 14496,
    SprMiniGolfHoleCBasePart1NeSw = 14497,
    SprMiniGolfHoleCBasePart2NeSw = 14498,
    SprMiniGolfHoleCBasePart1SeNw = 14499,
    SprMiniGolfHoleCBasePart2SeNw = 14500,
    SprMiniGolfHoleCTrimPart1SwNe = 14501,
    SprMiniGolfHoleCTrimPart2SwNe = 14502,
    SprMiniGolfHoleCTrimPart1NwSe = 14503,
    SprMiniGolfHoleCTrimPart2NwSe = 14504,
    SprMiniGolfHoleCTrimPart1NeSw = 14505,
    SprMiniGolfHoleCTrimPart2NeSw = 14506,
    SprMiniGolfHoleCTrimPart1SeNw = 14507,
    SprMiniGolfHoleCTrimPart2SeNw = 14508,

    SprMiniGolfHoleDBasePart1SwSe = 14509,
    SprMiniGolfHoleDBasePart2SwSe = 14510,
    SprMiniGolfHoleDBasePart3SwSe = 14511,
    SprMiniGolfHoleDBasePart1NwSw = 14512,
    SprMiniGolfHoleDBasePart2NwSw = 14513,
    SprMiniGolfHoleDBasePart3NwSw = 14514,
    SprMiniGolfHoleDBasePart1NeNw = 14515,
    SprMiniGolfHoleDBasePart2NeNw = 14516,
    SprMiniGolfHoleDBasePart3NeNw = 14517,
    SprMiniGolfHoleDBasePart1SeNe = 14518,
    SprMiniGolfHoleDBasePart2SeNe = 14519,
    SprMiniGolfHoleDBasePart3SeNe = 14520,
    SprMiniGolfHoleDTrimPart1SwSe = 14521,
    SprMiniGolfHoleDTrimPart2SwSe = 14522,
    SprMiniGolfHoleDTrimPart3SwSe = 14523,
    SprMiniGolfHoleDTrimPart1NwSw = 14524,
    SprMiniGolfHoleDTrimPart2NwSw = 14525,
    SprMiniGolfHoleDTrimPart3NwSw = 14526,
    SprMiniGolfHoleDTrimPart1NeNw = 14527,
    SprMiniGolfHoleDTrimPart2NeNw = 14528,
    SprMiniGolfHoleDTrimPart3NeNw = 14529,
    SprMiniGolfHoleDTrimPart1SeNe = 14530,
    SprMiniGolfHoleDTrimPart2SeNe = 14531,
    SprMiniGolfHoleDTrimPart3SeNe = 14532,

    SprMiniGolfHoleEBasePart1SeSw = 14533,
    SprMiniGolfHoleEBasePart2SeSw = 14534,
    SprMiniGolfHoleEBasePart3SeSw = 14535,
    SprMiniGolfHoleEBasePart1NeSe = 14536,
    SprMiniGolfHoleEBasePart2NeSe = 14537,
    SprMiniGolfHoleEBasePart3NeSe = 14538,
    SprMiniGolfHoleEBasePart1NwNe = 14539,
    SprMiniGolfHoleEBasePart2NwNe = 14540,
    SprMiniGolfHoleEBasePart3NwNe = 14541,
    SprMiniGolfHoleEBasePart1SwNw = 14542,
    SprMiniGolfHoleEBasePart2SwNw = 14543,
    SprMiniGolfHoleEBasePart3SwNw = 14544,
    SprMiniGolfHoleETrimPart1SeSw = 14545,
    SprMiniGolfHoleETrimPart2SeSw = 14546,
    SprMiniGolfHoleETrimPart3SeSw = 14547,
    SprMiniGolfHoleETrimPart1NeSe = 14548,
    SprMiniGolfHoleETrimPart2NeSe = 14549,
    SprMiniGolfHoleETrimPart3NeSe = 14550,
    SprMiniGolfHoleETrimPart1NwNe = 14551,
    SprMiniGolfHoleETrimPart2NwNe = 14552,
    SprMiniGolfHoleETrimPart3NwNe = 14553,
    SprMiniGolfHoleETrimPart1SwNw = 14554,
    SprMiniGolfHoleETrimPart2SwNw = 14555,
    SprMiniGolfHoleETrimPart3SwNw = 14556,
};

static constexpr const uint32_t MiniGolfTrackSprites25DegUp[][3] = {
    {
        SprMiniGolf25DegUpSwNe,
        SprMiniGolf25DegUpFenceBackSwNe,
        SprMiniGolf25DegUpFenceFrontSwNe,
    },
    {
        SprMiniGolf25DegUpNwSe,
        SprMiniGolf25DegUpFenceBackNwSe,
        SprMiniGolf25DegUpFenceFrontNwSe,
    },
    {
        SprMiniGolf25DegUpNeSw,
        SprMiniGolf25DegUpFenceBackNeSw,
        SprMiniGolf25DegUpFenceFrontNeSw,
    },
    {
        SprMiniGolf25DegUpSeNw,
        SprMiniGolf25DegUpFenceBackSeNw,
        SprMiniGolf25DegUpFenceFrontSeNw,
    },
};

static constexpr const uint32_t MiniGolfTrackSpritesFlatTo25DegUp[][3] = {
    {
        SprMiniGolfFlatTo25DegUpSwNe,
        SprMiniGolfFlatTo25DegUpFenceBackSwNe,
        SprMiniGolfFlatTo25DegUpFenceFrontSwNe,
    },
    {
        SprMiniGolfFlatTo25DegUpNwSe,
        SprMiniGolfFlatTo25DegUpFenceBackNwSe,
        SprMiniGolfFlatTo25DegUpFenceFrontNwSe,
    },
    {
        SprMiniGolfFlatTo25DegUpNeSw,
        SprMiniGolfFlatTo25DegUpFenceBackNeSw,
        SprMiniGolfFlatTo25DegUpFenceFrontNeSw,
    },
    {
        SprMiniGolfFlatTo25DegUpSeNw,
        SprMiniGolfFlatTo25DegUpFenceBackSeNw,
        SprMiniGolfFlatTo25DegUpFenceFrontSeNw,
    },
};

static constexpr const uint32_t MiniGolfTrackSprites25DegUpToFlat[][3] = {
    {
        SprMiniGolf25DegUpToFlatSwNe,
        SprMiniGolf25DegUpToFlatFenceBackSwNe,
        SprMiniGolf25DegUpToFlatFenceFrontSwNe,
    },
    {
        SprMiniGolf25DegUpToFlatNwSe,
        SprMiniGolf25DegUpToFlatFenceBackNwSe,
        SprMiniGolf25DegUpToFlatFenceFrontNwSe,
    },
    {
        SprMiniGolf25DegUpToFlatNeSw,
        SprMiniGolf25DegUpToFlatFenceBackNeSw,
        SprMiniGolf25DegUpToFlatFenceFrontNeSw,
    },
    {
        SprMiniGolf25DegUpToFlatSeNw,
        SprMiniGolf25DegUpToFlatFenceBackSeNw,
        SprMiniGolf25DegUpToFlatFenceFrontSeNw,
    },
};

static constexpr const uint32_t MiniGolfTrackSpritesQuarterTurn1Tile[] = {
    SprMiniGolfQuarterTurn1TileSwNw,
    SprMiniGolfQuarterTurn1TileNwNe,
    SprMiniGolfQuarterTurn1TileNeSe,
    SprMiniGolfQuarterTurn1TileSeSw,
};

static constexpr const uint32_t MiniGolfTrackSpritesQuarterTurn1TileFenceFront[] = {
    SprMiniGolfQuarterTurn1TileFenceFrontSwNw,
    SprMiniGolfQuarterTurn1TileFenceFrontNwNe,
    SprMiniGolfQuarterTurn1TileFenceFrontNeSe,
    SprMiniGolfQuarterTurn1TileFenceFrontSeSw,
};

static constexpr const uint32_t MiniGolfTrackSpritesHoleA[4][2][2] = {
    {
        { SprMiniGolfHoleABasePart1SwNe, SprMiniGolfHoleATrimPart1SwNe },
        { SprMiniGolfHoleABasePart2SwNe, SprMiniGolfHoleATrimPart2SwNe },
    },
    {
        { SprMiniGolfHoleABasePart1NwSe, SprMiniGolfHoleATrimPart1NwSe },
        { SprMiniGolfHoleABasePart2NwSe, SprMiniGolfHoleATrimPart2NwSe },
    },
    {
        { SprMiniGolfHoleABasePart1NeSw, SprMiniGolfHoleATrimPart1NeSw },
        { SprMiniGolfHoleABasePart2NeSw, SprMiniGolfHoleATrimPart2NeSw },
    },
    {
        { SprMiniGolfHoleABasePart1SeNw, SprMiniGolfHoleATrimPart1SeNw },
        { SprMiniGolfHoleABasePart2SeNw, SprMiniGolfHoleATrimPart2SeNw },
    },
};

static constexpr const uint32_t MiniGolfTrackSpritesHoleB[4][2][2] = {
    {
        { SprMiniGolfHoleBBasePart1SwNe, SprMiniGolfHoleBTrimPart1SwNe },
        { SprMiniGolfHoleBBasePart2SwNe, SprMiniGolfHoleBTrimPart2SwNe },
    },
    {
        { SprMiniGolfHoleBBasePart1NwSe, SprMiniGolfHoleBTrimPart1NwSe },
        { SprMiniGolfHoleBBasePart2NwSe, SprMiniGolfHoleBTrimPart2NwSe },
    },
    {
        { SprMiniGolfHoleBBasePart1NeSw, SprMiniGolfHoleBTrimPart1NeSw },
        { SprMiniGolfHoleBBasePart2NeSw, SprMiniGolfHoleBTrimPart2NeSw },
    },
    {
        { SprMiniGolfHoleBBasePart1SeNw, SprMiniGolfHoleBTrimPart1SeNw },
        { SprMiniGolfHoleBBasePart2SeNw, SprMiniGolfHoleBTrimPart2SeNw },
    },
};

static constexpr const uint32_t MiniGolfTrackSpritesHoleC[][2][2] = {
    {
        { SprMiniGolfHoleCBasePart1SwNe, SprMiniGolfHoleCTrimPart1SwNe },
        { SprMiniGolfHoleCBasePart2SwNe, SprMiniGolfHoleCTrimPart2SwNe },
    },
    {
        { SprMiniGolfHoleCBasePart1NwSe, SprMiniGolfHoleCTrimPart1NwSe },
        { SprMiniGolfHoleCBasePart2NwSe, SprMiniGolfHoleCTrimPart2NwSe },
    },
    {
        { SprMiniGolfHoleCBasePart1NeSw, SprMiniGolfHoleCTrimPart1NeSw },
        { SprMiniGolfHoleCBasePart2NeSw, SprMiniGolfHoleCTrimPart2NeSw },
    },
    {
        { SprMiniGolfHoleCBasePart1SeNw, SprMiniGolfHoleCTrimPart1SeNw },
        { SprMiniGolfHoleCBasePart2SeNw, SprMiniGolfHoleCTrimPart2SeNw },
    },
};

static constexpr const uint32_t MiniGolfTrackSpritesHoleD[][3][2] = {
    {
        { SprMiniGolfHoleDBasePart1SwSe, SprMiniGolfHoleDTrimPart1SwSe },
        { SprMiniGolfHoleDBasePart2SwSe, SprMiniGolfHoleDTrimPart2SwSe },
        { SprMiniGolfHoleDBasePart3SwSe, SprMiniGolfHoleDTrimPart3SwSe },
    },
    {
        { SprMiniGolfHoleDBasePart1NwSw, SprMiniGolfHoleDTrimPart1NwSw },
        { SprMiniGolfHoleDBasePart2NwSw, SprMiniGolfHoleDTrimPart2NwSw },
        { SprMiniGolfHoleDBasePart3NwSw, SprMiniGolfHoleDTrimPart3NwSw },
    },
    {
        { SprMiniGolfHoleDBasePart1NeNw, SprMiniGolfHoleDTrimPart1NeNw },
        { SprMiniGolfHoleDBasePart2NeNw, SprMiniGolfHoleDTrimPart2NeNw },
        { SprMiniGolfHoleDBasePart3NeNw, SprMiniGolfHoleDTrimPart3NeNw },
    },
    {
        { SprMiniGolfHoleDBasePart1SeNe, SprMiniGolfHoleDTrimPart1SeNe },
        { SprMiniGolfHoleDBasePart2SeNe, SprMiniGolfHoleDTrimPart2SeNe },
        { SprMiniGolfHoleDBasePart3SeNe, SprMiniGolfHoleDTrimPart3SeNe },
    },
};

static constexpr const uint32_t MiniGolfTrackSpritesHoleE[][3][2] = {
    {
        { SprMiniGolfHoleEBasePart1SwNw, SprMiniGolfHoleETrimPart1SwNw },
        { SprMiniGolfHoleEBasePart2SwNw, SprMiniGolfHoleETrimPart2SwNw },
        { SprMiniGolfHoleEBasePart3SwNw, SprMiniGolfHoleETrimPart3SwNw },
    },
    {
        { SprMiniGolfHoleEBasePart1NwNe, SprMiniGolfHoleETrimPart1NwNe },
        { SprMiniGolfHoleEBasePart2NwNe, SprMiniGolfHoleETrimPart2NwNe },
        { SprMiniGolfHoleEBasePart3NwNe, SprMiniGolfHoleETrimPart3NwNe },
    },
    {
        { SprMiniGolfHoleEBasePart1NeSe, SprMiniGolfHoleETrimPart1NeSe },
        { SprMiniGolfHoleEBasePart2NeSe, SprMiniGolfHoleETrimPart2NeSe },
        { SprMiniGolfHoleEBasePart3NeSe, SprMiniGolfHoleETrimPart3NeSe },
    },
    {
        { SprMiniGolfHoleEBasePart1SeSw, SprMiniGolfHoleETrimPart1SeSw },
        { SprMiniGolfHoleEBasePart2SeSw, SprMiniGolfHoleETrimPart2SeSw },
        { SprMiniGolfHoleEBasePart3SeSw, SprMiniGolfHoleETrimPart3SeSw },
    },
};

/** rct2: 0x00933471 */
// clang-format off
static constexpr const uint8_t MiniGolfPeepAnimationFramesWalk[] = {
    0, 1, 2, 3, 4, 5,
};

/** rct2: 0x00933478 */
static constexpr const uint8_t MiniGolfPeepAnimationFramesPlaceBallDownwards[] = {
    12, 13, 14, 15,
};

/** rct2: 0x009334B5 */
static constexpr const uint8_t MiniGolfPeepAnimationFramesSwing[] = {
    31, 31, 31, 31, 31, 31, 31, 31, 31, 32, 33, 33, 33, 33, 34,
};

/** rct2: 0x0093347D */
static constexpr const uint8_t MiniGolfPeepAnimationFramesSwingLeft[] = {
    6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8, 8, 8, 8, 9,
};

/** rct2: 0x0093348D */
static constexpr const uint8_t MiniGolfPeepAnimationFramesPickupBall[] = {
    12, 13, 14, 15, 14, 13, 12,
};

/** rct2: 0x00933495 */
static constexpr const uint8_t MiniGolfPeepAnimationFramesJump[] = {
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
};

/** rct2: 0x009334A5 */
static constexpr const uint8_t MiniGolfPeepAnimationFramesPlaceBallUpwards[] = {
    15, 14, 13, 12,
};

/** rct2: 0x009334C5 */
static constexpr const uint8_t MiniGolfPeepAnimationFramesPutt[] = {
    35, 36, 36, 36, 36, 36, 35, 35, 35, 35,
};

/** rct2: 0x009334AA */
static constexpr const uint8_t MiniGolfPeepAnimationFramesPuttLeft[] = {
    10, 11, 11, 11, 11, 11, 10, 10, 10, 10,
};

/** rct2: 0x008B8F74 */
static constexpr const uint8_t* MiniGolfPeepAnimationFrames[] = {
    MiniGolfPeepAnimationFramesWalk,
    MiniGolfPeepAnimationFramesPlaceBallDownwards,
    MiniGolfPeepAnimationFramesSwingLeft,
    MiniGolfPeepAnimationFramesPickupBall,
    MiniGolfPeepAnimationFramesJump,
    MiniGolfPeepAnimationFramesPlaceBallUpwards,
    MiniGolfPeepAnimationFramesPuttLeft,
    MiniGolfPeepAnimationFramesSwing,
    MiniGolfPeepAnimationFramesPutt,
};

const size_t MiniGolfPeepAnimationLengths[] = {
    std::size(MiniGolfPeepAnimationFramesWalk),
    std::size(MiniGolfPeepAnimationFramesPlaceBallDownwards),
    std::size(MiniGolfPeepAnimationFramesSwingLeft),
    std::size(MiniGolfPeepAnimationFramesPickupBall),
    std::size(MiniGolfPeepAnimationFramesJump),
    std::size(MiniGolfPeepAnimationFramesPlaceBallUpwards),
    std::size(MiniGolfPeepAnimationFramesPuttLeft),
    std::size(MiniGolfPeepAnimationFramesSwing),
    std::size(MiniGolfPeepAnimationFramesPutt),
};
// clang-format on

static paint_struct* MiniGolfPaintUtil7C(
    paint_session& session, uint8_t direction, ImageId imageId, int8_t x_offset, int8_t yOffset, int16_t boundBoxLengthX,
    int16_t boundBoxLengthY, int8_t boundBoxLengthZ, int16_t zOffset, int16_t boundBoxOffsetX, int16_t boundBoxOffsetY,
    int16_t boundBoxOffsetZ, uint32_t rotation)
{
    if (direction & 1)
    {
        return PaintAddImageAsParent(
            session, imageId, { yOffset, x_offset, zOffset }, { boundBoxLengthY, boundBoxLengthX, boundBoxLengthZ },
            { boundBoxOffsetY, boundBoxOffsetX, boundBoxOffsetZ });
    }

    return PaintAddImageAsParent(
        session, imageId, { x_offset, yOffset, zOffset }, { boundBoxLengthX, boundBoxLengthY, boundBoxLengthZ },
        { boundBoxOffsetX, boundBoxOffsetY, boundBoxOffsetZ });
}

static bool MiniGolfPaintUtilShouldDrawFence(paint_session& session, const TrackElement& trackElement)
{
    if (!(session.Flags & PaintSessionFlags::PassedSurface))
    {
        // Should be above ground (have passed surface rendering)
        return false;
    }

    const TileElement* surfaceElement = session.SurfaceElement;
    if (surfaceElement->base_height != trackElement.base_height)
    {
        return true;
    }

    if (surfaceElement->AsSurface()->GetSlope() != TILE_ELEMENT_SLOPE_FLAT)
    {
        return true;
    }

    return false;
}

/** rct2: 0x0087F10C */
static void PaintMiniGolfTrackFlat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;

    if (direction & 1)
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SprMiniGolfFlatNwSe);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 20, 32, 1 }, { 6, 0, height });
        paint_util_push_tunnel_right(session, height, TUNNEL_PATH_AND_MINI_GOLF);
    }
    else
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SprMiniGolfFlatSwNe);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 20, 1 }, { 0, 6, height });
        paint_util_push_tunnel_left(session, height, TUNNEL_PATH_AND_MINI_GOLF);
    }

    metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);

    if (MiniGolfPaintUtilShouldDrawFence(session, trackElement))
    {
        if (direction & 1)
        {
            imageId = session.TrackColours[SCHEME_MISC].WithIndex(SprMiniGolfFlatFenceBackNwSe);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 32, 7 }, { 10, 0, height + 2 });

            imageId = session.TrackColours[SCHEME_MISC].WithIndex(SprMiniGolfFlatFenceFrontNwSe);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 32, 7 }, { 22, 0, height + 2 });
        }
        else
        {
            imageId = session.TrackColours[SCHEME_MISC].WithIndex(SprMiniGolfFlatFenceBackSwNe);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 1, 7 }, { 0, 10, height + 2 });

            imageId = session.TrackColours[SCHEME_MISC].WithIndex(SprMiniGolfFlatFenceFrontSwNe);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 1, 7 }, { 0, 22, height + 2 });
        }
    }

    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0087F11C */
static void PaintMiniGolfTrack25DegUp(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;

    imageId = session.TrackColours[SCHEME_TRACK].WithIndex(MiniGolfTrackSprites25DegUp[direction][0]);
    MiniGolfPaintUtil7C(session, direction, imageId, 0, 0, 32, 20, 1, height, 0, 6, height, session.CurrentRotation);

    metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);

    imageId = session.TrackColours[SCHEME_MISC].WithIndex(MiniGolfTrackSprites25DegUp[direction][1]);
    MiniGolfPaintUtil7C(session, direction, imageId, 0, 0, 32, 1, 15, height, 0, 10, height + 2, session.CurrentRotation);

    imageId = session.TrackColours[SCHEME_MISC].WithIndex(MiniGolfTrackSprites25DegUp[direction][2]);
    MiniGolfPaintUtil7C(session, direction, imageId, 0, 0, 32, 1, 15, height, 0, 22, height + 2, session.CurrentRotation);

    switch (direction)
    {
        case 0:
            paint_util_push_tunnel_left(session, height - 8, TUNNEL_1);
            break;
        case 1:
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_2);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_2);
            break;
        case 3:
            paint_util_push_tunnel_right(session, height - 8, TUNNEL_1);
            break;
    }

    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x0087F12C */
static void PaintMiniGolfTrackFlatTo25DegUp(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;

    imageId = session.TrackColours[SCHEME_TRACK].WithIndex(MiniGolfTrackSpritesFlatTo25DegUp[direction][0]);
    MiniGolfPaintUtil7C(session, direction, imageId, 0, 0, 32, 20, 1, height, 0, 6, height, session.CurrentRotation);

    metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);

    imageId = session.TrackColours[SCHEME_MISC].WithIndex(MiniGolfTrackSpritesFlatTo25DegUp[direction][1]);
    MiniGolfPaintUtil7C(session, direction, imageId, 0, 0, 32, 1, 11, height, 0, 10, height + 2, session.CurrentRotation);

    imageId = session.TrackColours[SCHEME_MISC].WithIndex(MiniGolfTrackSpritesFlatTo25DegUp[direction][2]);
    MiniGolfPaintUtil7C(session, direction, imageId, 0, 0, 32, 1, 11, height, 0, 22, height + 2, session.CurrentRotation);

    switch (direction)
    {
        case 0:
            paint_util_push_tunnel_left(session, height, TUNNEL_PATH_AND_MINI_GOLF);
            break;
        case 1:
            paint_util_push_tunnel_right(session, height, TUNNEL_2);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height, TUNNEL_2);
            break;
        case 3:
            paint_util_push_tunnel_right(session, height, TUNNEL_PATH_AND_MINI_GOLF);
            break;
    }

    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x0087F13C */
static void PaintMiniGolfTrack25DegUpToFlat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;

    imageId = session.TrackColours[SCHEME_TRACK].WithIndex(MiniGolfTrackSprites25DegUpToFlat[direction][0]);
    MiniGolfPaintUtil7C(session, direction, imageId, 0, 0, 32, 20, 1, height, 0, 6, height, session.CurrentRotation);

    metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);

    imageId = session.TrackColours[SCHEME_MISC].WithIndex(MiniGolfTrackSprites25DegUpToFlat[direction][1]);
    MiniGolfPaintUtil7C(session, direction, imageId, 0, 0, 32, 1, 11, height, 0, 10, height + 2, session.CurrentRotation);

    imageId = session.TrackColours[SCHEME_MISC].WithIndex(MiniGolfTrackSprites25DegUpToFlat[direction][2]);
    MiniGolfPaintUtil7C(session, direction, imageId, 0, 0, 32, 1, 11, height, 0, 22, height + 2, session.CurrentRotation);

    switch (direction)
    {
        case 0:
            paint_util_push_tunnel_left(session, height - 8, TUNNEL_0);
            break;
        case 1:
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_PATH_AND_MINI_GOLF);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_PATH_AND_MINI_GOLF);
            break;
        case 3:
            paint_util_push_tunnel_right(session, height - 8, TUNNEL_0);
            break;
    }

    paint_util_set_general_support_height(session, height + 40, 0x20);
}

/** rct2: 0x0087F14C */
static void PaintMiniGolfTrack25DegDown(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintMiniGolfTrack25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

/** rct2: 0x0087F15C */
static void PaintMiniGolfTrackFlatTo25DegDown(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintMiniGolfTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

/** rct2: 0x0087F16C */
static void PaintMiniGolfTrack25DegDownToFlat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintMiniGolfTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

/** rct2: 0x0087F17C, 0x0087F18C, 0x0087F19C */
static void PaintMiniGolfStation(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    const auto* stationObj = ride.GetStationObject();
    ImageId imageId;
    bool hasFence;

    imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SprMiniGolfStationFloor);
    MiniGolfPaintUtil7C(session, direction, imageId, 0, 0, 32, 28, 1, height, 0, 0, height, session.CurrentRotation);

    if (direction & 1)
    {
        hasFence = track_paint_util_has_fence(EDGE_NE, session.MapPosition, trackElement, ride, session.CurrentRotation);
        if (hasFence)
        {
            imageId = session.TrackColours[SCHEME_MISC].WithIndex(SprMiniGolfFlatFenceBackNwSe);
            PaintAddImageAsParent(session, imageId, { -10, 0, height }, { 1, 32, 7 }, { 0, 0, height + 2 });
        }

        bool hasSWFence = track_paint_util_has_fence(EDGE_SW, session.MapPosition, trackElement, ride, session.CurrentRotation);
        if (hasFence)
        {
            imageId = session.TrackColours[SCHEME_MISC].WithIndex(SprMiniGolfFlatFenceFrontNwSe);
            PaintAddImageAsParent(session, imageId, { 10, 0, height }, { 1, 32, 7 }, { 31, 0, height + 2 });
        }

        track_paint_util_draw_station_covers(session, EDGE_NE, hasFence, stationObj, height);
        track_paint_util_draw_station_covers(session, EDGE_SW, hasSWFence, stationObj, height);

        // Was leftwards tunnel in game, seems odd
        paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        hasFence = track_paint_util_has_fence(EDGE_NW, session.MapPosition, trackElement, ride, session.CurrentRotation);
        if (hasFence)
        {
            imageId = session.TrackColours[SCHEME_MISC].WithIndex(SprMiniGolfFlatFenceBackSwNe);
            PaintAddImageAsParent(session, imageId, { 0, -10, height }, { 32, 1, 7 }, { 0, 0, height + 2 });
        }

        bool hasSEFence = track_paint_util_has_fence(EDGE_SE, session.MapPosition, trackElement, ride, session.CurrentRotation);
        if (hasFence)
        {
            imageId = session.TrackColours[SCHEME_MISC].WithIndex(SprMiniGolfFlatFenceFrontSwNe);
            PaintAddImageAsParent(session, imageId, { 0, 10, height }, { 32, 1, 7 }, { 0, 31, height + 2 });
        }

        track_paint_util_draw_station_covers(session, EDGE_NW, hasFence, stationObj, height);
        track_paint_util_draw_station_covers(session, EDGE_SE, hasSEFence, stationObj, height);

        paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
    }

    wooden_a_supports_paint_setup(session, (direction & 1), 0, height, session.TrackColours[SCHEME_SUPPORTS]);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0087F1AC */
static void PaintMiniGolfTrackLeftQuarterTurn1Tile(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;

    track_paint_util_left_quarter_turn_1_tile_paint(
        session, 1, height, 0, direction, session.TrackColours[SCHEME_TRACK], MiniGolfTrackSpritesQuarterTurn1Tile);

    metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);

    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D0, direction), 0xFFFF, 0);

    const bool shouldDrawFence = MiniGolfPaintUtilShouldDrawFence(session, trackElement);

    switch (direction)
    {
        case 0:
            paint_util_push_tunnel_left(session, height, TUNNEL_PATH_AND_MINI_GOLF);
            if (!shouldDrawFence)
                break;

            imageId = session.TrackColours[SCHEME_MISC].WithIndex(SprMiniGolfQuarterTurn1TileFenceBackSwNw);
            PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 26, 24, 1 }, { 6, 2, height });

            break;

        case 1:
            if (!shouldDrawFence)
                break;

            imageId = session.TrackColours[SCHEME_MISC].WithIndex(SprMiniGolfQuarterTurn1TileFenceBackNwNe);
            PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 26, 26, 1 }, { 0, 0, height });
            break;

        case 2:
            paint_util_push_tunnel_right(session, height, TUNNEL_PATH_AND_MINI_GOLF);
            if (!shouldDrawFence)
                break;

            imageId = session.TrackColours[SCHEME_MISC].WithIndex(SprMiniGolfQuarterTurn1TileFenceBackNeSe);
            PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 24, 26, 1 }, { 2, 6, height });
            break;

        case 3:
            paint_util_push_tunnel_left(session, height, TUNNEL_PATH_AND_MINI_GOLF);
            paint_util_push_tunnel_right(session, height, TUNNEL_PATH_AND_MINI_GOLF);
            if (!shouldDrawFence)
                break;

            imageId = session.TrackColours[SCHEME_MISC].WithIndex(SprMiniGolfQuarterTurn1TileFenceBackSeSw);
            PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 24, 24, 1 }, { 6, 6, height });
            break;
    }

    if (shouldDrawFence)
    {
        // TODO: The back fence uses the same x/y offsets, but uses another paint function. See if this occurs more often.
        track_paint_util_left_quarter_turn_1_tile_paint(
            session, 0, height, 24, direction, session.TrackColours[SCHEME_MISC],
            MiniGolfTrackSpritesQuarterTurn1TileFenceFront);

        switch (direction)
        {
            case 0:
                imageId = session.TrackColours[SCHEME_MISC].WithIndex(SprMiniGolfQuarterTurn1TileFenceInsideSwNw);
                PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 5, 5, 5 }, { 24, 0, height + 2 });
                break;
            case 2:
                imageId = session.TrackColours[SCHEME_MISC].WithIndex(SprMiniGolfQuarterTurn1TileFenceInsideNeSe);
                PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 5, 5, 5 }, { 0, 24, height + 2 });
                break;
        }
    }

    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0087F1BC */
static void PaintMiniGolfTrackRightQuarterTurn1Tile(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintMiniGolfTrackLeftQuarterTurn1Tile(session, ride, trackSequence, (direction + 3) % 4, height, trackElement);
}

static void PaintMiniGolfHoleAb(
    paint_session& session, uint8_t trackSequence, uint8_t direction, int32_t height, const uint32_t sprites[4][2][2])
{
    ImageId imageId;
    CoordsXY boundBox, boundBoxOffset;

    bool drewSupports = wooden_a_supports_paint_setup(
        session, (direction & 1), 0, height, session.TrackColours[SCHEME_SUPPORTS]);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);

    if ((direction == 0 && trackSequence == 0) || (direction == 2 && trackSequence == 1))
    {
        paint_util_push_tunnel_left(session, height, TUNNEL_PATH_AND_MINI_GOLF);
    }
    else if ((direction == 3 && trackSequence == 0) || (direction == 1 && trackSequence == 1))
    {
        paint_util_push_tunnel_right(session, height, TUNNEL_PATH_AND_MINI_GOLF);
    }

    if (direction & 1)
    {
        boundBox = { 26, 32 };
        boundBoxOffset = { 3, 0 };
    }
    else
    {
        boundBox = { 32, 26 };
        boundBoxOffset = { 0, 3 };
    }

    imageId = session.TrackColours[SCHEME_TRACK].WithIndex(sprites[direction][trackSequence][1]);
    PaintAddImageAsParent(
        session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 0 }, { boundBoxOffset.x, boundBoxOffset.y, height + 24 });

    if (drewSupports)
    {
        imageId = session.TrackColours[SCHEME_SUPPORTS].WithIndex(
            ((direction & 1) ? SPR_FLOOR_PLANKS_90_DEG : SPR_FLOOR_PLANKS));
        PaintAddImageAsParent(
            session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 1 }, { boundBoxOffset.x, boundBoxOffset.y, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(sprites[direction][trackSequence][0]);
        PaintAddImageAsChild(
            session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 1 }, { boundBoxOffset.x, boundBoxOffset.y, height });
    }
    else
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(sprites[direction][trackSequence][0]);
        PaintAddImageAsParent(
            session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 1 }, { boundBoxOffset.x, boundBoxOffset.y, height });
    }
}

/** rct2: 0x0087F1CC */
static void PaintMiniGolfHoleA(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintMiniGolfHoleAb(session, trackSequence, direction, height, MiniGolfTrackSpritesHoleA);
}

/** rct2: 0x0087F1DC */
static void PaintMiniGolfHoleB(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintMiniGolfHoleAb(session, trackSequence, direction, height, MiniGolfTrackSpritesHoleB);
}

/** rct2: 0x0087F1EC */
static void PaintMiniGolfHoleC(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;
    CoordsXY boundBox, boundBoxOffset;

    bool drewSupports = wooden_a_supports_paint_setup(
        session, (direction & 1), 0, height, session.TrackColours[SCHEME_SUPPORTS]);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);

    if ((direction == 0 && trackSequence == 0) || (direction == 2 && trackSequence == 1))
    {
        paint_util_push_tunnel_left(session, height, TUNNEL_PATH_AND_MINI_GOLF);
    }
    else if ((direction == 3 && trackSequence == 0) || (direction == 1 && trackSequence == 1))
    {
        paint_util_push_tunnel_right(session, height, TUNNEL_PATH_AND_MINI_GOLF);
    }

    if (direction & 1)
    {
        boundBox = { 26, 32 };
        boundBoxOffset = { 3, 0 };
    }
    else
    {
        boundBox = { 32, 26 };
        boundBoxOffset = { 0, 3 };
    }

    imageId = session.TrackColours[SCHEME_TRACK].WithIndex(MiniGolfTrackSpritesHoleC[direction][trackSequence][1]);

    switch ((direction << 4) | trackSequence)
    {
        case 0x01:
        case 0x20:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 2, 26, 3 }, { 30, 3, height + 4 });
            break;
        case 0x10:
        case 0x31:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 26, 2, 3 }, { 3, 30, height + 4 });
            break;
        default:
            PaintAddImageAsParent(
                session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 0 },
                { boundBoxOffset.x, boundBoxOffset.y, height + 24 });
            break;
    }

    if (drewSupports)
    {
        imageId = session.TrackColours[SCHEME_SUPPORTS].WithIndex(
            ((direction & 1) ? SPR_FLOOR_PLANKS_90_DEG : SPR_FLOOR_PLANKS));
        PaintAddImageAsParent(
            session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 1 }, { boundBoxOffset.x, boundBoxOffset.y, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(MiniGolfTrackSpritesHoleC[direction][trackSequence][0]);
        PaintAddImageAsChild(
            session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 1 }, { boundBoxOffset.x, boundBoxOffset.y, height });
    }
    else
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(MiniGolfTrackSpritesHoleC[direction][trackSequence][0]);
        PaintAddImageAsParent(
            session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 1 }, { boundBoxOffset.x, boundBoxOffset.y, height });
    }
}

/** rct2: 0x0087F1FC */
static void PaintMiniGolfHoleD(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;
    CoordsXY boundBox, boundBoxOffset;

    int32_t supportType = (direction & 1);
    if (trackSequence == 2)
        supportType = 1 - supportType;
    bool drewSupports = wooden_a_supports_paint_setup(session, supportType, 0, height, session.TrackColours[SCHEME_SUPPORTS]);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);

    switch ((direction << 4) | trackSequence)
    {
        case 0x00:
        case 0x12:
            paint_util_push_tunnel_left(session, height, TUNNEL_PATH_AND_MINI_GOLF);
            break;

        case 0x02:
        case 0x30:
            paint_util_push_tunnel_right(session, height, TUNNEL_PATH_AND_MINI_GOLF);
            break;
    }

    if (supportType & 1)
    {
        boundBox = { 26, 32 };
        boundBoxOffset = { 3, 0 };
    }
    else
    {
        boundBox = { 32, 26 };
        boundBoxOffset = { 0, 3 };
    }

    imageId = session.TrackColours[SCHEME_TRACK].WithIndex(MiniGolfTrackSpritesHoleD[direction][trackSequence][1]);

    switch ((direction << 4) | trackSequence)
    {
        case 0x01:
        case 0x32:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 2, 26, 3 }, { 30, 3, height + 4 });
            break;
        case 0x02:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 23, 2, 3 }, { 3, 30, height + 4 });
            break;
        case 0x10:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 2, 24, 3 }, { 30, 3, height + 4 });
            break;
        case 0x20:
        case 0x31:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 26, 2, 3 }, { 3, 30, height + 4 });
            break;
        default:
            PaintAddImageAsParent(
                session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 0 },
                { boundBoxOffset.x, boundBoxOffset.y, height + 24 });
            break;
    }

    switch ((direction << 4) | trackSequence)
    {
        case 0x02:
            boundBox = { 23, 32 };
            break;
        case 0x10:
            boundBox = { 24, 32 };
            break;
    }

    if (drewSupports)
    {
        imageId = session.TrackColours[SCHEME_SUPPORTS].WithIndex(
            ((supportType & 1) ? SPR_FLOOR_PLANKS_90_DEG : SPR_FLOOR_PLANKS));
        PaintAddImageAsParent(
            session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 1 }, { boundBoxOffset.x, boundBoxOffset.y, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(MiniGolfTrackSpritesHoleD[direction][trackSequence][0]);
        PaintAddImageAsChild(
            session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 1 }, { boundBoxOffset.x, boundBoxOffset.y, height });
    }
    else
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(MiniGolfTrackSpritesHoleD[direction][trackSequence][0]);
        PaintAddImageAsParent(
            session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 1 }, { boundBoxOffset.x, boundBoxOffset.y, height });
    }
}

/** rct2: 0x0087F1FC */
static void PaintMiniGolfHoleE(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;
    CoordsXY boundBox, boundBoxOffset;

    int32_t supportType = (direction & 1);
    if (trackSequence == 2)
        supportType = 1 - supportType;
    bool drewSupports = wooden_a_supports_paint_setup(session, supportType, 0, height, session.TrackColours[SCHEME_SUPPORTS]);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);

    switch ((direction << 4) | trackSequence)
    {
        case 0x00:
        case 0x12:
            paint_util_push_tunnel_left(session, height, TUNNEL_PATH_AND_MINI_GOLF);
            break;

        case 0x02:
        case 0x30:
            paint_util_push_tunnel_right(session, height, TUNNEL_PATH_AND_MINI_GOLF);
            break;
    }

    if (supportType & 1)
    {
        boundBox = { 26, 32 };
        boundBoxOffset = { 3, 0 };
    }
    else
    {
        boundBox = { 32, 26 };
        boundBoxOffset = { 0, 3 };
    }

    imageId = session.TrackColours[SCHEME_TRACK].WithIndex(MiniGolfTrackSpritesHoleE[direction][trackSequence][1]);

    switch ((direction << 4) | trackSequence)
    {
        case 0x01:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 2, 26, 3 }, { 30, 3, height + 4 });
            break;
        case 0x02:
        case 0x20:
        case 0x31:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 26, 2, 3 }, { 3, 30, height + 4 });
            break;
        case 0x10:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 2, 24, 3 }, { 30, 3, height + 4 });
            break;
        case 0x32:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 2, 23, 3 }, { 30, 3, height + 4 });
            break;
        default:
            PaintAddImageAsParent(
                session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 0 },
                { boundBoxOffset.x, boundBoxOffset.y, height + 24 });
            break;
    }

    switch ((direction << 4) | trackSequence)
    {
        case 0x10:
            boundBox = { 24, 32 };
            break;
        case 0x32:
            boundBox = { 32, 23 };
            break;
    }

    if (drewSupports)
    {
        imageId = session.TrackColours[SCHEME_SUPPORTS].WithIndex(
            ((supportType & 1) ? SPR_FLOOR_PLANKS_90_DEG : SPR_FLOOR_PLANKS));
        PaintAddImageAsParent(
            session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 1 }, { boundBoxOffset.x, boundBoxOffset.y, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(MiniGolfTrackSpritesHoleE[direction][trackSequence][0]);
        PaintAddImageAsChild(
            session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 1 }, { boundBoxOffset.x, boundBoxOffset.y, height });
    }
    else
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(MiniGolfTrackSpritesHoleE[direction][trackSequence][0]);
        PaintAddImageAsParent(
            session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 1 }, { boundBoxOffset.x, boundBoxOffset.y, height });
    }
}

/**
 * rct2: 0x0087EDC4
 */
TRACK_PAINT_FUNCTION GetTrackPaintFunctionMiniGolf(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return PaintMiniGolfTrackFlat;

        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
        case TrackElemType::EndStation:
            return PaintMiniGolfStation;

        case TrackElemType::Up25:
            return PaintMiniGolfTrack25DegUp;
        case TrackElemType::FlatToUp25:
            return PaintMiniGolfTrackFlatTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return PaintMiniGolfTrack25DegUpToFlat;

        case TrackElemType::Down25:
            return PaintMiniGolfTrack25DegDown;
        case TrackElemType::FlatToDown25:
            return PaintMiniGolfTrackFlatTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return PaintMiniGolfTrack25DegDownToFlat;

        case TrackElemType::LeftQuarterTurn1Tile:
            return PaintMiniGolfTrackLeftQuarterTurn1Tile;
        case TrackElemType::RightQuarterTurn1Tile:
            return PaintMiniGolfTrackRightQuarterTurn1Tile;

        case TrackElemType::MinigolfHoleA:
            return PaintMiniGolfHoleA;
        case TrackElemType::MinigolfHoleB:
            return PaintMiniGolfHoleB;
        case TrackElemType::MinigolfHoleC:
            return PaintMiniGolfHoleC;
        case TrackElemType::MinigolfHoleD:
            return PaintMiniGolfHoleD;
        case TrackElemType::MinigolfHoleE:
            return PaintMiniGolfHoleE;
    }

    return nullptr;
}

/**
 * rct2: 0x006D42F0
 */
void vehicle_visual_mini_golf_player(
    paint_session& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle)
{
    if (vehicle->num_peeps == 0)
    {
        return;
    }

    rct_drawpixelinfo* edi = &session.DPI;
    if (edi->zoom_level >= ZoomLevel{ 2 })
    {
        return;
    }

    auto ride = vehicle->GetRide();
    if (ride == nullptr)
        return;

    auto rideEntry = ride->GetRideEntry();
    if (rideEntry == nullptr)
        return;

    auto* peep = GetEntity<Guest>(vehicle->peep[0]);
    if (peep == nullptr)
        return;

    uint8_t frame = MiniGolfPeepAnimationFrames[EnumValue(vehicle->mini_golf_current_animation)][vehicle->animation_frame];
    uint32_t ebx = (frame << 2) + OpenRCT2::Entity::Yaw::YawTo4(imageDirection);

    ImageIndex index = rideEntry->Cars[0].base_image_id + 1 + ebx;
    auto image = ImageId(index, peep->TshirtColour, peep->TrousersColour);
    PaintAddImageAsParent(session, image, { 0, 0, z }, { 1, 1, 11 }, { 0, 0, z + 5 });
}

/**
 * rct2: 0x006D43C6
 */
void VehicleVisualMiniGolfBall(
    paint_session& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle)
{
    if (vehicle->mini_golf_current_animation != MiniGolfAnimation::PlaceBallDown)
    {
        return;
    }

    rct_drawpixelinfo* edi = &session.DPI;
    if (edi->zoom_level >= ZoomLevel{ 1 })
    {
        return;
    }

    auto ride = vehicle->GetRide();
    if (ride == nullptr)
        return;

    auto rideEntry = ride->GetRideEntry();
    if (rideEntry == nullptr)
        return;

    uint32_t image_id = rideEntry->Cars[0].base_image_id;
    PaintAddImageAsParent(session, ImageId(image_id), { 0, 0, z }, { 1, 1, 0 }, { 0, 0, z + 3 });
}
