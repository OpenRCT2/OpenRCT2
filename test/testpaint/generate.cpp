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

#include <algorithm>
#include <cstdarg>
#include <string>
#include <vector>

#include "FunctionCall.hpp"
#include "PaintIntercept.hpp"
#include "SegmentSupportHeightCall.hpp"
#include "SideTunnelCall.hpp"
#include "String.hpp"
#include "Utils.hpp"

#include "data.h"
#include <openrct2/interface/viewport.h>
#include <openrct2/rct2.h>
#include <openrct2/ride/ride.h>
#include <openrct2/ride/ride_data.h>
#include <openrct2/ride/track.h>
#include <openrct2/ride/track_data.h>

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

    bool        _conditionalSupports;
    bool        _invertedTrack;

    void Generate()
    {
        GenerateCopyrightHeader();
        GenerateIncludes();
        GenerateTrackFunctions();
        GenerateMainFunction();
    }

    void GenerateCopyrightHeader()
    {
        const char * copyrights[] =
        {
            "#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers",
            "/*****************************************************************************",
            " * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.",
            " *",
            " * OpenRCT2 is the work of many authors, a full list can be found in contributors.md",
            " * For more information, visit https://github.com/OpenRCT2/OpenRCT2",
            " *",
            " * OpenRCT2 is free software: you can redistribute it and/or modify",
            " * it under the terms of the GNU General Public License as published by",
            " * the Free Software Foundation, either version 3 of the License, or",
            " * (at your option) any later version.",
            " *",
            " * A full copy of the GNU General Public License can be found in licence.txt",
            " *****************************************************************************/",
            "#pragma endregion",
        };

        for (const auto copyright : copyrights)
        {
            WriteLine(0, copyright);
        }
        WriteLine();
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

            if (trackType == TRACK_ELEM_END_STATION)
            {
                const uint32 * paintFunctionList = RideTypeTrackPaintFunctionsOld[_rideType];
                WriteLine(0, "/** rct2: 0x%08X, 0x%08X, 0x%08X */", paintFunctionList[TRACK_ELEM_END_STATION], paintFunctionList[TRACK_ELEM_BEGIN_STATION], paintFunctionList[TRACK_ELEM_MIDDLE_STATION]);
                WriteLine(0, "static void " + _rideName + "_track_station(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)");
                WriteLine(0, "{");
                WriteLine(0, "}");
                WriteLine();
            }
        }
    }

    void GenerateTrackFunction(int trackType)
    {
        WriteLine(0, "/** rct2: 0x%08X */", RideTypeTrackPaintFunctionsOld[_rideType][trackType]);
        WriteLine(0, "static void " + GetTrackFunctionName(trackType) + "(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)");
        WriteLine(0, "{");
        if (!GenerateMirrorCall(1, trackType))
        {
            if (_rideType == RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER ||
                _rideType == RIDE_TYPE_FLYING_ROLLER_COASTER ||
                _rideType == RIDE_TYPE_LAY_DOWN_ROLLER_COASTER)
            {
                WriteLine(1, "if (!track_element_is_inverted(mapElement)) {");
                _invertedTrack = false;
                GenerateTrackFunctionBody(2, trackType);
                WriteLine(1, "} else {");
                _invertedTrack = true;
                GenerateTrackFunctionBody(2, trackType);
                WriteLine(1, "}");
            }
            else
            {
                _invertedTrack = false;
                GenerateTrackFunctionBody(1, trackType);
            }
        }
        WriteLine(0, "}");
    }

    void GenerateTrackFunctionBody(int tabs, int trackType)
    {
        int numSequences = Utils::getTrackSequenceCount(_rideType, trackType);
        if (numSequences > 1)
        {
            WriteLine(tabs, "switch (trackSequence) {");
            for (int trackSequence = 0; trackSequence < numSequences; trackSequence++)
            {
                WriteLine(tabs, "case %d:", trackSequence);
                GenerateTrackSequence(tabs + 1, trackType, trackSequence);
                WriteLine(tabs + 1, "break;");
            }
            WriteLine(tabs, "}");
        }
        else
        {
            GenerateTrackSequence(tabs, trackType, 0);
        }
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
            { 0, TRACK_ELEM_60_DEG_DOWN_TO_FLAT, TRACK_ELEM_FLAT_TO_60_DEG_UP },
            { 0, TRACK_ELEM_FLAT_TO_60_DEG_DOWN, TRACK_ELEM_60_DEG_UP_TO_FLAT },
            { 0, TRACK_ELEM_25_DEG_DOWN_COVERED, TRACK_ELEM_25_DEG_UP_COVERED },
            { 0, TRACK_ELEM_60_DEG_DOWN_COVERED, TRACK_ELEM_60_DEG_UP_COVERED },
            { 0, TRACK_ELEM_FLAT_TO_25_DEG_DOWN_COVERED, TRACK_ELEM_25_DEG_UP_TO_FLAT_COVERED },
            { 0, TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN_COVERED, TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP_COVERED },
            { 0, TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN_COVERED, TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP_COVERED },
            { 0, TRACK_ELEM_25_DEG_DOWN_TO_FLAT_COVERED, TRACK_ELEM_FLAT_TO_25_DEG_UP_COVERED },
            { 0, TRACK_ELEM_25_DEG_DOWN_LEFT_BANKED, TRACK_ELEM_25_DEG_UP_RIGHT_BANKED },
            { 0, TRACK_ELEM_25_DEG_DOWN_RIGHT_BANKED, TRACK_ELEM_25_DEG_UP_LEFT_BANKED },
            { 0, TRACK_ELEM_90_DEG_DOWN, TRACK_ELEM_90_DEG_UP },
            { 0, TRACK_ELEM_90_DEG_DOWN_TO_60_DEG_DOWN, TRACK_ELEM_60_DEG_UP_TO_90_DEG_UP },
            // { 0, TRACK_ELEM_60_DEG_DOWN_TO_90_DEG_DOWN, TRACK_ELEM_90_DEG_UP_TO_60_DEG_UP },
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
            { 1, TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_COVERED, TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_COVERED },
            { 1, TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN, TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP },
            { 2, TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_DOWN, TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_UP },
            { 2, TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN, TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP },

            { 3, TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES, TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES },
            { 3, TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_BANK, TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_BANK },
            { 3, TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN, TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP },
            { 3, TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_COVERED, TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_COVERED },
            { 3, TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN, TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP },
            { 4, TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN, TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP },
            { 4, TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN, TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP },

            { 5, TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE, TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE },
            { 5, TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_DOWN, TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_UP },
            { 5, TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_DOWN, TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_UP },
            { 6, TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_DOWN, TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_UP },
            { 6, TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_DOWN, TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_UP },

            { 7, TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL, TRACK_ELEM_LEFT_EIGHTH_TO_DIAG },
            { 7, TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_ORTHOGONAL, TRACK_ELEM_LEFT_EIGHTH_BANK_TO_DIAG },
            { 8, TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL, TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG },
            { 8, TRACK_ELEM_LEFT_EIGHTH_BANK_TO_ORTHOGONAL, TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_DIAG },

            { 9, TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_SMALL, TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL },
            { 10, TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_SMALL, TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_SMALL },
            { 11, TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE, TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_LARGE },
            { 12, TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_LARGE, TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_LARGE },

            { 13, TRACK_ELEM_FLAT_TO_60_DEG_DOWN_LONG_BASE, TRACK_ELEM_FLAT_TO_60_DEG_UP_LONG_BASE },
            { 13, TRACK_ELEM_60_DEG_UP_TO_FLAT_LONG_BASE_122, TRACK_ELEM_60_DEG_UP_TO_FLAT_LONG_BASE },

            { 14, TRACK_ELEM_RIGHT_CORKSCREW_DOWN, TRACK_ELEM_LEFT_CORKSCREW_UP },
            { 15, TRACK_ELEM_LEFT_CORKSCREW_DOWN, TRACK_ELEM_RIGHT_CORKSCREW_UP },

            { 16, TRACK_ELEM_HALF_LOOP_DOWN, TRACK_ELEM_HALF_LOOP_UP },

            { 17, TRACK_ELEM_INVERTED_FLAT_TO_90_DEG_QUARTER_LOOP_DOWN, TRACK_ELEM_90_DEG_TO_INVERTED_FLAT_QUARTER_LOOP_UP },

            { 18, TRACK_ELEM_LEFT_BARREL_ROLL_DOWN_TO_UP, TRACK_ELEM_LEFT_BARREL_ROLL_UP_TO_DOWN },
            { 18, TRACK_ELEM_RIGHT_BARREL_ROLL_DOWN_TO_UP, TRACK_ELEM_RIGHT_BARREL_ROLL_UP_TO_DOWN },

            { 19, TRACK_ELEM_LEFT_LARGE_HALF_LOOP_DOWN, TRACK_ELEM_LEFT_LARGE_HALF_LOOP_UP },
            { 19, TRACK_ELEM_RIGHT_LARGE_HALF_LOOP_DOWN, TRACK_ELEM_RIGHT_LARGE_HALF_LOOP_UP },
        };

        for (size_t i = 0; i < (sizeof(mirrorTable) / sizeof(mirrorTable[0])); i++)
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
                case 3:
                    WriteLine(tabs, "trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];");
                    WriteLine(tabs, "%s(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);", destFuncName.c_str());
                    break;
                case 4:
                    WriteLine(tabs, "trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];");
                    WriteLine(tabs, "%s(rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);", destFuncName.c_str());
                    break;
                case 5:
                    WriteLine(tabs, "%s(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);", destFuncName.c_str());
                    break;
                case 6:
                    WriteLine(tabs, "%s(rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);", destFuncName.c_str());
                    break;
                case 7:
                    WriteLine(tabs, "trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];");
                    WriteLine(tabs, "%s(rideIndex, trackSequence, (direction + 3) & 3, height, mapElement);", destFuncName.c_str());
                    break;
                case 8:
                    WriteLine(tabs, "trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];");
                    WriteLine(tabs, "%s(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);", destFuncName.c_str());
                    break;
                case 9:
                    WriteLine(tabs, "if (trackSequence >= 4) {");
                    WriteLine(tabs + 1, "trackSequence -= 4;");
                    WriteLine(tabs + 1, "direction = (direction + 1) & 3;");
                    WriteLine(tabs, "}");
                    WriteLine(tabs, "trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];");
                    WriteLine(tabs, "%s(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);", destFuncName.c_str());
                    break;
                case 10:
                    WriteLine(tabs, "if (trackSequence >= 4) {");
                    WriteLine(tabs + 1, "trackSequence -= 4;");
                    WriteLine(tabs + 1, "direction = (direction - 1) & 3;");
                    WriteLine(tabs, "}");
                    WriteLine(tabs, "trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];");
                    WriteLine(tabs, "%s(rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);", destFuncName.c_str());
                    break;
                case 11:
                    WriteLine(tabs, "if (trackSequence >= 7) {");
                    WriteLine(tabs + 1, "trackSequence -= 7;");
                    WriteLine(tabs + 1, "direction = (direction + 1) & 3;");
                    WriteLine(tabs, "}");
                    WriteLine(tabs, "trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];");
                    WriteLine(tabs, "%s(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);", destFuncName.c_str());
                    break;
                case 12:
                    WriteLine(tabs, "if (trackSequence >= 7) {");
                    WriteLine(tabs + 1, "trackSequence -= 7;");
                    WriteLine(tabs + 1, "direction = (direction - 1) & 3;");
                    WriteLine(tabs, "}");
                    WriteLine(tabs, "trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];");
                    WriteLine(tabs, "%s(rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);", destFuncName.c_str());
                    break;
                case 13:
                    WriteLine(tabs, "%s(rideIndex, 3 - trackSequence, (direction + 2) & 3, height, mapElement);", destFuncName.c_str());
                    break;
                case 14:
                    WriteLine(tabs, "%s(rideIndex, 2 - trackSequence, (direction - 1) & 3, height, mapElement);", destFuncName.c_str());
                    break;
                case 15:
                    WriteLine(tabs, "%s(rideIndex, 2 - trackSequence, (direction + 1) & 3, height, mapElement);", destFuncName.c_str());
                    break;
                case 16:
                    WriteLine(tabs, "%s(rideIndex, 3 - trackSequence, direction, height, mapElement);", destFuncName.c_str());
                    break;
                case 17:
                    WriteLine(tabs, "%s(rideIndex, 2 - trackSequence, direction, height, mapElement);", destFuncName.c_str());
                    break;
                case 18:
                    WriteLine(tabs, "%s(rideIndex, 2 - trackSequence, (direction + 2) & 3, height, mapElement);", destFuncName.c_str());
                    break;
                case 19:
                    WriteLine(tabs, "%s(rideIndex, 6 - trackSequence, direction, height, mapElement);", destFuncName.c_str());
                    break;
                }
                return true;
            }
        }
        return false;
    }

    void ExtractMetalSupportCalls(std::vector<function_call> calls[4], std::vector<function_call> output[4])
    {
        for (int direction = 0; direction < 4; direction++) {

            auto cutPoint = std::find_if(calls[direction].begin(), calls[direction].end(), [](function_call call) {
                return (call.function == SUPPORTS_METAL_A || call.function == SUPPORTS_METAL_B);
            });
            output[direction].insert(output[direction].begin(), cutPoint, calls[direction].end());
            calls[direction].erase(cutPoint, calls[direction].end());
        }
    }

    void GenerateTrackSequence(int tabs, int trackType, int trackSequence)
    {
        int height = 48;
        _conditionalSupports = false;
        bool blockSegmentsBeforeSupports = false;

        std::vector<function_call> calls[4], chainLiftCalls[4], cableLiftCalls[4];
        TunnelCall tileTunnelCalls[4][4];
        sint16 verticalTunnelHeights[4];
        std::vector<SegmentSupportCall> segmentSupportCalls[4];
        support_height generalSupports[4] = { 0 };
        for (int direction = 0; direction < 4; direction++) {
            rct_map_element mapElement = { 0 };
            mapElement.flags |= MAP_ELEMENT_FLAG_LAST_TILE;
            mapElement.properties.track.type = trackType;
            mapElement.base_height = 3;
            if (_invertedTrack)
            {
                mapElement.properties.track.colour |= TRACK_ELEMENT_COLOUR_FLAG_INVERTED;
            }
            g_currently_drawn_item = &mapElement;

            // Set position
            RCT2_GLOBAL(0x009DE56A, sint16) = 64;
            RCT2_GLOBAL(0x009DE56E, sint16) = 64;

            function_call callBuffer[256] = { 0 };
            PaintIntercept::ClearCalls();
            CallOriginal(trackType, direction, trackSequence, height, &mapElement);
            int numCalls = PaintIntercept::GetCalls(callBuffer);
            calls[direction].insert(calls[direction].begin(), callBuffer, callBuffer + numCalls);

            for (auto &&call : calls[direction]) {
                if (call.function == SET_SEGMENT_HEIGHT) {
                    blockSegmentsBeforeSupports = true;
                    break;
                }
            }

            segmentSupportCalls[direction] = SegmentSupportHeightCall::getSegmentCalls(gSupportSegments, direction);
            generalSupports[direction] = gSupport;
            if (gSupport.slope != 0xFF && gSupport.height != 0)
            {
                generalSupports[direction].height -= height;
            }

            // Get chain lift calls
            mapElement.type |= 0x80;
            PaintIntercept::ClearCalls();
            CallOriginal(trackType, direction, trackSequence, height, &mapElement);
            numCalls = PaintIntercept::GetCalls(callBuffer);
            chainLiftCalls[direction].insert(chainLiftCalls[direction].begin(), callBuffer, callBuffer + numCalls);

            // Get cable lift calls (giga coaster only)
            if (_rideType == RIDE_TYPE_GIGA_COASTER)
            {
                mapElement.type = 0;
                mapElement.properties.track.colour |= TRACK_ELEMENT_COLOUR_FLAG_CABLE_LIFT;
                PaintIntercept::ClearCalls();
                CallOriginal(trackType, direction, trackSequence, height, &mapElement);
                numCalls = PaintIntercept::GetCalls(callBuffer);
                cableLiftCalls[direction].insert(cableLiftCalls[direction].begin(), callBuffer, callBuffer + numCalls);
            }

            // Check a different position for direction 0 to see if supports are different
            if (direction == 0)
            {
                RCT2_GLOBAL(0x009DE56A, sint16) = 64 + 32;
                RCT2_GLOBAL(0x009DE56E, sint16) = 64;
                mapElement.type = 0;
                mapElement.properties.track.colour &= ~TRACK_ELEMENT_COLOUR_FLAG_CABLE_LIFT;
                PaintIntercept::ClearCalls();
                CallOriginal(trackType, direction, trackSequence, height, &mapElement);
                numCalls = PaintIntercept::GetCalls(callBuffer);
                std::vector<function_call> checkCalls = std::vector<function_call>(callBuffer, callBuffer + numCalls);
                if (!CompareFunctionCalls(checkCalls, calls[direction]))
                {
                    _conditionalSupports = true;
                }
            }

            GetTunnelCalls(trackType, direction, trackSequence, height, &mapElement, tileTunnelCalls, verticalTunnelHeights);
        }

        std::vector<function_call> supportCalls[4], chainLiftSupportCalls[4], cableLiftSupportCalls[4];
        if (blockSegmentsBeforeSupports) {
            ExtractMetalSupportCalls(calls, supportCalls);
            ExtractMetalSupportCalls(cableLiftCalls, cableLiftSupportCalls);
            ExtractMetalSupportCalls(chainLiftCalls, chainLiftSupportCalls);
        }

        if (_rideType == RIDE_TYPE_GIGA_COASTER && !CompareFunctionCalls(calls, cableLiftCalls))
        {
            WriteLine(tabs, "if (track_element_is_cable_lift(mapElement)) {");
            GenerateCalls(tabs + 1, cableLiftCalls, height);

            if (!CompareFunctionCalls(calls, chainLiftCalls))
            {
                WriteLine(tabs, "} else if (track_element_is_lift_hill(mapElement)) {");
                GenerateCalls(tabs + 1, chainLiftCalls, height);
            }

            WriteLine(tabs, "} else {");
            GenerateCalls(tabs + 1, calls, height);
            WriteLine(tabs, "}");
        }
        else if (!CompareFunctionCalls(calls, chainLiftCalls))
        {
            WriteLine(tabs, "if (track_element_is_lift_hill(mapElement)) {");
            GenerateCalls(tabs + 1, chainLiftCalls, height);
            WriteLine(tabs, "} else {");
            GenerateCalls(tabs + 1, calls, height);
            WriteLine(tabs, "}");
        }
        else
        {
            GenerateCalls(tabs, calls, height);
        }

        if (blockSegmentsBeforeSupports) {
            if (_rideType == RIDE_TYPE_GIGA_COASTER && !CompareFunctionCalls(supportCalls, cableLiftSupportCalls)) {
                printf("Error: Supports differ for cable lift.\n");
            } else if (!CompareFunctionCalls(supportCalls, chainLiftSupportCalls)) {
                printf("Error: Supports differ for chain lift\n");
            }
            WriteLine();
            GenerateSegmentSupportCall(tabs, segmentSupportCalls);

            bool conditionalSupports = _conditionalSupports;
            _conditionalSupports = false;
            if (conditionalSupports) {
                WriteLine(tabs, "if (track_paint_util_should_paint_supports(gPaintMapPosition)) {");
                tabs++;
            }
            GenerateCalls(tabs, supportCalls, height);
            if (conditionalSupports) {
                tabs--;
                WriteLine(tabs, "}");
            }
            WriteLine();
        }

        GenerateTunnelCall(tabs, tileTunnelCalls, verticalTunnelHeights);
        if (!blockSegmentsBeforeSupports) {
            GenerateSegmentSupportCall(tabs, segmentSupportCalls);
        }
        GenerateGeneralSupportCall(tabs, generalSupports);
    }

    void GenerateCalls(int tabs, std::vector<function_call> calls[4], int height)
    {
        std::vector<function_call> commonCalls = TrimCommonCallsEnd(calls);

        int totalCalls = 0;
        for (int direction = 0; direction < 4; direction++)
        {
            totalCalls += calls[direction].size();
        }
        if (totalCalls != 0)
        {
            WriteLine(tabs, "switch (direction) {");
            for (int direction = 0; direction < 4; direction++)
            {
                if (calls[direction].size() == 0) continue;

                WriteLine(tabs, "case %d:", direction);
                for (int d2 = direction + 1; d2 < 4; d2++)
                {
                    if (CompareFunctionCalls(calls[direction], calls[d2]))
                    {
                        // Clear identical other direction calls and add case for it
                        calls[d2].clear();
                        WriteLine(tabs, "case %d:", d2);
                    }
                }

                for (auto call : calls[direction])
                {
                    GenerateCalls(tabs + 1, call, height, direction);
                }
                WriteLine(tabs + 1, "break;");
            }
            WriteLine(tabs, "}");
        }

        for (auto call : commonCalls)
        {
            GenerateCalls(tabs, call, height, 0);
        }
    }

    void GenerateCalls(int tabs, const function_call &call, int height, int direction)
    {
        switch (call.function) {
        case PAINT_98196C:
        case PAINT_98197C:
        case PAINT_98198C:
        case PAINT_98199C:
            GeneratePaintCall(tabs, call, height, direction);
            break;
        case SUPPORTS_METAL_A:
        case SUPPORTS_METAL_B:
        {
            int callTabs = tabs;
            if (_conditionalSupports)
            {
                WriteLine(tabs, "if (track_paint_util_should_paint_supports(gPaintMapPosition)) {");
                callTabs++;
            }

            WriteLine(callTabs, "%s(%d, %d, %d, height%s, %s);",
                GetFunctionCallName(call.function),
                call.supports.type,
                call.supports.segment,
                call.supports.special,
                GetOffsetExpressionString(call.supports.height - height).c_str(),
                GetImageIdString(call.supports.colour_flags).c_str());

            if (_conditionalSupports)
            {
                WriteLine(tabs, "}");
            }
            break;
        }
        case SUPPORTS_WOOD_A:
        case SUPPORTS_WOOD_B:
            WriteLine(tabs, "%s(%d, %d, height%s, %s, NULL);",
                GetFunctionCallName(call.function),
                call.supports.type,
                call.supports.special,
                GetOffsetExpressionString(call.supports.height - height).c_str(),
                GetImageIdString(call.supports.colour_flags).c_str());
            break;
        }
    }

    void GeneratePaintCall(int tabs, const function_call &call, int height, int direction)
    {
        const char * funcName = GetFunctionCallName(call.function);
        std::string imageId = GetImageIdString(call.paint.image_id);
        std::string s = String::Format("%s_rotated(direction, %s, ", funcName, imageId.c_str());
        s += FormatXYSwap(call.paint.offset.x, call.paint.offset.y, direction);
        s += ", ";
        s += FormatXYSwap(call.paint.bound_box_length.x, call.paint.bound_box_length.y, direction);
        s += String::Format(", %d, height%s", call.paint.bound_box_length.z, GetOffsetExpressionString(call.paint.z_offset - height).c_str());

        if (call.function != PAINT_98196C)
        {
            s += ", ";
            s += FormatXYSwap(call.paint.bound_box_offset.x, call.paint.bound_box_offset.y, direction);
            s += String::Format(", height%s", GetOffsetExpressionString(call.paint.bound_box_offset.z - height).c_str());
        }

        s += ");";
        WriteLine(tabs, s);
    }

    std::string FormatXYSwap(sint16 x, sint16 y, int direction)
    {
        if (direction & 1)
        {
            return String::Format("%d, %d", y, x);
        }
        else
        {
            return String::Format("%d, %d", x, y);
        }
    }

    std::vector<function_call> TrimCommonCallsEnd(std::vector<function_call> calls[4])
    {
        std::vector<function_call> commonCalls;

        while (calls[0].size() != 0)
        {
            function_call lastCall = calls[0].back();
            for (int i = 0; i < 4; i++)
            {
                if (calls[i].size() == 0 || !CompareFunctionCall(calls[i].back(), lastCall))
                {
                    goto finished;
                }
            }
            for (int i = 0; i < 4; i++)
            {
                calls[i].pop_back();
            }
            commonCalls.push_back(lastCall);
        }

    finished:
        return commonCalls;
    }

    bool CompareFunctionCalls(const std::vector<function_call> a[4], const std::vector<function_call> b[4])
    {
        for (size_t i = 0; i < 4; i++)
        {
            if (!CompareFunctionCalls(a[i], b[i]))
            {
                return false;
            }
        }
        return true;
    }

    bool CompareFunctionCalls(const std::vector<function_call> &a, const std::vector<function_call> &b)
    {
        if (a.size() != b.size()) return false;
        for (size_t i = 0; i < a.size(); i++)
        {
            if (!CompareFunctionCall(a[i], b[i]))
            {
                return false;
            }
        }
        return true;
    }

    bool CompareFunctionCall(const function_call a, const function_call &b)
    {
        return FunctionCall::AssertsEquals(a, b);
    }

    const char * GetFunctionCallName(int function)
    {
        const char * functionNames[] = {
            "sub_98196C",
            "sub_98197C",
            "sub_98198C",
            "sub_98199C",
            "metal_a_supports_paint_setup",
            "metal_b_supports_paint_setup",
            "wooden_a_supports_paint_setup",
            "wooden_b_supports_paint_setup",
        };
        return functionNames[function];
    }

    bool GetTunnelCalls(int trackType,
                        int direction,
                        int trackSequence,
                        int height,
                        rct_map_element * mapElement,
                        TunnelCall tileTunnelCalls[4][4],
                        sint16 verticalTunnelHeights[4])
    {
        TestPaint::ResetTunnels();

        for (int offset = -8; offset <= 8; offset += 8)
        {
            CallOriginal(trackType, direction, trackSequence, height + offset, mapElement);
        }

        uint8 rightIndex = (4 - direction) % 4;
        uint8 leftIndex = (rightIndex + 1) % 4;

        for (int i = 0; i < 4; ++i) {
            tileTunnelCalls[direction][i].call = TUNNELCALL_SKIPPED;
        }
        if (gRightTunnelCount == 0) {
            tileTunnelCalls[direction][rightIndex].call = TUNNELCALL_NONE;
        } else if (gRightTunnelCount == 3) {
            tileTunnelCalls[direction][rightIndex].call = TUNNELCALL_CALL;
            tileTunnelCalls[direction][rightIndex].offset = SideTunnelCall::GetTunnelOffset(height, gRightTunnels);
            tileTunnelCalls[direction][rightIndex].type = gRightTunnels[0].type;
        } else {
            printf("Multiple tunnels on one side aren't supported.\n");
            return false;
        }

        if (gLeftTunnelCount == 0) {
            tileTunnelCalls[direction][leftIndex].call = TUNNELCALL_NONE;
        } else if (gLeftTunnelCount == 3) {
            tileTunnelCalls[direction][leftIndex].call = TUNNELCALL_CALL;
            tileTunnelCalls[direction][leftIndex].offset = SideTunnelCall::GetTunnelOffset(height, gLeftTunnels);
            tileTunnelCalls[direction][leftIndex].type = gLeftTunnels[0].type;
        } else {
            printf("Multiple tunnels on one side aren't supported.\n");
            return false;
        }

        // Vertical tunnel
        gVerticalTunnelHeight = 0;
        CallOriginal(trackType, direction, trackSequence, height, mapElement);

        int verticalTunnelHeight = gVerticalTunnelHeight;
        if (verticalTunnelHeight != 0)
        {
            verticalTunnelHeight = (verticalTunnelHeight * 16) - height;
        }
        verticalTunnelHeights[direction] = verticalTunnelHeight;
        return true;
    }

    void GenerateTunnelCall(int tabs, TunnelCall tileTunnelCalls[4][4], sint16 verticalTunnelHeights[4])
    {
        constexpr uint8 TunnelLeft = 0;
        constexpr uint8 TunnelRight = 1;
        constexpr uint8 TunnelNA = 255;
        static const uint8 dsToWay[4][4] =
        {
            { TunnelRight, TunnelLeft, TunnelNA, TunnelNA },
            { TunnelLeft, TunnelNA, TunnelNA, TunnelRight },
            { TunnelNA, TunnelNA, TunnelRight, TunnelLeft },
            { TunnelNA, TunnelRight, TunnelLeft, TunnelNA },
        };


        sint16 tunnelOffset[4] = { 0 };
        uint8 tunnelType[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
        for (int direction = 0; direction < 4; direction++)
        {
            for (int side = 0; side < 4; side++)
            {
                auto tunnel = tileTunnelCalls[direction][side];
                if (tunnel.call == TUNNELCALL_CALL)
                {
                    tunnelOffset[direction] = tunnel.offset;
                    tunnelType[direction] = tunnel.type;
                    break;
                }
            }
        }

        if (AllMatch(tunnelOffset, 4) && AllMatch(tunnelType, 4))
        {
            if (tunnelType[0] != 0xFF)
            {
                GenerateTunnelCall(tabs, tunnelOffset[0], tunnelType[0]);
            }
        }
        else if (tunnelOffset[0] == tunnelOffset[3] && tunnelType[0] == tunnelType[3] &&
                 tunnelOffset[1] == tunnelOffset[2] && tunnelType[1] == tunnelType[2] &&
                 tunnelType[0] != 0xFF)
        {
            if (tunnelType[0] != 0xFF)
            {
                WriteLine(tabs, "if (direction == 0 || direction == 3) {");
                GenerateTunnelCall(tabs + 1, tunnelOffset[0], tunnelType[0]);
                if (tunnelType[1] != 0xFF)
                {
                    WriteLine(tabs, "} else {");
                    GenerateTunnelCall(tabs + 1, tunnelOffset[1], tunnelType[1]);
                }
                WriteLine(tabs, "}");
            }
            else
            {
                WriteLine(tabs, "if (direction == 1 || direction == 2) {");
                GenerateTunnelCall(tabs + 1, tunnelOffset[1], tunnelType[1]);
                WriteLine(tabs, "}");
            }
        }
        else
        {
            WriteLine(tabs, "switch (direction) {");
            for (int i = 0; i < 4; i++)
            {
                if (tunnelType[i] != 0xFF)
                {
                    WriteLine(tabs, "case %d:", i);
                    for (int side = 0; side < 4; side++)
                    {
                        if (tileTunnelCalls[i][side].call == TUNNELCALL_CALL)
                        {
                            GenerateTunnelCall(tabs + 1, tileTunnelCalls[i][side].offset, tileTunnelCalls[i][side].type, dsToWay[i][side]);
                        }
                    }
                    WriteLine(tabs + 1, "break;");
                }
            }
            WriteLine(tabs, "}");
        }

        if (AllMatch(verticalTunnelHeights, 4))
        {
            int tunnelHeight = verticalTunnelHeights[0];
            if (tunnelHeight != 0)
            {
                WriteLine(tabs, "paint_util_set_vertical_tunnel(session, height%s);", GetOffsetExpressionString(tunnelHeight).c_str());
            }
        }
    }

    void GenerateTunnelCall(int tabs, int offset, int type, int way)
    {
        switch (way) {
        case 0:
            WriteLine(tabs, "paint_util_push_tunnel_left(session, height%s, TUNNEL_%d);", GetOffsetExpressionString(offset).c_str(), type);
            break;
        case 1:
            WriteLine(tabs, "paint_util_push_tunnel_right(session, height%s, TUNNEL_%d);", GetOffsetExpressionString(offset).c_str(), type);
            break;
        }
    }

    void GenerateTunnelCall(int tabs, int offset, int type)
    {
        WriteLine(tabs, "paint_util_push_tunnel_rotated(session, direction, height%s, TUNNEL_%d);", GetOffsetExpressionString(offset).c_str(), type);
    }

    void GenerateSegmentSupportCall(int tabs, std::vector<SegmentSupportCall> segmentSupportCalls[4])
    {
        for (size_t i = 0; i < segmentSupportCalls[0].size(); i++)
        {
            auto ssh = segmentSupportCalls[0][i];
            std::string szCall = "paint_util_set_segment_support_height(session, ";
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
                szCall += String::Format(", 0);", ssh.slope);
            }
            else
            {
                szCall += std::to_string(ssh.height);
                szCall += String::Format(", 0x%02X);", ssh.slope);
            }
            WriteLine(tabs, szCall);
        }
    }

    void GenerateGeneralSupportCall(int tabs, support_height generalSupports[4])
    {
        if (generalSupports[0].height == 0 &&
            generalSupports[0].slope == 0xFF)
        {
            return;
        }

        WriteLine(tabs, "paint_util_set_general_support_height(session, height%s, 0x%02X);",
            GetOffsetExpressionString((sint16)generalSupports[0].height).c_str(),
            generalSupports[0].slope);
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
        if (palette == TestPaint::DEFAULT_SCHEME_TRACK) paletteName = "gTrackColours[SCHEME_TRACK]";
        else if (palette == TestPaint::DEFAULT_SCHEME_SUPPORTS) paletteName = "gTrackColours[SCHEME_SUPPORTS]";
        else if (palette == TestPaint::DEFAULT_SCHEME_MISC) paletteName = "gTrackColours[SCHEME_MISC]";
        else if (palette == TestPaint::DEFAULT_SCHEME_3) paletteName = "gTrackColours[SCHEME_3]";
        else {
            paletteName = String::Format("0x%08X", palette);
        }

        if (image == 0) {
            result = paletteName;
        } else if (image & 0x70000) {
            result = String::Format("%s | vehicle.base_image_id + %d", paletteName.c_str(), image & ~0x70000);
        } else {
            result = String::Format("%s | %d", paletteName.c_str(), image);
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
                s += String::Format("SEGMENT_%02X", 0xB4 + 4 * i);
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
        TestPaint::ResetEnvironment();
        TestPaint::ResetSupportHeights();

        uint32 *trackDirectionList = (uint32 *)RideTypeTrackPaintFunctionsOld[_rideType][trackType];
        // Have to call from this point as it pushes esi and expects callee to pop it
        RCT2_CALLPROC_X(
            0x006C4934,
            _rideType,
            (int) trackDirectionList,
            direction,
            height,
            (int)mapElement,
            0 * sizeof(Ride),
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
            if (trackType == TRACK_ELEM_END_STATION) {
                WriteLine(1, "case " + std::string(TrackElemNames[TRACK_ELEM_END_STATION]) + ":");
                WriteLine(1, "case " + std::string(TrackElemNames[TRACK_ELEM_BEGIN_STATION]) + ":");
                WriteLine(1, "case " + std::string(TrackElemNames[TRACK_ELEM_MIDDLE_STATION]) + ":");
                WriteLine(2, "return %s_track_station;", _rideName.c_str());
                continue;
            }

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
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);

        WriteLine(tabs, std::string(buffer));
    }

    void WriteLine(int tabs, std::string s)
    {
        for (int i = 0; i < tabs; i++)
        {
            fprintf(_file, "\t");
        }
        fprintf(_file, "%s\n", s.c_str());
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
