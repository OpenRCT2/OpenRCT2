/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Printer.hpp"

#include "FunctionCall.hpp"
#include "String.hpp"

#include <iterator>
#include <openrct2/sprites.h>

namespace Printer
{
    static const char* functionNames[] = {
        "sub_98196C",
        "sub_98197C",
        "sub_98198C",
        "sub_98199C",
        "metal_a_supports_paint_setup",
        "metal_b_supports_paint_setup",
        "wooden_a_supports_paint_setup",
        "wooden_b_supports_paint_setup",
        "paint_util_set_segment_support_height",
    };

    static std::string GetImageIdString(uint32_t imageId);

    static std::string GetOffsetExpressionString(int offset);

    static std::string PrintSegmentSupportHeightCall(SegmentSupportCall call);

    static std::string PrintSideTunnelEdge(TunnelCall edge);

    std::string PrintFunctionCalls(std::vector<function_call> calls, uint16_t baseHeight)
    {
        std::string out;

        for (auto&& call : calls)
        {
            out += PrintFunctionCall(call, baseHeight).c_str();
            out += "\n";
        }

        return out;
    }

    std::string PrintFunctionCall(function_call call, uint16_t baseHeight)
    {
        std::string imageId = GetImageIdString(call.supports.colour_flags);
        assert(call.function < std::size(functionNames));
        const char* functionName = functionNames[call.function];
        std::string out = "";

        switch (call.function)
        {
            case SUPPORTS_WOOD_A:
            case SUPPORTS_WOOD_B:
                out += String::Format(
                    "%s(%d, %d, %s, %s)", functionName, call.supports.type, call.supports.special,
                    PrintHeightOffset(call.supports.height, baseHeight).c_str(), imageId.c_str());
                if (call.supports.special == 14 || call.supports.special == 15 || call.supports.special == 18
                    || call.supports.special == 19 || call.supports.special == 22 || call.supports.special == 23)
                {
                    if (call.supports.prepend_to == SPR_NONE)
                    {
                        out += " [prependTo:SPR_NONE]";
                    }
                    else
                    {
                        std::string prependId = GetImageIdString(call.supports.prepend_to);
                        out += String::Format(" [prependTo:%s]", prependId.c_str());
                    }
                }
                else if (call.supports.prepend_to != SPR_NONE)
                {
                    std::string prependId = GetImageIdString(call.supports.prepend_to);
                    out += String::Format(" [prependTo:%s]", prependId.c_str());
                }
                return out;

            case SUPPORTS_METAL_A:
            case SUPPORTS_METAL_B:
                return String::Format(
                    "%s(%d, %d, %d, %s, %s)", functionName, call.supports.type, call.supports.segment, call.supports.special,
                    PrintHeightOffset(call.supports.height, baseHeight).c_str(), imageId.c_str());

            case SET_SEGMENT_HEIGHT:
                return "paint_util_set_segment_support_height";
        }

        std::string s = String::Format("%s(", functionName);

        imageId = GetImageIdString(call.paint.image_id);
        s += String::Format("%s, ", imageId.c_str());
        s += String::Format("%d, %d, ", call.paint.offset.x, call.paint.offset.y);
        s += String::Format(
            "%d, %d, %d, ", call.paint.bound_box_length.x, call.paint.bound_box_length.y, call.paint.bound_box_length.z);
        s += String::Format("%s, ", PrintHeightOffset(call.paint.z_offset, baseHeight).c_str());

        if (call.function != PAINT_98196C)
        {
            s += String::Format(
                "%d, %d, %s, ", call.paint.bound_box_offset.x, call.paint.bound_box_offset.y,
                PrintHeightOffset(call.paint.bound_box_offset.z, baseHeight).c_str());
        }

        s += String::Format("%d)", call.paint.rotation);

        if (call.function != PAINT_98196C)
        {
            s += String::Format(
                "    = { %d, %d, %s }, { %d, %d, %s }, { %d, %d, %d }", call.paint.offset.x, call.paint.offset.y,
                PrintHeightOffset(call.paint.z_offset, baseHeight).c_str(), call.paint.bound_box_offset.x,
                call.paint.bound_box_offset.y, PrintHeightOffset(call.paint.bound_box_offset.z, baseHeight).c_str(),
                call.paint.bound_box_length.x, call.paint.bound_box_length.y, call.paint.bound_box_length.z);
        }

        return s;
    }

    std::string PrintSegmentSupportHeightCalls(std::vector<SegmentSupportCall> calls)
    {
        std::string out = "";

        for (auto&& call : calls)
        {
            out += PrintSegmentSupportHeightCall(call);
        }

        return out;
    }

    static std::string PrintSegmentSupportHeightCall(SegmentSupportCall call)
    {
        std::string out = "";

        if (call.segments == SEGMENTS_ALL)
        {
            out += "SEGMENTS_ALL";
        }
        else
        {
            int segmentsPrinted = 0;
            for (int i = 0; i < 9; i++)
            {
                if (call.segments & segment_offsets[i])
                {
                    if (segmentsPrinted > 0)
                    {
                        out += " | ";
                    }
                    out += String::Format("SEGMENT_%02X", 0xB4 + 4 * i);
                    segmentsPrinted++;
                }
            }
        }

        if (call.height == 0xFFFF)
        {
            out += ", 0xFFFF";
        }
        else
        {
            out += String::Format(", %d", call.height);
        }

        out += String::Format(", 0x%02X\n", call.slope);

        return out;
    }

    std::string PrintSideTunnelCalls(TunnelCall tunnelCalls[4][4])
    {
        std::string s;

        for (int direction = 0; direction < 4; ++direction)
        {
            s += "        +        ";
        }
        s += "\n";

        for (int direction = 0; direction < 4; ++direction)
        {
            std::string leftEdge = PrintSideTunnelEdge(tunnelCalls[direction][3]);
            std::string rightEdge = PrintSideTunnelEdge(tunnelCalls[direction][2]);
            s += String::Format("   %s %s   ", leftEdge.c_str(), rightEdge.c_str());
        }
        s += "\n";

        for (int direction = 0; direction < 4; ++direction)
        {
            s += "  +           +  ";
        }
        s += "\n";

        for (int direction = 0; direction < 4; ++direction)
        {
            std::string leftEdge = PrintSideTunnelEdge(tunnelCalls[direction][0]);
            std::string rightEdge = PrintSideTunnelEdge(tunnelCalls[direction][1]);
            s += String::Format("   %s %s   ", leftEdge.c_str(), rightEdge.c_str());
        }
        s += "\n";

        for (int direction = 0; direction < 4; ++direction)
        {
            s += "        +        ";
        }
        s += "\n";

        for (int direction = 0; direction < 4; ++direction)
        {
            s += String::Format("   direction %d   ", direction);
        }
        s += "\n";

        return s;
    }

    static std::string PrintSideTunnelEdge(TunnelCall edge)
    {
        std::string s;

        switch (edge.call)
        {
            case TUNNELCALL_SKIPPED:
                s = "     ";
                break;

            case TUNNELCALL_NONE:
                s = "  -  ";
                break;

            case TUNNELCALL_CALL:
                std::string offset;

                if (edge.offset <= 0)
                {
                    offset = String::Format("%d", edge.offset);
                }
                else
                {
                    offset = String::Format("+%d", edge.offset);
                }
                s = String::Format("%3s/%X", offset.c_str(), edge.type);
                break;
        }

        return s;
    }

    static std::string GetImageIdString(uint32_t imageId)
    {
        std::string result;

        uint32_t image = imageId & 0x7FFFF;
        uint32_t palette = imageId & ~0x7FFFF;

        std::string paletteName;
        if (palette == TestPaint::DEFAULT_SCHEME_TRACK)
            paletteName = "SCHEME_TRACK";
        else if (palette == TestPaint::DEFAULT_SCHEME_SUPPORTS)
            paletteName = "SCHEME_SUPPORTS";
        else if (palette == TestPaint::DEFAULT_SCHEME_MISC)
            paletteName = "SCHEME_MISC";
        else if (palette == TestPaint::DEFAULT_SCHEME_3)
            paletteName = "SCHEME_3";
        else
        {
            paletteName = String::Format("0x%08X", palette);
        }

        if (image == 0)
        {
            result = paletteName;
        }
        else if (image & 0x70000)
        {
            result = String::Format("%s | vehicle.base_image_id + %d", paletteName.c_str(), image & ~0x70000);
        }
        else
        {
            result = String::Format("%s | %d", paletteName.c_str(), image);
        }

        return result;
    }

    std::string PrintHeightOffset(uint16_t height, uint16_t baseHeight)
    {
        int offset = height - baseHeight;

        return String::Format("height%s", GetOffsetExpressionString(offset).c_str());
    }

    static std::string GetOffsetExpressionString(int offset)
    {
        if (offset < 0)
            return std::string(" - ") + std::to_string(-offset);
        if (offset > 0)
            return std::string(" + ") + std::to_string(offset);
        return std::string();
    }
}; // namespace Printer
