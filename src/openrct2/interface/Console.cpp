#pragma region Copyright (c) 2014-2018 OpenRCT2 Developers
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
#include <cmath>
#include <cstdarg>
#include <deque>
#include <string>
#include <thread>

#include "../thirdparty/linenoise.hpp"

#include "../config/Config.h"
#include "../Context.h"
#include "../core/Guard.hpp"
#include "../core/Math.hpp"
#include "../core/String.hpp"
#include "../drawing/Drawing.h"
#include "../drawing/Font.h"
#include "../EditorObjectSelectionSession.h"
#include "../Game.h"
#include "../interface/Colour.h"
#include "../interface/themes.h"
#include "../localisation/Localisation.h"
#include "../localisation/User.h"
#include "../management/Finance.h"
#include "../management/Research.h"
#include "../network/network.h"
#include "../object/Object.h"
#include "../object/ObjectList.h"
#include "../object/ObjectManager.h"
#include "../object/ObjectRepository.h"
#include "../OpenRCT2.h"
#include "../peep/Staff.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../util/Util.h"
#include "../Version.h"
#include "../windows/Intent.h"
#include "../world/Climate.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/Sprite.h"
#include "Console.h"
#include "Viewport.h"

#ifndef NO_TTF
#include "../drawing/TTF.h"

#endif

#define CONSOLE_MAX_LINES 300
#define CONSOLE_HISTORY_SIZE 64
#define CONSOLE_INPUT_SIZE 256
#define CONSOLE_CARET_FLASH_THRESHOLD 15
#define CONSOLE_EDGE_PADDING 4
#define CONSOLE_CARET_WIDTH 6

bool gConsoleOpen = false;

static sint32                   _consoleLeft, _consoleTop, _consoleRight, _consoleBottom;
static sint32                   _lastMainViewportX, _lastMainViewportY;
static std::deque<std::string>  _consoleLines;
static utf8                     _consoleCurrentLine[CONSOLE_INPUT_SIZE];
static sint32                   _consoleCaretTicks;
static sint32                   _consoleScrollPos = 0;
static TextInputSession *       _consoleTextInputSession;

static utf8   _consoleHistory[CONSOLE_HISTORY_SIZE][CONSOLE_INPUT_SIZE];
static sint32 _consoleHistoryIndex = 0;
static sint32 _consoleHistoryCount = 0;

static InGameConsole _inGameConsole;

static void console_write_all_commands(InteractiveConsole &console);
static sint32 console_parse_int(const utf8 *src, bool *valid);
static double console_parse_double(const utf8 *src, bool *valid);

static sint32 cc_variables(InteractiveConsole &console, const utf8 **argv, sint32 argc);
static sint32 cc_windows(InteractiveConsole &console, const utf8 **argv, sint32 argc);
static sint32 cc_help(InteractiveConsole &console, const utf8 **argv, sint32 argc);

static bool invalidArguments(bool *invalid, bool arguments);

static sint32 console_get_num_visible_lines();

#define SET_FLAG(variable, flag, value) {if (value) variable |= flag; else variable &= ~flag;}

void console_open()
{
    _inGameConsole.Open();
}

void console_close()
{
    _inGameConsole.Close();
}

void console_toggle()
{
    _inGameConsole.Toggle();
}

void console_update()
{
    _inGameConsole.Update();
}

void console_draw(rct_drawpixelinfo *dpi)
{
    _inGameConsole.Draw(dpi);
}

void console_input(CONSOLE_INPUT input)
{
    _inGameConsole.Input(input);
}

void console_writeline(const utf8 * src, uint32 colourFormat)
{
    _inGameConsole.WriteLine(src, colourFormat);
}

void console_scroll(sint32 linesToScroll)
{
    _inGameConsole.Scroll(linesToScroll);
}

sint32 console_parse_int(const utf8 *src, bool *valid) {
    utf8 *end;
    sint32 value;
    value = strtol(src, &end, 10); *valid = (*end == '\0');
    return value;
}

double console_parse_double(const utf8 *src, bool *valid) {
    utf8 *end;
    double value;
    value = strtod(src, &end); *valid = (*end == '\0');
    return value;
}

// Calculates the amount of visible lines, based on the console size, excluding the input line.
static sint32 console_get_num_visible_lines()
{
    const sint32 lineHeight     = font_get_line_height(gCurrentFontSpriteBase);
    const sint32 consoleHeight  = _consoleBottom - _consoleTop;
    const sint32 drawableHeight = consoleHeight - 2 * lineHeight - 4; // input line, separator - padding
    return drawableHeight / lineHeight;
}

void console_refresh_caret()
{
    _inGameConsole.RefreshCaret();
}

static sint32 cc_clear(InteractiveConsole &console, const utf8 **argv, sint32 argc)
{
    console.Clear();
    return 0;
}

static sint32 cc_hide(InteractiveConsole &console, const utf8 **argv, sint32 argc)
{
    console_close();
    return 0;
}

static sint32 cc_echo(InteractiveConsole &console, const utf8 **argv, sint32 argc)
{
    if (argc > 0)
        console.WriteLine(argv[0]);
    return 0;
}

static sint32 cc_rides(InteractiveConsole &console, const utf8 **argv, sint32 argc)
{
    if (argc > 0) {
        if (strcmp(argv[0], "list") == 0) {
            Ride *ride;
            sint32 i;
            FOR_ALL_RIDES(i, ride) {
                char name[128];
                format_string(name, 128, ride->name, &ride->name_arguments);
                console.WriteFormatLine("ride: %03d type: %02u subtype %03u operating mode: %02u name: %s", i, ride->type, ride->subtype, ride->mode, name);
            }
        } else if (strcmp(argv[0], "set") == 0) {
            if (argc < 4) {
                if (argc > 1 && strcmp(argv[1], "mode") == 0){
                    console.WriteFormatLine("Ride modes are specified using integer IDs as given below:");
                    for (sint32 i = 0; i < RIDE_MODE_COUNT; i++) {
                        char mode_name[128] = { 0 };
                        rct_string_id mode_string_id = RideModeNames[i];
                        format_string(mode_name, 128, mode_string_id, nullptr);
                        console.WriteFormatLine("%02d - %s", i, mode_name);
                    }

                } else {
                    console.WriteFormatLine("rides set type <ride id> <ride type>");
                    console.WriteFormatLine("rides set mode [<ride id> <operating mode>]");
                    console.WriteFormatLine("rides set mass <ride id> <mass value>");
                    console.WriteFormatLine("rides set excitement <ride id> <excitement value>");
                    console.WriteFormatLine("rides set intensity <ride id> <intensity value>");
                    console.WriteFormatLine("rides set nausea <ride id> <nausea value>");
                }
                return 0;
            }
            if (strcmp(argv[1], "type") == 0) {
                bool int_valid[2] = { 0 };
                sint32 ride_index = console_parse_int(argv[2], &int_valid[0]);
                sint32 type = console_parse_int(argv[3], &int_valid[1]);
                if (!int_valid[0] || !int_valid[1]) {
                    console.WriteFormatLine("This command expects integer arguments");
                } else if (ride_index < 0) {
                    console.WriteFormatLine("Ride index must not be negative");
                } else {
                    gGameCommandErrorTitle = STR_CANT_CHANGE_OPERATING_MODE;
                    sint32 res = game_do_command(0, (type << 8) | 1, 0, (RIDE_SETTING_RIDE_TYPE << 8) | ride_index, GAME_COMMAND_SET_RIDE_SETTING, 0, 0);
                    if (res == MONEY32_UNDEFINED) {
                        console.WriteFormatLine("That didn't work");
                    }
                }
            }
            else if (strcmp(argv[1], "mode") == 0) {
                bool int_valid[2] = { 0 };
                sint32 ride_index = console_parse_int(argv[2], &int_valid[0]);
                sint32 mode = console_parse_int(argv[3], &int_valid[1]);
                if (!int_valid[0] || !int_valid[1]) {
                    console.WriteFormatLine("This command expects integer arguments");
                } else if (ride_index < 0) {
                    console.WriteFormatLine("Ride index must not be negative");
                } else {
                    Ride *ride = get_ride(ride_index);
                    if (mode <= 0 || mode > (RIDE_MODE_COUNT - 1)) {
                        console.WriteFormatLine("Invalid ride mode.");
                    }
                    else if (ride == nullptr || ride->type == RIDE_TYPE_NULL) {
                        console.WriteFormatLine("No ride found with index %d", ride_index);
                    }
                    else {
                        ride->mode = mode;
                        invalidate_test_results(ride_index);
                    }
                }
            }
            else if (strcmp(argv[1], "mass") == 0) {
                bool int_valid[2] = { 0 };
                sint32 ride_index = console_parse_int(argv[2], &int_valid[0]);
                sint32 mass = console_parse_int(argv[3], &int_valid[1]);

                if (ride_index < 0) {
                    console.WriteFormatLine("Ride index must not be negative");
                }
                else if (!int_valid[0] || !int_valid[1]) {
                    console.WriteFormatLine("This command expects integer arguments");
                }
                else {
                    Ride *ride = get_ride(ride_index);
                    if (mass <= 0) {
                        console.WriteFormatLine("Friction value must be strictly positive");
                    }
                    else if (ride->type == RIDE_TYPE_NULL) {
                        console.WriteFormatLine("No ride found with index %d", ride_index);
                    }
                    else {
                        for (sint32 i = 0; i < ride->num_vehicles; i++) {
                            uint16 vehicle_index = ride->vehicles[i];
                            while (vehicle_index != SPRITE_INDEX_NULL) {
                                rct_vehicle *vehicle = GET_VEHICLE(vehicle_index);
                                vehicle->mass = mass;
                                vehicle_index = vehicle->next_vehicle_on_train;
                            }
                        }
                    }
                }
            }
            else if (strcmp(argv[1], "excitement") == 0) {
                bool int_valid[2] = { 0 };
                sint32 ride_index = console_parse_int(argv[2], &int_valid[0]);
                ride_rating excitement = console_parse_int(argv[3], &int_valid[1]);

                if (ride_index < 0) {
                    console.WriteFormatLine("Ride index must not be negative");
                }
                else if (!int_valid[0] || !int_valid[1]) {
                    console.WriteFormatLine("This command expects integer arguments");
                }
                else {
                    Ride *ride = get_ride(ride_index);
                    if (excitement <= 0) {
                        console.WriteFormatLine("Excitement value must be strictly positive");
                    }
                    else if (ride->type == RIDE_TYPE_NULL) {
                        console.WriteFormatLine("No ride found with index %d", ride_index);
                    }
                    else {
                        ride->excitement = excitement;
                    }
                }
            }
            else if (strcmp(argv[1], "intensity") == 0) {
                bool int_valid[2] = { 0 };
                sint32 ride_index = console_parse_int(argv[2], &int_valid[0]);
                ride_rating intensity = console_parse_int(argv[3], &int_valid[1]);

                if (ride_index < 0) {
                    console.WriteFormatLine("Ride index must not be negative");
                }
                else if (!int_valid[0] || !int_valid[1]) {
                    console.WriteFormatLine("This command expects integer arguments");
                }
                else {
                    Ride *ride = get_ride(ride_index);
                    if (intensity <= 0) {
                        console.WriteFormatLine("Intensity value must be strictly positive");
                    }
                    else if (ride->type == RIDE_TYPE_NULL) {
                        console.WriteFormatLine("No ride found with index %d", ride_index);
                    }
                    else {
                        ride->intensity = intensity;
                    }
                }
            }
            else if (strcmp(argv[1], "nausea") == 0) {
                bool int_valid[2] = { 0 };
                sint32 ride_index = console_parse_int(argv[2], &int_valid[0]);
                ride_rating nausea = console_parse_int(argv[3], &int_valid[1]);

                if (ride_index < 0) {
                    console.WriteFormatLine("Ride index must not be negative");
                }
                else if (!int_valid[0] || !int_valid[1]) {
                    console.WriteFormatLine("This command expects integer arguments");
                }
                else {
                    Ride *ride = get_ride(ride_index);
                    if (nausea <= 0) {
                        console.WriteFormatLine("Nausea value must be strictly positive");
                    }
                    else if (ride->type == RIDE_TYPE_NULL) {
                        console.WriteFormatLine("No ride found with index %d", ride_index);
                    }
                    else {
                        ride->nausea = nausea;
                    }
                }
            }
        }
    } else {
        console.WriteFormatLine("subcommands: list, set");
    }
    return 0;
}

static sint32 cc_staff(InteractiveConsole &console, const utf8 **argv, sint32 argc)
{
    if (argc > 0) {
        if (strcmp(argv[0], "list") == 0) {
            rct_peep *peep;
            sint32 i;
            FOR_ALL_STAFF(i, peep) {
                char name[128];
                format_string(name, 128, peep->name_string_idx, &peep->id);
                console.WriteFormatLine("staff id %03d type: %02u energy %03u name %s", i, peep->staff_type, peep->energy, name);
            }
        } else if (strcmp(argv[0], "set") == 0) {
            if (argc < 4) {
                console.WriteFormatLine("staff set energy <staff id> <value 0-255>");
                console.WriteFormatLine("staff set costume <staff id> <costume id>");
                for (sint32 i = 0; i < ENTERTAINER_COSTUME_COUNT; i++) {
                    char costume_name[128] = { 0 };
                    rct_string_id costume = StaffCostumeNames[i];
                    format_string(costume_name, 128, STR_DROPDOWN_MENU_LABEL, &costume);
                    // That's a terrible hack here. Costume names include inline sprites
                    // that don't work well with the console, so manually skip past them.
                    console.WriteFormatLine("        costume %i: %s", i, costume_name + 7);
                }
                return 0;
            }
            if (strcmp(argv[1], "energy") == 0) {
                sint32 int_val[3];
                bool int_valid[3] = { 0 };
                int_val[0] = console_parse_int(argv[2], &int_valid[0]);
                int_val[1] = console_parse_int(argv[3], &int_valid[1]);

                if (int_valid[0] && int_valid[1] && ((GET_PEEP(int_val[0])) != nullptr)) {
                    rct_peep *peep = GET_PEEP(int_val[0]);

                    peep->energy = int_val[1];
                    peep->energy_target = int_val[1];
                }
            } else if (strcmp(argv[1], "costume") == 0) {
                sint32 int_val[2];
                bool int_valid[2] = { 0 };
                int_val[0] = console_parse_int(argv[2], &int_valid[0]);
                int_val[1] = console_parse_int(argv[3], &int_valid[1]);
                rct_peep *peep = nullptr;
                if (!int_valid[0]) {
                    console.WriteLineError("Invalid staff ID");
                    return 1;
                }
                peep = GET_PEEP(int_val[0]);
                bool is_entertainer = peep != nullptr && peep->type == PEEP_TYPE_STAFF && peep->staff_type == STAFF_TYPE_ENTERTAINER;
                if (!is_entertainer) {
                    console.WriteLineError("Specified staff is not entertainer");
                    return 1;
                }
                if (!int_valid[1] || int_val[1] < 0 || int_val[1] >= ENTERTAINER_COSTUME_COUNT) {
                    console.WriteLineError("Invalid costume ID");
                    return 1;
                }

                sint32 costume = int_val[1] | 0x80;
                game_do_command(peep->x, (costume << 8) | 1, peep->y, int_val[0], GAME_COMMAND_SET_STAFF_ORDER, 0, 0);
            }
        }
    } else {
        console.WriteFormatLine("subcommands: list, set");
    }
    return 0;
}

static sint32 cc_get(InteractiveConsole &console, const utf8 **argv, sint32 argc)
{
    if (argc > 0) {
        if (strcmp(argv[0], "park_rating") == 0) {
            console.WriteFormatLine("park_rating %d", gParkRating);
        }
        else if (strcmp(argv[0], "park_value") == 0) {
            console.WriteFormatLine("park_value %d", gParkValue / 10);
        }
        else if (strcmp(argv[0], "company_value") == 0) {
            console.WriteFormatLine("company_value %d", gCompanyValue / 10);
        }
        else if (strcmp(argv[0], "money") == 0) {
            console.WriteFormatLine("money %d.%d0", gCash / 10, gCash % 10);
        }
        else if (strcmp(argv[0], "scenario_initial_cash") == 0) {
            console.WriteFormatLine("scenario_initial_cash %d", gInitialCash / 10);
        }
        else if (strcmp(argv[0], "current_loan") == 0) {
            console.WriteFormatLine("current_loan %d", gBankLoan / 10);
        }
        else if (strcmp(argv[0], "max_loan") == 0) {
            console.WriteFormatLine("max_loan %d", gMaxBankLoan / 10);
        }
        else if (strcmp(argv[0], "guest_initial_cash") == 0) {
            console.WriteFormatLine("guest_initial_cash %d.%d0", gGuestInitialCash / 10, gGuestInitialCash % 10);
        }
        else if (strcmp(argv[0], "guest_initial_happiness") == 0) {
            uint32 current_happiness = gGuestInitialHappiness;
            for (sint32 i = 15; i <= 99; i++) {
                if (i == 99) {
                    console.WriteFormatLine("guest_initial_happiness %d%%  (%d)", 15, gGuestInitialHappiness);
                }
                else if (current_happiness == calculate_guest_initial_happiness(i)) {
                    console.WriteFormatLine("guest_initial_happiness %d%%  (%d)", i, gGuestInitialHappiness);
                    break;
                }
            }
        }
        else if (strcmp(argv[0], "guest_initial_hunger") == 0) {
            console.WriteFormatLine("guest_initial_hunger %d%%  (%d)", ((255 - gGuestInitialHunger) * 100) / 255, gGuestInitialHunger);
        }
        else if (strcmp(argv[0], "guest_initial_thirst") == 0) {
            console.WriteFormatLine("guest_initial_thirst %d%%  (%d)", ((255 - gGuestInitialThirst) * 100) / 255, gGuestInitialThirst);
        }
        else if (strcmp(argv[0], "guest_prefer_less_intense_rides") == 0) {
            console.WriteFormatLine("guest_prefer_less_intense_rides %d", (gParkFlags & PARK_FLAGS_PREF_LESS_INTENSE_RIDES) != 0);
        }
        else if (strcmp(argv[0], "guest_prefer_more_intense_rides") == 0) {
            console.WriteFormatLine("guest_prefer_more_intense_rides %d", (gParkFlags & PARK_FLAGS_PREF_MORE_INTENSE_RIDES) != 0);
        }
        else if (strcmp(argv[0], "forbid_marketing_campagns") == 0) {
            console.WriteFormatLine("forbid_marketing_campagns %d", (gParkFlags & PARK_FLAGS_FORBID_MARKETING_CAMPAIGN) != 0);
        }
        else if (strcmp(argv[0], "forbid_landscape_changes") == 0) {
            console.WriteFormatLine("forbid_landscape_changes %d", (gParkFlags & PARK_FLAGS_FORBID_LANDSCAPE_CHANGES) != 0);
        }
        else if (strcmp(argv[0], "forbid_tree_removal") == 0) {
            console.WriteFormatLine("forbid_tree_removal %d", (gParkFlags & PARK_FLAGS_FORBID_TREE_REMOVAL) != 0);
        }
        else if (strcmp(argv[0], "forbid_high_construction") == 0) {
            console.WriteFormatLine("forbid_high_construction %d", (gParkFlags & PARK_FLAGS_FORBID_HIGH_CONSTRUCTION) != 0);
        }
        else if (strcmp(argv[0], "pay_for_rides") == 0) {
            console.WriteFormatLine("pay_for_rides %d", (gParkFlags & PARK_FLAGS_PARK_FREE_ENTRY) != 0);
        }
        else if (strcmp(argv[0], "no_money") == 0) {
            console.WriteFormatLine("no_money %d", (gParkFlags & PARK_FLAGS_NO_MONEY) != 0);
        }
        else if (strcmp(argv[0], "difficult_park_rating") == 0) {
            console.WriteFormatLine("difficult_park_rating %d", (gParkFlags & PARK_FLAGS_DIFFICULT_PARK_RATING) != 0);
        }
        else if (strcmp(argv[0], "difficult_guest_generation") == 0) {
            console.WriteFormatLine("difficult_guest_generation %d", (gParkFlags & PARK_FLAGS_DIFFICULT_GUEST_GENERATION) != 0);
        }
        else if (strcmp(argv[0], "park_open") == 0) {
            console.WriteFormatLine("park_open %d", (gParkFlags & PARK_FLAGS_PARK_OPEN) != 0);
        }
        else if (strcmp(argv[0], "land_rights_cost") == 0) {
            console.WriteFormatLine("land_rights_cost %d.%d0", gLandPrice / 10, gLandPrice % 10);
        }
        else if (strcmp(argv[0], "construction_rights_cost") == 0) {
            console.WriteFormatLine("construction_rights_cost %d.%d0", gConstructionRightsPrice / 10, gConstructionRightsPrice % 10);
        }
        else if (strcmp(argv[0], "climate") == 0) {
            const utf8* climate_names[] = { "cool_and_wet", "warm", "hot_and_dry", "cold" };
            console.WriteFormatLine("climate %s  (%d)", climate_names[gClimate], gClimate);
        }
        else if (strcmp(argv[0], "game_speed") == 0) {
            console.WriteFormatLine("game_speed %d", gGameSpeed);
        }
        else if (strcmp(argv[0], "console_small_font") == 0) {
            console.WriteFormatLine("console_small_font %d", gConfigInterface.console_small_font);
        }
        else if (strcmp(argv[0], "test_unfinished_tracks") == 0) {
            console.WriteFormatLine("test_unfinished_tracks %d", gConfigGeneral.test_unfinished_tracks);
        }
        else if (strcmp(argv[0], "no_test_crashes") == 0) {
            console.WriteFormatLine("no_test_crashes %d", gConfigGeneral.no_test_crashes);
        }
        else if (strcmp(argv[0], "location") == 0) {
            rct_window *w = window_get_main();
            if (w != nullptr) {
                sint32 interactionType;
                rct_tile_element *tileElement;
                LocationXY16 mapCoord = { 0 };
                rct_viewport * viewport = window_get_viewport(w);
                get_map_coordinates_from_pos(viewport->view_width / 2, viewport->view_height / 2, VIEWPORT_INTERACTION_MASK_TERRAIN, &mapCoord.x, &mapCoord.y, &interactionType, &tileElement, nullptr);
                mapCoord.x -= 16;
                mapCoord.x /= 32;
                mapCoord.y -= 16;
                mapCoord.y /= 32;
                mapCoord.x++;
                mapCoord.y++;
                console.WriteFormatLine("location %d %d", mapCoord.x, mapCoord.y);
            }
        }
        else if (strcmp(argv[0], "window_scale") == 0) {
            console.WriteFormatLine("window_scale %.3f", gConfigGeneral.window_scale);
        }
        else if (strcmp(argv[0], "window_limit") == 0) {
            console.WriteFormatLine("window_limit %d", gConfigGeneral.window_limit);
        }
        else if (strcmp(argv[0], "render_weather_effects") == 0) {
            console.WriteFormatLine("render_weather_effects %d", gConfigGeneral.render_weather_effects);
        }
        else if (strcmp(argv[0], "render_weather_gloom") == 0) {
            console.WriteFormatLine("render_weather_gloom %d", gConfigGeneral.render_weather_gloom);
        }
        else if (strcmp(argv[0], "cheat_sandbox_mode") == 0) {
            console.WriteFormatLine("cheat_sandbox_mode %d", gCheatsSandboxMode);
        }
        else if (strcmp(argv[0], "cheat_disable_clearance_checks") == 0) {
            console.WriteFormatLine("cheat_disable_clearance_checks %d", gCheatsDisableClearanceChecks);
        }
        else if (strcmp(argv[0], "cheat_disable_support_limits") == 0) {
            console.WriteFormatLine("cheat_disable_support_limits %d", gCheatsDisableSupportLimits);
        }
#ifndef NO_TTF
        else if (strcmp(argv[0], "enable_hinting") == 0) {
            console.WriteFormatLine("enable_hinting %d", gConfigFonts.enable_hinting);
        }
#endif
        else {
            console.WriteLineWarning("Invalid variable.");
        }
    }
    return 0;
}
static sint32 cc_set(InteractiveConsole &console, const utf8 **argv, sint32 argc)
{
    sint32 i;
    if (argc > 1) {
        sint32 int_val[4];
        bool int_valid[4];
        double double_val[4];
        bool double_valid[4];
        bool invalidArgs = false;

        for (i = 0; i < 4; i++) {
            if (i + 1 < argc) {
                int_val[i] = console_parse_int(argv[i + 1], &int_valid[i]);
                double_val[i] = console_parse_double(argv[i + 1], &double_valid[i]);
            }
            else {
                int_val[i] = 0; int_valid[i] = false;
                double_val[i] = 0; double_valid[i] = false;
            }
        }

        if (strcmp(argv[0], "money") == 0 && invalidArguments(&invalidArgs, double_valid[0])) {
            money32 money = MONEY((sint32)double_val[0], ((sint32)(double_val[0] * 100)) % 100);
            bool run_get_money = true;
            if (gCash != money) {
                if (game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SETMONEY, money, GAME_COMMAND_CHEAT, 0, 0) != MONEY32_UNDEFINED) {
                    // When in networked client mode, console.Execute("get money")
                    // does not print value accurately. Instead, print the argument.
                    if (network_get_mode() == NETWORK_MODE_CLIENT) {
                        run_get_money = false;
                        console.WriteFormatLine("money %d.%d0", money / 10, money % 10);
                    }
                }
                else {
                    console.WriteLineError("Network error: Permission denied!");
                }
            }
            if (run_get_money) {
                console.Execute("get money");
            }
        }
        else if (strcmp(argv[0], "scenario_initial_cash") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
            gInitialCash = Math::Clamp(MONEY(0, 0), MONEY(int_val[0], 0), MONEY(1000000, 00));
            console.Execute("get scenario_initial_cash");
        }
        else if (strcmp(argv[0], "current_loan") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
            gBankLoan = Math::Clamp(MONEY(0, 0), MONEY(int_val[0] - (int_val[0] % 1000), 0), gMaxBankLoan);
            console.Execute("get current_loan");
        }
        else if (strcmp(argv[0], "max_loan") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
            gMaxBankLoan = Math::Clamp(MONEY(0, 0), MONEY(int_val[0] - (int_val[0] % 1000), 0), MONEY(5000000, 0));
            console.Execute("get max_loan");
        }
        else if (strcmp(argv[0], "guest_initial_cash") == 0 && invalidArguments(&invalidArgs, double_valid[0])) {
            gGuestInitialCash = Math::Clamp(MONEY(0, 0), MONEY((sint32)double_val[0], ((sint32)(double_val[0] * 100)) % 100), MONEY(1000, 0));
            console.Execute("get guest_initial_cash");
        }
        else if (strcmp(argv[0], "guest_initial_happiness") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
            gGuestInitialHappiness = calculate_guest_initial_happiness((uint8)int_val[0]);
            console.Execute("get guest_initial_happiness");
        }
        else if (strcmp(argv[0], "guest_initial_hunger") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
            gGuestInitialHunger = (Math::Clamp(1, int_val[0], 84) * 255 / 100 - 255) * -1;
            console.Execute("get guest_initial_hunger");
        }
        else if (strcmp(argv[0], "guest_initial_thirst") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
            gGuestInitialThirst = (Math::Clamp(1, int_val[0], 84) * 255 / 100 - 255) * -1;
            console.Execute("get guest_initial_thirst");
        }
        else if (strcmp(argv[0], "guest_prefer_less_intense_rides") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
            SET_FLAG(gParkFlags, PARK_FLAGS_PREF_LESS_INTENSE_RIDES, int_val[0]);
            console.Execute("get guest_prefer_less_intense_rides");
        }
        else if (strcmp(argv[0], "guest_prefer_more_intense_rides") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
            SET_FLAG(gParkFlags, PARK_FLAGS_PREF_MORE_INTENSE_RIDES, int_val[0]);
            console.Execute("get guest_prefer_more_intense_rides");
        }
        else if (strcmp(argv[0], "forbid_marketing_campagns") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
            SET_FLAG(gParkFlags, PARK_FLAGS_FORBID_MARKETING_CAMPAIGN, int_val[0]);
            console.Execute("get forbid_marketing_campagns");
        }
        else if (strcmp(argv[0], "forbid_landscape_changes") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
            SET_FLAG(gParkFlags, PARK_FLAGS_FORBID_LANDSCAPE_CHANGES, int_val[0]);
            console.Execute("get forbid_landscape_changes");
        }
        else if (strcmp(argv[0], "forbid_tree_removal") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
            SET_FLAG(gParkFlags, PARK_FLAGS_FORBID_TREE_REMOVAL, int_val[0]);
            console.Execute("get forbid_tree_removal");
        }
        else if (strcmp(argv[0], "forbid_high_construction") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
            SET_FLAG(gParkFlags, PARK_FLAGS_FORBID_HIGH_CONSTRUCTION, int_val[0]);
            console.Execute("get forbid_high_construction");
        }
        else if (strcmp(argv[0], "pay_for_rides") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
            SET_FLAG(gParkFlags, PARK_FLAGS_PARK_FREE_ENTRY, int_val[0]);
            console.Execute("get pay_for_rides");
        }
        else if (strcmp(argv[0], "no_money") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
            SET_FLAG(gParkFlags, PARK_FLAGS_NO_MONEY, int_val[0]);
            console.Execute("get no_money");
        }
        else if (strcmp(argv[0], "difficult_park_rating") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
            SET_FLAG(gParkFlags, PARK_FLAGS_DIFFICULT_PARK_RATING, int_val[0]);
            console.Execute("get difficult_park_rating");
        }
        else if (strcmp(argv[0], "difficult_guest_generation") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
            SET_FLAG(gParkFlags, PARK_FLAGS_DIFFICULT_GUEST_GENERATION, int_val[0]);
            console.Execute("get difficult_guest_generation");
        }
        else if (strcmp(argv[0], "park_open") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
            SET_FLAG(gParkFlags, PARK_FLAGS_PARK_OPEN, int_val[0]);
            console.Execute("get park_open");
        }
        else if (strcmp(argv[0], "land_rights_cost") == 0 && invalidArguments(&invalidArgs, double_valid[0])) {
            gLandPrice = Math::Clamp(MONEY(0, 0), MONEY((sint32)double_val[0], ((sint32)(double_val[0] * 100)) % 100), MONEY(200, 0));
            console.Execute("get land_rights_cost");
        }
        else if (strcmp(argv[0], "construction_rights_cost") == 0 && invalidArguments(&invalidArgs, double_valid[0])) {
            gConstructionRightsPrice = Math::Clamp(MONEY(0, 0), MONEY((sint32)double_val[0], ((sint32)(double_val[0] * 100)) % 100), MONEY(200, 0));
            console.Execute("get construction_rights_cost");
        }
        else if (strcmp(argv[0], "climate") == 0) {
            if (int_valid[0]) {
                gClimate = Math::Clamp(0, int_val[0], 3);
            }
            else {
                const utf8 * climate_names[] = { "cool_and_wet", "warm", "hot_and_dry", "cold" };
                for (i = 0; i < 4; i++) {
                    if (strcmp(argv[1], climate_names[i]) == 0) {
                        gClimate = i;
                        break;
                    }
                }
            }
            if (i == 4)
                invalidArgs = true;
            else
            console.Execute("get climate");
        }
        else if (strcmp(argv[0], "game_speed") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
            gGameSpeed = Math::Clamp(1, int_val[0], 8);
            console.Execute("get game_speed");
        }
        else if (strcmp(argv[0], "console_small_font") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
            gConfigInterface.console_small_font = (int_val[0] != 0);
            config_save_default();
            console.Execute("get console_small_font");
        }
        else if (strcmp(argv[0], "test_unfinished_tracks") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
            gConfigGeneral.test_unfinished_tracks = (int_val[0] != 0);
            config_save_default();
            console.Execute("get test_unfinished_tracks");
        }
        else if (strcmp(argv[0], "no_test_crashes") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
            gConfigGeneral.no_test_crashes = (int_val[0] != 0);
            config_save_default();
            console.Execute("get no_test_crashes");
        }
        else if (strcmp(argv[0], "location") == 0 && invalidArguments(&invalidArgs, int_valid[0] && int_valid[1])) {
            rct_window *w = window_get_main();
            if (w != nullptr) {
                sint32 x = (sint16)(int_val[0] * 32 + 16);
                sint32 y = (sint16)(int_val[1] * 32 + 16);
                sint32 z = tile_element_height(x, y);
                window_set_location(w, x, y, z);
                viewport_update_position(w);
                console.Execute("get location");
            }
        }
        else if (strcmp(argv[0], "window_scale") == 0 && invalidArguments(&invalidArgs, double_valid[0])) {
            float newScale = (float)(0.001*std::trunc(1000*double_val[0]));
            gConfigGeneral.window_scale = Math::Clamp(0.5f, newScale, 5.0f);
            config_save_default();
            gfx_invalidate_screen();
            context_trigger_resize();
            context_update_cursor_scale();
            console.Execute("get window_scale");
        }
        else if (strcmp(argv[0], "window_limit") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
            window_set_window_limit(int_val[0]);
            console.Execute("get window_limit");
        }
        else if (strcmp(argv[0], "render_weather_effects") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
            gConfigGeneral.render_weather_effects = (int_val[0] != 0);
            config_save_default();
            console.Execute("get render_weather_effects");
        }
        else if (strcmp(argv[0], "render_weather_gloom") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
            gConfigGeneral.render_weather_gloom = (int_val[0] != 0);
            config_save_default();
            console.Execute("get render_weather_gloom");
        }
        else if (strcmp(argv[0], "cheat_sandbox_mode") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
            if (gCheatsSandboxMode != (int_val[0] != 0)) {
                if (game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SANDBOXMODE, (int_val[0] != 0), GAME_COMMAND_CHEAT, 0, 0) != MONEY32_UNDEFINED) {
                    // Change it locally so it shows the accurate value in the
                    // "console.Execute("get cheat_sandbox_mode")" line when in networked client mode
                    gCheatsSandboxMode = (int_val[0] != 0);
                }
                else {
                    console.WriteLineError("Network error: Permission denied!");
                }
            }
            console.Execute("get cheat_sandbox_mode");
        }
        else if (strcmp(argv[0], "cheat_disable_clearance_checks") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
            if (gCheatsDisableClearanceChecks != (int_val[0] != 0)) {
                if (game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_DISABLECLEARANCECHECKS, (int_val[0] != 0), GAME_COMMAND_CHEAT, 0, 0) != MONEY32_UNDEFINED) {
                    // Change it locally so it shows the accurate value in the
                    // "console.Execute("get cheat_disable_clearance_checks")" line when in networked client mode
                    gCheatsDisableClearanceChecks = (int_val[0] != 0);
                }
                else {
                    console.WriteLineError("Network error: Permission denied!");
                }
            }
            console.Execute("get cheat_disable_clearance_checks");
        }
        else if (strcmp(argv[0], "cheat_disable_support_limits") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
            if (gCheatsDisableSupportLimits != (int_val[0] != 0)) {
                if (game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_DISABLESUPPORTLIMITS, (int_val[0] != 0), GAME_COMMAND_CHEAT, 0, 0) != MONEY32_UNDEFINED) {
                    // Change it locally so it shows the accurate value in the
                    // "console.Execute("get cheat_disable_support_limits")" line when in networked client mode
                    gCheatsDisableSupportLimits = (int_val[0] != 0);
                }
                else {
                    console.WriteLineError("Network error: Permission denied!");
                }
            }
            console.Execute("get cheat_disable_support_limits");
        }
#ifndef NO_TTF
        else if (strcmp(argv[0], "enable_hinting") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
            gConfigFonts.enable_hinting = (int_val[0] != 0);
            config_save_default();
            console.Execute("get enable_hinting");
            ttf_toggle_hinting();
        }
#endif
        else if (invalidArgs) {
            console.WriteLineError("Invalid arguments.");
        }
        else {
            console.WriteLineError("Invalid variable.");
        }

        gfx_invalidate_screen();
    }
    else {
        console.WriteLineError("Value required.");
    }
    return 0;
}
static sint32 cc_twitch(InteractiveConsole &console, const utf8 **argv, sint32 argc)
{
#ifdef DISABLE_TWITCH
    console.WriteLineError("OpenRCT2 build not compiled with Twitch integration.");
#else
    // TODO: Add some twitch commands
#endif
    return 0;
}

static sint32 cc_load_object(InteractiveConsole &console, const utf8 **argv, sint32 argc) {
    if (argc > 0) {
        char name[9] = { 0 };
        memset(name, ' ', 8);
        sint32 i = 0;
        for (const char * ch = argv[0]; *ch != '\0' && i < 8; ch++) {
            name[i++] = *ch;
        }

        const ObjectRepositoryItem * ori = object_repository_find_object_by_name(name);
        if (ori == nullptr) {
            console.WriteLineError("Could not find the object.");
            return 1;
        }

        const rct_object_entry * entry = &ori->ObjectEntry;
        void * loadedObject = object_manager_get_loaded_object(entry);
        if (loadedObject != nullptr) {
            console.WriteLineError("Object is already in scenario.");
            return 1;
        }

        loadedObject = object_manager_load_object(entry);
        if (loadedObject == nullptr) {
            console.WriteLineError("Unable to load object.");
            return 1;
        }
        sint32 groupIndex = object_manager_get_loaded_object_entry_index(loadedObject);

        uint8 objectType = object_entry_get_type(entry);
        if (objectType == OBJECT_TYPE_RIDE) {
            // Automatically research the ride so it's supported by the game.
            rct_ride_entry *rideEntry;
            sint32 rideType;

            rideEntry = get_ride_entry(groupIndex);

            for (sint32 j = 0; j < MAX_RIDE_TYPES_PER_RIDE_ENTRY; j++) {
                rideType = rideEntry->ride_type[j];
                if (rideType != RIDE_TYPE_NULL)
                    research_insert(true, RESEARCH_ENTRY_RIDE_MASK | (rideType << 8) | groupIndex, rideEntry->category[0]);
            }

            gSilentResearch = true;
            research_reset_current_item();
            gSilentResearch = false;
        }
        else if (objectType == OBJECT_TYPE_SCENERY_GROUP) {
            research_insert(true, groupIndex, RESEARCH_CATEGORY_SCENERY_GROUP);

            gSilentResearch = true;
            research_reset_current_item();
            gSilentResearch = false;
        }
        scenery_set_default_placement_configuration();

        auto intent = Intent(INTENT_ACTION_REFRESH_NEW_RIDES);
        context_broadcast_intent(&intent);

        gWindowUpdateTicks = 0;
        gfx_invalidate_screen();
        console.WriteLine("Object file loaded.");
    }

    return 0;
}

static sint32 cc_object_count(InteractiveConsole &console, const utf8 **argv, sint32 argc) {
    const utf8* object_type_names[] = { "Rides", "Small scenery", "Large scenery", "Walls", "Banners", "Paths", "Path Additions", "Scenery groups", "Park entrances", "Water" };
    for (sint32 i = 0; i < 10; i++) {

        sint32 entryGroupIndex = 0;
        for (; entryGroupIndex < object_entry_group_counts[i]; entryGroupIndex++){
            if (object_entry_get_chunk(i, entryGroupIndex) == nullptr)
            {
                break;
            }
        }
        console.WriteFormatLine("%s: %d/%d", object_type_names[i], entryGroupIndex, object_entry_group_counts[i]);
    }

    return 0;
}

static sint32 cc_reset_user_strings(InteractiveConsole &console, const utf8 **argv, sint32 argc)
{
    reset_user_strings();
    return 0;
}

static sint32 cc_open(InteractiveConsole &console, const utf8 **argv, sint32 argc) {
    if (argc > 0) {
        bool title = (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) != 0;
        bool invalidTitle = false;
        if (strcmp(argv[0], "object_selection") == 0 && invalidArguments(&invalidTitle, !title)) {
            // Only this window should be open for safety reasons
            window_close_all();
            context_open_window(WC_EDITOR_OBJECT_SELECTION);
        } else if (strcmp(argv[0], "inventions_list") == 0 && invalidArguments(&invalidTitle, !title)) {
            context_open_window(WC_EDITOR_INVENTION_LIST);
        } else if (strcmp(argv[0], "scenario_options") == 0 && invalidArguments(&invalidTitle, !title)) {
            context_open_window(WC_EDITOR_SCENARIO_OPTIONS);
        } else if (strcmp(argv[0], "options") == 0) {
            context_open_window(WC_OPTIONS);
        } else if (strcmp(argv[0], "themes") == 0) {
            context_open_window(WC_THEMES);
        } else if (strcmp(argv[0], "title_sequences") == 0) {
            context_open_window(WC_TITLE_EDITOR);
        } else if (invalidTitle) {
            console.WriteLineError("Cannot open this window in the title screen.");
        } else {
            console.WriteLineError("Invalid window.");
        }
    }
    return 0;
}

static sint32 cc_remove_unused_objects(InteractiveConsole &console, const utf8 **argv, sint32 argc) 
{
    sint32 result = editor_remove_unused_objects();
    console.WriteFormatLine("%d unused object entries have been removed.", result);
    return 0;
}

static sint32 cc_remove_park_fences(InteractiveConsole &console, const utf8 **argv, sint32 argc)
{
    tile_element_iterator it;
    tile_element_iterator_begin(&it);
    do
    {
        if (tile_element_get_type(it.element) == TILE_ELEMENT_TYPE_SURFACE)
        {
            // Remove all park fence flags
            it.element->properties.surface.ownership &= 0xF0;
        }
    }
    while (tile_element_iterator_next(&it));

    console.WriteFormatLine("Park fences have been removed.");
    return 0;
}

static sint32 cc_show_limits(InteractiveConsole &console, const utf8 ** argv, sint32 argc)
{
    map_reorganise_elements();
    sint32 tileElementCount = gNextFreeTileElement - gTileElements - 1;

    sint32 rideCount = 0;
    for (sint32 i = 0; i < MAX_RIDES; ++i) 
    {
        Ride * ride = get_ride(i);
        if (ride->type != RIDE_TYPE_NULL) 
        {
            rideCount++;
        }
    }

    sint32 spriteCount = 0;
    for (sint32 i = 1; i < NUM_SPRITE_LISTS; ++i)
    {
        spriteCount += gSpriteListCount[i];
    }

    sint32 staffCount = 0;
    for (sint32 i = 0; i < STAFF_MAX_COUNT; ++i) 
    {
        if (gStaffModes[i] & 1)
        {
            staffCount++;
        }
    }

    sint32 bannerCount = 0;
    for (sint32 i = 0; i < MAX_BANNERS; ++i)
    {
        if (gBanners[i].type != BANNER_NULL)
        {
            bannerCount++;
        }
    }

    console.WriteFormatLine("Sprites: %d/%d", spriteCount, MAX_SPRITES);
    console.WriteFormatLine("Map Elements: %d/%d", tileElementCount, MAX_TILE_ELEMENTS);
    console.WriteFormatLine("Banners: %d/%d", bannerCount, MAX_BANNERS);
    console.WriteFormatLine("Rides: %d/%d", rideCount, MAX_RIDES);
    console.WriteFormatLine("Staff: %d/%d", staffCount, STAFF_MAX_COUNT);
    return 0;
}

static sint32 cc_for_date(InteractiveConsole &console, const utf8 **argv, sint32 argc)
{
    sint32 year = 0;
    sint32 month = 0;
    sint32 day = 0;
    if (argc < 1 || argc > 3)
    {
        return -1;
    }

    // All cases involve providing a year, so grab that first
    year = atoi(argv[0]);
    if (year < 1 || year > 8192)
    {
        return -1;
    }

    // YYYY (no month provided, preserve existing month)
    if (argc == 1)
    {
        month = gDateMonthsElapsed % MONTH_COUNT + 1;
    }

    // YYYY MM or YYYY MM DD (month provided)
    if (argc >= 2)
    {
        month = atoi(argv[1]);
        month -= 2;
        if (month < 1 || month > MONTH_COUNT)
        {
            return -1;
        }
    }

    // YYYY OR YYYY MM (no day provided, preserve existing day)
    if (argc <= 2)
    {
        day = Math::Clamp(1, gDateMonthTicks / (0x10000 / days_in_month[month - 1]) + 1, (int)days_in_month[month - 1]);
    }

    // YYYY MM DD (year, month, and day provided)
    if (argc == 3)
    {
        day = atoi(argv[2]);
        if (day < 1 || day > days_in_month[month-1])
        {
            return -1;
        }
    }

    date_set(year, month, day);
    window_invalidate_by_class(WC_BOTTOM_TOOLBAR);

    return 1;
}

using console_command_func = sint32 (*)(InteractiveConsole &console, const utf8 ** argv, sint32 argc);
struct console_command {
    const utf8 * command;
    console_command_func func;
    const utf8 * help;
    const utf8 * usage;
};

static constexpr const utf8* console_variable_table[] = {
    "park_rating",
    "park_value",
    "company_value",
    "money",
    "scenario_initial_cash",
    "current_loan",
    "max_loan",
    "guest_initial_cash",
    "guest_initial_happiness",
    "guest_initial_hunger",
    "guest_initial_thirst",
    "guest_prefer_less_intense_rides",
    "guest_prefer_more_intense_rides",
    "forbid_marketing_campagn",
    "forbid_landscape_changes",
    "forbid_tree_removal",
    "forbid_high_construction",
    "pay_for_rides",
    "no_money",
    "difficult_park_rating",
    "difficult_guest_generation",
    "land_rights_cost",
    "construction_rights_cost",
    "park_open",
    "climate",
    "game_speed",
    "console_small_font",
    "test_unfinished_tracks",
    "no_test_crashes",
    "location",
    "window_scale",
    "window_limit",
    "render_weather_effects",
    "render_weather_gloom",
    "cheat_sandbox_mode",
    "cheat_disable_clearance_checks",
    "cheat_disable_support_limits",
};
static constexpr const utf8* console_window_table[] = {
    "object_selection",
    "inventions_list",
    "scenario_options",
    "options",
    "themes",
    "title_sequences"
};

static constexpr const console_command console_command_table[] = {
    { "clear", cc_clear, "Clears the console.", "clear"},
    { "hide", cc_hide, "Hides the console.", "hide"},
    { "echo", cc_echo, "Echoes the text to the console.", "echo <text>" },
    { "help", cc_help, "Lists commands or info about a command.", "help [command]" },
    { "get", cc_get, "Gets the value of the specified variable.", "get <variable>" },
    { "set", cc_set, "Sets the variable to the specified value.", "set <variable> <value>" },
    { "open", cc_open, "Opens the window with the give name.", "open <window>." },
    { "variables", cc_variables, "Lists all the variables that can be used with get and sometimes set.", "variables" },
    { "windows", cc_windows, "Lists all the windows that can be opened.", "windows" },
    { "load_object", cc_load_object, "Loads the object file into the scenario.\n"
                                    "Loading a scenery group will not load its associated objects.\n"
                                    "This is a safer method opposed to \"open object_selection\".",
                                    "load_object <objectfilenodat>" },
    { "object_count", cc_object_count, "Shows the number of objects of each type in the scenario.", "object_count" },
    { "twitch", cc_twitch, "Twitch API" },
    { "reset_user_strings", cc_reset_user_strings, "Resets all user-defined strings, to fix incorrectly occurring 'Chosen name in use already' errors.", "reset_user_strings" },
    { "rides", cc_rides, "Ride management.", "rides <subcommand>" },
    { "staff", cc_staff, "Staff management.", "staff <subcommand>"},
    { "remove_unused_objects", cc_remove_unused_objects, "Removes all the unused objects from the object selection.", "remove_unused_objects" },
    { "remove_park_fences", cc_remove_park_fences, "Removes all park fences from the surface", "remove_park_fences"},
    { "show_limits", cc_show_limits, "Shows the map data counts and limits.", "show_limits" },
    { "date", cc_for_date, "Sets the date to a given date.", "Format <year>[ <month>[ <day>]]."}
};

static sint32 cc_windows(InteractiveConsole &console, const utf8 **argv, sint32 argc) {
    for (auto s : console_window_table)
    {
        console.WriteLine(s);
    }
    return 0;
}
static sint32 cc_variables(InteractiveConsole &console, const utf8 **argv, sint32 argc)
{
    for (auto s : console_variable_table)
    {
        console.WriteLine(s);
    }
    return 0;
}

static sint32 cc_help(InteractiveConsole &console, const utf8 **argv, sint32 argc)
{
    if (argc > 0)
    {
        for (const auto &c : console_command_table)
        {
            if (strcmp(c.command, argv[0]) == 0)
            {
                console.WriteLine(c.help);
                console.WriteFormatLine("\nUsage:   %s", c.usage);
            }
        }
    }
    else
    {
        console_write_all_commands(console);
    }
    return 0;
}

static void console_write_all_commands(InteractiveConsole &console)
{
    for (const auto &c : console_command_table)
    {
        console.WriteLine(c.command);
    }
}

static bool invalidArguments(bool *invalid, bool arguments)
{
    if (!arguments) {
        *invalid = true;
        return false;
    }
    return true;
}

void InteractiveConsole::Execute(const std::string &s)
{
    sint32 argc = 0;
    sint32 argvCapacity = 8;
    utf8 **argv = (utf8 * *)malloc(argvCapacity * sizeof(utf8*));
    const utf8 *start = s.c_str();
    const utf8 *end;
    bool inQuotes = false;
    do {
        while (*start == ' ')
            start++;

        if (*start == '"') {
            inQuotes = true;
            start++;
        }
        else {
            inQuotes = false;
        }

        end = start;
        while (*end != 0) {
            if (*end == ' ' && !inQuotes)
                break;
            if (*end == '"' && inQuotes)
                break;
            end++;
        }
        size_t length = end - start;

        if (length > 0) {
            utf8 *arg = (utf8 *)malloc(length + 1);
            memcpy(arg, start, length);
            arg[length] = 0;

            if (argc >= argvCapacity) {
                argvCapacity *= 2;
                argv = (utf8 * *)realloc(argv, argvCapacity * sizeof(utf8*));
            }
            argv[argc] = arg;
            argc++;
        }

        start = end;
    } while (*end != 0);

    if (argc == 0)
        return;

    // Aliases for hiding the console
    if(strcmp(argv[0],"quit") == 0 || strcmp(argv[0],"exit") == 0) {
        free(argv[0]);
        argv[0] = _strdup("hide");
    }

    bool validCommand = false;
    for (const auto &c : console_command_table)
    {
        if (strcmp(argv[0], c.command) == 0)
        {
            c.func(*this, (const utf8 **)(argv + 1), argc - 1);
            validCommand = true;
            break;
        }
    }

    for (sint32 i = 0; i < argc; i++)
        free(argv[i]);
    free(argv);

    if (!validCommand) {
        WriteLineError("Unknown command. Type help to list available commands.");
    }
}

void InteractiveConsole::WriteLine(const std::string &s)
{
    WriteLine(s, FORMAT_WINDOW_COLOUR_2);
}

void InteractiveConsole::WriteLineError(const std::string &s)
{
    WriteLine(s, FORMAT_RED);
}

void InteractiveConsole::WriteLineWarning(const std::string &s)
{
    WriteLine(s, FORMAT_YELLOW);
}

void InteractiveConsole::WriteFormatLine(const std::string &format, ...)
{
    va_list list;
    va_start(list, format);
    auto buffer = String::Format_VA(format.c_str(), list);
    va_end(list);

    auto s = std::string(buffer);
    std::free(buffer);
    WriteLine(s);
}

InGameConsole::InGameConsole()
{
    InteractiveConsole::WriteLine(OPENRCT2_NAME " " OPENRCT2_VERSION);
    InteractiveConsole::WriteLine("Type 'help' for a list of available commands. Type 'hide' to hide the console.");
    InteractiveConsole::WriteLine("");
    WritePrompt();
}

void InGameConsole::WritePrompt()
{
    InteractiveConsole::WriteLine("> ");
}

void InGameConsole::Input(CONSOLE_INPUT input)
{
    switch (input) {
    case CONSOLE_INPUT_LINE_CLEAR:
        ClearInput();
        RefreshCaret();
        break;
    case CONSOLE_INPUT_LINE_EXECUTE:
        if (_consoleCurrentLine[0] != '\0') {
            HistoryAdd(_consoleCurrentLine);
            Execute(_consoleCurrentLine);
            WritePrompt();
            ClearInput();
            RefreshCaret();
        }
        ScrollToEnd();
        break;
    case CONSOLE_INPUT_HISTORY_PREVIOUS:
        if (_consoleHistoryIndex > 0) {
            _consoleHistoryIndex--;
            memcpy(_consoleCurrentLine, _consoleHistory[_consoleHistoryIndex], 256);
        }
        _consoleTextInputSession->Size = strlen(_consoleTextInputSession->Buffer);
        _consoleTextInputSession->Length = utf8_length(_consoleTextInputSession->Buffer);
        _consoleTextInputSession->SelectionStart = strlen(_consoleCurrentLine);
        break;
    case CONSOLE_INPUT_HISTORY_NEXT:
        if (_consoleHistoryIndex < _consoleHistoryCount - 1) {
            _consoleHistoryIndex++;
            memcpy(_consoleCurrentLine, _consoleHistory[_consoleHistoryIndex], 256);
            _consoleTextInputSession->Size = strlen(_consoleTextInputSession->Buffer);
            _consoleTextInputSession->Length = utf8_length(_consoleTextInputSession->Buffer);
            _consoleTextInputSession->SelectionStart = strlen(_consoleCurrentLine);
        } else {
            _consoleHistoryIndex = _consoleHistoryCount;
            ClearInput();
        }
        break;
    case CONSOLE_INPUT_SCROLL_PREVIOUS:
    {
        sint32 scrollAmt = console_get_num_visible_lines() - 1;
        Scroll(scrollAmt);
        break;
    }
    case CONSOLE_INPUT_SCROLL_NEXT:
    {
        sint32 scrollAmt = console_get_num_visible_lines() - 1;
        Scroll(-scrollAmt);
        break;
    }
    default:
        break;
    }
}

void InGameConsole::ClearInput()
{
    _consoleCurrentLine[0] = 0;
    if (gConsoleOpen) {
        context_start_text_input(_consoleCurrentLine, sizeof(_consoleCurrentLine));
    }
}

void InGameConsole::HistoryAdd(const utf8 * src)
{
    if (_consoleHistoryCount >= CONSOLE_HISTORY_SIZE) {
        for (sint32 i = 0; i < _consoleHistoryCount - 1; i++)
            memcpy(_consoleHistory[i], _consoleHistory[i + 1], CONSOLE_INPUT_SIZE);
        _consoleHistoryCount--;
    }
    memcpy(_consoleHistory[_consoleHistoryCount++], src, CONSOLE_INPUT_SIZE);
    _consoleHistoryIndex = _consoleHistoryCount;
}

void InGameConsole::ScrollToEnd()
{
    _consoleScrollPos = Math::Max<sint32>(0, (sint32)_consoleLines.size() - console_get_num_visible_lines());
}

void InGameConsole::RefreshCaret()
{
    _consoleCaretTicks = 0;
}

void InGameConsole::Scroll(sint32 linesToScroll)
{
    const sint32 maxVisibleLines = console_get_num_visible_lines();
    const sint32 numLines = (sint32)_consoleLines.size();
    if (numLines > maxVisibleLines)
    {
        sint32 maxScrollValue = numLines - maxVisibleLines;
        _consoleScrollPos     = Math::Clamp<sint32>(0, _consoleScrollPos - linesToScroll, maxScrollValue);
    }
}

void InGameConsole::Clear()
{
    _consoleLines.clear();
    ScrollToEnd();
}

void InGameConsole::ClearLine()
{
    _consoleCurrentLine[0] = 0;
    RefreshCaret();
}

void InGameConsole::Open()
{
    gConsoleOpen = true;
    ScrollToEnd();
    RefreshCaret();
    _consoleTextInputSession = context_start_text_input(_consoleCurrentLine, sizeof(_consoleCurrentLine));
}

void InGameConsole::Close()
{
    gConsoleOpen = false;
    Invalidate();
    context_stop_text_input();
}

void InGameConsole::Toggle()
{
    if (gConsoleOpen)
        console_close();
    else
        console_open();
}

void InGameConsole::WriteLine(const std::string &input, uint32 colourFormat)
{
    // Include text colour format only for special cases
    // The draw function handles the default text colour differently
    utf8 colourCodepoint[4]{};
    if (colourFormat != FORMAT_WINDOW_COLOUR_2)
        utf8_write_codepoint(colourCodepoint, colourFormat);

    std::string line;
    std::size_t splitPos     = 0;
    std::size_t stringOffset = 0;
    while (splitPos != std::string::npos)
    {
        splitPos = input.find('\n', stringOffset);
        line     = input.substr(stringOffset, splitPos - stringOffset);
        _consoleLines.push_back(colourCodepoint + line);
        stringOffset = splitPos + 1;
    }

    if (_consoleLines.size() > CONSOLE_MAX_LINES)
    {
        const std::size_t linesToErase = _consoleLines.size() - CONSOLE_MAX_LINES;
        _consoleLines.erase(_consoleLines.begin(), _consoleLines.begin() + linesToErase);
    }
}

void InGameConsole::Invalidate()
{
    gfx_set_dirty_blocks(_consoleLeft, _consoleTop, _consoleRight, _consoleBottom);
}

void InGameConsole::Update()
{
    _consoleLeft = 0;
    _consoleTop = 0;
    _consoleRight = context_get_width();
    _consoleBottom = 322;

    if (gConsoleOpen) {
        // When scrolling the map, the console pixels get copied... therefore invalidate the screen
        rct_window *mainWindow = window_get_main();
        if (mainWindow != nullptr) {
            rct_viewport *mainViewport = window_get_viewport(mainWindow);
            if (mainViewport != nullptr) {
                if (_lastMainViewportX != mainViewport->view_x || _lastMainViewportY != mainViewport->view_y) {
                    _lastMainViewportX = mainViewport->view_x;
                    _lastMainViewportY = mainViewport->view_y;

                    gfx_invalidate_screen();
                }
            }
        }

        // Remove unwanted characters in console input
        utf8_remove_format_codes(_consoleCurrentLine, false);
    }

    // Flash the caret
    _consoleCaretTicks = (_consoleCaretTicks + 1) % 30;
}

void InGameConsole::Draw(rct_drawpixelinfo * dpi)
{
    if (!gConsoleOpen)
        return;

    // Set font
    gCurrentFontSpriteBase           = (gConfigInterface.console_small_font ? FONT_SPRITE_BASE_SMALL : FONT_SPRITE_BASE_MEDIUM);
    gCurrentFontFlags                = 0;
    uint8        textColour          = NOT_TRANSLUCENT(theme_get_colour(WC_CONSOLE, 1));
    const sint32 lineHeight          = font_get_line_height(gCurrentFontSpriteBase);
    const sint32 maxLines            = console_get_num_visible_lines();

    // This is something of a hack to ensure the text is actually black
    // as opposed to a desaturated grey
    std::string colourFormatStr;
    if (textColour == COLOUR_BLACK)
    {
        utf8 extraTextFormatCode[4]{};
        utf8_write_codepoint(extraTextFormatCode, FORMAT_BLACK);
        colourFormatStr = extraTextFormatCode;
    }

    // TTF looks far better without the outlines
    if (!gUseTrueTypeFont)
    {
        textColour |= COLOUR_FLAG_OUTLINE;
    }

    Invalidate();

    // Give console area a translucent effect.
    gfx_filter_rect(dpi, _consoleLeft, _consoleTop, _consoleRight, _consoleBottom, PALETTE_51);

    // Make input area more opaque.
    gfx_filter_rect(dpi, _consoleLeft, _consoleBottom - lineHeight - 10, _consoleRight, _consoleBottom - 1, PALETTE_51);

    // Paint background colour.
    uint8 backgroundColour = theme_get_colour(WC_CONSOLE, 0);
    gfx_fill_rect_inset(dpi, _consoleLeft, _consoleTop, _consoleRight, _consoleBottom, backgroundColour, INSET_RECT_FLAG_FILL_NONE);
    gfx_fill_rect_inset(dpi, _consoleLeft + 1, _consoleTop + 1, _consoleRight - 1, _consoleBottom - 1, backgroundColour, INSET_RECT_FLAG_BORDER_INSET);

    std::string lineBuffer;
    sint32      x = _consoleLeft + CONSOLE_EDGE_PADDING;
    sint32      y = _consoleTop + CONSOLE_EDGE_PADDING;

    // Draw text inside console
    for (std::size_t i = 0; i < _consoleLines.size() && i < (size_t)maxLines; i++) {
        const size_t index = i + _consoleScrollPos;
        lineBuffer         = colourFormatStr + _consoleLines[index];
        gfx_draw_string(dpi, lineBuffer.c_str(), textColour, x, y);
        y += lineHeight;
    }

    y = _consoleBottom - lineHeight - CONSOLE_EDGE_PADDING - 1;

    // Draw current line
    lineBuffer = colourFormatStr + _consoleCurrentLine;
    gfx_draw_string(dpi, lineBuffer.c_str(), TEXT_COLOUR_255, x, y);

    // Draw caret
    if (_consoleCaretTicks < CONSOLE_CARET_FLASH_THRESHOLD) {
        sint32 caretX = x + gfx_get_string_width(_consoleCurrentLine);
        sint32 caretY = y + lineHeight;

        uint8 caretColour = ColourMapA[BASE_COLOUR(textColour)].lightest;
        gfx_fill_rect(dpi, caretX, caretY, caretX + CONSOLE_CARET_WIDTH, caretY, caretColour);
    }

    // What about border colours?
    uint8 borderColour1 = ColourMapA[BASE_COLOUR(backgroundColour)].light;
    uint8 borderColour2 = ColourMapA[BASE_COLOUR(backgroundColour)].mid_dark;

    // Input area top border
    gfx_fill_rect(dpi, _consoleLeft, _consoleBottom - lineHeight - 11, _consoleRight, _consoleBottom - lineHeight - 11, borderColour1);
    gfx_fill_rect(dpi, _consoleLeft, _consoleBottom - lineHeight - 10, _consoleRight, _consoleBottom - lineHeight - 10, borderColour2);

    // Input area bottom border
    gfx_fill_rect(dpi, _consoleLeft, _consoleBottom - 1, _consoleRight, _consoleBottom - 1, borderColour1);
    gfx_fill_rect(dpi, _consoleLeft, _consoleBottom - 0, _consoleRight, _consoleBottom - 0, borderColour2);
}

void StdInOutConsole::Start()
{
    std::thread replThread ([this]() -> void
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        linenoise::SetMultiLine(true);
        linenoise::SetHistoryMaxLen(32);

        std::string prompt = "\033[32mopenrct2 $\x1b[0m ";
        while (true)
        {
            std::string line;
            std::string left = prompt;
            auto quit = linenoise::Readline(left.c_str(), line);
            if (quit) {
                openrct2_finish();
                break;
            }
            linenoise::AddHistory(line.c_str());
            Eval(line).wait();
        }
    });
    replThread.detach();
}

std::future<void> StdInOutConsole::Eval(const std::string &s)
{
    // Push on-demand evaluations onto a queue so that it can be processed deterministically
    // on the main thead at the right time.
    std::promise<void> barrier;
    auto future = barrier.get_future();
    _evalQueue.emplace(std::move(barrier), s);
    return future;
}

void StdInOutConsole::ProcessEvalQueue()
{
    while (_evalQueue.size() > 0)
    {
        auto item = std::move(_evalQueue.front());
        _evalQueue.pop();
        auto promise = std::move(std::get<0>(item));
        auto command = std::move(std::get<1>(item));

        Execute(command);

        // Signal the promise so caller can continue
        promise.set_value();
    }
}

void StdInOutConsole::Clear()
{
    linenoise::linenoiseClearScreen();
}

void StdInOutConsole::Close()
{
    openrct2_finish();
}

void StdInOutConsole::WriteLine(const std::string &s, uint32 colourFormat)
{
    std::string formatBegin;
    if (colourFormat != FORMAT_WINDOW_COLOUR_2)
    {
        switch (colourFormat)
        {
            case FORMAT_RED:
                formatBegin = "\033[31m";
                break;
            case FORMAT_YELLOW:
                formatBegin = "\033[33m";
                break;
        }
    }

    if (formatBegin.empty())
    {
        std::printf("%s\n", s.c_str());
    }
    else
    {
        std::printf("%s%s%s\n", formatBegin.c_str(), s.c_str(), "\x1b[0m");
    }
}
