/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3
 *****************************************************************************/

#pragma once

#include <openrct2/common.h>
#include "TestPaint.hpp"

enum
{
    PAINT_98196C,
    PAINT_98197C,
    PAINT_98198C,
    PAINT_98199C,

    SUPPORTS_METAL_A,
    SUPPORTS_METAL_B,
    SUPPORTS_WOOD_A,
    SUPPORTS_WOOD_B,

    SET_SEGMENT_HEIGHT,
};

struct function_call
{
    uint8 function;
    struct paint
    {
        uint32 image_id;
        LocationXY16 offset;
        LocationXYZ16 bound_box_length;
        sint16 z_offset;
        LocationXYZ16 bound_box_offset;
        uint32 rotation;
        paint_struct output_struct;
    } paint;
    struct supports
    {
        int type;
        uint8 segment;
        int special;
        int height;
        uint32 colour_flags;
        sint32 prepend_to;
    } supports;
};

class FunctionCall {
public:
    static bool AssertsEquals(function_call expected, function_call actual);
    static bool AssertsEquals(std::vector<function_call> expected, std::vector<function_call> actual);
};
