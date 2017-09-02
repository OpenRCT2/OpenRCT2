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


#include "PaintIntercept.hpp"
#include "FunctionCall.hpp"

extern "C" {
#include <openrct2/common.h>
#include <openrct2/rct2/hook.h>
#include <openrct2/interface/viewport.h>
#include <openrct2/paint/supports.h>
#include <openrct2/sprites.h>
}

paint_session gPaintSession;

static bool _woodenSupports = false;
static uint8 _callCount = 0;
static function_call _calls[256] = {0};
static paint_struct _paintStructs = {0};

namespace PaintIntercept {
    static uint8 InterceptWoodenASupports(registers *regs);
    static uint8 InterceptWoodenBSupports(registers *regs);
    static uint8 InterceptMetalASupports(registers *regs);
    static uint8 InterceptMetalBSupports(registers *regs);
    static uint8 InterceptPaint6C(registers *regs);
    static uint8 InterceptPaint7C(registers *regs);
    static uint8 InterceptPaint8C(registers *regs);
    static uint8 InterceptPaint9C(registers *regs);
    static uint8 InterceptPaintFull(uint8 function, registers *regs);

    bool PaintMetalSupports(uint8 function, int supportType, uint8 segment, int special, int height, uint32 imageColourFlags, const support_height * supportSegments);
    bool PaintWoodenSupports(uint8 function, int supportType, int special, int height, uint32 imageColourFlags, bool *underground, const paint_struct * prependTo);
    static void CheckSegmentSupportHeight(const support_height * supportSegments);

    void InitHooks() {
        addhook(0x006629BC, InterceptWoodenASupports);
        addhook(0x00662D5C, InterceptWoodenBSupports);

        addhook(0x00663105, InterceptMetalASupports);
        addhook(0x00663584, InterceptMetalBSupports);

        addhook(0x006861AC, InterceptPaint6C);
        addhook(0x00686337, InterceptPaint6C);
        addhook(0x006864D0, InterceptPaint6C);
        addhook(0x0068666B, InterceptPaint6C);

        addhook(0x00686806, InterceptPaint7C);
        addhook(0x006869B2, InterceptPaint7C);
        addhook(0x00686B6F, InterceptPaint7C);
        addhook(0x00686D31, InterceptPaint7C);

        addhook(0x00686EF0, InterceptPaint8C);
        addhook(0x00687056, InterceptPaint8C);
        addhook(0x006871C8, InterceptPaint8C);
        addhook(0x0068733C, InterceptPaint8C);

        addhook(0x006874B0, InterceptPaint9C);
        addhook(0x00687618, InterceptPaint9C);
        addhook(0x0068778C, InterceptPaint9C);
        addhook(0x00687902, InterceptPaint9C);
    }

    bool PaintWoodenSupports(uint8 function, int supportType, int special, int height, uint32 imageColourFlags, bool *underground, const paint_struct * prependTo) {
        function_call * call = &_calls[_callCount];
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

    bool PaintMetalSupports(uint8 function, int supportType, uint8 segment, int special, int height, uint32 imageColourFlags, const support_height * supportSegments) {
        CheckSegmentSupportHeight(supportSegments);

        function_call * call = &_calls[_callCount];
        call->function = function;
        call->supports.type = supportType;
        call->supports.segment = segment;
        call->supports.special = special;
        call->supports.height = height;
        call->supports.colour_flags = imageColourFlags;

        _callCount++;

        return false;
    }

    static paint_struct *Paint6C(
        uint32 imageID,
        sint8 xOffset, sint8 yOffset,
        sint16 boundBoxLengthX, sint16 boundBoxLengthY, sint8 boundBoxLengthZ,
        sint16 zOffset,
        uint32 rotation
    ) {
        function_call * call = &_calls[_callCount];
        call->function = PAINT_98196C;
        call->paint.image_id = imageID;
        call->paint.offset = {xOffset, yOffset};
        call->paint.bound_box_length = {boundBoxLengthX, boundBoxLengthY, boundBoxLengthZ};
        call->paint.z_offset = zOffset;
        call->paint.rotation = rotation;

        _callCount++;

        return &call->paint.output_struct;
    }

    static paint_struct *PaintFull(
        uint8 function,
        uint32 imageID,
        sint8 xOffset, sint8 yOffset,
        sint16 boundBoxLengthX, sint16 boundBoxLengthY, sint8 boundBoxLengthZ,
        sint16 zOffset,
        sint16 boundBoxOffsetX, sint16 boundBoxOffsetY, sint16 boundBoxOffsetZ,
        uint32 rotation
    ) {
        function_call * call = &_calls[_callCount];
        call->function = function;
        call->paint.image_id = imageID;
        call->paint.offset = {xOffset, yOffset};
        call->paint.bound_box_length = {boundBoxLengthX, boundBoxLengthY, boundBoxLengthZ};
        call->paint.bound_box_offset = {boundBoxOffsetX, boundBoxOffsetY, boundBoxOffsetZ};
        call->paint.z_offset = zOffset;
        call->paint.rotation = rotation;

        _callCount++;

        return &call->paint.output_struct;
    }

    void ClearCalls() {
        _callCount = 0;
        memset(_calls, 0, sizeof(_calls));
    }

    int GetCalls(function_call *buffer) {
        memcpy(buffer, _calls, _callCount * sizeof(function_call));
        return _callCount;
    }

    void SetSimulateWoodenSupports(bool enabled) {
        _woodenSupports = enabled;
    }

    static uint8 InterceptMetalASupports(registers *regs)
    {
        bool output = PaintMetalSupports(SUPPORTS_METAL_A, regs->edi, regs->ebx, (sint16) regs->ax, regs->dx, regs->ebp, gSupportSegments);

        return output ? X86_FLAG_CARRY : 0;
    }

    static uint8 InterceptMetalBSupports(registers *regs)
    {
        bool output = PaintMetalSupports(SUPPORTS_METAL_B, regs->edi, regs->ebx, (sint16) regs->ax, regs->dx, regs->ebp, gSupportSegments);

        return output ? X86_FLAG_CARRY : 0;
    }

    static void CheckSegmentSupportHeight(const support_height * supportSegments) {
        bool hasChanged = false;
        for (int i = 0; i < 9; i++) {
            if (supportSegments[i].height != 0) hasChanged = true;
            if (supportSegments[i].slope != 0xFF) hasChanged = true;
        }

        if (!hasChanged) {
            return;
        }

        function_call call = {0};
        call.function = SET_SEGMENT_HEIGHT;

        _calls[_callCount] = call;
        _callCount++;
    }

    static uint8 InterceptWoodenASupports(registers *regs)
    {
        bool cf = false;
        regs->al = PaintWoodenSupports(SUPPORTS_WOOD_A, regs->edi, regs->ax, regs->dx, regs->ebp, &cf, gWoodenSupportsPrependTo);

        if (cf)
        {
            return X86_FLAG_CARRY;
        }

        return 0;
    }

    static uint8 InterceptWoodenBSupports(registers *regs)
    {
        bool cf = false;
        regs->al = PaintWoodenSupports(SUPPORTS_WOOD_B, regs->edi, regs->ax, regs->dx, regs->ebp, &cf, gWoodenSupportsPrependTo);

        if (cf)
        {
            return X86_FLAG_CARRY;
        }

        return 0;
    }

    static uint8 InterceptPaint6C(registers *regs)
    {
        if ((regs->ebp & 0x03) != get_current_rotation())
        {
            // Log error
            log_error("Ebp is different from current rotation");
        }

        paint_struct *out = Paint6C(
            regs->ebx,
            (sint8) regs->al, (sint8) regs->cl,
            (sint16) regs->di, (sint16) regs->si, (sint8) regs->ah,
            regs->dx,
            regs->ebp & 0x03
        );

        if (out == nullptr)
        {
            return X86_FLAG_CARRY;
        }

        regs->ebp = (int) out;
        regs->al = 1;
        return 0;
    }

    static uint8 InterceptPaint7C(registers *regs)
    {
        return InterceptPaintFull(PAINT_98197C, regs);
    }

    static uint8 InterceptPaint8C(registers *regs)
    {
        return InterceptPaintFull(PAINT_98198C, regs);
    }

    static uint8 InterceptPaint9C(registers *regs)
    {
        return InterceptPaintFull(PAINT_98199C, regs);
    }

    static uint8 InterceptPaintFull(uint8 function, registers *regs) {
        if ((regs->ebp & 0x03) != get_current_rotation()) {
            // Log error
            log_error("Ebp is different from current rotation");
        }

        rct_xyz16 boundOffset = {
            RCT2_GLOBAL(RCT2_ADDRESS_PAINT_BOUNDBOX_OFFSET_X, sint16),
            RCT2_GLOBAL(RCT2_ADDRESS_PAINT_BOUNDBOX_OFFSET_Y, sint16),
            RCT2_GLOBAL(RCT2_ADDRESS_PAINT_BOUNDBOX_OFFSET_Z, sint16)
        };

        paint_struct *out = PaintFull(
            function,
            regs->ebx,
            (sint8) regs->al, (sint8) regs->cl,
            (sint16) regs->di, (sint16) regs->si, (sint8) regs->ah,
            regs->dx,
            boundOffset.x, boundOffset.y, boundOffset.z,
            regs->ebp & 0x03
        );

        if (out == nullptr)
        {
            return X86_FLAG_CARRY;
        }

        regs->ebp = (int) out;
        return 0;
    }
};


extern "C" {
bool wooden_a_supports_paint_setup(int supportType, int special, int height, uint32 imageColourFlags, bool *underground) {
    return PaintIntercept::PaintWoodenSupports(SUPPORTS_WOOD_A, supportType, special, height, imageColourFlags, underground, gPaintSession.WoodenSupportsPrependTo);
}

bool wooden_b_supports_paint_setup(int supportType, int special, int height, uint32 imageColourFlags, bool *underground) {
    return PaintIntercept::PaintWoodenSupports(SUPPORTS_WOOD_B, supportType, special, height, imageColourFlags, underground, gPaintSession.WoodenSupportsPrependTo);
}

bool metal_a_supports_paint_setup(uint8 supportType, uint8 segment, int special, int height, uint32 imageColourFlags) {
    return PaintIntercept::PaintMetalSupports(SUPPORTS_METAL_A, supportType, segment, special, height, imageColourFlags, gPaintSession.SupportSegments);
}

bool metal_b_supports_paint_setup(uint8 supportType, uint8 segment, int special, int height, uint32 imageColourFlags) {
    return PaintIntercept::PaintMetalSupports(SUPPORTS_METAL_B, supportType, segment, special, height, imageColourFlags, gPaintSession.SupportSegments);
}

paint_struct *sub_98196C(uint32 image_id, sint8 x_offset, sint8 y_offset, sint16 bound_box_length_x, sint16 bound_box_length_y, sint8 bound_box_length_z, sint16 z_offset, uint32 rotation) {
    return PaintIntercept::Paint6C(image_id, x_offset, y_offset, bound_box_length_x, bound_box_length_y, bound_box_length_z, z_offset, rotation);
}

paint_struct *sub_98197C(
    uint32 image_id,
    sint8 x_offset, sint8 y_offset,
    sint16 bound_box_length_x, sint16 bound_box_length_y, sint8 bound_box_length_z,
    sint16 z_offset,
    sint16 bound_box_offset_x, sint16 bound_box_offset_y, sint16 bound_box_offset_z,
    uint32 rotation
) {
    return PaintIntercept::PaintFull(
        PAINT_98197C,
        image_id,
        x_offset, y_offset,
        bound_box_length_x, bound_box_length_y, bound_box_length_z,
        z_offset,
        bound_box_offset_x, bound_box_offset_y, bound_box_offset_z,
        rotation
    );
}

paint_struct *sub_98198C(
    uint32 image_id,
    sint8 x_offset, sint8 y_offset,
    sint16 bound_box_length_x, sint16 bound_box_length_y, sint8 bound_box_length_z,
    sint16 z_offset,
    sint16 bound_box_offset_x, sint16 bound_box_offset_y, sint16 bound_box_offset_z,
    uint32 rotation
) {
    return PaintIntercept::PaintFull(
        PAINT_98198C,
        image_id,
        x_offset, y_offset,
        bound_box_length_x, bound_box_length_y, bound_box_length_z,
        z_offset,
        bound_box_offset_x, bound_box_offset_y, bound_box_offset_z,
        rotation
    );
}

paint_struct *sub_98199C(
    uint32 image_id,
    sint8 x_offset, sint8 y_offset,
    sint16 bound_box_length_x, sint16 bound_box_length_y, sint8 bound_box_length_z,
    sint16 z_offset,
    sint16 bound_box_offset_x, sint16 bound_box_offset_y, sint16 bound_box_offset_z,
    uint32 rotation
) {
    return PaintIntercept::PaintFull(
        PAINT_98199C,
        image_id,
        x_offset, y_offset,
        bound_box_length_x, bound_box_length_y, bound_box_length_z,
        z_offset,
        bound_box_offset_x, bound_box_offset_y, bound_box_offset_z,
        rotation
    );
}

bool paint_attach_to_previous_ps(uint32 image_id, uint16 x, uint16 y) {
    return false;
}

}
