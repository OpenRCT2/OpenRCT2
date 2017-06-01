#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

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

typedef struct
{
    uint8 function;
    struct paint
    {
        uint32 image_id;
        rct_xy16 offset;
        rct_xyz16 bound_box_length;
        sint16 z_offset;
        rct_xyz16 bound_box_offset;
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
} function_call;

class FunctionCall {
public:
    static bool AssertsEquals(function_call expected, function_call actual);
    static bool AssertsEquals(std::vector<function_call> expected, std::vector<function_call> actual);
};
