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
        _rideName = std::string(RideNames[rideType]);
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
        GenerateTrackFunctions();
        GenerateMainFunction();
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
        int height = 48;

        WriteLine(0, "static void " + GetTrackFunctionName(trackType) + "(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)");
        WriteLine(0, "{");

        Intercept2::TunnelCall tileTunnelCalls[4][4];
        std::vector<Intercept2::SegmentSupportCall> segmentSupportCalls[4];
        support_height generalSupports[4] = { 0 };
        for (int direction = 0; direction < 4; direction++) {
            int trackSequence = 0;

            rct_map_element mapElement = { 0 };
            mapElement.flags |= MAP_ELEMENT_FLAG_LAST_TILE;
            mapElement.properties.track.type = trackType;
            mapElement.base_height = 3;
            g_currently_drawn_item = &mapElement;

            CallOriginal(trackType, direction, trackSequence, height, &mapElement);

            segmentSupportCalls[direction] = Intercept2::getSegmentCalls(gSupportSegments, direction);
            generalSupports[direction] = gSupport;
            generalSupports[direction].height -= height;

            GetTunnelCalls(trackType, direction, trackSequence, height, &mapElement, tileTunnelCalls);
        }

        GenerateTunnelCall(tileTunnelCalls);
        GenerateSegmentSupportCall(segmentSupportCalls);
        GenerateGeneralSupportCall(generalSupports);

        WriteLine(0, "}");
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

    void GenerateTunnelCall(Intercept2::TunnelCall tileTunnelCalls[4][4])
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
            GenerateTunnelCall(1, tunnelOffset[0], tunnelType[0]);
        }
        else
        {
            WriteLine(1, "if (direction == 0 || direction == 3) {");
            GenerateTunnelCall(2, tunnelOffset[0], tunnelType[0]);
            WriteLine(1, "} else {");
            GenerateTunnelCall(2, tunnelOffset[1], tunnelType[1]);
            WriteLine(1, "}");
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

    void GenerateSegmentSupportCall(std::vector<Intercept2::SegmentSupportCall> segmentSupportCalls[4])
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
            WriteLine(1, szCall);
        }
    }

    void GenerateGeneralSupportCall(support_height generalSupports[4])
    {
        if (AllMatch(generalSupports, 4))
        {
            WriteLine(1, "paint_util_set_general_support_height(height + %d, 0x%02X);", generalSupports[0].height, generalSupports[0].slope);
        }
        else
        {
            WriteLine(1, "#error Unsupported: different directional general supports");
        }
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
        std::string trackName = TrackNames[trackType];
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
