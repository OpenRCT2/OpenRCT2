#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

extern "C" {
#include "../../src/common.h"
#include "../../src/hook.h"
#include "../../src/interface/viewport.h"
#include "../../src/paint/supports.h"
}

static bool _woodenSupports = false;
static uint8 _callCount = 0;
static function_call _calls[256] = {0};

namespace PaintIntercept {
    static uint32 InterceptWoodenASupports(uint32 eax, uint32 ebx, uint32 edx, uint32 edi, uint32 ebp);
    static uint32 InterceptWoodenBSupports(uint32 eax, uint32 ebx, uint32 edx, uint32 edi, uint32 ebp);
    static uint32 InterceptMetalASupports(uint32 eax, uint32 ebx, uint32 edx, uint32 edi, uint32 ebp);
    static uint32 InterceptMetalBSupports(uint32 eax, uint32 ebx, uint32 edx, uint32 edi, uint32 ebp);
    static uint32 InterceptPaint6C(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp);
    static uint32 InterceptPaint7C(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp);
    static uint32 InterceptPaint8C(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp);
    static uint32 InterceptPaint9C(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp);
    static uint32 InterceptPaintFull(uint8 function, uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp);
    bool PaintMetalSupports(uint8 function, int supportType, uint8 segment, int special, int height, uint32 imageColourFlags);
    bool PaintWoodenSupports(uint8 function, int supportType, int special, int height, uint32 imageColourFlags, bool *underground);
    static void CheckSegmentSupportHeight();

    void InitHooks() {
        int supportsRegisterArgs[] = {EAX, EBX, EDX, EDI, EBP, END};
        addhook(0x006629BC, (int) InterceptWoodenASupports, 0, supportsRegisterArgs, 0, EAX);
        addhook(0x00662D5C, (int) InterceptWoodenBSupports, 0, supportsRegisterArgs, 0, EAX);

        addhook(0x00663105, (int) InterceptMetalASupports, 0, supportsRegisterArgs, 0, EAX);
        addhook(0x00663584, (int) InterceptMetalBSupports, 0, supportsRegisterArgs, 0, EAX);

        int paintRegisterArgs[] = {EAX, EBX, ECX, EDX, ESI, EDI, EBP, END};
        addhook(0x006861AC, (int) InterceptPaint6C, 0, paintRegisterArgs, 0, EBP);
        addhook(0x00686337, (int) InterceptPaint6C, 0, paintRegisterArgs, 0, EBP);
        addhook(0x006864D0, (int) InterceptPaint6C, 0, paintRegisterArgs, 0, EBP);
        addhook(0x0068666B, (int) InterceptPaint6C, 0, paintRegisterArgs, 0, EBP);

        addhook(0x00686806, (int) InterceptPaint7C, 0, paintRegisterArgs, 0, EBP);
        addhook(0x006869B2, (int) InterceptPaint7C, 0, paintRegisterArgs, 0, EBP);
        addhook(0x00686B6F, (int) InterceptPaint7C, 0, paintRegisterArgs, 0, EBP);
        addhook(0x00686D31, (int) InterceptPaint7C, 0, paintRegisterArgs, 0, EBP);

        addhook(0x00686EF0, (int) InterceptPaint8C, 0, paintRegisterArgs, 0, EBP);
        addhook(0x00687056, (int) InterceptPaint8C, 0, paintRegisterArgs, 0, EBP);
        addhook(0x006871C8, (int) InterceptPaint8C, 0, paintRegisterArgs, 0, EBP);
        addhook(0x0068733C, (int) InterceptPaint8C, 0, paintRegisterArgs, 0, EBP);

        addhook(0x006874B0, (int) InterceptPaint9C, 0, paintRegisterArgs, 0, EBP);
        addhook(0x00687618, (int) InterceptPaint9C, 0, paintRegisterArgs, 0, EBP);
        addhook(0x0068778C, (int) InterceptPaint9C, 0, paintRegisterArgs, 0, EBP);
        addhook(0x00687902, (int) InterceptPaint9C, 0, paintRegisterArgs, 0, EBP);
    }

    bool PaintWoodenSupports(uint8 function, int supportType, int special, int height, uint32 imageColourFlags, bool *underground) {
        function_call call = {0};
        call.function = function;
        call.supports.type = supportType;
        call.supports.special = special;
        call.supports.height = height;
        call.supports.colour_flags = imageColourFlags;

        _calls[_callCount] = call;
        _callCount++;

        return _woodenSupports;
    }

    bool PaintMetalSupports(uint8 function, int supportType, uint8 segment, int special, int height, uint32 imageColourFlags) {
        CheckSegmentSupportHeight();

        function_call call = {0};
        call.function = function;
        call.supports.type = supportType;
        call.supports.segment = segment;
        call.supports.special = special;
        call.supports.height = height;
        call.supports.colour_flags = imageColourFlags;

        _calls[_callCount] = call;
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
        function_call call = {0};
        call.function = PAINT_98196C;
        call.paint.image_id = imageID;
        call.paint.offset = {xOffset, yOffset};
        call.paint.bound_box_length = {boundBoxLengthX, boundBoxLengthY, boundBoxLengthZ};
        call.paint.z_offset = zOffset;
        call.paint.rotation = rotation;

        _calls[_callCount] = call;
        _callCount++;

        return nullptr;
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
        function_call call = {0};
        call.function = function;
        call.paint.image_id = imageID;
        call.paint.offset = {xOffset, yOffset};
        call.paint.bound_box_length = {boundBoxLengthX, boundBoxLengthY, boundBoxLengthZ};
        call.paint.bound_box_offset = {boundBoxOffsetX, boundBoxOffsetY, boundBoxOffsetZ};
        call.paint.z_offset = zOffset;
        call.paint.rotation = rotation;

        _calls[_callCount] = call;
        _callCount++;

        return nullptr;
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

    static uint32 InterceptMetalASupports(uint32 eax, uint32 ebx, uint32 edx, uint32 edi, uint32 ebp) {
        bool output = PaintMetalSupports(SUPPORTS_METAL_A, edi, ebx, (sint16) (eax & 0xFFFF), (edx & 0xFFFF), ebp);

        return output ? 1 : 0;
    }

    static uint32 InterceptMetalBSupports(uint32 eax, uint32 ebx, uint32 edx, uint32 edi, uint32 ebp) {
        bool output = PaintMetalSupports(SUPPORTS_METAL_B, edi, ebx, (sint16) (eax & 0xFFFF), (edx & 0xFFFF), ebp);

        return output ? 1 : 0;
    }

    static void CheckSegmentSupportHeight() {
        bool hasChanged = false;
        for (int i = 0; i < 9; i++) {
            if (gSupportSegments[i].height != 0) hasChanged = true;
            if (gSupportSegments[i].slope != 0xFF) hasChanged = true;
        }

        if (!hasChanged) {
            return;
        }

        function_call call = {0};
        call.function = SET_SEGMENT_HEIGHT;

        _calls[_callCount] = call;
        _callCount++;
    }

    static uint32 InterceptWoodenASupports(uint32 eax, uint32 ebx, uint32 edx, uint32 edi, uint32 ebp) {
        bool output = PaintWoodenSupports(SUPPORTS_WOOD_A, edi, eax & 0xFFFF, edx & 0xFFFF, ebp, nullptr);

        return output ? 1 : 0;
    }

    static uint32 InterceptWoodenBSupports(uint32 eax, uint32 ebx, uint32 edx, uint32 edi, uint32 ebp) {
        bool output = PaintWoodenSupports(SUPPORTS_WOOD_B, edi, eax & 0xFFFF, edx & 0xFFFF, ebp, nullptr);

        return output ? 1 : 0;
    }

    static uint32 InterceptPaint6C(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp) {
        if ((ebp & 0x03) != get_current_rotation()) {
            // Log error
            log_error("Ebp is different from current rotation");
        }

        return (uintptr_t) Paint6C(
            ebx,
            (sint8) (eax & 0xFF), (sint8) (ecx & 0xFF),
            (sint16) (edi & 0xFFFF), (sint16) (esi & 0xFFFF), (sint8) ((eax >> 8) & 0xFF),
            edx & 0xFFFF,
            ebp & 0x03
        );
    }

    static uint32 InterceptPaint7C(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp) {
        return InterceptPaintFull(PAINT_98197C, eax, ebx, ecx, edx, esi, edi, ebp);
    }

    static uint32 InterceptPaint8C(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp) {
        return InterceptPaintFull(PAINT_98198C, eax, ebx, ecx, edx, esi, edi, ebp);
    }

    static uint32 InterceptPaint9C(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp) {
        return InterceptPaintFull(PAINT_98199C, eax, ebx, ecx, edx, esi, edi, ebp);
    }

    static uint32 InterceptPaintFull(uint8 function, uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp) {
        if ((ebp & 0x03) != get_current_rotation()) {
            // Log error
            log_error("Ebp is different from current rotation");
        }

        rct_xyz16 boundOffset = {
            RCT2_GLOBAL(RCT2_ADDRESS_PAINT_BOUNDBOX_OFFSET_X, sint16),
            RCT2_GLOBAL(RCT2_ADDRESS_PAINT_BOUNDBOX_OFFSET_Y, sint16),
            RCT2_GLOBAL(RCT2_ADDRESS_PAINT_BOUNDBOX_OFFSET_Z, sint16)
        };

        return (uintptr_t) PaintFull(
            function,
            ebx,
            (sint8) (eax & 0xFF), (sint8) (ecx & 0xFF),
            (sint16) (edi & 0xFFFF), (sint16) (esi & 0xFFFF), (sint8) ((eax >> 8) & 0xFF),
            edx & 0xFFFF,
            boundOffset.x, boundOffset.y, boundOffset.z,
            ebp & 0x03
        );
    }
};


extern "C" {
bool wooden_a_supports_paint_setup(int supportType, int special, int height, uint32 imageColourFlags, bool *underground) {
    return PaintIntercept::PaintWoodenSupports(SUPPORTS_WOOD_A, supportType, special, height, imageColourFlags, underground);
}

bool wooden_b_supports_paint_setup(int supportType, int special, int height, uint32 imageColourFlags, bool *underground) {
    return PaintIntercept::PaintWoodenSupports(SUPPORTS_WOOD_B, supportType, special, height, imageColourFlags, underground);
}

bool metal_a_supports_paint_setup(uint8 supportType, uint8 segment, int special, int height, uint32 imageColourFlags) {
    return PaintIntercept::PaintMetalSupports(SUPPORTS_METAL_A, supportType, segment, special, height, imageColourFlags);
}

bool metal_b_supports_paint_setup(uint8 supportType, uint8 segment, int special, int height, uint32 imageColourFlags) {
    return PaintIntercept::PaintMetalSupports(SUPPORTS_METAL_B, supportType, segment, special, height, imageColourFlags);
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
