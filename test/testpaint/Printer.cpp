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

#include <string>

#include "intercept.h"

class Printer {
public:
    static std::string PrintFunction(function_call call, uint16 baseHeight) {
        std::string imageId = GetImageIdString(call.paint.image_id);

        switch (call.function) {
            case SUPPORTS_WOOD_A:
                return StringFormat("wooden_a_supports_paint_setup(%d, %d, %s, %s)", call.supports.type, call.supports.special,
                                    GetHeightOffset(call.supports.height, baseHeight).c_str(), imageId.c_str());
            case SUPPORTS_WOOD_B:
                return StringFormat("wooden_b_supports_paint_setup(%d, %d, %s, %s)", call.supports.type, call.supports.special,
                                    call.supports.height, imageId.c_str());

            case SUPPORTS_METAL_A:
                return StringFormat("metal_a_supports_paint_setup(%d, %d, %d, %s, %s)", call.supports.type,
                                    call.supports.segment, call.supports.special, GetHeightOffset(call.supports.height, baseHeight).c_str(), imageId.c_str());
            case SUPPORTS_METAL_B:
                return StringFormat("metal_b_supports_paint_setup(%d, %d, %d, %s, %s)", call.supports.type,
                                    call.supports.segment, call.supports.special, GetHeightOffset(call.supports.height, baseHeight).c_str(), imageId.c_str());

            case SET_SEGMENT_HEIGHT:
                return "paint_util_set_segment_support_height";
        }

        std::string functionCallName = "?";
        switch (call.function) {
            case PAINT_98196C:
                functionCallName = "sub_98196C";
                break;
            case PAINT_98197C:
                functionCallName = "sub_98197C";
                break;
            case PAINT_98198C:
                functionCallName = "sub_98198C";
                break;
            case PAINT_98199C:
                functionCallName = "sub_98199C";
                break;
        }

        std::string s = StringFormat("%s(", functionCallName.c_str());

        s += StringFormat("%s, ", imageId.c_str());
        s += StringFormat("%d, %d, ", call.paint.offset.x, call.paint.offset.y);
        s += StringFormat(
            "%d, %d, %d, ",
            call.paint.bound_box_length.x, call.paint.bound_box_length.y, call.paint.bound_box_length.z
        );
        s += StringFormat("%s, ", GetHeightOffset(call.paint.z_offset, baseHeight).c_str());

        if (call.function != PAINT_98196C) {
            s += StringFormat(
                "%d, %d, %s, ",
                call.paint.bound_box_offset.x, call.paint.bound_box_offset.y, GetHeightOffset(call.paint.bound_box_offset.z, baseHeight).c_str()
            );
        }

        s += StringFormat("%d)", call.paint.rotation);


        if (call.function != PAINT_98196C) {
            s += StringFormat(
                "    = { %d, %d, %s }, { %d, %d, %s }, { %d, %d, %d }",
                call.paint.offset.x, call.paint.offset.y, GetHeightOffset(call.paint.z_offset, baseHeight).c_str(),
                call.paint.bound_box_offset.x, call.paint.bound_box_offset.y,
                GetHeightOffset(call.paint.bound_box_offset.z, baseHeight).c_str(),
                call.paint.bound_box_length.x, call.paint.bound_box_length.y, call.paint.bound_box_length.z);
        }

        return s;
    }

private:
    static std::string GetImageIdString(uint32 imageId)
    {
        std::string result;

        uint32 image = imageId & 0x7FFFF;
        uint32 palette = imageId & ~0x7FFFF;

        std::string paletteName;
        if (palette == Intercept2::DEFAULT_SCHEME_TRACK) paletteName = "SCHEME_TRACK";
        else if (palette == Intercept2::DEFAULT_SCHEME_SUPPORTS) paletteName = "SCHEME_SUPPORTS";
        else if (palette == Intercept2::DEFAULT_SCHEME_MISC) paletteName = "SCHEME_MISC";
        else if (palette == Intercept2::DEFAULT_SCHEME_3) paletteName = "SCHEME_3";
        else {
            paletteName = StringFormat("0x%08X", palette);
        }

        if (image == 0) {
            result = paletteName;
        } else if (image & 0x70000) {
            result = StringFormat("%s | vehicle.base_image_id + %d", paletteName.c_str(), image & ~0x70000);
        } else {
            result = StringFormat("%s | %d", paletteName.c_str(), image);
        }

        return result;
    }

    static std::string GetHeightOffset(uint16 height, uint16 baseHeight) {
        int offset = height - baseHeight;

        return StringFormat("height%s", GetOffsetExpressionString(offset).c_str());
    }

    static std::string GetOffsetExpressionString(int offset)
    {
        if (offset < 0) return std::string(" - ") + std::to_string(-offset);
        if (offset > 0) return std::string(" + ") + std::to_string(offset);
        return std::string();
    }

    static std::string StringFormat(const char *format, ...) {
        va_list args;
        char buffer[512];

        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);

        return std::string(buffer);
    }
};

extern "C" {
void printFunctionCall(utf8string out, size_t len, function_call call, uint16 baseHeight) {
    snprintf(out, len, "%s", Printer::PrintFunction(call, baseHeight).c_str());
}
}
