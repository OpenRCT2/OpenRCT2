/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "PaintIntercept.hpp"

#include "FunctionCall.hpp"
#include "TestPaint.hpp"

#include <cstring>
#include <openrct2/common.h>
#include <openrct2/interface/Viewport.h>
#include <openrct2/paint/Supports.h>
#include <openrct2/sprites.h>

paint_session gPaintSession;

static bool _woodenSupports = false;
static uint8_t _callCount = 0;
static function_call _calls[256] = {};
static paint_struct _paintStructs = {};

namespace PaintIntercept
{
    bool PaintMetalSupports(
        uint8_t function, int supportType, uint8_t segment, int special, int height, uint32_t imageColourFlags,
        const support_height* supportSegments);
    bool PaintWoodenSupports(
        uint8_t function, int supportType, int special, int height, uint32_t imageColourFlags, bool* underground,
        const paint_struct* prependTo);
    static void CheckSegmentSupportHeight(const support_height* supportSegments);

    bool PaintWoodenSupports(
        uint8_t function, int supportType, int special, int height, uint32_t imageColourFlags, bool* underground,
        const paint_struct* prependTo)
    {
        function_call* call = &_calls[_callCount];
        call->function = function;
        call->supports.type = supportType;
        call->supports.special = special;
        call->supports.height = height;
        call->supports.colour_flags = imageColourFlags;

        call->supports.prepend_to = SPR_NONE;
        if (prependTo != nullptr)
        {
            for (int i = 0; i < _callCount; i++)
            {
                if (&_calls[i].paint.output_struct == prependTo)
                {
                    call->supports.prepend_to = _calls[i].paint.image_id;
                    break;
                }
            }
        }

        _callCount++;

        return _woodenSupports;
    }

    bool PaintMetalSupports(
        uint8_t function, int supportType, uint8_t segment, int special, int height, uint32_t imageColourFlags,
        const support_height* supportSegments)
    {
        CheckSegmentSupportHeight(supportSegments);

        function_call* call = &_calls[_callCount];
        call->function = function;
        call->supports.type = supportType;
        call->supports.segment = segment;
        call->supports.special = special;
        call->supports.height = height;
        call->supports.colour_flags = imageColourFlags;

        _callCount++;

        return false;
    }

    static paint_struct* Paint6C(
        uint32_t imageID, int8_t xOffset, int8_t yOffset, int16_t boundBoxLengthX, int16_t boundBoxLengthY,
        int8_t boundBoxLengthZ, int16_t zOffset, uint32_t rotation)
    {
        function_call* call = &_calls[_callCount];
        call->function = PAINT_98196C;
        call->paint.image_id = imageID;
        call->paint.offset = { xOffset, yOffset };
        call->paint.bound_box_length = { boundBoxLengthX, boundBoxLengthY, boundBoxLengthZ };
        call->paint.z_offset = zOffset;
        call->paint.rotation = rotation;

        _callCount++;

        return &call->paint.output_struct;
    }

    static paint_struct* PaintFull(
        uint8_t function, uint32_t imageID, int8_t xOffset, int8_t yOffset, int16_t boundBoxLengthX, int16_t boundBoxLengthY,
        int8_t boundBoxLengthZ, int16_t zOffset, int16_t boundBoxOffsetX, int16_t boundBoxOffsetY, int16_t boundBoxOffsetZ,
        uint32_t rotation)
    {
        function_call* call = &_calls[_callCount];
        call->function = function;
        call->paint.image_id = imageID;
        call->paint.offset = { xOffset, yOffset };
        call->paint.bound_box_length = { boundBoxLengthX, boundBoxLengthY, boundBoxLengthZ };
        call->paint.bound_box_offset = { boundBoxOffsetX, boundBoxOffsetY, boundBoxOffsetZ };
        call->paint.z_offset = zOffset;
        call->paint.rotation = rotation;

        _callCount++;

        return &call->paint.output_struct;
    }

    static paint_struct* PaintFull(
        uint8_t function, uint32_t image_id, const CoordsXYZ& offset, const CoordsXYZ& boundBoxLength,
        const CoordsXYZ& boundBoxOffset, uint32_t rotation)
    {
        return PaintFull(
            function, image_id, static_cast<int8_t>(offset.x), static_cast<int8_t>(offset.y),
            static_cast<int16_t>(boundBoxLength.x), static_cast<int16_t>(boundBoxLength.y),
            static_cast<int16_t>(boundBoxLength.z), static_cast<int16_t>(offset.z), static_cast<int16_t>(boundBoxOffset.x),
            static_cast<int16_t>(boundBoxOffset.y), static_cast<int16_t>(boundBoxOffset.z), rotation);
    }

    void ClearCalls()
    {
        _callCount = 0;
        memset(_calls, 0, sizeof(_calls));
    }

    int GetCalls(function_call* buffer)
    {
        memcpy(buffer, _calls, _callCount * sizeof(function_call));
        return _callCount;
    }

    void SetSimulateWoodenSupports(bool enabled)
    {
        _woodenSupports = enabled;
    }

    static void CheckSegmentSupportHeight(const support_height* supportSegments)
    {
        bool hasChanged = false;
        for (int i = 0; i < 9; i++)
        {
            if (supportSegments[i].height != 0)
                hasChanged = true;
            if (supportSegments[i].slope != 0xFF)
                hasChanged = true;
        }

        if (!hasChanged)
        {
            return;
        }

        function_call call = {};
        call.function = SET_SEGMENT_HEIGHT;

        _calls[_callCount] = call;
        _callCount++;
    }

}; // namespace PaintIntercept

bool wooden_a_supports_paint_setup(
    paint_session* session, int supportType, int special, int height, uint32_t imageColourFlags, bool* underground)
{
    return PaintIntercept::PaintWoodenSupports(
        SUPPORTS_WOOD_A, supportType, special, height, imageColourFlags, underground, gPaintSession.WoodenSupportsPrependTo);
}

bool wooden_b_supports_paint_setup(
    paint_session* session, int supportType, int special, int height, uint32_t imageColourFlags, bool* underground)
{
    return PaintIntercept::PaintWoodenSupports(
        SUPPORTS_WOOD_B, supportType, special, height, imageColourFlags, underground, gPaintSession.WoodenSupportsPrependTo);
}

bool metal_a_supports_paint_setup(
    paint_session* session, uint8_t supportType, uint8_t segment, int special, int height, uint32_t imageColourFlags)
{
    return PaintIntercept::PaintMetalSupports(
        SUPPORTS_METAL_A, supportType, segment, special, height, imageColourFlags, gPaintSession.SupportSegments);
}

bool metal_b_supports_paint_setup(
    paint_session* session, uint8_t supportType, uint8_t segment, int special, int height, uint32_t imageColourFlags)
{
    return PaintIntercept::PaintMetalSupports(
        SUPPORTS_METAL_B, supportType, segment, special, height, imageColourFlags, gPaintSession.SupportSegments);
}

paint_struct* PaintAddImageAsParent(
    paint_session* session, uint32_t image_id, int8_t x_offset, int8_t y_offset, int16_t bound_box_length_x,
    int16_t bound_box_length_y, int8_t bound_box_length_z, int16_t z_offset)
{
    return PaintIntercept::Paint6C(
        image_id, x_offset, y_offset, bound_box_length_x, bound_box_length_y, bound_box_length_z, z_offset,
        session->CurrentRotation);
}

paint_struct* PaintAddImageAsParent(
    paint_session* session, uint32_t image_id, const CoordsXYZ& offset, const CoordsXYZ& boundBoxSize)
{
    return PaintIntercept::Paint6C(
        image_id, offset.x, offset.y, boundBoxSize.x, boundBoxSize.y, boundBoxSize.z, offset.z, session->CurrentRotation);
}

paint_struct* PaintAddImageAsParent(
    paint_session* session, uint32_t image_id, int8_t x_offset, int8_t y_offset, int16_t bound_box_length_x,
    int16_t bound_box_length_y, int8_t bound_box_length_z, int16_t z_offset, int16_t bound_box_offset_x,
    int16_t bound_box_offset_y, int16_t bound_box_offset_z)
{
    return PaintIntercept::PaintFull(
        PAINT_98197C, image_id, x_offset, y_offset, bound_box_length_x, bound_box_length_y, bound_box_length_z, z_offset,
        bound_box_offset_x, bound_box_offset_y, bound_box_offset_z, session->CurrentRotation);
}

paint_struct* PaintAddImageAsChild(
    paint_session* session, uint32_t image_id, const CoordsXYZ& offset, const CoordsXYZ& boundBoxLength,
    const CoordsXYZ& boundBoxOffset)
{
    return PaintIntercept::PaintFull(
        PAINT_98198C_COORDS, image_id, offset, boundBoxLength, boundBoxOffset, session->CurrentRotation);
}

paint_struct* PaintAddImageAsOrphan(
    paint_session* session, uint32_t image_id, int8_t x_offset, int8_t y_offset, int16_t bound_box_length_x,
    int16_t bound_box_length_y, int8_t bound_box_length_z, int16_t z_offset, int16_t bound_box_offset_x,
    int16_t bound_box_offset_y, int16_t bound_box_offset_z)
{
    return PaintIntercept::PaintFull(
        PAINT_98198C, image_id, x_offset, y_offset, bound_box_length_x, bound_box_length_y, bound_box_length_z, z_offset,
        bound_box_offset_x, bound_box_offset_y, bound_box_offset_z, session->CurrentRotation);
}

paint_struct* PaintAddImageAsChild(
    paint_session* session, uint32_t image_id, int8_t x_offset, int8_t y_offset, int16_t bound_box_length_x,
    int16_t bound_box_length_y, int8_t bound_box_length_z, int16_t z_offset, int16_t bound_box_offset_x,
    int16_t bound_box_offset_y, int16_t bound_box_offset_z)
{
    return PaintIntercept::PaintFull(
        PAINT_98199C, image_id, x_offset, y_offset, bound_box_length_x, bound_box_length_y, bound_box_length_z, z_offset,
        bound_box_offset_x, bound_box_offset_y, bound_box_offset_z, session->CurrentRotation);
}

bool PaintAttachToPreviousPS(paint_session* session, uint32_t image_id, int16_t x, int16_t y)
{
    return false;
}
