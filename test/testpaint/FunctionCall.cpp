/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "FunctionCall.hpp"

#include <openrct2/sprites.h>

enum SpriteGroup
{
    SPRITEGROUP_NONE,

    SPRITEGROUP_FENCE_METAL_A,      // 14568
    SPRITEGROUP_FENCE_METAL_B,      // 14990
    SPRITEGROUP_FENCE_SPIRAL_SLIDE, // 20564
    SPRITEGROUP_FLOOR_CORK,         // 22134
    SPRITEGROUP_FENCE_ROPE,         // 22138
};

static void canonicalizeFunctionCall(function_call* call);
static SpriteGroup getSpriteGroup(uint16_t spriteIndex);

bool FunctionCall::AssertsEquals(std::vector<function_call> expected, std::vector<function_call> actual)
{
    if (expected.size() != actual.size())
    {
        return false;
    }

    for (size_t i = 0; i < expected.size(); i++)
    {
        function_call expectedCall = expected[i];
        function_call actualCall = actual[i];

        if (!AssertsEquals(expectedCall, actualCall))
        {
            return false;
        }
    }

    return true;
}

bool FunctionCall::AssertsEquals(function_call expected, function_call actual)
{
    canonicalizeFunctionCall(&actual);
    canonicalizeFunctionCall(&expected);

    if (expected.function != actual.function)
    {
        return false;
    }

    uint8_t function = expected.function;

    if (function == SUPPORTS_WOOD_A || function == SUPPORTS_WOOD_B)
    {
        if (expected.supports.type != actual.supports.type)
            return false;
        if (expected.supports.special != actual.supports.special)
            return false;
        if (expected.supports.height != actual.supports.height)
            return false;
        if (expected.supports.colour_flags != actual.supports.colour_flags)
            return false;

        if (expected.supports.special == 14 || expected.supports.special == 15 || expected.supports.special == 18
            || expected.supports.special == 19 || expected.supports.special == 22 || expected.supports.special == 23)
        {
            if (expected.supports.prepend_to != actual.supports.prepend_to)
                return false;
        }

        return true;
    }

    if (function == SUPPORTS_METAL_A || function == SUPPORTS_METAL_B)
    {
        if (expected.supports.type != actual.supports.type)
            return false;
        if (expected.supports.segment != actual.supports.segment)
            return false;
        if (expected.supports.special != actual.supports.special)
            return false;
        if (expected.supports.height != actual.supports.height)
            return false;
        if (expected.supports.colour_flags != actual.supports.colour_flags)
            return false;

        return true;
    }

    if (function == SET_SEGMENT_HEIGHT)
    {
        return true;
    }

    if (expected.paint.image_id != actual.paint.image_id)
    {
        SpriteGroup expectedSpriteGroup = getSpriteGroup(expected.paint.image_id & 0x7FFFF);
        SpriteGroup actualSpriteGroup = getSpriteGroup(actual.paint.image_id & 0x7FFFF);

        if (expectedSpriteGroup != actualSpriteGroup)
            return false;

        if (expectedSpriteGroup == SPRITEGROUP_NONE)
            return false;

        return true;
    }

    if (expected.paint.offset.x != actual.paint.offset.x)
        return false;
    if (expected.paint.offset.y != actual.paint.offset.y)
        return false;
    if (expected.paint.bound_box_length.x != actual.paint.bound_box_length.x)
        return false;
    if (expected.paint.bound_box_length.y != actual.paint.bound_box_length.y)
        return false;
    if (expected.paint.bound_box_length.z != actual.paint.bound_box_length.z)
        return false;
    if (function != PAINT_98196C)
    {
        if (expected.paint.bound_box_offset.x != actual.paint.bound_box_offset.x)
            return false;
        if (expected.paint.bound_box_offset.y != actual.paint.bound_box_offset.y)
            return false;
        if (expected.paint.bound_box_offset.z != actual.paint.bound_box_offset.z)
            return false;
    }
    if (expected.paint.z_offset != actual.paint.z_offset)
        return false;
    if (expected.paint.rotation != actual.paint.rotation)
        return false;

    return true;
}

static void canonicalizeFunctionCall(function_call* call)
{
    if (call->function != PAINT_98197C)
        return;
    if (call->paint.offset.x != call->paint.bound_box_offset.x)
        return;
    if (call->paint.offset.y != call->paint.bound_box_offset.y)
        return;
    if (call->paint.z_offset != call->paint.bound_box_offset.z)
        return;

    call->function = PAINT_98196C;
}

static SpriteGroup getSpriteGroup(uint16_t spriteIndex)
{
    if (spriteIndex >= 14568 && spriteIndex <= 14571)
    {
        return SPRITEGROUP_FENCE_METAL_A;
    }

    if (spriteIndex >= 14990 && spriteIndex <= 14993)
    {
        return SPRITEGROUP_FENCE_METAL_B;
    }

    if (spriteIndex >= 20564 && spriteIndex <= 20567)
    {
        return SPRITEGROUP_FENCE_SPIRAL_SLIDE;
    }

    if (spriteIndex >= 22134 && spriteIndex <= 22137)
    {
        return SPRITEGROUP_FLOOR_CORK;
    }

    if (spriteIndex >= 22138 && spriteIndex <= 22141)
    {
        return SPRITEGROUP_FENCE_ROPE;
    }

    return SPRITEGROUP_NONE;
}
