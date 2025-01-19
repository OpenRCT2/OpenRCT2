/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Drawing.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../Game.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../config/Config.h"
#include "../core/Guard.hpp"
#include "../object/Object.h"
#include "../object/ObjectEntryManager.h"
#include "../object/WaterEntry.h"
#include "../platform/Platform.h"
#include "../sprites.h"
#include "../util/Util.h"
#include "../world/Climate.h"
#include "../world/Location.hpp"
#include "LightFX.h"

#include <array>
#include <cassert>
#include <cstring>
#include <numeric>

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;

static auto _defaultPaletteMapping = []() {
    std::array<uint8_t, 256> res;
    std::iota(res.begin(), res.end(), 0);
    return res;
}();

PaletteMap PaletteMap::GetDefault()
{
    return PaletteMap(_defaultPaletteMapping);
}

uint8_t& PaletteMap::operator[](size_t index)
{
    return _data[index];
}

uint8_t PaletteMap::operator[](size_t index) const
{
    return _data[index];
}

uint8_t PaletteMap::Blend(uint8_t src, uint8_t dst) const
{
#ifdef _DEBUG
    // src = 0 would be transparent so there is no blend palette for that, hence (src - 1)
    assert(src != 0 && (src - 1) < _numMaps);
    assert(dst < _mapLength);
#endif
    auto idx = ((src - 1) * 256) + dst;
    return _data[idx];
}

void PaletteMap::Copy(size_t dstIndex, const PaletteMap& src, size_t srcIndex, size_t length)
{
    auto maxLength = std::min(_data.size() - srcIndex, _data.size() - dstIndex);
    assert(length <= maxLength);
    auto copyLength = std::min(length, maxLength);
    std::copy(src._data.begin() + srcIndex, src._data.begin() + srcIndex + copyLength, _data.begin() + dstIndex);
}

OpenRCT2::Drawing::GamePalette gPalette;
OpenRCT2::Drawing::GamePalette gGamePalette;
uint32_t gPaletteEffectFrame;

ImageId gPickupPeepImage;
int32_t gPickupPeepX;
int32_t gPickupPeepY;

// Originally 0x9ABE04
uint8_t gTextPalette[0x8] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

enum
{
    SPR_PALETTE_3100 = 3100,
    SPR_PALETTE_3101 = 3101,
    SPR_PALETTE_3102 = 3102,
    SPR_PALETTE_3103 = 3103,
    SPR_PALETTE_3104 = 3104,
    SPR_PALETTE_3105 = 3105,
    SPR_PALETTE_3106 = 3106,
    SPR_PALETTE_3107 = 3107,
    SPR_PALETTE_3108 = 3108,
    SPR_PALETTE_3109 = 3109,
    SPR_PALETTE_3110 = 3110,

    SPR_PALETTE_BLACK = 4915,
    SPR_PALETTE_GREY = 4916,
    SPR_PALETTE_WHITE = 4917,
    SPR_PALETTE_DARK_PURPLE = 4918,
    SPR_PALETTE_LIGHT_PURPLE = 4919,
    SPR_PALETTE_BRIGHT_PURPLE = 4920,
    SPR_PALETTE_DARK_BLUE = 4921,
    SPR_PALETTE_LIGHT_BLUE = 4922,
    SPR_PALETTE_ICY_BLUE = 4923,
    SPR_PALETTE_TEAL = 4924,
    SPR_PALETTE_AQUAMARINE = 4925,
    SPR_PALETTE_SATURATED_GREEN = 4926,
    SPR_PALETTE_DARK_GREEN = 4927,
    SPR_PALETTE_MOSS_GREEN = 4928,
    SPR_PALETTE_BRIGHT_GREEN = 4929,
    SPR_PALETTE_OLIVE_GREEN = 4930,
    SPR_PALETTE_DARK_OLIVE_GREEN = 4931,
    SPR_PALETTE_BRIGHT_YELLOW = 4932,
    SPR_PALETTE_YELLOW = 4933,
    SPR_PALETTE_DARK_YELLOW = 4934,
    SPR_PALETTE_LIGHT_ORANGE = 4935,
    SPR_PALETTE_DARK_ORANGE = 4936,
    SPR_PALETTE_LIGHT_BROWN = 4937,
    SPR_PALETTE_SATURATED_BROWN = 4938,
    SPR_PALETTE_DARK_BROWN = 4939,
    SPR_PALETTE_SALMON_PINK = 4940,
    SPR_PALETTE_BORDEAUX_RED = 4941,
    SPR_PALETTE_SATURATED_RED = 4942,
    SPR_PALETTE_BRIGHT_RED = 4943,
    SPR_PALETTE_DARK_PINK = 4944,
    SPR_PALETTE_BRIGHT_PINK = 4945,
    SPR_PALETTE_LIGHT_PINK = 4946,
    SPR_PALETTE_WATER = 4947,
    SPR_PALETTE_4948 = 4948,
    SPR_PALETTE_4949 = 4949,
    SPR_PALETTE_4950 = 4950,
    SPR_PALETTE_DARKEN_3 = 4951,
    SPR_PALETTE_4952 = 4952,
    SPR_PALETTE_DARKEN_1 = 4953,
    SPR_PALETTE_DARKEN_2 = 4954,
    SPR_PALETTE_4955 = 4955,
    SPR_PALETTE_TRANSLUCENT_GREY = 4956,
    SPR_PALETTE_TRANSLUCENT_GREY_HIGHLIGHT = 4957,
    SPR_PALETTE_TRANSLUCENT_GREY_SHADOW = 4958,
    SPR_PALETTE_TRANSLUCENT_LIGHT_BLUE = 4959,
    SPR_PALETTE_TRANSLUCENT_LIGHT_BLUE_HIGHLIGHT = 4960,
    SPR_PALETTE_TRANSLUCENT_LIGHT_BLUE_SHADOW = 4961,
    SPR_PALETTE_TRANSLUCENT_BORDEAUX_RED = 4962,
    SPR_PALETTE_TRANSLUCENT_BORDEAUX_RED_HIGHLIGHT = 4963,
    SPR_PALETTE_TRANSLUCENT_BORDEAUX_RED_SHADOW = 4964,
    SPR_PALETTE_TRANSLUCENT_DARK_GREEN = 4965,
    SPR_PALETTE_TRANSLUCENT_DARK_GREEN_HIGHLIGHT = 4966,
    SPR_PALETTE_TRANSLUCENT_DARK_GREEN_SHADOW = 4967,
    SPR_PALETTE_TRANSLUCENT_LIGHT_PURPLE = 4968,
    SPR_PALETTE_TRANSLUCENT_LIGHT_PURPLE_HIGHLIGHT = 4969,
    SPR_PALETTE_TRANSLUCENT_LIGHT_PURPLE_SHADOW = 4970,
    SPR_PALETTE_TRANSLUCENT_DARK_OLIVE_GREEN = 4971,
    SPR_PALETTE_TRANSLUCENT_DARK_OLIVE_GREEN_HIGHLIGHT = 4972,
    SPR_PALETTE_TRANSLUCENT_DARK_OLIVE_GREEN_SHADOW = 4973,
    SPR_PALETTE_TRANSLUCENT_LIGHT_BROWN = 4974,
    SPR_PALETTE_TRANSLUCENT_LIGHT_BROWN_HIGHLIGHT = 4975,
    SPR_PALETTE_TRANSLUCENT_LIGHT_BROWN_SHADOW = 4976,
    SPR_PALETTE_TRANSLUCENT_YELLOW = 4977,
    SPR_PALETTE_TRANSLUCENT_YELLOW_HIGHLIGHT = 4978,
    SPR_PALETTE_TRANSLUCENT_YELLOW_SHADOW = 4979,
    SPR_PALETTE_TRANSLUCENT_MOSS_GREEN = 4980,
    SPR_PALETTE_TRANSLUCENT_MOSS_GREEN_HIGHLIGHT = 4981,
    SPR_PALETTE_TRANSLUCENT_MOSS_GREEN_SHADOW = 4982,
    SPR_PALETTE_TRANSLUCENT_OLIVE_GREEN = 4983,
    SPR_PALETTE_TRANSLUCENT_OLIVE_GREEN_HIGHLIGHT = 4984,
    SPR_PALETTE_TRANSLUCENT_OLIVE_GREEN_SHADOW = 4985,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_GREEN = 4986,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_GREEN_HIGHLIGHT = 4987,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_GREEN_SHADOW = 4988,
    SPR_PALETTE_TRANSLUCENT_SALMON_PINK = 4989,
    SPR_PALETTE_TRANSLUCENT_SALMON_PINK_HIGHLIGHT = 4990,
    SPR_PALETTE_TRANSLUCENT_SALMON_PINK_SHADOW = 4991,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_PURPLE = 4992,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_PURPLE_HIGHLIGHT = 4993,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_PURPLE_SHADOW = 4994,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_RED = 4995,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_RED_HIGHLIGHT = 4996,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_RED_SHADOW = 4997,
    SPR_PALETTE_TRANSLUCENT_LIGHT_ORANGE = 4998,
    SPR_PALETTE_TRANSLUCENT_LIGHT_ORANGE_HIGHLIGHT = 4999,
    SPR_PALETTE_TRANSLUCENT_LIGHT_ORANGE_SHADOW = 5000,
    SPR_PALETTE_TRANSLUCENT_TEAL = 5001,
    SPR_PALETTE_TRANSLUCENT_TEAL_HIGHLIGHT = 5002,
    SPR_PALETTE_TRANSLUCENT_TEAL_SHADOW = 5003,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_PINK = 5004,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_PINK_HIGHLIGHT = 5005,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_PINK_SHADOW = 5006,
    SPR_PALETTE_TRANSLUCENT_DARK_BROWN = 5007,
    SPR_PALETTE_TRANSLUCENT_DARK_BROWN_HIGHLIGHT = 5008,
    SPR_PALETTE_TRANSLUCENT_DARK_BROWN_SHADOW = 5009,
    SPR_PALETTE_TRANSLUCENT_LIGHT_PINK = 5010,
    SPR_PALETTE_TRANSLUCENT_LIGHT_PINK_HIGHLIGHT = 5011,
    SPR_PALETTE_TRANSLUCENT_LIGHT_PINK_SHADOW = 5012,
    SPR_PALETTE_TRANSLUCENT_WHITE = 5013,
    SPR_PALETTE_TRANSLUCENT_WHITE_HIGHLIGHT = 5014,
    SPR_PALETTE_TRANSLUCENT_WHITE_SHADOW = 5015,
    SPR_PALETTE_GLASS_BLACK = 5016,
    SPR_PALETTE_GLASS_GREY = 5017,
    SPR_PALETTE_GLASS_WHITE = 5018,
    SPR_PALETTE_GLASS_DARK_PURPLE = 5019,
    SPR_PALETTE_GLASS_LIGHT_PURPLE = 5020,
    SPR_PALETTE_GLASS_BRIGHT_PURPLE = 5021,
    SPR_PALETTE_GLASS_DARK_BLUE = 5022,
    SPR_PALETTE_GLASS_LIGHT_BLUE = 5023,
    SPR_PALETTE_GLASS_ICY_BLUE = 5024,
    SPR_PALETTE_GLASS_TEAL = 5025,
    SPR_PALETTE_GLASS_AQUAMARINE = 5026,
    SPR_PALETTE_GLASS_SATURATED_GREEN = 5027,
    SPR_PALETTE_GLASS_DARK_GREEN = 5028,
    SPR_PALETTE_GLASS_MOSS_GREEN = 5029,
    SPR_PALETTE_GLASS_BRIGHT_GREEN = 5030,
    SPR_PALETTE_GLASS_OLIVE_GREEN = 5031,
    SPR_PALETTE_GLASS_DARK_OLIVE_GREEN = 5032,
    SPR_PALETTE_GLASS_BRIGHT_YELLOW = 5033,
    SPR_PALETTE_GLASS_YELLOW = 5034,
    SPR_PALETTE_GLASS_DARK_YELLOW = 5035,
    SPR_PALETTE_GLASS_LIGHT_ORANGE = 5036,
    SPR_PALETTE_GLASS_DARK_ORANGE = 5037,
    SPR_PALETTE_GLASS_LIGHT_BROWN = 5038,
    SPR_PALETTE_GLASS_SATURATED_BROWN = 5039,
    SPR_PALETTE_GLASS_DARK_BROWN = 5040,
    SPR_PALETTE_GLASS_SALMON_PINK = 5041,
    SPR_PALETTE_GLASS_BORDEAUX_RED = 5042,
    SPR_PALETTE_GLASS_SATURATED_RED = 5043,
    SPR_PALETTE_GLASS_BRIGHT_RED = 5044,
    SPR_PALETTE_GLASS_DARK_PINK = 5045,
    SPR_PALETTE_GLASS_BRIGHT_PINK = 5046,
    SPR_PALETTE_GLASS_LIGHT_PINK = 5047,

    SPR_PALETTE_DARK_OLIVE_DARK = SPR_G2_PALETTE_BEGIN,
    SPR_PALETTE_DARK_OLIVE_LIGHT,
    SPR_PALETTE_SATURATED_BROWN_LIGHT,
    SPR_PALETTE_BORDEAUX_RED_DARK,
    SPR_PALETTE_BORDEAUX_RED_LIGHT,
    SPR_PALETTE_GRASS_GREEN_DARK,
    SPR_PALETTE_GRASS_GREEN_LIGHT,
    SPR_PALETTE_OLIVE_DARK,
    SPR_PALETTE_OLIVE_LIGHT,
    SPR_PALETTE_SATURATED_GREEN_LIGHT,
    SPR_PALETTE_TAN_DARK,
    SPR_PALETTE_TAN_LIGHT,
    SPR_PALETTE_DULL_PURPLE_LIGHT,
    SPR_PALETTE_DULL_GREEN_DARK,
    SPR_PALETTE_DULL_GREEN_LIGHT,
    SPR_PALETTE_SATURATED_PURPLE_DARK,
    SPR_PALETTE_SATURATED_PURPLE_LIGHT,
    SPR_PALETTE_ORANGE_LIGHT,
    SPR_PALETTE_AQUA_DARK,
    SPR_PALETTE_MAGENTA_LIGHT,
    SPR_PALETTE_DULL_BROWN_DARK,
    SPR_PALETTE_DULL_BROWN_LIGHT,
    SPR_PALETTE_INVISIBLE,
    SPR_PALETTE_VOID,

    SPR_PALETTE_GLASS_DARK_OLIVE_DARK,
    SPR_PALETTE_GLASS_DARK_OLIVE_LIGHT,
    SPR_PALETTE_GLASS_SATURATED_BROWN_LIGHT,
    SPR_PALETTE_GLASS_BORDEAUX_RED_DARK,
    SPR_PALETTE_GLASS_BORDEAUX_RED_LIGHT,
    SPR_PALETTE_GLASS_GRASS_GREEN_DARK,
    SPR_PALETTE_GLASS_GRASS_GREEN_LIGHT,
    SPR_PALETTE_GLASS_OLIVE_DARK,
    SPR_PALETTE_GLASS_OLIVE_LIGHT,
    SPR_PALETTE_GLASS_SATURATED_GREEN_LIGHT,
    SPR_PALETTE_GLASS_TAN_DARK,
    SPR_PALETTE_GLASS_TAN_LIGHT,
    SPR_PALETTE_GLASS_DULL_PURPLE_LIGHT,
    SPR_PALETTE_GLASS_DULL_GREEN_DARK,
    SPR_PALETTE_GLASS_DULL_GREEN_LIGHT,
    SPR_PALETTE_GLASS_SATURATED_PURPLE_DARK,
    SPR_PALETTE_GLASS_SATURATED_PURPLE_LIGHT,
    SPR_PALETTE_GLASS_ORANGE_LIGHT,
    SPR_PALETTE_GLASS_AQUA_DARK,
    SPR_PALETTE_GLASS_MAGENTA_LIGHT,
    SPR_PALETTE_GLASS_DULL_BROWN_DARK,
    SPR_PALETTE_GLASS_DULL_BROWN_LIGHT,
    SPR_PALETTE_GLASS_INVISIBLE,
    SPR_PALETTE_GLASS_VOID,
};

static constexpr FilterPaletteID kGlassPaletteIds[COLOUR_COUNT] = {
    FilterPaletteID::PaletteGlassBlack,
    FilterPaletteID::PaletteGlassGrey,
    FilterPaletteID::PaletteGlassWhite,
    FilterPaletteID::PaletteGlassDarkPurple,
    FilterPaletteID::PaletteGlassLightPurple,
    FilterPaletteID::PaletteGlassBrightPurple,
    FilterPaletteID::PaletteGlassDarkBlue,
    FilterPaletteID::PaletteGlassLightBlue,
    FilterPaletteID::PaletteGlassIcyBlue,
    FilterPaletteID::PaletteGlassTeal,
    FilterPaletteID::PaletteGlassAquamarine,
    FilterPaletteID::PaletteGlassSaturatedGreen,
    FilterPaletteID::PaletteGlassDarkGreen,
    FilterPaletteID::PaletteGlassMossGreen,
    FilterPaletteID::PaletteGlassBrightGreen,
    FilterPaletteID::PaletteGlassOliveGreen,
    FilterPaletteID::PaletteGlassDarkOliveGreen,
    FilterPaletteID::PaletteGlassBrightYellow,
    FilterPaletteID::PaletteGlassYellow,
    FilterPaletteID::PaletteGlassDarkYellow,
    FilterPaletteID::PaletteGlassLightOrange,
    FilterPaletteID::PaletteGlassDarkOrange,
    FilterPaletteID::PaletteGlassLightBrown,
    FilterPaletteID::PaletteGlassSaturatedBrown,
    FilterPaletteID::PaletteGlassDarkBrown,
    FilterPaletteID::PaletteGlassSalmonPink,
    FilterPaletteID::PaletteGlassBordeauxRed,
    FilterPaletteID::PaletteGlassSaturatedRed,
    FilterPaletteID::PaletteGlassBrightRed,
    FilterPaletteID::PaletteGlassDarkPink,
    FilterPaletteID::PaletteGlassBrightPink,
    FilterPaletteID::PaletteGlassLightPink,
    FilterPaletteID::PaletteGlassDarkOliveDark,
    FilterPaletteID::PaletteGlassDarkOliveLight,
    FilterPaletteID::PaletteGlassSaturatedBrownLight,
    FilterPaletteID::PaletteGlassBordeauxRedDark,
    FilterPaletteID::PaletteGlassBordeauxRedLight,
    FilterPaletteID::PaletteGlassGrassGreenDark,
    FilterPaletteID::PaletteGlassGrassGreenLight,
    FilterPaletteID::PaletteGlassOliveDark,
    FilterPaletteID::PaletteGlassOliveLight,
    FilterPaletteID::PaletteGlassSaturatedGreenLight,
    FilterPaletteID::PaletteGlassTanDark,
    FilterPaletteID::PaletteGlassTanLight,
    FilterPaletteID::PaletteGlassDullPurpleLight,
    FilterPaletteID::PaletteGlassDullGreenDark,
    FilterPaletteID::PaletteGlassDullGreenLight,
    FilterPaletteID::PaletteGlassSaturatedPurpleDark,
    FilterPaletteID::PaletteGlassSaturatedPurpleLight,
    FilterPaletteID::PaletteGlassOrangeLight,
    FilterPaletteID::PaletteGlassAquaDark,
    FilterPaletteID::PaletteGlassMagentaLight,
    FilterPaletteID::PaletteGlassDullBrownDark,
    FilterPaletteID::PaletteGlassDullBrownLight,
    FilterPaletteID::PaletteGlassInvisible,
    FilterPaletteID::PaletteGlassVoid,
};

// Previously 0x97FCBC use it to get the correct palette from g1_elements
// clang-format off
static constexpr uint16_t kPaletteToG1Offset[kPaletteTotalOffsets] = {
    SPR_PALETTE_BLACK,
    SPR_PALETTE_GREY,
    SPR_PALETTE_WHITE,
    SPR_PALETTE_DARK_PURPLE,
    SPR_PALETTE_LIGHT_PURPLE,
    SPR_PALETTE_BRIGHT_PURPLE,
    SPR_PALETTE_DARK_BLUE,
    SPR_PALETTE_LIGHT_BLUE,
    SPR_PALETTE_ICY_BLUE,
    SPR_PALETTE_TEAL,
    SPR_PALETTE_AQUAMARINE,
    SPR_PALETTE_SATURATED_GREEN,
    SPR_PALETTE_DARK_GREEN,
    SPR_PALETTE_MOSS_GREEN,
    SPR_PALETTE_BRIGHT_GREEN,
    SPR_PALETTE_OLIVE_GREEN,
    SPR_PALETTE_DARK_OLIVE_GREEN,
    SPR_PALETTE_BRIGHT_YELLOW,
    SPR_PALETTE_YELLOW,
    SPR_PALETTE_DARK_YELLOW,
    SPR_PALETTE_LIGHT_ORANGE,
    SPR_PALETTE_DARK_ORANGE,
    SPR_PALETTE_LIGHT_BROWN,
    SPR_PALETTE_SATURATED_BROWN,
    SPR_PALETTE_DARK_BROWN,
    SPR_PALETTE_SALMON_PINK,
    SPR_PALETTE_BORDEAUX_RED,
    SPR_PALETTE_SATURATED_RED,
    SPR_PALETTE_BRIGHT_RED,
    SPR_PALETTE_DARK_PINK,
    SPR_PALETTE_BRIGHT_PINK,
    SPR_PALETTE_LIGHT_PINK,
    SPR_PALETTE_DARK_OLIVE_DARK,
    SPR_PALETTE_DARK_OLIVE_LIGHT,
    SPR_PALETTE_SATURATED_BROWN_LIGHT,
    SPR_PALETTE_BORDEAUX_RED_DARK,
    SPR_PALETTE_BORDEAUX_RED_LIGHT,
    SPR_PALETTE_GRASS_GREEN_DARK,
    SPR_PALETTE_GRASS_GREEN_LIGHT,
    SPR_PALETTE_OLIVE_DARK,
    SPR_PALETTE_OLIVE_LIGHT,
    SPR_PALETTE_SATURATED_GREEN_LIGHT,
    SPR_PALETTE_TAN_DARK,
    SPR_PALETTE_TAN_LIGHT,
    SPR_PALETTE_DULL_PURPLE_LIGHT,
    SPR_PALETTE_DULL_GREEN_DARK,
    SPR_PALETTE_DULL_GREEN_LIGHT,
    SPR_PALETTE_SATURATED_PURPLE_DARK,
    SPR_PALETTE_SATURATED_PURPLE_LIGHT,
    SPR_PALETTE_ORANGE_LIGHT,
    SPR_PALETTE_AQUA_DARK,
    SPR_PALETTE_MAGENTA_LIGHT,
    SPR_PALETTE_DULL_BROWN_DARK,
    SPR_PALETTE_DULL_BROWN_LIGHT,
    SPR_PALETTE_INVISIBLE,
    SPR_PALETTE_VOID,

    SPR_PALETTE_WATER,      // PaletteWater (water)
    SPR_PALETTE_3100,
    SPR_PALETTE_3101,       // Palette34
    SPR_PALETTE_3102,
    SPR_PALETTE_3103,
    SPR_PALETTE_3104,
    SPR_PALETTE_3106,
    SPR_PALETTE_3107,
    SPR_PALETTE_3108,       // 40
    SPR_PALETTE_3109,
    SPR_PALETTE_3110,
    SPR_PALETTE_3105,
    SPR_PALETTE_4948,
    SPR_PALETTE_4949,       // Palette45
    SPR_PALETTE_4950,
    SPR_PALETTE_DARKEN_3,   // PaletteDarken3
    SPR_PALETTE_4952,       // Decreases contrast
    SPR_PALETTE_DARKEN_1,   // PaletteDarken1
    SPR_PALETTE_DARKEN_2,   // PaletteDarken2 (construction marker)
    SPR_PALETTE_4955,       // Palette51

    SPR_PALETTE_TRANSLUCENT_GREY,
    SPR_PALETTE_TRANSLUCENT_GREY_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_GREY_SHADOW,
    SPR_PALETTE_TRANSLUCENT_LIGHT_BLUE,
    SPR_PALETTE_TRANSLUCENT_LIGHT_BLUE_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_LIGHT_BLUE_SHADOW,
    SPR_PALETTE_TRANSLUCENT_BORDEAUX_RED,
    SPR_PALETTE_TRANSLUCENT_BORDEAUX_RED_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_BORDEAUX_RED_SHADOW,
    SPR_PALETTE_TRANSLUCENT_DARK_GREEN,
    SPR_PALETTE_TRANSLUCENT_DARK_GREEN_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_DARK_GREEN_SHADOW,
    SPR_PALETTE_TRANSLUCENT_LIGHT_PURPLE,
    SPR_PALETTE_TRANSLUCENT_LIGHT_PURPLE_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_LIGHT_PURPLE_SHADOW,
    SPR_PALETTE_TRANSLUCENT_DARK_OLIVE_GREEN,
    SPR_PALETTE_TRANSLUCENT_DARK_OLIVE_GREEN_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_DARK_OLIVE_GREEN_SHADOW,
    SPR_PALETTE_TRANSLUCENT_LIGHT_BROWN,
    SPR_PALETTE_TRANSLUCENT_LIGHT_BROWN_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_LIGHT_BROWN_SHADOW,
    SPR_PALETTE_TRANSLUCENT_YELLOW,
    SPR_PALETTE_TRANSLUCENT_YELLOW_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_YELLOW_SHADOW,
    SPR_PALETTE_TRANSLUCENT_MOSS_GREEN,
    SPR_PALETTE_TRANSLUCENT_MOSS_GREEN_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_MOSS_GREEN_SHADOW,
    SPR_PALETTE_TRANSLUCENT_OLIVE_GREEN,
    SPR_PALETTE_TRANSLUCENT_OLIVE_GREEN_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_OLIVE_GREEN_SHADOW,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_GREEN,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_GREEN_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_GREEN_SHADOW,
    SPR_PALETTE_TRANSLUCENT_SALMON_PINK,
    SPR_PALETTE_TRANSLUCENT_SALMON_PINK_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_SALMON_PINK_SHADOW,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_PURPLE,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_PURPLE_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_PURPLE_SHADOW,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_RED,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_RED_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_RED_SHADOW,
    SPR_PALETTE_TRANSLUCENT_LIGHT_ORANGE,
    SPR_PALETTE_TRANSLUCENT_LIGHT_ORANGE_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_LIGHT_ORANGE_SHADOW,
    SPR_PALETTE_TRANSLUCENT_TEAL,
    SPR_PALETTE_TRANSLUCENT_TEAL_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_TEAL_SHADOW,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_PINK,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_PINK_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_BRIGHT_PINK_SHADOW,
    SPR_PALETTE_TRANSLUCENT_DARK_BROWN,
    SPR_PALETTE_TRANSLUCENT_DARK_BROWN_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_DARK_BROWN_SHADOW,
    SPR_PALETTE_TRANSLUCENT_LIGHT_PINK,
    SPR_PALETTE_TRANSLUCENT_LIGHT_PINK_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_LIGHT_PINK_SHADOW,
    SPR_PALETTE_TRANSLUCENT_WHITE,
    SPR_PALETTE_TRANSLUCENT_WHITE_HIGHLIGHT,
    SPR_PALETTE_TRANSLUCENT_WHITE_SHADOW,

    SPR_PALETTE_GLASS_BLACK,
    SPR_PALETTE_GLASS_GREY,
    SPR_PALETTE_GLASS_WHITE,
    SPR_PALETTE_GLASS_DARK_PURPLE,
    SPR_PALETTE_GLASS_LIGHT_PURPLE,
    SPR_PALETTE_GLASS_BRIGHT_PURPLE,
    SPR_PALETTE_GLASS_DARK_BLUE,
    SPR_PALETTE_GLASS_LIGHT_BLUE,
    SPR_PALETTE_GLASS_ICY_BLUE,
    SPR_PALETTE_GLASS_TEAL,
    SPR_PALETTE_GLASS_AQUAMARINE,
    SPR_PALETTE_GLASS_SATURATED_GREEN,
    SPR_PALETTE_GLASS_DARK_GREEN,
    SPR_PALETTE_GLASS_MOSS_GREEN,
    SPR_PALETTE_GLASS_BRIGHT_GREEN,
    SPR_PALETTE_GLASS_OLIVE_GREEN,
    SPR_PALETTE_GLASS_DARK_OLIVE_GREEN,
    SPR_PALETTE_GLASS_BRIGHT_YELLOW,
    SPR_PALETTE_GLASS_YELLOW,
    SPR_PALETTE_GLASS_DARK_YELLOW,
    SPR_PALETTE_GLASS_LIGHT_ORANGE,
    SPR_PALETTE_GLASS_DARK_ORANGE,
    SPR_PALETTE_GLASS_LIGHT_BROWN,
    SPR_PALETTE_GLASS_SATURATED_BROWN,
    SPR_PALETTE_GLASS_DARK_BROWN,
    SPR_PALETTE_GLASS_SALMON_PINK,
    SPR_PALETTE_GLASS_BORDEAUX_RED,
    SPR_PALETTE_GLASS_SATURATED_RED,
    SPR_PALETTE_GLASS_BRIGHT_RED,
    SPR_PALETTE_GLASS_DARK_PINK,
    SPR_PALETTE_GLASS_BRIGHT_PINK,
    SPR_PALETTE_GLASS_LIGHT_PINK,
    SPR_PALETTE_GLASS_DARK_OLIVE_DARK,
    SPR_PALETTE_GLASS_DARK_OLIVE_LIGHT,
    SPR_PALETTE_GLASS_SATURATED_BROWN_LIGHT,
    SPR_PALETTE_GLASS_BORDEAUX_RED_DARK,
    SPR_PALETTE_GLASS_BORDEAUX_RED_LIGHT,
    SPR_PALETTE_GLASS_GRASS_GREEN_DARK,
    SPR_PALETTE_GLASS_GRASS_GREEN_LIGHT,
    SPR_PALETTE_GLASS_OLIVE_DARK,
    SPR_PALETTE_GLASS_OLIVE_LIGHT,
    SPR_PALETTE_GLASS_SATURATED_GREEN_LIGHT,
    SPR_PALETTE_GLASS_TAN_DARK,
    SPR_PALETTE_GLASS_TAN_LIGHT,
    SPR_PALETTE_GLASS_DULL_PURPLE_LIGHT,
    SPR_PALETTE_GLASS_DULL_GREEN_DARK,
    SPR_PALETTE_GLASS_DULL_GREEN_LIGHT,
    SPR_PALETTE_GLASS_SATURATED_PURPLE_DARK,
    SPR_PALETTE_GLASS_SATURATED_PURPLE_LIGHT,
    SPR_PALETTE_GLASS_ORANGE_LIGHT,
    SPR_PALETTE_GLASS_AQUA_DARK,
    SPR_PALETTE_GLASS_MAGENTA_LIGHT,
    SPR_PALETTE_GLASS_DULL_BROWN_DARK,
    SPR_PALETTE_GLASS_DULL_BROWN_LIGHT,
    SPR_PALETTE_GLASS_INVISIBLE,
    SPR_PALETTE_GLASS_VOID,
};

#define WINDOW_PALETTE_GREY                 {FilterPaletteID::PaletteTranslucentGrey,                  FilterPaletteID::PaletteTranslucentGreyHighlight,             FilterPaletteID::PaletteTranslucentGreyShadow}
#define WINDOW_PALETTE_LIGHT_PURPLE         {FilterPaletteID::PaletteTranslucentLightPurple,          FilterPaletteID::PaletteTranslucentLightPurpleHighlight,     FilterPaletteID::PaletteTranslucentLightPurpleShadow}
#define WINDOW_PALETTE_BRIGHT_PURPLE        {FilterPaletteID::PaletteTranslucentBrightPurple,     FilterPaletteID::PaletteTranslucentBrightPurpleHighlight,    FilterPaletteID::PaletteTranslucentBrightPurpleShadow}
#define WINDOW_PALETTE_LIGHT_BLUE           {FilterPaletteID::PaletteTranslucentLightBlue,            FilterPaletteID::PaletteTranslucentLightBlueHighlight,       FilterPaletteID::PaletteTranslucentLightBlueShadow}
#define WINDOW_PALETTE_TEAL                 {FilterPaletteID::PaletteTranslucentTeal,                  FilterPaletteID::PaletteTranslucentTealHighlight,             FilterPaletteID::PaletteTranslucentTealShadow}
#define WINDOW_PALETTE_BRIGHT_GREEN         {FilterPaletteID::PaletteTranslucentBrightGreen,          FilterPaletteID::PaletteTranslucentBrightGreenHighlight,     FilterPaletteID::PaletteTranslucentBrightGreenShadow}
#define WINDOW_PALETTE_DARK_GREEN           {FilterPaletteID::PaletteTranslucentDarkGreen,        FilterPaletteID::PaletteTranslucentDarkGreenHighlight,       FilterPaletteID::PaletteTranslucentDarkGreenShadow}
#define WINDOW_PALETTE_MOSS_GREEN           {FilterPaletteID::PaletteTranslucentMossGreen,        FilterPaletteID::PaletteTranslucentMossGreenHighlight,       FilterPaletteID::PaletteTranslucentMossGreenShadow}
#define WINDOW_PALETTE_OLIVE_GREEN          {FilterPaletteID::PaletteTranslucentOliveGreen,       FilterPaletteID::PaletteTranslucentOliveGreenHighlight,      FilterPaletteID::PaletteTranslucentOliveGreenShadow}
#define WINDOW_PALETTE_DARK_OLIVE_GREEN     {FilterPaletteID::PaletteTranslucentDarkOliveGreen,  FilterPaletteID::PaletteTranslucentDarkOliveGreenHighlight, FilterPaletteID::PaletteTranslucentDarkOliveGreenShadow}
#define WINDOW_PALETTE_YELLOW               {FilterPaletteID::PaletteTranslucentYellow,                FilterPaletteID::PaletteTranslucentYellowHighlight,           FilterPaletteID::PaletteTranslucentYellowShadow}
#define WINDOW_PALETTE_LIGHT_ORANGE         {FilterPaletteID::PaletteTranslucentLightOrange,          FilterPaletteID::PaletteTranslucentLightOrangeHighlight,     FilterPaletteID::PaletteTranslucentLightOrangeShadow}
#define WINDOW_PALETTE_LIGHT_BROWN          {FilterPaletteID::PaletteTranslucentLightBrown,           FilterPaletteID::PaletteTranslucentLightBrownHighlight,      FilterPaletteID::PaletteTranslucentLightBrownShadow}
#define WINDOW_PALETTE_DARK_BROWN           {FilterPaletteID::PaletteTranslucentDarkBrown,        FilterPaletteID::PaletteTranslucentDarkBrownHighlight,       FilterPaletteID::PaletteTranslucentDarkBrownShadow}
#define WINDOW_PALETTE_SALMON_PINK          {FilterPaletteID::PaletteTranslucentSalmonPink,       FilterPaletteID::PaletteTranslucentSalmonPinkHighlight,      FilterPaletteID::PaletteTranslucentSalmonPinkShadow}
#define WINDOW_PALETTE_BORDEAUX_RED         {FilterPaletteID::PaletteTranslucentBordeauxRed,      FilterPaletteID::PaletteTranslucentBordeauxRedHighlight,     FilterPaletteID::PaletteTranslucentBordeauxRedShadow}
#define WINDOW_PALETTE_BRIGHT_RED           {FilterPaletteID::PaletteTranslucentBrightRed,            FilterPaletteID::PaletteTranslucentBrightRedHighlight,       FilterPaletteID::PaletteTranslucentBrightRedShadow}
#define WINDOW_PALETTE_BRIGHT_PINK          {FilterPaletteID::PaletteTranslucentBrightPink,           FilterPaletteID::PaletteTranslucentBrightPinkHighlight,      FilterPaletteID::PaletteTranslucentBrightPinkShadow}

const TranslucentWindowPalette TranslucentWindowPalettes[COLOUR_COUNT] = {
    WINDOW_PALETTE_GREY,                    // COLOUR_BLACK
    WINDOW_PALETTE_GREY,                    // COLOUR_GREY
    {FilterPaletteID::PaletteTranslucentWhite,             FilterPaletteID::PaletteTranslucentWhiteHighlight,            FilterPaletteID::PaletteTranslucentWhiteShadow},
    WINDOW_PALETTE_LIGHT_PURPLE,            // COLOUR_DARK_PURPLE
    WINDOW_PALETTE_LIGHT_PURPLE,            // COLOUR_LIGHT_PURPLE
    WINDOW_PALETTE_BRIGHT_PURPLE,           // COLOUR_BRIGHT_PURPLE
    WINDOW_PALETTE_LIGHT_BLUE,              // COLOUR_DARK_BLUE
    WINDOW_PALETTE_LIGHT_BLUE,              // COLOUR_LIGHT_BLUE
    WINDOW_PALETTE_LIGHT_BLUE,              // COLOUR_ICY_BLUE
    WINDOW_PALETTE_TEAL,                    // COLOUR_TEAL
    WINDOW_PALETTE_TEAL,                    // COLOUR_AQUAMARINE
    WINDOW_PALETTE_BRIGHT_GREEN,            // COLOUR_SATURATED_GREEN
    WINDOW_PALETTE_DARK_GREEN,              // COLOUR_DARK_GREEN
    WINDOW_PALETTE_MOSS_GREEN,              // COLOUR_MOSS_GREEN
    WINDOW_PALETTE_BRIGHT_GREEN,            // COLOUR_BRIGHT_GREEN
    WINDOW_PALETTE_OLIVE_GREEN,             // COLOUR_OLIVE_GREEN
    WINDOW_PALETTE_DARK_OLIVE_GREEN,        // COLOUR_DARK_OLIVE_GREEN
    WINDOW_PALETTE_YELLOW,                  // COLOUR_BRIGHT_YELLOW
    WINDOW_PALETTE_YELLOW,                  // COLOUR_YELLOW
    WINDOW_PALETTE_YELLOW,                  // COLOUR_DARK_YELLOW
    WINDOW_PALETTE_LIGHT_ORANGE,            // COLOUR_LIGHT_ORANGE
    WINDOW_PALETTE_LIGHT_ORANGE,            // COLOUR_DARK_ORANGE
    WINDOW_PALETTE_LIGHT_BROWN,             // COLOUR_LIGHT_BROWN
    WINDOW_PALETTE_LIGHT_BROWN,             // COLOUR_SATURATED_BROWN
    WINDOW_PALETTE_DARK_BROWN,              // COLOUR_DARK_BROWN
    WINDOW_PALETTE_SALMON_PINK,             // COLOUR_SALMON_PINK
    WINDOW_PALETTE_BORDEAUX_RED,            // COLOUR_BORDEAUX_RED
    WINDOW_PALETTE_BRIGHT_RED,              // COLOUR_SATURATED_RED
    WINDOW_PALETTE_BRIGHT_RED,              // COLOUR_BRIGHT_RED
    WINDOW_PALETTE_BRIGHT_PINK,             // COLOUR_DARK_PINK
    WINDOW_PALETTE_BRIGHT_PINK,             // COLOUR_BRIGHT_PINK
    {FilterPaletteID::PaletteTranslucentLightPink,        FilterPaletteID::PaletteTranslucentLightPinkHighlight,       FilterPaletteID::PaletteTranslucentLightPinkShadow},
    WINDOW_PALETTE_DARK_OLIVE_GREEN,        // COLOUR_DARK_OLIVE_DARK
    WINDOW_PALETTE_DARK_OLIVE_GREEN,        // COLOUR_DARK_OLIVE_LIGHT
    WINDOW_PALETTE_LIGHT_BROWN,             // COLOUR_SATURATED_BROWN_LIGHT
    WINDOW_PALETTE_BORDEAUX_RED,            // COLOUR_BORDEAUX_RED_DARK
    WINDOW_PALETTE_BORDEAUX_RED,            // COLOUR_BORDEAUX_RED_LIGHT
    WINDOW_PALETTE_MOSS_GREEN,              // COLOUR_GRASS_GREEN_DARK
    WINDOW_PALETTE_MOSS_GREEN,              // COLOUR_GRASS_GREEN_LIGHT
    WINDOW_PALETTE_OLIVE_GREEN,             // COLOUR_OLIVE_DARK
    WINDOW_PALETTE_OLIVE_GREEN,             // COLOUR_OLIVE_LIGHT
    WINDOW_PALETTE_BRIGHT_GREEN,            // COLOUR_SATURATED_GREEN_LIGHT
    WINDOW_PALETTE_SALMON_PINK,             // COLOUR_TAN_DARK
    WINDOW_PALETTE_SALMON_PINK,             // COLOUR_TAN_LIGHT
    WINDOW_PALETTE_LIGHT_PURPLE,            // COLOUR_DULL_PURPLE_LIGHT
    WINDOW_PALETTE_DARK_GREEN,              // COLOUR_DULL_GREEN_DARK
    WINDOW_PALETTE_DARK_GREEN,              // COLOUR_DULL_GREEN_LIGHT
    WINDOW_PALETTE_BRIGHT_PURPLE,           // COLOUR_SATURATED_PURPLE_DARK
    WINDOW_PALETTE_BRIGHT_PURPLE,           // COLOUR_SATURATED_PURPLE_LIGHT
    WINDOW_PALETTE_LIGHT_ORANGE,            // COLOUR_ORANGE_LIGHT
    WINDOW_PALETTE_TEAL,                    // COLOUR_AQUA_DARK
    WINDOW_PALETTE_BRIGHT_PINK,             // COLOUR_MAGENTA_LIGHT
    WINDOW_PALETTE_DARK_BROWN,              // COLOUR_DULL_BROWN_DARK
    WINDOW_PALETTE_DARK_BROWN,              // COLOUR_DULL_BROWN_LIGHT
    {FilterPaletteID::PaletteDarken1,           FilterPaletteID::PaletteDarken1,      FilterPaletteID::PaletteDarken1},
    {FilterPaletteID::PaletteDarken2,           FilterPaletteID::PaletteDarken2,      FilterPaletteID::PaletteDarken2},
};
// clang-format on

ImageCatalogue ImageId::GetCatalogue() const
{
    auto index = GetIndex();
    if (index == SPR_TEMP)
    {
        return ImageCatalogue::TEMPORARY;
    }
    if (index < SPR_RCTC_G1_END)
    {
        return ImageCatalogue::G1;
    }
    if (index < SPR_G2_END)
    {
        return ImageCatalogue::G2;
    }
    if (index < SPR_CSG_END)
    {
        return ImageCatalogue::CSG;
    }
    if (index < SPR_IMAGE_LIST_END)
    {
        return ImageCatalogue::OBJECT;
    }
    return ImageCatalogue::UNKNOWN;
}

static auto GetMaskFunction()
{
    if (Platform::AVX2Available())
    {
        LOG_VERBOSE("registering AVX2 mask function");
        return MaskAvx2;
    }
    else if (Platform::SSE41Available())
    {
        LOG_VERBOSE("registering SSE4.1 mask function");
        return MaskSse4_1;
    }
    else
    {
        LOG_VERBOSE("registering scalar mask function");
        return MaskScalar;
    }
}

static const auto MaskFunc = GetMaskFunction();

void MaskFn(
    int32_t width, int32_t height, const uint8_t* RESTRICT maskSrc, const uint8_t* RESTRICT colourSrc, uint8_t* RESTRICT dst,
    int32_t maskWrap, int32_t colourWrap, int32_t dstWrap)
{
    MaskFunc(width, height, maskSrc, colourSrc, dst, maskWrap, colourWrap, dstWrap);
}

void GfxFilterPixel(DrawPixelInfo& dpi, const ScreenCoordsXY& coords, FilterPaletteID palette)
{
    GfxFilterRect(dpi, { coords, coords }, palette);
}

/**
 *
 *  rct2: 0x00683854
 * a1 (ebx)
 * product (cl)
 */
void GfxTransposePalette(int32_t pal, uint8_t product)
{
    const G1Element* g1 = GfxGetG1Element(pal);
    if (g1 != nullptr)
    {
        int32_t width = g1->width;
        int32_t x = g1->x_offset;
        uint8_t* source_pointer = g1->offset;

        for (; width > 0; width--)
        {
            auto& dest_pointer = gGamePalette[x];
            dest_pointer.Blue = (source_pointer[0] * product) >> 8;
            dest_pointer.Green = (source_pointer[1] * product) >> 8;
            dest_pointer.Red = (source_pointer[2] * product) >> 8;
            source_pointer += 3;

            x++;
        }
        UpdatePalette(gGamePalette, 10, 236);
    }
}

/**
 *
 *  rct2: 0x006837E3
 */
void LoadPalette()
{
    if (gOpenRCT2NoGraphics)
    {
        return;
    }

    uint32_t palette = SPR_DEFAULT_PALETTE;

    auto water_type = OpenRCT2::ObjectManager::GetObjectEntry<WaterObjectEntry>(0);
    if (water_type != nullptr)
    {
        Guard::Assert(water_type->image_id != kImageIndexUndefined, "Failed to load water palette");
        palette = water_type->image_id;
    }

    const G1Element* g1 = GfxGetG1Element(palette);
    if (g1 != nullptr)
    {
        int32_t width = g1->width;
        int32_t x = g1->x_offset;
        uint8_t* src = g1->offset;
        for (; width > 0; width--)
        {
            auto& dst = gGamePalette[x];
            dst.Blue = src[0];
            dst.Green = src[1];
            dst.Red = src[2];
            src += 3;
            x++;
        }
    }
    UpdatePalette(gGamePalette, 10, 236);
    GfxInvalidateScreen();
}

/**
 *
 *  rct2: 0x006ED7E5
 */
void GfxInvalidateScreen()
{
    GfxSetDirtyBlocks({ { 0, 0 }, { ContextGetWidth(), ContextGetHeight() } });
}

/*
 *
 * rct2: 0x006EE53B
 * left (ax)
 * width (bx)
 * top (cx)
 * height (dx)
 * drawpixelinfo (edi)
 */
bool ClipDrawPixelInfo(DrawPixelInfo& dst, DrawPixelInfo& src, const ScreenCoordsXY& coords, int32_t width, int32_t height)
{
    assert(src.zoom_level == ZoomLevel{ 0 });
    int32_t right = coords.x + width;
    int32_t bottom = coords.y + height;

    dst = src;
    dst.zoom_level = ZoomLevel{ 0 };

    if (coords.x > dst.x)
    {
        uint16_t clippedFromLeft = coords.x - dst.x;
        dst.width -= clippedFromLeft;
        dst.x = coords.x;
        dst.pitch += clippedFromLeft;
        dst.bits += clippedFromLeft;
    }

    int32_t stickOutWidth = dst.x + dst.width - right;
    if (stickOutWidth > 0)
    {
        dst.width -= stickOutWidth;
        dst.pitch += stickOutWidth;
    }

    if (coords.y > dst.y)
    {
        uint16_t clippedFromTop = coords.y - dst.y;
        dst.height -= clippedFromTop;
        dst.y = coords.y;
        uint32_t bitsPlus = dst.LineStride() * clippedFromTop;
        dst.bits += bitsPlus;
    }

    int32_t bp = dst.y + dst.height - bottom;
    if (bp > 0)
    {
        dst.height -= bp;
    }

    if (dst.width > 0 && dst.height > 0)
    {
        dst.x -= coords.x;
        dst.y -= coords.y;
        return true;
    }

    return false;
}

void GfxInvalidatePickedUpPeep()
{
    auto imageId = gPickupPeepImage;
    if (imageId.HasValue())
    {
        auto* g1 = GfxGetG1Element(imageId);
        if (g1 != nullptr)
        {
            int32_t left = gPickupPeepX + g1->x_offset;
            int32_t top = gPickupPeepY + g1->y_offset;
            int32_t right = left + g1->width;
            int32_t bottom = top + g1->height;
            GfxSetDirtyBlocks({ { left, top }, { right, bottom } });
        }
    }
}

void GfxDrawPickedUpPeep(DrawPixelInfo& dpi)
{
    if (gPickupPeepImage.HasValue())
    {
        GfxDrawSprite(dpi, gPickupPeepImage, { gPickupPeepX, gPickupPeepY });
    }
}

std::optional<uint32_t> GetPaletteG1Index(colour_t paletteId)
{
    if (paletteId < kPaletteTotalOffsets)
    {
        return kPaletteToG1Offset[paletteId];
    }
    return std::nullopt;
}

std::optional<PaletteMap> GetPaletteMapForColour(colour_t paletteId)
{
    auto g1Index = GetPaletteG1Index(paletteId);
    if (g1Index.has_value())
    {
        auto g1 = GfxGetG1Element(g1Index.value());
        if (g1 != nullptr)
        {
            return PaletteMap(g1->offset, g1->height, g1->width);
        }
    }
    return std::nullopt;
}

uint8_t* DrawPixelInfo::GetBitsOffset(const ScreenCoordsXY& pos) const
{
    return bits + pos.x + pos.y * LineStride();
}

DrawPixelInfo DrawPixelInfo::Crop(const ScreenCoordsXY& pos, const ScreenSize& size) const
{
    DrawPixelInfo result = *this;
    result.bits = GetBitsOffset(pos);
    result.x = pos.x;
    result.y = pos.y;
    result.width = size.width;
    result.height = size.height;
    result.pitch = width + pitch - size.width;
    return result;
}

FilterPaletteID GetGlassPaletteId(colour_t c)
{
    return kGlassPaletteIds[c];
}

void UpdatePalette(std::span<const OpenRCT2::Drawing::PaletteBGRA> palette, int32_t start_index, int32_t num_colours)
{
    for (int32_t i = start_index; i < num_colours + start_index; i++)
    {
        const auto& colour = palette[i];
        uint8_t b = colour.Blue;
        uint8_t g = colour.Green;
        uint8_t r = colour.Red;

        if (LightFx::IsAvailable())
        {
            LightFx::ApplyPaletteFilter(i, &r, &g, &b);
        }
        else
        {
            float night = gDayNightCycle;
            if (night >= 0 && gClimateLightningFlash == 0)
            {
                r = Lerp(r, SoftLight(r, 8), night);
                g = Lerp(g, SoftLight(g, 8), night);
                b = Lerp(b, SoftLight(b, 128), night);
            }
        }

        gPalette[i].Blue = b;
        gPalette[i].Green = g;
        gPalette[i].Red = r;
        gPalette[i].Alpha = 0;
    }

    // Fix #1749 and #6535: rainbow path, donut shop and pause button contain black spots that should be white.
    gPalette[255].Blue = 255;
    gPalette[255].Green = 255;
    gPalette[255].Red = 255;
    gPalette[255].Alpha = 0;

    if (!gOpenRCT2Headless)
    {
        DrawingEngineSetPalette(gPalette);
    }
}

enum
{
    SPR_GAME_PALETTE_DEFAULT = 1532,
    SPR_GAME_PALETTE_WATER = 1533,
    SPR_GAME_PALETTE_WATER_DARKER_1 = 1534,
    SPR_GAME_PALETTE_WATER_DARKER_2 = 1535,
    SPR_GAME_PALETTE_3 = 1536,
    SPR_GAME_PALETTE_3_DARKER_1 = 1537,
    SPR_GAME_PALETTE_3_DARKER_2 = 1538,
    SPR_GAME_PALETTE_4 = 1539,
    SPR_GAME_PALETTE_4_DARKER_1 = 1540,
    SPR_GAME_PALETTE_4_DARKER_2 = 1541,
};

/**
 *
 *  rct2: 0x006838BD
 */
void UpdatePaletteEffects()
{
    auto water_type = OpenRCT2::ObjectManager::GetObjectEntry<WaterObjectEntry>(0);

    if (gClimateLightningFlash == 1)
    {
        // Change palette to lighter colour during lightning
        int32_t palette = SPR_GAME_PALETTE_DEFAULT;

        if (water_type != nullptr)
        {
            palette = water_type->image_id;
        }
        const G1Element* g1 = GfxGetG1Element(palette);
        if (g1 != nullptr)
        {
            int32_t xoffset = g1->x_offset;

            for (int32_t i = 0; i < g1->width; i++)
            {
                auto& paletteOffset = gGamePalette[xoffset + i];
                paletteOffset.Blue = -((0xFF - g1->offset[(i * 3) + 0]) / 2) - 1;
                paletteOffset.Green = -((0xFF - g1->offset[(i * 3) + 1]) / 2) - 1;
                paletteOffset.Red = -((0xFF - g1->offset[(i * 3) + 2]) / 2) - 1;
            }

            UpdatePalette(gGamePalette, kPaletteOffsetDynamic, kPaletteLengthDynamic);
        }
        gClimateLightningFlash++;
    }
    else
    {
        if (gClimateLightningFlash == 2)
        {
            // Change palette back to normal after lightning
            int32_t palette = SPR_GAME_PALETTE_DEFAULT;

            if (water_type != nullptr)
            {
                palette = water_type->image_id;
            }

            const G1Element* g1 = GfxGetG1Element(palette);
            if (g1 != nullptr)
            {
                int32_t xoffset = g1->x_offset;

                for (int32_t i = 0; i < g1->width; i++)
                {
                    auto& paletteOffset = gGamePalette[xoffset + i];
                    paletteOffset.Blue = g1->offset[(i * 3) + 0];
                    paletteOffset.Green = g1->offset[(i * 3) + 1];
                    paletteOffset.Red = g1->offset[(i * 3) + 2];
                }
            }
        }

        // Animate the water/lava/chain movement palette
        uint32_t shade = 0;
        if (Config::Get().general.RenderWeatherGloom)
        {
            auto paletteId = ClimateGetWeatherGloomPaletteId(GetGameState().ClimateCurrent);
            if (paletteId != FilterPaletteID::PaletteNull)
            {
                shade = 1;
                if (paletteId != FilterPaletteID::PaletteDarken1)
                {
                    shade = 2;
                }
            }
        }
        uint32_t j = gPaletteEffectFrame;
        j = ((static_cast<uint16_t>((~j / 2) * 128) * 15) >> 16);
        uint32_t waterId = SPR_GAME_PALETTE_WATER;
        if (water_type != nullptr)
        {
            waterId = water_type->palette_index_1;
        }
        const G1Element* g1 = GfxGetG1Element(shade + waterId);
        if (g1 != nullptr)
        {
            uint8_t* vs = &g1->offset[j * 3];
            int32_t n = kPaletteLengthWaterWaves;
            for (int32_t i = 0; i < n; i++)
            {
                auto& vd = gGamePalette[kPaletteOffsetWaterWaves + i];
                vd.Blue = vs[0];
                vd.Green = vs[1];
                vd.Red = vs[2];
                vs += 9;
                if (vs >= &g1->offset[9 * n])
                {
                    vs -= 9 * n;
                }
            }
        }

        waterId = SPR_GAME_PALETTE_3;
        if (water_type != nullptr)
        {
            waterId = water_type->palette_index_2;
        }

        g1 = GfxGetG1Element(shade + waterId);
        if (g1 != nullptr)
        {
            uint8_t* vs = &g1->offset[j * 3];
            int32_t n = kPaletteLengthWaterSparkles;
            for (int32_t i = 0; i < n; i++)
            {
                auto& vd = gGamePalette[kPaletteOffsetWaterSparkles + i];
                vd.Blue = vs[0];
                vd.Green = vs[1];
                vd.Red = vs[2];
                vs += 9;
                if (vs >= &g1->offset[9 * n])
                {
                    vs -= 9 * n;
                }
            }
        }

        j = (static_cast<uint16_t>(gPaletteEffectFrame * -960) * 3) >> 16;
        waterId = SPR_GAME_PALETTE_4;
        g1 = GfxGetG1Element(shade + waterId);
        if (g1 != nullptr)
        {
            uint8_t* vs = &g1->offset[j * 3];
            int32_t n = 3;
            for (int32_t i = 0; i < n; i++)
            {
                auto& vd = gGamePalette[PALETTE_INDEX_243 + i];
                vd.Blue = vs[0];
                vd.Green = vs[1];
                vd.Red = vs[2];
                vs += 3;
                if (vs >= &g1->offset[3 * n])
                {
                    vs -= 3 * n;
                }
            }
        }

        UpdatePalette(gGamePalette, kPaletteOffsetAnimated, kPaletteLengthAnimated);
        if (gClimateLightningFlash == 2)
        {
            UpdatePalette(gGamePalette, kPaletteOffsetDynamic, kPaletteLengthDynamic);
            gClimateLightningFlash = 0;
        }
    }
}

void RefreshVideo()
{
    ContextRecreateWindow();
    DrawingEngineSetPalette(gPalette);
    GfxInvalidateScreen();
}

void ToggleWindowedMode()
{
    int32_t targetMode = Config::Get().general.FullscreenMode == 0 ? 2 : 0;
    ContextSetFullscreenMode(targetMode);
    Config::Get().general.FullscreenMode = targetMode;
    Config::Save();
}

void DebugDPI(DrawPixelInfo& dpi)
{
    ScreenCoordsXY topLeft = { dpi.x, dpi.y };
    ScreenCoordsXY bottomRight = { dpi.x + dpi.width - 1, dpi.y + dpi.height - 1 };
    ScreenCoordsXY topRight = { dpi.x + dpi.width - 1, dpi.y };
    ScreenCoordsXY bottomLeft = { dpi.x, dpi.y + dpi.height - 1 };

    GfxDrawLine(dpi, { topLeft, bottomRight }, PALETTE_INDEX_136);
    GfxDrawLine(dpi, { bottomLeft, topRight }, PALETTE_INDEX_136);
    GfxDrawLine(dpi, { topLeft, topRight }, PALETTE_INDEX_129);
    GfxDrawLine(dpi, { topRight, bottomRight }, PALETTE_INDEX_129);
    GfxDrawLine(dpi, { bottomLeft, bottomRight }, PALETTE_INDEX_129);
    GfxDrawLine(dpi, { topLeft, bottomLeft }, PALETTE_INDEX_129);

    GfxDrawLine(dpi, { topLeft, topLeft + ScreenCoordsXY{ 4, 0 } }, PALETTE_INDEX_136);

    const auto str = std::to_string(dpi.x);
    DrawText(dpi, ScreenCoordsXY{ dpi.x, dpi.y }, { COLOUR_WHITE, FontStyle::Tiny }, str.c_str());

    const auto str2 = std::to_string(dpi.y);
    DrawText(dpi, ScreenCoordsXY{ dpi.x, dpi.y + 6 }, { COLOUR_WHITE, FontStyle::Tiny }, str2.c_str());
}
