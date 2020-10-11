/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma warning(disable : 4706) // assignment within conditional expression

#include "CmdlineTrack.h"

#include "Context.h"
#include "OpenRCT2.h"
#include "core/Imaging.h"
#include "core/Json.hpp"
#include "drawing/Drawing.h"
#include "drawing/ImageImporter.h"
#include "object/ObjectLimits.h"
#include "object/ObjectManager.h"
#include "object/ObjectRepository.h"
#include "platform/platform.h"
#include "ride/RideData.h"
#include "ride/TrackDesign.h"
#include "util/Util.h"
#include "world/Entrance.h"
#include "world/Scenery.h"

#include <cassert>
#include <cmath>
#include <cstring>

#ifdef _WIN32
#    include "core/String.hpp"
#endif

enum class TrackSubcommand
{
    None,
    Info,

};

static TrackSubcommand track_subcommand_from_str(const char* s)
{
    if (_strcmpi(s, "info") == 0)
        return TrackSubcommand::Info;
    return TrackSubcommand::None;
}

static bool needs_context(TrackSubcommand c)
{
    return true;
}

static json_t track_to_json(std::unique_ptr<TrackDesign> design)
{
    // Logic from window_track_list_paint
    json_t j;
    j["name"] = design->name;
    j["type"] = design->type;
    j["vehicle_type"] = design->vehicle_type;
    j["excitement"] = design->excitement / 10.0;
    j["intensity"] = design->intensity / 10.0;
    j["nausea"] = design->nausea / 10.0;

    if (ride_type_has_flag(design->type, RIDE_TYPE_FLAG_HAS_TRACK))
    {
        if (design->type != RIDE_TYPE_MAZE)
        {
            if (design->type == RIDE_TYPE_MINI_GOLF)
            {
                j["holes"] = design->holes & 0x1F;
            }
            else
            {
                uint16_t speed = ((design->max_speed << 16) * 9) >> 18;
                j["max_speed"] = speed;
                speed = ((design->average_speed << 16) * 9) >> 18;
                j["average_speed"] = speed;
            }
            j["ride_length"] = design->ride_length;
            if (ride_type_has_flag(design->type, RIDE_TYPE_FLAG_HAS_G_FORCES))
            {
                int32_t gForces = design->max_positive_vertical_g * 32;
                j["max_positive_vertical_g"] = gForces / 100.0;
                gForces = design->max_negative_vertical_g * 32;
                j["max_negative_vertical_g"] = gForces / 100.0;
                gForces = design->max_lateral_g * 32;
                j["max_lateral_g"] = gForces / 100.0;
                if (design->total_air_time != 0)
                {
                    int32_t airTime = design->total_air_time * 25;
                    j["total_air_time"] = airTime;
                }
            }
            if (ride_type_has_flag(design->type, RIDE_TYPE_FLAG_HAS_DROPS))
            {
                j["drops"] = design->drops & 0x3F;
                uint16_t highestDropHeight = (design->highest_drop_height * 3) / 4;
                j["highest_drop_height"] = highestDropHeight;
            }
            if (design->type != RIDE_TYPE_MINI_GOLF)
            {
                uint16_t inversions = design->inversions & 0x1F;
                if (inversions != 0)
                {
                    j["inversions"] = inversions;
                }
            }
        }
    }
    if (design->space_required_x != 0xFF)
    {
        j["space_required"] = { design->space_required_x, design->space_required_y };
    }
    if (design->cost != 0)
    {
        j["cost"] = design->cost;
    }
    return j;
}

int32_t cmdline_for_track(const char** argv, int32_t argc)
{
    gOpenRCT2Headless = true;
    if (argc == 0)
        return -1;

    TrackSubcommand subcommand = track_subcommand_from_str(argv[0]);
    if (subcommand == TrackSubcommand::None)
    {
        return -1;
    }

    std::unique_ptr<OpenRCT2::IContext> context;
    if (needs_context(subcommand))
    {
        context = OpenRCT2::CreateContext();
        context->Initialise();
    }

    switch (subcommand)
    {
        case TrackSubcommand::Info:
            for (int32_t i = 1; i < argc; ++i)
            {
                const char* path = argv[i];
                std::unique_ptr<TrackDesign> design = track_design_open(path);
                if (!design)
                {
                    fprintf(stderr, "Failed to load track %s\n", path);
                    continue;
                }
                json_t info = track_to_json(std::move(design));
                info["path"] = path;
                puts(info.dump().c_str());
            }

            break;
        default:
            // -Wswitch can't tell that we handled TrackSubcommand::None above
            assert(false);
    };

    return 0;
}
