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

extern "C"
{
    #include "data.h"
    #include "intercept.h"
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
        std::string trackName = TrackNames[trackType];
        int numSequences = getTrackSequenceCount(_rideType, trackType);
        int height = 48;

        WriteLine(0, "static void " + _rideName + "_track_" + trackName + "(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)");
        WriteLine(0, "{");

        support_height generalSupports[4] = { 0 };

        for (int direction = 0; direction < 4; direction++) {
            int trackSequence = 0;

            rct_map_element mapElement = { 0 };
            mapElement.flags |= MAP_ELEMENT_FLAG_LAST_TILE;
            mapElement.properties.track.type = trackType;
            mapElement.base_height = 3;
            g_currently_drawn_item = &mapElement;

            CallOriginal(trackType, direction, trackSequence, height, &mapElement);

            generalSupports[direction] = gSupport;
        }

        if (AllMatch(generalSupports, 4))
        {
            WriteLine(1, "paint_util_set_general_support_height(height + %d, 0x%02X);", generalSupports[0].height - height, generalSupports[0].slope);
        }
        else
        {
            WriteLine(1, "#error Unsupported: different directional general supports");
        }

        WriteLine(0, "}");
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
        constexpr uint32 DEFAULT_SCHEME_TRACK = COLOUR_GREY << 19 | COLOUR_WHITE << 24 | 0xA0000000;
        constexpr uint32 DEFAULT_SCHEME_SUPPORTS = COLOUR_LIGHT_BLUE << 19 | COLOUR_ICY_BLUE << 24 | 0xA0000000;
        constexpr uint32 DEFAULT_SCHEME_MISC = COLOUR_DARK_PURPLE << 19 | COLOUR_LIGHT_PURPLE << 24 | 0xA0000000;
        constexpr uint32 DEFAULT_SCHEME_3 = COLOUR_BRIGHT_PURPLE << 19 | COLOUR_DARK_BLUE << 24 | 0xA0000000;

        gPaintInteractionType = VIEWPORT_INTERACTION_ITEM_RIDE;
        gTrackColours[SCHEME_TRACK] = DEFAULT_SCHEME_TRACK;
        gTrackColours[SCHEME_SUPPORTS] = DEFAULT_SCHEME_SUPPORTS;
        gTrackColours[SCHEME_MISC] = DEFAULT_SCHEME_MISC;
        gTrackColours[SCHEME_3] = DEFAULT_SCHEME_3;

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
                WriteLine(2, "return NULL;");
            }
        }
        WriteLine(1, "}");
        WriteLine(1, "return NULL;");
        WriteLine(0, "}");
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
