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

#include <algorithm>
#include <string>
#include <vector>

#include "intercept.h"

extern "C"
{
    #include "data.h"
    #include "../../src/interface/viewport.h"
    #include "../../src/rct2.h"
    #include "../../src/ride/ride.h"
    #include "../../src/ride/ride_data.h"
    #include "../../src/ride/track.h"
    #include "../../src/ride/track_data.h"
}

class PaintCodeGenerator
{
public:
    int Generate(uint8 rideType)
    {
        auto filename = "paint_" + std::to_string(rideType) + ".c";
        FILE * file = fopen(filename.c_str(), "w");
        if (file == nullptr)
        {
            fprintf(stderr, "Unable to save to ./%s\n", filename.c_str());
            return 1;
        }

        _file = file;
        _rideType = rideType;
        _rideName = std::string(RideCodeNames[rideType]);
        Generate();

        fclose(file);
        return 0;
    }

private:
    std::string _rideName;
    uint8       _rideType;
    FILE *      _file;

    void Generate()
    {
        GenerateIncludes();
        GenerateTrackFunctions();
        GenerateMainFunction();
    }

    void GenerateIncludes()
    {
        const char * includes[] =
        {
            "../../drawing/drawing.h",
            "../../paint/supports.h",
            "../../interface/viewport.h",
            "../../paint/map_element/map_element.h",
            "../../paint/paint.h",
            "../../sprites.h",
            "../../world/map.h",
            "../../world/sprite.h",
            "../ride_data.h",
            "../track_data.h",
            "../track_paint.h",
        };
        for (auto include : includes)
        {
            WriteLine(0, "#include \"%s\"", include);
        }
        WriteLine();
    }

    void GenerateTrackFunctions()
    {
        for (int trackType = 0; trackType < 256; trackType++)
        {
            if (IsTrackTypeSupported(trackType))
            {
                GenerateTrackFunction(trackType);
                WriteLine();
            }
        }
    }

    void GenerateTrackFunction(int trackType)
    {
        int numSequences = getTrackSequenceCount(_rideType, trackType);

        WriteLine(0, "static void " + GetTrackFunctionName(trackType) + "(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)");
        WriteLine(0, "{");
        if (!GenerateMirrorCall(1, trackType))
        {
            if (numSequences > 1)
            {
                WriteLine(1, "switch (trackSequence) {");
                for (int trackSequence = 0; trackSequence < numSequences; trackSequence++)
                {
                    WriteLine(1, "case %d:", trackSequence);
                    GenerateTrackSequence(2, trackType, trackSequence);
                    WriteLine(2, "break;");
                }
                WriteLine(1, "}");
            }
            else
            {
                GenerateTrackSequence(1, trackType, 0);
            }
        }
        WriteLine(0, "}");
    }

    bool GenerateMirrorCall(int tabs, int trackType)
    {
        uint8 mirrorTable[][3] = {
            { 0, TRACK_ELEM_25_DEG_DOWN, TRACK_ELEM_25_DEG_UP },
            { 0, TRACK_ELEM_60_DEG_DOWN, TRACK_ELEM_60_DEG_UP },
            { 0, TRACK_ELEM_FLAT_TO_25_DEG_DOWN, TRACK_ELEM_25_DEG_UP_TO_FLAT },
            { 0, TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN, TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP },
            { 0, TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN, TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP },
            { 0, TRACK_ELEM_25_DEG_DOWN_TO_FLAT, TRACK_ELEM_FLAT_TO_25_DEG_UP },
            { 0, TRACK_ELEM_LEFT_BANK_TO_25_DEG_DOWN, TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANK },
            { 0, TRACK_ELEM_RIGHT_BANK_TO_25_DEG_DOWN, TRACK_ELEM_25_DEG_UP_TO_LEFT_BANK },
            { 0, TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANK, TRACK_ELEM_RIGHT_BANK_TO_25_DEG_UP },
            { 0, TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANK, TRACK_ELEM_LEFT_BANK_TO_25_DEG_UP },
            { 0, TRACK_ELEM_RIGHT_BANK, TRACK_ELEM_LEFT_BANK },
            { 0, TRACK_ELEM_25_DEG_DOWN_RIGHT_BANKED, TRACK_ELEM_25_DEG_UP_LEFT_BANKED },
            { 0, TRACK_ELEM_25_DEG_DOWN_LEFT_BANKED, TRACK_ELEM_25_DEG_UP_RIGHT_BANKED },
            { 0, TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN, TRACK_ELEM_25_DEG_UP_TO_LEFT_BANKED_25_DEG_UP },
            { 0, TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN, TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANKED_25_DEG_UP },
            { 0, TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANKED_25_DEG_DOWN, TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_25_DEG_UP },
            { 0, TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANKED_25_DEG_DOWN, TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_25_DEG_UP },
            { 0, TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_RIGHT_BANKED_FLAT, TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_UP },
            { 0, TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_LEFT_BANKED_FLAT, TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_UP },
            { 0, TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN, TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_LEFT_BANKED_FLAT },
            { 0, TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_DOWN, TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_RIGHT_BANKED_FLAT },
            { 0, TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_FLAT, TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_UP },
            { 0, TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_FLAT, TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_UP },
            { 0, TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN, TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_FLAT },
            { 0, TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_DOWN, TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_FLAT },

            { 1, TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES, TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES },
            { 1, TRACK_ELEM_BANKED_RIGHT_QUARTER_TURN_5_TILES, TRACK_ELEM_BANKED_LEFT_QUARTER_TURN_5_TILES },
            { 1, TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_DOWN, TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_UP },
            { 1, TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN, TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP },

            { 2, TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_DOWN, TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_UP },
            { 2, TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN, TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP },
        };

        for (int i = 0; i < (sizeof(mirrorTable) / sizeof(mirrorTable[0])); i++)
        {
            if (mirrorTable[i][1] == trackType)
            {
                std::string destFuncName = GetTrackFunctionName(mirrorTable[i][2]);
                switch (mirrorTable[i][0]) {
                case 0:
                    WriteLine(tabs, "%s(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);", destFuncName.c_str());
                    break;
                case 1:
                    WriteLine(tabs, "trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];");
                    WriteLine(tabs, "%s(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);", destFuncName.c_str());
                    break;
                case 2:
                    WriteLine(tabs, "trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];");
                    WriteLine(tabs, "%s(rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);", destFuncName.c_str());
                    break;
                }
                return true;
            }
        }
        return false;
    }

    void GenerateTrackSequence(int tabs, int trackType, int trackSequence)
    {
        int height = 48;

        std::vector<function_call> calls[4];
        Intercept2::TunnelCall tileTunnelCalls[4][4];
        std::vector<Intercept2::SegmentSupportCall> segmentSupportCalls[4];
        support_height generalSupports[4] = { 0 };
        for (int direction = 0; direction < 4; direction++) {
            rct_map_element mapElement = { 0 };
            mapElement.flags |= MAP_ELEMENT_FLAG_LAST_TILE;
            mapElement.properties.track.type = trackType;
            mapElement.base_height = 3;
            g_currently_drawn_item = &mapElement;

            function_call callBuffer[256] = { 0 };
            intercept_clear_calls();
            CallOriginal(trackType, direction, trackSequence, height, &mapElement);
            int numCalls = intercept_get_calls(callBuffer);
            calls[direction].insert(calls[direction].begin(), callBuffer, callBuffer + numCalls);

            segmentSupportCalls[direction] = Intercept2::getSegmentCalls(gSupportSegments, direction);
            generalSupports[direction] = gSupport;
            generalSupports[direction].height -= height;

            GetTunnelCalls(trackType, direction, trackSequence, height, &mapElement, tileTunnelCalls);
        }

        GenerateCalls(tabs, calls, height);
        GenerateTunnelCall(tabs, tileTunnelCalls);
        GenerateSegmentSupportCall(tabs, segmentSupportCalls);
        GenerateGeneralSupportCall(tabs, generalSupports);
    }

    void GenerateCalls(int tabs, std::vector<function_call> calls[4], int height)
    {
        int totalCalls = 0;
        for (int direction = 0; direction < 4; direction++)
        {
            totalCalls += calls[direction].size();
        }
        if (totalCalls == 0)
        {
            return;
        }

        WriteLine(tabs, "switch (direction) {");
        for (int direction = 0; direction < 4; direction++)
        {
            if (calls[direction].size() == 0) continue;

            WriteLine(tabs, "case %d:", direction);
            for (auto call : calls[direction])
            {
                GenerateCalls(tabs + 1, call, height);
            }
            WriteLine(tabs + 1, "break;");
        }
        WriteLine(tabs, "}");
    }

    void GenerateCalls(int tabs, const function_call &call, int height)
    {
        const char * funcName = GetFunctionCallName(call.function);
        switch (call.function) {
        case PAINT_98196C:
        case PAINT_98197C:
        case PAINT_98198C:
        case PAINT_98199C:
        {
            std::string s = StringFormat("%s(%s, %d, %d, %d, %d, %d, height%s, ",
                funcName,
                GetImageIdString(call.paint.image_id).c_str(),
                call.paint.offset.x,
                call.paint.offset.y,
                call.paint.bound_box_length.x,
                call.paint.bound_box_length.y,
                call.paint.bound_box_length.z,
                GetOffsetExpressionString(call.paint.z_offset - height).c_str());

            if (call.function != PAINT_98196C)
            {
                s += StringFormat("%d, %d, height%s, ",
                    call.paint.bound_box_offset.x,
                    call.paint.bound_box_offset.y,
                    GetOffsetExpressionString(call.paint.bound_box_offset.z - height).c_str());
            }

            s += "get_current_rotation());";
            WriteLine(tabs, s);
            break;
        }
        case SUPPORTS_METAL_A:
            WriteLine(tabs, "metal_a_supports_paint_setup(%d, %d, %d, height%s, %s);",
                call.supports.type,
                call.supports.segment,
                call.supports.special,
                GetOffsetExpressionString(call.supports.height - height).c_str(),
                GetImageIdString(call.supports.colour_flags).c_str());
            break;
        case SUPPORTS_METAL_B:
            WriteLine(tabs, "metal_b_supports_paint_setup(%d, %d, %d, height%s, %s);",
                call.supports.type,
                call.supports.segment,
                call.supports.special,
                GetOffsetExpressionString(call.supports.height - height).c_str(),
                GetImageIdString(call.supports.colour_flags).c_str());
            break;
        case SUPPORTS_WOOD_A:
            WriteLine(tabs, "wooden_a_supports_paint_setup(%d, %d, height%s, %s, NULL);",
                call.supports.type,
                call.supports.special,
                GetOffsetExpressionString(call.supports.height - height).c_str(),
                GetImageIdString(call.supports.colour_flags).c_str());
            break;
        case SUPPORTS_WOOD_B:
            WriteLine(tabs, "wooden_a_supports_paint_setup(%d, %d, height%s, %s, NULL);",
                call.supports.type,
                call.supports.special,
                GetOffsetExpressionString(call.supports.height - height).c_str(),
                GetImageIdString(call.supports.colour_flags).c_str());
            break;
        }
    }

    const char * GetFunctionCallName(int function)
    {
        const char * functionNames[] = {
            "sub_98196C",
            "sub_98197C",
            "sub_98198C",
            "sub_98199C",
            "wooden_a_supports_paint_setup",
            "wooden_a_supports_paint_setup",
            "metal_a_supports_paint_setup",
            "metal_b_supports_paint_setup",
        };
        return functionNames[function];
    }

    bool GetTunnelCalls(int trackType, int direction, int trackSequence, int height, rct_map_element * mapElement, Intercept2::TunnelCall tileTunnelCalls[4][4])
    {
        gLeftTunnelCount = 0;
        gRightTunnelCount = 0;
        for (int offset = -8; offset <= 8; offset += 8)
        {
            CallOriginal(trackType, direction, trackSequence, height + offset, mapElement);
        }

        uint8 rightIndex = (4 - direction) % 4;
        uint8 leftIndex = (rightIndex + 1) % 4;

        for (int i = 0; i < 4; ++i) {
            tileTunnelCalls[direction][i].call = Intercept2::TUNNELCALL_SKIPPED;
        }
        if (gRightTunnelCount == 0) {
            tileTunnelCalls[direction][rightIndex].call = Intercept2::TUNNELCALL_NONE;
        } else if (gRightTunnelCount == 3) {
            tileTunnelCalls[direction][rightIndex].call = Intercept2::TUNNELCALL_CALL;
            tileTunnelCalls[direction][rightIndex].offset = Intercept2::getTunnelOffset(height, gRightTunnels);
            tileTunnelCalls[direction][rightIndex].type = gRightTunnels[0].type;
        } else {
            printf("Multiple tunnels on one side aren't supported.\n");
            return false;
        }

        if (gLeftTunnelCount == 0) {
            tileTunnelCalls[direction][leftIndex].call = Intercept2::TUNNELCALL_NONE;
        } else if (gLeftTunnelCount == 3) {
            tileTunnelCalls[direction][leftIndex].call = Intercept2::TUNNELCALL_CALL;
            tileTunnelCalls[direction][leftIndex].offset = Intercept2::getTunnelOffset(height, gLeftTunnels);
            tileTunnelCalls[direction][leftIndex].type = gLeftTunnels[0].type;
        } else {
            printf("Multiple tunnels on one side aren't supported.\n");
            return false;
        }

        return true;
    }

    void GenerateTunnelCall(int tabs, Intercept2::TunnelCall tileTunnelCalls[4][4])
    {
        sint16 tunnelOffset[4] = { 0 };
        uint8 tunnelType[4] = { 0xFF };
        for (int direction = 0; direction < 4; direction++)
        {
            for (int side = 0; side < 4; side++)
            {
                auto tunnel = tileTunnelCalls[direction][side];
                if (tunnel.call == Intercept2::TUNNELCALL_CALL)
                {
                    tunnelOffset[direction] = tunnel.offset;
                    tunnelType[direction] = tunnel.type;
                    break;
                }
            }
        }

        if (tunnelOffset[0] == tunnelOffset[1] &&
            tunnelOffset[0] == tunnelOffset[2] &&
            tunnelOffset[0] == tunnelOffset[3])
        {
            GenerateTunnelCall(tabs, tunnelOffset[0], tunnelType[0]);
        }
        else
        {
            WriteLine(tabs, "if (direction == 0 || direction == 3) {");
            GenerateTunnelCall(tabs + 1, tunnelOffset[0], tunnelType[0]);
            WriteLine(tabs, "} else {");
            GenerateTunnelCall(tabs + 1, tunnelOffset[1], tunnelType[1]);
            WriteLine(tabs, "}");
        }
    }

    void GenerateTunnelCall(int tabs, int offset, int type)
    {
        if (offset == 0)
        {
            WriteLine(tabs, "paint_util_push_tunnel_rotated(direction, height, TUNNEL_%d);", offset);
        }
        else if (offset < 0)
        {
            WriteLine(tabs, "paint_util_push_tunnel_rotated(direction, height - %d, TUNNEL_%d);", -offset, type);
        }
        else
        {
            WriteLine(tabs, "paint_util_push_tunnel_rotated(direction, height + %d, TUNNEL_%d);", offset, type);
        }
    }

    void GenerateSegmentSupportCall(int tabs, std::vector<Intercept2::SegmentSupportCall> segmentSupportCalls[4])
    {
        for (size_t i = 0; i < segmentSupportCalls[0].size(); i++)
        {
            auto ssh = segmentSupportCalls[0][i];
            std::string szCall = "paint_util_set_segment_support_height(";
            if (ssh.segments == SEGMENTS_ALL)
            {
                szCall += "SEGMENTS_ALL";
            }
            else
            {
                szCall += "paint_util_rotate_segments(";
                szCall += GetORedSegments(ssh.segments);
                szCall += ", direction)";
            }
            szCall += ", ";
            if (ssh.height == 0xFFFF)
            {
                szCall += "0xFFFF";
                szCall += StringFormat(", 0);", ssh.slope);
            }
            else
            {
                szCall += std::to_string(ssh.height);
                szCall += StringFormat(", 0x%02X);", ssh.slope);
            }
            WriteLine(tabs, szCall);
        }
    }

    void GenerateGeneralSupportCall(int tabs, support_height generalSupports[4])
    {
        WriteLine(tabs, "paint_util_set_general_support_height(height + %d, 0x%02X);", generalSupports[0].height, generalSupports[0].slope);
        if (!AllMatch(generalSupports, 4))
        {
            // WriteLine(tabs, "#error Unsupported: different directional general supports");
        }
    }

    std::string GetImageIdString(uint32 imageId)
    {
        std::string result;

        uint32 image = imageId & 0x7FFFF;
        uint32 palette = imageId & ~0x7FFFF;

        std::string paletteName;
        if (palette == Intercept2::DEFAULT_SCHEME_TRACK) paletteName = "gTrackColours[SCHEME_TRACK]";
        else if (palette == Intercept2::DEFAULT_SCHEME_SUPPORTS) paletteName = "gTrackColours[SCHEME_SUPPORTS]";
        else if (palette == Intercept2::DEFAULT_SCHEME_MISC) paletteName = "gTrackColours[SCHEME_MISC]";
        else if (palette == Intercept2::DEFAULT_SCHEME_3) paletteName = "gTrackColours[SCHEME_3]";
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

    std::string GetOffsetExpressionString(int offset)
    {
        if (offset < 0) return std::string(" - ") + std::to_string(-offset);
        if (offset > 0) return std::string(" + ") + std::to_string(offset);
        return std::string();
    }

    std::string GetORedSegments(int segments)
    {
        std::string s;
        int segmentsPrinted = 0;
        for (int i = 0; i < 9; i++) {
            if (segments & segment_offsets[i]) {
                if (segmentsPrinted > 0) {
                    s += " | ";
                }
                s += StringFormat("SEGMENT_%02X", 0xB4 + 4 * i);
                segmentsPrinted++;
            }
        }
        return s;
    }

    template<typename T>
    bool AllMatch(T * arr, size_t count)
    {
        for (size_t i = 1; i < count; i++)
        {
            if (memcmp((const void *)&arr[i], (const void *)&arr[0], sizeof(T)) != 0)
            {
                return false;
            }
        }
        return true;
    }

    void CallOriginal(int trackType, int direction, int trackSequence, int height, rct_map_element *mapElement)
    {
        gPaintInteractionType = VIEWPORT_INTERACTION_ITEM_RIDE;
        gTrackColours[SCHEME_TRACK] = Intercept2::DEFAULT_SCHEME_TRACK;
        gTrackColours[SCHEME_SUPPORTS] = Intercept2::DEFAULT_SCHEME_SUPPORTS;
        gTrackColours[SCHEME_MISC] = Intercept2::DEFAULT_SCHEME_MISC;
        gTrackColours[SCHEME_3] = Intercept2::DEFAULT_SCHEME_3;

        rct_drawpixelinfo dpi = { 0 };
        dpi.zoom_level = 1;
        unk_140E9A8 = &dpi;

        rct_ride ride = {0};

        rct_ride_entry rideEntry = {0};
        rct_ride_entry_vehicle vehicleEntry { 0 };
        vehicleEntry.base_image_id = 0x70000;
        rideEntry.vehicles[0] = vehicleEntry;

        gRideList[0] = ride;
        gRideEntries[0] = &rideEntry;

        for (int s = 0; s < 9; ++s)
        {
            gSupportSegments[s].height = 0;
            gSupportSegments[s].slope = 0xFF;
        }

        gSupport.height = 0;
        gSupport.slope = 0xFF;
        g141E9DB = G141E9DB_FLAG_1 | G141E9DB_FLAG_2;

        uint32 *trackDirectionList = (uint32 *)RideTypeTrackPaintFunctionsOld[_rideType][trackType];
        // Have to call from this point as it pushes esi and expects callee to pop it
        RCT2_CALLPROC_X(
            0x006C4934,
            _rideType,
            (int) trackDirectionList,
            direction,
            height,
            (int) &mapElement,
            0 * sizeof(rct_ride),
            trackSequence
        );
    }

    void GenerateMainFunction()
    {
        WriteLine(0, "TRACK_PAINT_FUNCTION get_track_paint_function_" + _rideName + "(int trackType, int direction)");
        WriteLine(0, "{");
        WriteLine(1, "switch (trackType) {");
        for (int trackType = 0; trackType < 256; trackType++)
        {
            if (IsTrackTypeSupported(trackType))
            {
                WriteLine(1, "case " + std::string(TrackElemNames[trackType]) + ":");
                WriteLine(2, "return %s;", GetTrackFunctionName(trackType).c_str());
            }
        }
        WriteLine(1, "}");
        WriteLine(1, "return NULL;");
        WriteLine(0, "}");
    }

    std::string GetTrackFunctionName(int trackType)
    {
        std::string trackName = TrackCodeNames[trackType];
        return _rideName + "_track_" + trackName;
    }

    bool IsTrackTypeSupported(int trackType)
    {
        if (trackType == TRACK_ELEM_BEGIN_STATION ||
            trackType == TRACK_ELEM_MIDDLE_STATION ||
            trackType == TRACK_ELEM_END_STATION)
        {
            return false;
        }
        if (RideTypeTrackPaintFunctionsOld[_rideType][trackType] != 0)
        {
            return true;
        }
        return false;
    }

    void WriteLine()
    {
        WriteLine(0, "");
    }

    void WriteLine(int tabs, const char * format, ...)
    {
        va_list args;
        char buffer[512];

        va_start(args, format);
        vsprintf(buffer, format, args);
        va_end(args);

        WriteLine(tabs, std::string(buffer));
    }

    void WriteLine(int tabs, std::string s)
    {
        for (int i = 0; i < tabs; i++)
        {
            fprintf(_file, "\t");
        }
        fprintf(_file, s.c_str());
        fprintf(_file, "\n");
    }

    static std::string StringFormat(const char * format, ...)
    {
        va_list args;
        char buffer[512];

        va_start(args, format);
        vsprintf(buffer, format, args);
        va_end(args);

        return std::string(buffer);
    }
};

extern "C"
{
    int generatePaintCode(uint8 rideType)
    {
        if (ride_type_has_flag(rideType, RIDE_TYPE_FLAG_FLAT_RIDE))
        {
            fprintf(stderr, "Flat rides not supported.\n");
        }

        auto pcg = PaintCodeGenerator();
        return pcg.Generate(rideType);
    }
}
