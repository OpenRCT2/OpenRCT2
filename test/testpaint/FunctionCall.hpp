/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "TestPaint.hpp"

#include <openrct2/common.h>

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
    PAINT_98198C_COORDS
};

struct function_call
{
    uint8_t function;
    struct paint
    {
        uint32_t image_id;
        LocationXY16 offset;
        LocationXYZ16 bound_box_length;
        int16_t z_offset;
        LocationXYZ16 bound_box_offset;
        uint32_t rotation;
        paint_struct output_struct;
    } paint;
    struct supports
    {
        int type;
        uint8_t segment;
        int special;
        int height;
        uint32_t colour_flags;
        int32_t prepend_to;
    } supports;
};

class FunctionCall
{
public:
    static bool AssertsEquals(function_call expected, function_call actual);
    static bool AssertsEquals(std::vector<function_call> expected, std::vector<function_call> actual);
};
