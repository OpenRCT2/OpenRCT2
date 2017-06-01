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

#include <stdarg.h>
#include <SDL_scancode.h>

#include "../config/Config.h"
#include "../Context.h"
#include "../drawing/drawing.h"
#include "../game.h"
#include "../input.h"
#include "../localisation/localisation.h"
#include "../localisation/user.h"
#include "../management/finance.h"
#include "../management/research.h"
#include "../network/network.h"
#include "../network/twitch.h"
#include "../object.h"
#include "../object/ObjectManager.h"
#include "../object/ObjectRepository.h"
#include "../peep/staff.h"
#include "../platform/platform.h"
#include "../rct2.h"
#include "../ride/ride.h"
#include "../ride/ride_data.h"
#include "../util/sawyercoding.h"
#include "../util/util.h"
#include "../Version.h"
#include "../world/banner.h"
#include "../world/Climate.h"
#include "../world/park.h"
#include "../world/scenery.h"
#include "console.h"
#include "viewport.h"

#define CONSOLE_BUFFER_SIZE 8192
#define CONSOLE_BUFFER_2_SIZE 256
#define CONSOLE_HISTORY_SIZE 64
#define CONSOLE_INPUT_SIZE 256

bool gConsoleOpen = false;

static bool _consoleInitialised = false;
static sint32 _consoleLeft, _consoleTop, _consoleRight, _consoleBottom;
static sint32 _lastMainViewportX, _lastMainViewportY;
static utf8 _consoleBuffer[CONSOLE_BUFFER_SIZE] = { 0 };
static utf8 *_consoleBufferPointer = _consoleBuffer;
static utf8 *_consoleViewBufferStart = _consoleBuffer;
static utf8 _consoleCurrentLine[CONSOLE_INPUT_SIZE];
static sint32 _consoleCaretTicks;
static utf8 _consolePrintfBuffer[CONSOLE_BUFFER_2_SIZE];
static utf8 _consoleErrorBuffer[CONSOLE_BUFFER_2_SIZE];
static sint32 _consoleScrollPos = 0;
static TextInputSession * _consoleTextInputSession;

static utf8 _consoleHistory[CONSOLE_HISTORY_SIZE][CONSOLE_INPUT_SIZE];
static sint32 _consoleHistoryIndex = 0;
static sint32 _consoleHistoryCount = 0;

static void console_invalidate();
static void console_write_prompt();
static void console_update_scroll();
static void console_clear_input();
static void console_history_add(const utf8 *src);
static void console_write_all_commands();
static sint32 console_parse_int(const utf8 *src, bool *valid);
static double console_parse_double(const utf8 *src, bool *valid);

static sint32 cc_variables(const utf8 **argv, sint32 argc);
static sint32 cc_windows(const utf8 **argv, sint32 argc);
static sint32 cc_help(const utf8 **argv, sint32 argc);

static bool invalidArguments(bool *invalid, bool arguments);

#define SET_FLAG(variable, flag, value) {if (value) variable |= flag; else variable &= ~flag;}

void console_open()
{
	gConsoleOpen = true;
	_consoleScrollPos = 0;
	console_refresh_caret();
	console_update_scroll();
	_consoleTextInputSession = context_start_text_input(_consoleCurrentLine, sizeof(_consoleCurrentLine));
}

void console_close()
{
	gConsoleOpen = false;
	console_invalidate();
	context_stop_text_input();
}

void console_toggle()
{
	if (gConsoleOpen)
		console_close();
	else
		console_open();
}

static void console_init()
{
	_consoleInitialised = true;
	console_writeline(OPENRCT2_NAME " " OPENRCT2_VERSION);
	console_writeline("Type 'help' for a list of available commands. Type 'hide' to hide the console.");
	console_writeline("");
	console_write_prompt();
}

void console_update()
{
	if (!_consoleInitialised)
		console_init();

	_consoleLeft = 0;
	_consoleTop = 0;
	_consoleRight = context_get_width();
	_consoleBottom = 322;

	if (gConsoleOpen) {
		// When scrolling the map, the console pixels get copied... therefore invalidate the screen
		rct_window *mainWindow = window_get_main();
		if (mainWindow != NULL) {
			rct_viewport *mainViewport = mainWindow->viewport;
			if (mainViewport != NULL) {
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

void console_draw(rct_drawpixelinfo *dpi)
{
	if (!gConsoleOpen)
		return;

	// Set font
	gCurrentFontSpriteBase = (gConfigInterface.console_small_font ? FONT_SPRITE_BASE_SMALL : FONT_SPRITE_BASE_MEDIUM);
	gCurrentFontFlags = 0;
	sint32 lineHeight = font_get_line_height(gCurrentFontSpriteBase);

	sint32 lines = 0;
	sint32 maxLines = ((_consoleBottom - 22 - _consoleTop) / lineHeight) - 1;
	utf8 *ch = strchr(_consoleBuffer, 0);
	while (ch > _consoleBuffer) {
		ch--;
		if (*ch == '\n')
			lines++;
	}

	// Background
	console_invalidate();
	gfx_filter_rect(dpi, _consoleLeft, _consoleTop, _consoleRight, _consoleBottom, PALETTE_TRANSLUCENT_LIGHT_BLUE_HIGHLIGHT);

	sint32 x = _consoleLeft + 4;
	sint32 y = _consoleTop + 4;

	// Draw previous lines
	utf8 lineBuffer[2 + 256], *lineCh;
	ch = _consoleViewBufferStart;
	sint32 currentLine = 0;
	sint32 drawLines = 0;
	while (*ch != 0) {
		// Find line break or null terminator
		utf8 *nextLine = ch;
		while (*nextLine != 0 && *nextLine != '\n') {
			nextLine++;
		}

		currentLine++;
		if (currentLine < (lines - maxLines + 4) - _consoleScrollPos) {
			if (*nextLine == '\n') {
				ch = nextLine + 1;
				x = _consoleLeft + 4;
				// y += lineHeight;
			}
			else {
				break;
			}
			continue;
		}

		if (drawLines >= maxLines)
			break;
		drawLines++;

		size_t lineLength = min(sizeof(lineBuffer) - (size_t)utf8_get_codepoint_length(FORMAT_WHITE), (size_t)(nextLine - ch));
		lineCh = lineBuffer;
		lineCh = utf8_write_codepoint(lineCh, FORMAT_WHITE);
		memcpy(lineCh, ch, lineLength);
		lineCh[lineLength] = 0;

		gfx_draw_string(dpi, lineBuffer, COLOUR_LIGHT_PURPLE | COLOUR_FLAG_OUTLINE | COLOUR_FLAG_INSET, x, y);

		x = gLastDrawStringX;

		if (*nextLine == '\n') {
			ch = nextLine + 1;
			x = _consoleLeft + 4;
			y += lineHeight;
		} else {
			break;
		}
	}
	x = _consoleLeft + 4;
	y = _consoleBottom - 15;

	// Draw current line
	lineCh = lineBuffer;
	lineCh = utf8_write_codepoint(lineCh, FORMAT_WHITE);
	safe_strcpy(lineCh, _consoleCurrentLine, sizeof(lineBuffer) - (lineCh - lineBuffer));
	gfx_draw_string(dpi, lineBuffer, TEXT_COLOUR_255, x, y);

	// Draw caret
	if (_consoleCaretTicks < 15) {
		memcpy(lineBuffer, _consoleCurrentLine, _consoleTextInputSession->SelectionStart);
		lineBuffer[_consoleTextInputSession->SelectionStart] = 0;
		sint32 caretX = x + gfx_get_string_width(lineBuffer);
		sint32 caretY = y + lineHeight;

		gfx_fill_rect(dpi, caretX, caretY, caretX + 6, caretY + 1, PALETTE_INDEX_144);
	}
	gfx_fill_rect(dpi, _consoleLeft, _consoleBottom - 21, _consoleRight, _consoleBottom - 21, PALETTE_INDEX_14);
	gfx_fill_rect(dpi, _consoleLeft, _consoleBottom - 20, _consoleRight, _consoleBottom - 20, PALETTE_INDEX_11);

	gfx_fill_rect(dpi, _consoleLeft, _consoleBottom - 1, _consoleRight, _consoleBottom - 1, PALETTE_INDEX_14);
	gfx_fill_rect(dpi, _consoleLeft, _consoleBottom - 0, _consoleRight, _consoleBottom - 0, PALETTE_INDEX_12);
}

void console_input(sint32 c)
{
	switch (c) {
	case SDL_SCANCODE_ESCAPE:
		console_clear_input();
		console_refresh_caret();
		break;
	case SDL_SCANCODE_RETURN:
		if (_consoleCurrentLine[0] != 0) {
			console_history_add(_consoleCurrentLine);
			console_execute(_consoleCurrentLine);
			console_write_prompt();
			console_clear_input();
			console_refresh_caret();
		}
		break;
	case SDL_SCANCODE_UP:
		if (_consoleHistoryIndex > 0) {
			_consoleHistoryIndex--;
			memcpy(_consoleCurrentLine, _consoleHistory[_consoleHistoryIndex], 256);
		}
		_consoleTextInputSession->Size = strlen(_consoleTextInputSession->Buffer);
		_consoleTextInputSession->Length = utf8_length(_consoleTextInputSession->Buffer);
		_consoleTextInputSession->SelectionStart = strlen(_consoleCurrentLine);
		break;
	case SDL_SCANCODE_DOWN:
		if (_consoleHistoryIndex < _consoleHistoryCount - 1) {
			_consoleHistoryIndex++;
			memcpy(_consoleCurrentLine, _consoleHistory[_consoleHistoryIndex], 256);
			_consoleTextInputSession->Size = strlen(_consoleTextInputSession->Buffer);
			_consoleTextInputSession->Length = utf8_length(_consoleTextInputSession->Buffer);
			_consoleTextInputSession->SelectionStart = strlen(_consoleCurrentLine);
		} else {
			_consoleHistoryIndex = _consoleHistoryCount;
			console_clear_input();
		}
		break;
	}
}

static void console_invalidate()
{
	gfx_set_dirty_blocks(_consoleLeft, _consoleTop, _consoleRight, _consoleBottom);
}

static void console_write_prompt()
{
	console_write("> ");
}

void console_write(const utf8 *src)
{
	size_t charactersRemainingInBuffer = CONSOLE_BUFFER_SIZE - (_consoleBufferPointer - _consoleBuffer) - 1;
	size_t charactersToWrite = strlen(src);
	size_t bufferShift = charactersToWrite - charactersRemainingInBuffer;
	if (charactersToWrite > charactersRemainingInBuffer) {
		memmove(_consoleBuffer, _consoleBuffer + bufferShift, CONSOLE_BUFFER_SIZE - bufferShift);
		_consoleBufferPointer -= bufferShift;
		charactersRemainingInBuffer = CONSOLE_BUFFER_SIZE - (_consoleBufferPointer - _consoleBuffer) - 1;
	}
	safe_strcpy(_consoleBufferPointer, src, charactersRemainingInBuffer);
	_consoleBufferPointer += charactersToWrite;
	console_update_scroll();
}

void console_writeline(const utf8 *src)
{
	console_write(src);
	console_write("\n");
}

void console_writeline_error(const utf8 *src)
{
	safe_strcpy(_consoleErrorBuffer + 1, src, CONSOLE_BUFFER_2_SIZE - 1);
	_consoleErrorBuffer[0] = (utf8)(uint8)FORMAT_RED;
	console_writeline(_consoleErrorBuffer);
}

void console_writeline_warning(const utf8 *src)
{
	safe_strcpy(_consoleErrorBuffer + 1, src, CONSOLE_BUFFER_2_SIZE - 1);
	_consoleErrorBuffer[0] = (utf8)(uint8)FORMAT_YELLOW;
	console_writeline(_consoleErrorBuffer);
}

void console_printf(const utf8 *format, ...)
{
	va_list list;
	va_start(list, format);
	vsnprintf(_consolePrintfBuffer, sizeof(_consolePrintfBuffer), format, list);
	va_end(list);
	console_writeline(_consolePrintfBuffer);
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

static void console_update_scroll()
{
	/*sint32 lines = 0;
	sint32 maxLines = ((_consoleBottom - 22 - _consoleTop) / 10) - 1;
	char *ch = strchr(_consoleBuffer, 0);
	while (ch > _consoleBuffer && lines < maxLines) {
		ch--;
		if (*ch == '\n')
			lines++;
	}

	if (*ch == '\n')
		ch++;
	_consoleViewBufferStart = ch;*/
}

void console_scroll(sint32 delta)
{
	sint32 speed = 3;
	sint32 lines = 0;
	sint32 maxLines = ((_consoleBottom - 22 - _consoleTop) / 10) - 1;
	utf8 *ch = strchr(_consoleBuffer, 0);
	while (ch > _consoleBuffer) {
		ch--;
		if (*ch == '\n')
			lines++;
	}
	if (delta > 0 && _consoleScrollPos + 1 < (lines - maxLines + 4)) {
		_consoleScrollPos = min(_consoleScrollPos + speed, (lines - maxLines + 4));
	}
	else if (delta < 0 && _consoleScrollPos > 0) {
		_consoleScrollPos = max(_consoleScrollPos - speed, 0);
	}
}

void console_clear()
{
	_consoleScrollPos = 0;
	_consoleBuffer[0] = 0;
	_consoleBufferPointer = _consoleBuffer;
}

void console_clear_line()
{
	_consoleCurrentLine[0] = 0;
	console_refresh_caret();
}

void console_refresh_caret()
{
	_consoleCaretTicks = 0;
}

static void console_clear_input()
{
	_consoleCurrentLine[0] = 0;
	if (gConsoleOpen) {
		context_start_text_input(_consoleCurrentLine, sizeof(_consoleCurrentLine));
	}
}

static void console_history_add(const utf8 *src)
{
	if (_consoleHistoryCount >= CONSOLE_HISTORY_SIZE) {
		for (sint32 i = 0; i < _consoleHistoryCount - 1; i++)
			memcpy(_consoleHistory[i], _consoleHistory[i + 1], CONSOLE_INPUT_SIZE);
		_consoleHistoryCount--;
	}
	memcpy(_consoleHistory[_consoleHistoryCount++], src, CONSOLE_INPUT_SIZE);
	_consoleHistoryIndex = _consoleHistoryCount;
}

static sint32 cc_clear(const utf8 **argv, sint32 argc)
{
	console_clear();
	return 0;
}

static sint32 cc_hide(const utf8 **argv, sint32 argc)
{
	console_close();
	return 0;
}

static sint32 cc_echo(const utf8 **argv, sint32 argc)
{
	if (argc > 0)
		console_writeline(argv[0]);
	return 0;
}

static sint32 cc_rides(const utf8 **argv, sint32 argc)
{
	if (argc > 0) {
		if (strcmp(argv[0], "list") == 0) {
			rct_ride *ride;
			sint32 i;
			FOR_ALL_RIDES(i, ride) {
				char name[128];
				format_string(name, 128, ride->name, &ride->name_arguments);
				console_printf("ride: %03d type: %02u subtype %03u operating mode: %02u name: %s", i, ride->type, ride->subtype, ride->mode, name);
			}
		} else if (strcmp(argv[0], "set") == 0) {
			if (argc < 4) {
				if (argc > 1 && strcmp(argv[1], "mode") == 0){
					console_printf("Ride modes are specified using integer IDs as given below:");
					for (sint32 i = 0; i < RIDE_MODE_COUNT; i++) {
						char mode_name[128] = { 0 };
						rct_string_id mode_string_id = RideModeNames[i];
						format_string(mode_name, 128, mode_string_id, 0);
						console_printf("%02d - %s", i, mode_name);
					}
					
				} else {
					console_printf("rides set type <ride id> <ride type>");
					console_printf("rides set mode [<ride id> <operating mode>]");
					console_printf("rides set friction <ride id> <friction value>");
					console_printf("rides set excitement <ride id> <excitement value>");
					console_printf("rides set intensity <ride id> <intensity value>");
					console_printf("rides set nausea <ride id> <nausea value>");
				}
				return 0;
			}
			if (strcmp(argv[1], "type") == 0) {
				bool int_valid[2] = { 0 };
				sint32 ride_index = console_parse_int(argv[2], &int_valid[0]);
				sint32 type = console_parse_int(argv[3], &int_valid[1]);
				if (!int_valid[0] || !int_valid[1]) {
					console_printf("This command expects integer arguments");
				} else if (ride_index < 0) {
					console_printf("Ride index must not be negative");
				} else {
					gGameCommandErrorTitle = STR_CANT_CHANGE_OPERATING_MODE;
					sint32 res = game_do_command(0, (type << 8) | 1, 0, (RIDE_SETTING_RIDE_TYPE << 8) | ride_index, GAME_COMMAND_SET_RIDE_SETTING, 0, 0);
					if (res == MONEY32_UNDEFINED) {
						console_printf("That didn't work");
					}
				}
			}
			else if (strcmp(argv[1], "mode") == 0) {
				bool int_valid[2] = { 0 };
				sint32 ride_index = console_parse_int(argv[2], &int_valid[0]);
				sint32 mode = console_parse_int(argv[3], &int_valid[1]);
				if (!int_valid[0] || !int_valid[1]) {
					console_printf("This command expects integer arguments");
				} else if (ride_index < 0) {
					console_printf("Ride index must not be negative");
				} else {
					rct_ride *ride = get_ride(ride_index);
					if (mode <= 0 || mode > (RIDE_MODE_COUNT - 1)) {
						console_printf("Invalid ride mode.");
					}
					else if (ride == NULL || ride->type == RIDE_TYPE_NULL) {
						console_printf("No ride found with index %d", ride_index);
					}
					else {
						ride->mode = mode;
						invalidate_test_results(ride_index);
					}
				}
			}
			else if (strcmp(argv[1], "friction") == 0) {
				bool int_valid[2] = { 0 };
				sint32 ride_index = console_parse_int(argv[2], &int_valid[0]);
				sint32 friction = console_parse_int(argv[3], &int_valid[1]);

				if (ride_index < 0) {
					console_printf("Ride index must not be negative");
				}
				else if (!int_valid[0] || !int_valid[1]) {
					console_printf("This command expects integer arguments");
				}
				else {
					rct_ride *ride = get_ride(ride_index);
					if (friction <= 0) {
						console_printf("Friction value must be strictly positive");
					}
					else if (ride->type == RIDE_TYPE_NULL) {
						console_printf("No ride found with index %d", ride_index);
					}
					else {
						for (sint32 i = 0; i < ride->num_vehicles; i++) {
							uint16 vehicle_index = ride->vehicles[i];
							while (vehicle_index != SPRITE_INDEX_NULL) {
								rct_vehicle *vehicle = GET_VEHICLE(vehicle_index);
								vehicle->friction = friction;
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
					console_printf("Ride index must not be negative");
				}
				else if (!int_valid[0] || !int_valid[1]) {
					console_printf("This command expects integer arguments");
				}
				else {
					rct_ride *ride = get_ride(ride_index);
					if (excitement <= 0) {
						console_printf("Excitement value must be strictly positive");
					}
					else if (ride->type == RIDE_TYPE_NULL) {
						console_printf("No ride found with index %d", ride_index);
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
					console_printf("Ride index must not be negative");
				}
				else if (!int_valid[0] || !int_valid[1]) {
					console_printf("This command expects integer arguments");
				}
				else {
					rct_ride *ride = get_ride(ride_index);
					if (intensity <= 0) {
						console_printf("Intensity value must be strictly positive");
					}
					else if (ride->type == RIDE_TYPE_NULL) {
						console_printf("No ride found with index %d", ride_index);
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
					console_printf("Ride index must not be negative");
				}
				else if (!int_valid[0] || !int_valid[1]) {
					console_printf("This command expects integer arguments");
				}
				else {
					rct_ride *ride = get_ride(ride_index);
					if (nausea <= 0) {
						console_printf("Nausea value must be strictly positive");
					}
					else if (ride->type == RIDE_TYPE_NULL) {
						console_printf("No ride found with index %d", ride_index);
					}
					else {
						ride->nausea = nausea;
					}
				}
			}
		}
	} else {
		console_printf("subcommands: list, set");
	}
	return 0;
}

static sint32 cc_staff(const utf8 **argv, sint32 argc)
{
	if (argc > 0) {
		if (strcmp(argv[0], "list") == 0) {
			rct_peep *peep;
			sint32 i;
			FOR_ALL_STAFF(i, peep) {
				char name[128];
				format_string(name, 128, peep->name_string_idx, &peep->id);
				console_printf("staff id %03d type: %02u energy %03u name %s", i, peep->staff_type, peep->energy, name);
			}
		} else if (strcmp(argv[0], "set") == 0) {
			if (argc < 4) {
				console_printf("staff set energy <staff id> <value 0-255>");
				console_printf("staff set costume <staff id> <costume id>");
				for (sint32 i = 0; i < ENTERTAINER_COSTUME_COUNT; i++) {
					char costume_name[128] = { 0 };
					rct_string_id costume = StaffCostumeNames[i];
					format_string(costume_name, 128, STR_DROPDOWN_MENU_LABEL, &costume);
					// That's a terrible hack here. Costume names include inline sprites
					// that don't work well with the console, so manually skip past them.
					console_printf("        costume %i: %s", i, costume_name + 7);
				}
				return 0;
			}
			if (strcmp(argv[1], "energy") == 0) {
				sint32 int_val[3];
				bool int_valid[3] = { 0 };
				int_val[0] = console_parse_int(argv[2], &int_valid[0]);
				int_val[1] = console_parse_int(argv[3], &int_valid[1]);

				if (int_valid[0] && int_valid[1] && ((GET_PEEP(int_val[0])) != NULL)) {
					rct_peep *peep = GET_PEEP(int_val[0]);

					peep->energy = int_val[1];
					peep->energy_growth_rate = int_val[1];
				}
			} else if (strcmp(argv[1], "costume") == 0) {
				sint32 int_val[2];
				bool int_valid[2] = { 0 };
				int_val[0] = console_parse_int(argv[2], &int_valid[0]);
				int_val[1] = console_parse_int(argv[3], &int_valid[1]);
				rct_peep *peep = NULL;
				if (!int_valid[0]) {
					console_writeline_error("Invalid staff ID");
					return 1;
				}
				peep = GET_PEEP(int_val[0]);
				bool is_entertainer = peep != NULL && peep->type == PEEP_TYPE_STAFF && peep->staff_type == STAFF_TYPE_ENTERTAINER;
				if (!is_entertainer) {
					console_writeline_error("Specified staff is not entertainer");
					return 1;
				}
				if (!int_valid[1] || int_val[1] < 0 || int_val[1] >= ENTERTAINER_COSTUME_COUNT) {
					console_writeline_error("Invalid costume ID");
					return 1;
				}

				sint32 costume = int_val[1] | 0x80;
				game_do_command(peep->x, (costume << 8) | 1, peep->y, int_val[0], GAME_COMMAND_SET_STAFF_ORDER, 0, 0);
			}
		}
	} else {
		console_printf("subcommands: list, set");
	}
	return 0;
}

static sint32 cc_get(const utf8 **argv, sint32 argc)
{
	if (argc > 0) {
		if (strcmp(argv[0], "park_rating") == 0) {
			console_printf("park_rating %d", gParkRating);
		}
		else if (strcmp(argv[0], "money") == 0) {
			console_printf("money %d.%d0", DECRYPT_MONEY(gCashEncrypted) / 10, DECRYPT_MONEY(gCashEncrypted) % 10);
		}
		else if (strcmp(argv[0], "scenario_initial_cash") == 0) {
			console_printf("scenario_initial_cash %d", gInitialCash / 10);
		}
		else if (strcmp(argv[0], "current_loan") == 0) {
			console_printf("current_loan %d", gBankLoan / 10);
		}
		else if (strcmp(argv[0], "max_loan") == 0) {
			console_printf("max_loan %d", gMaxBankLoan / 10);
		}
		else if (strcmp(argv[0], "guest_initial_cash") == 0) {
			console_printf("guest_initial_cash %d.%d0", gGuestInitialCash / 10, gGuestInitialCash % 10);
		}
		else if (strcmp(argv[0], "guest_initial_happiness") == 0) {
			uint32 current_happiness = gGuestInitialHappiness;
			for (sint32 i = 15; i <= 99; i++) {
				if (i == 99) {
					console_printf("guest_initial_happiness %d%%  (%d)", 15, gGuestInitialHappiness);
				}
				else if (current_happiness == calculate_guest_initial_happiness(i)) {
					console_printf("guest_initial_happiness %d%%  (%d)", i, gGuestInitialHappiness);
					break;
				}
			}
		}
		else if (strcmp(argv[0], "guest_initial_hunger") == 0) {
			console_printf("guest_initial_hunger %d%%  (%d)", ((255 - gGuestInitialHunger) * 100) / 255, gGuestInitialHunger);
		}
		else if (strcmp(argv[0], "guest_initial_thirst") == 0) {
			console_printf("guest_initial_thirst %d%%  (%d)", ((255 - gGuestInitialThirst) * 100) / 255, gGuestInitialThirst);
		}
		else if (strcmp(argv[0], "guest_prefer_less_intense_rides") == 0) {
			console_printf("guest_prefer_less_intense_rides %d", (gParkFlags & PARK_FLAGS_PREF_LESS_INTENSE_RIDES) != 0);
		}
		else if (strcmp(argv[0], "guest_prefer_more_intense_rides") == 0) {
			console_printf("guest_prefer_more_intense_rides %d", (gParkFlags & PARK_FLAGS_PREF_MORE_INTENSE_RIDES) != 0);
		}
		else if (strcmp(argv[0], "forbid_marketing_campagns") == 0) {
			console_printf("forbid_marketing_campagns %d", (gParkFlags & PARK_FLAGS_FORBID_MARKETING_CAMPAIGN) != 0);
		}
		else if (strcmp(argv[0], "forbid_landscape_changes") == 0) {
			console_printf("forbid_landscape_changes %d", (gParkFlags & PARK_FLAGS_FORBID_LANDSCAPE_CHANGES) != 0);
		}
		else if (strcmp(argv[0], "forbid_tree_removal") == 0) {
			console_printf("forbid_tree_removal %d", (gParkFlags & PARK_FLAGS_FORBID_TREE_REMOVAL) != 0);
		}
		else if (strcmp(argv[0], "forbid_high_construction") == 0) {
			console_printf("forbid_high_construction %d", (gParkFlags & PARK_FLAGS_FORBID_HIGH_CONSTRUCTION) != 0);
		}
		else if (strcmp(argv[0], "pay_for_rides") == 0) {
			console_printf("pay_for_rides %d", (gParkFlags & PARK_FLAGS_PARK_FREE_ENTRY) != 0);
		}
		else if (strcmp(argv[0], "no_money") == 0) {
			console_printf("no_money %d", (gParkFlags & PARK_FLAGS_NO_MONEY) != 0);
		}
		else if (strcmp(argv[0], "difficult_park_rating") == 0) {
			console_printf("difficult_park_rating %d", (gParkFlags & PARK_FLAGS_DIFFICULT_PARK_RATING) != 0);
		}
		else if (strcmp(argv[0], "difficult_guest_generation") == 0) {
			console_printf("difficult_guest_generation %d", (gParkFlags & PARK_FLAGS_DIFFICULT_GUEST_GENERATION) != 0);
		}
		else if (strcmp(argv[0], "park_open") == 0) {
			console_printf("park_open %d", (gParkFlags & PARK_FLAGS_PARK_OPEN) != 0);
		}
		else if (strcmp(argv[0], "land_rights_cost") == 0) {
			console_printf("land_rights_cost %d.%d0", gLandPrice / 10, gLandPrice % 10);
		}
		else if (strcmp(argv[0], "construction_rights_cost") == 0) {
			console_printf("construction_rights_cost %d.%d0", gConstructionRightsPrice / 10, gConstructionRightsPrice % 10);
		}
		else if (strcmp(argv[0], "climate") == 0) {
			const utf8* climate_names[] = { "cool_and_wet", "warm", "hot_and_dry", "cold" };
			console_printf("climate %s  (%d)", climate_names[gClimate], gClimate);
		}
		else if (strcmp(argv[0], "game_speed") == 0) {
			console_printf("game_speed %d", gGameSpeed);
		}
		else if (strcmp(argv[0], "console_small_font") == 0) {
			console_printf("console_small_font %d", gConfigInterface.console_small_font);
		}
		else if (strcmp(argv[0], "test_unfinished_tracks") == 0) {
			console_printf("test_unfinished_tracks %d", gConfigGeneral.test_unfinished_tracks);
		}
		else if (strcmp(argv[0], "no_test_crashes") == 0) {
			console_printf("no_test_crashes %d", gConfigGeneral.no_test_crashes);
		}
		else if (strcmp(argv[0], "location") == 0) {
			rct_window *w = window_get_main();
			if (w != NULL) {
				sint32 interactionType;
				rct_map_element *mapElement;
				rct_xy16 mapCoord = { 0 };
				get_map_coordinates_from_pos(w->viewport->view_width / 2, w->viewport->view_height / 2, VIEWPORT_INTERACTION_MASK_TERRAIN, &mapCoord.x, &mapCoord.y, &interactionType, &mapElement, NULL);
				mapCoord.x -= 16;
				mapCoord.x /= 32;
				mapCoord.y -= 16;
				mapCoord.y /= 32;
				mapCoord.x++;
				mapCoord.y++;
				console_printf("location %d %d", mapCoord.x, mapCoord.y);
			}
		}
		else if (strcmp(argv[0], "window_scale") == 0) {
			console_printf("window_scale %.3f", gConfigGeneral.window_scale);
		}
		else if (strcmp(argv[0], "window_limit") == 0) {
			console_printf("window_limit %d", gConfigGeneral.window_limit);
		}
		else if (strcmp(argv[0], "render_weather_effects") == 0) {
			console_printf("render_weather_effects %d", gConfigGeneral.render_weather_effects);
		}
		else if (strcmp(argv[0], "render_weather_gloom") == 0) {
			console_printf("render_weather_gloom %d", gConfigGeneral.render_weather_gloom);
		}
		else if (strcmp(argv[0], "cheat_sandbox_mode") == 0) {
			console_printf("cheat_sandbox_mode %d", gCheatsSandboxMode);
		}
		else if (strcmp(argv[0], "cheat_disable_clearance_checks") == 0) {
			console_printf("cheat_disable_clearance_checks %d", gCheatsDisableClearanceChecks);
		}
		else if (strcmp(argv[0], "cheat_disable_support_limits") == 0) {
			console_printf("cheat_disable_support_limits %d", gCheatsDisableSupportLimits);
		}
		else {
			console_writeline_warning("Invalid variable.");
		}
	}
	return 0;
}
static sint32 cc_set(const utf8 **argv, sint32 argc)
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
			if (gCashEncrypted != ENCRYPT_MONEY(money)) {
				if (game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SETMONEY, money, GAME_COMMAND_CHEAT, 0, 0) != MONEY32_UNDEFINED) {
					// When in networked client mode, console_execute_silent("get money")
					// does not print value accurately. Instead, print the argument.
					if (network_get_mode() == NETWORK_MODE_CLIENT) {
						run_get_money = false;
						console_printf("money %d.%d0", money / 10, money % 10);
					}
				}
				else {
					console_writeline_error("Network error: Permission denied!");
				}
			}
			if (run_get_money) {
				console_execute_silent("get money");
			}
		}
		else if (strcmp(argv[0], "scenario_initial_cash") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
			gInitialCash = clamp(MONEY(int_val[0], 0), MONEY(0, 0), MONEY(1000000, 00));
			console_execute_silent("get scenario_initial_cash");
		}
		else if (strcmp(argv[0], "current_loan") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
			gBankLoan = clamp(MONEY(int_val[0] - (int_val[0] % 1000), 0), MONEY(0, 0), gMaxBankLoan);
			console_execute_silent("get current_loan");
		}
		else if (strcmp(argv[0], "max_loan") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
			gMaxBankLoan = clamp(MONEY(int_val[0] - (int_val[0] % 1000), 0), MONEY(0, 0), MONEY(5000000, 0));
			console_execute_silent("get max_loan");
		}
		else if (strcmp(argv[0], "guest_initial_cash") == 0 && invalidArguments(&invalidArgs, double_valid[0])) {
			gGuestInitialCash = clamp(MONEY((sint32)double_val[0], ((sint32)(double_val[0] * 100)) % 100), MONEY(0, 0), MONEY(1000, 0));
			console_execute_silent("get guest_initial_cash");
		}
		else if (strcmp(argv[0], "guest_initial_happiness") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
			gGuestInitialHappiness = calculate_guest_initial_happiness((uint8)int_val[0]);
			console_execute_silent("get guest_initial_happiness");
		}
		else if (strcmp(argv[0], "guest_initial_hunger") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
			gGuestInitialHunger = (clamp(int_val[0], 1, 84) * 255 / 100 - 255) * -1;
			console_execute_silent("get guest_initial_hunger");
		}
		else if (strcmp(argv[0], "guest_initial_thirst") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
			gGuestInitialThirst = (clamp(int_val[0], 1, 84) * 255 / 100 - 255) * -1;
			console_execute_silent("get guest_initial_thirst");
		}
		else if (strcmp(argv[0], "guest_prefer_less_intense_rides") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
			SET_FLAG(gParkFlags, PARK_FLAGS_PREF_LESS_INTENSE_RIDES, int_val[0]);
			console_execute_silent("get guest_prefer_less_intense_rides");
		}
		else if (strcmp(argv[0], "guest_prefer_more_intense_rides") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
			SET_FLAG(gParkFlags, PARK_FLAGS_PREF_MORE_INTENSE_RIDES, int_val[0]);
			console_execute_silent("get guest_prefer_more_intense_rides");
		}
		else if (strcmp(argv[0], "forbid_marketing_campagns") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
			SET_FLAG(gParkFlags, PARK_FLAGS_FORBID_MARKETING_CAMPAIGN, int_val[0]);
			console_execute_silent("get forbid_marketing_campagns");
		}
		else if (strcmp(argv[0], "forbid_landscape_changes") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
			SET_FLAG(gParkFlags, PARK_FLAGS_FORBID_LANDSCAPE_CHANGES, int_val[0]);
			console_execute_silent("get forbid_landscape_changes");
		}
		else if (strcmp(argv[0], "forbid_tree_removal") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
			SET_FLAG(gParkFlags, PARK_FLAGS_FORBID_TREE_REMOVAL, int_val[0]);
			console_execute_silent("get forbid_tree_removal");
		}
		else if (strcmp(argv[0], "forbid_high_construction") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
			SET_FLAG(gParkFlags, PARK_FLAGS_FORBID_HIGH_CONSTRUCTION, int_val[0]);
			console_execute_silent("get forbid_high_construction");
		}
		else if (strcmp(argv[0], "pay_for_rides") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
			SET_FLAG(gParkFlags, PARK_FLAGS_PARK_FREE_ENTRY, int_val[0]);
			console_execute_silent("get pay_for_rides");
		}
		else if (strcmp(argv[0], "no_money") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
			SET_FLAG(gParkFlags, PARK_FLAGS_NO_MONEY, int_val[0]);
			console_execute_silent("get no_money");
		}
		else if (strcmp(argv[0], "difficult_park_rating") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
			SET_FLAG(gParkFlags, PARK_FLAGS_DIFFICULT_PARK_RATING, int_val[0]);
			console_execute_silent("get difficult_park_rating");
		}
		else if (strcmp(argv[0], "difficult_guest_generation") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
			SET_FLAG(gParkFlags, PARK_FLAGS_DIFFICULT_GUEST_GENERATION, int_val[0]);
			console_execute_silent("get difficult_guest_generation");
		}
		else if (strcmp(argv[0], "park_open") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
			SET_FLAG(gParkFlags, PARK_FLAGS_PARK_OPEN, int_val[0]);
			console_execute_silent("get park_open");
		}
		else if (strcmp(argv[0], "land_rights_cost") == 0 && invalidArguments(&invalidArgs, double_valid[0])) {
			gLandPrice = clamp(MONEY((sint32)double_val[0], ((sint32)(double_val[0] * 100)) % 100), MONEY(0, 0), MONEY(200, 0));
			console_execute_silent("get land_rights_cost");
		}
		else if (strcmp(argv[0], "construction_rights_cost") == 0 && invalidArguments(&invalidArgs, double_valid[0])) {
			gConstructionRightsPrice = clamp(MONEY((sint32)double_val[0], ((sint32)(double_val[0] * 100)) % 100), MONEY(0, 0), MONEY(200, 0));
			console_execute_silent("get construction_rights_cost");
		}
		else if (strcmp(argv[0], "climate") == 0) {
			if (int_valid[0]) {
				gClimate = clamp(int_val[0], 0, 3);
			}
			else {
				utf8* climate_names[] = { "cool_and_wet", "warm", "hot_and_dry", "cold" };
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
			console_execute_silent("get climate");
		}
		else if (strcmp(argv[0], "game_speed") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
			gGameSpeed = clamp(int_val[0], 1, 8);
			console_execute_silent("get game_speed");
		}
		else if (strcmp(argv[0], "console_small_font") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
			gConfigInterface.console_small_font = (int_val[0] != 0);
			config_save_default();
			console_execute_silent("get console_small_font");
		}
		else if (strcmp(argv[0], "test_unfinished_tracks") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
			gConfigGeneral.test_unfinished_tracks = (int_val[0] != 0);
			config_save_default();
			console_execute_silent("get test_unfinished_tracks");
		}
		else if (strcmp(argv[0], "no_test_crashes") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
			gConfigGeneral.no_test_crashes = (int_val[0] != 0);
			config_save_default();
			console_execute_silent("get no_test_crashes");
		}
		else if (strcmp(argv[0], "location") == 0 && invalidArguments(&invalidArgs, int_valid[0] && int_valid[1])) {
			rct_window *w = window_get_main();
			if (w != NULL) {
				sint32 x = (sint16)(int_val[0] * 32 + 16);
				sint32 y = (sint16)(int_val[1] * 32 + 16);
				sint32 z = map_element_height(x, y);
				window_set_location(w, x, y, z);
				viewport_update_position(w);
				console_execute_silent("get location");
			}
		}
		else if (strcmp(argv[0], "window_scale") == 0 && invalidArguments(&invalidArgs, double_valid[0])) {
			float newScale = (float)(0.001*trunc(1000*double_val[0]));
			gConfigGeneral.window_scale = clamp(newScale, 0.5f, 5.0f);
			config_save_default();
			gfx_invalidate_screen();
			context_trigger_resize();
			console_execute_silent("get window_scale");
		}
		else if (strcmp(argv[0], "window_limit") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
			window_set_window_limit(int_val[0]);
			console_execute_silent("get window_limit");
		}
		else if (strcmp(argv[0], "render_weather_effects") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
			gConfigGeneral.render_weather_effects = (int_val[0] != 0);
			config_save_default();
			console_execute_silent("get render_weather_effects");
		}
		else if (strcmp(argv[0], "render_weather_gloom") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
			gConfigGeneral.render_weather_gloom = (int_val[0] != 0);
			config_save_default();
			console_execute_silent("get render_weather_gloom");
		}
		else if (strcmp(argv[0], "cheat_sandbox_mode") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
			if (gCheatsSandboxMode != (int_val[0] != 0)) {
				if (game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SANDBOXMODE, (int_val[0] != 0), GAME_COMMAND_CHEAT, 0, 0) != MONEY32_UNDEFINED) {
					// Change it locally so it shows the accurate value in the
					// "console_execute_silent("get cheat_sandbox_mode")" line when in networked client mode
					gCheatsSandboxMode = (int_val[0] != 0);
				}
				else {
					console_writeline_error("Network error: Permission denied!");
				}
			}
			console_execute_silent("get cheat_sandbox_mode");
		}
		else if (strcmp(argv[0], "cheat_disable_clearance_checks") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
			if (gCheatsDisableClearanceChecks != (int_val[0] != 0)) {
				if (game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_DISABLECLEARANCECHECKS, (int_val[0] != 0), GAME_COMMAND_CHEAT, 0, 0) != MONEY32_UNDEFINED) {
					// Change it locally so it shows the accurate value in the
					// "console_execute_silent("get cheat_disable_clearance_checks")" line when in networked client mode
					gCheatsDisableClearanceChecks = (int_val[0] != 0);
				}
				else {
					console_writeline_error("Network error: Permission denied!");
				}
			}
			console_execute_silent("get cheat_disable_clearance_checks");
		}
		else if (strcmp(argv[0], "cheat_disable_support_limits") == 0 && invalidArguments(&invalidArgs, int_valid[0])) {
			if (gCheatsDisableSupportLimits != (int_val[0] != 0)) {
				if (game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_DISABLESUPPORTLIMITS, (int_val[0] != 0), GAME_COMMAND_CHEAT, 0, 0) != MONEY32_UNDEFINED) {
					// Change it locally so it shows the accurate value in the
					// "console_execute_silent("get cheat_disable_support_limits")" line when in networked client mode
					gCheatsDisableSupportLimits = (int_val[0] != 0);
				}
				else {
					console_writeline_error("Network error: Permission denied!");
				}
			}
			console_execute_silent("get cheat_disable_support_limits");
		}
		else if (invalidArgs) {
			console_writeline_error("Invalid arguments.");
		}
		else {
			console_writeline_error("Invalid variable.");
		}

		gfx_invalidate_screen();
	}
	else {
		console_writeline_error("Value required.");
	}
	return 0;
}
static sint32 cc_twitch(const utf8 **argv, sint32 argc)
{
#ifdef DISABLE_TWITCH
	console_writeline_error("OpenRCT2 build not compiled with Twitch integration.");
#else
	// TODO: Add some twitch commands
#endif
	return 0;
}

static sint32 cc_load_object(const utf8 **argv, sint32 argc) {
	if (argc > 0) {
		char name[9] = { 0 };
		memset(name, ' ', 8);
		sint32 i = 0;
		for (const char * ch = argv[0]; *ch != '\0' && i < 8; ch++) {
			name[i++] = *ch;
		}

		const ObjectRepositoryItem * ori = object_repository_find_object_by_name(name);
		if (ori == NULL) {
			console_writeline_error("Could not find the object.");
			return 1;
		}

		const rct_object_entry * entry = &ori->ObjectEntry;
		void * loadedObject = object_manager_get_loaded_object(entry);
		if (loadedObject != NULL) {
			console_writeline_error("Object is already in scenario.");
			return 1;
		}

		loadedObject = object_manager_load_object(entry);
		if (loadedObject == NULL) {
			console_writeline_error("Unable to load object.");
			return 1;
		}
		sint32 groupIndex = object_manager_get_loaded_object_entry_index(loadedObject);

		uint8 objectType = entry->flags & 0x0F;
		if (objectType == OBJECT_TYPE_RIDE) {
			// Automatically research the ride so it's supported by the game.
			rct_ride_entry *rideEntry;
			sint32 rideType;

			rideEntry = get_ride_entry(groupIndex);

			for (sint32 j = 0; j < MAX_RIDE_TYPES_PER_RIDE_ENTRY; j++) {
				rideType = rideEntry->ride_type[j];
				if (rideType != 255)
					research_insert(true, 0x10000 | (rideType << 8) | groupIndex, rideEntry->category[0]);
			}

			gSilentResearch = true;
			sub_684AC3();
			gSilentResearch = false;
		}
		else if (objectType == OBJECT_TYPE_SCENERY_SETS) {
			research_insert(true, groupIndex, RESEARCH_CATEGORY_SCENERYSET);

			gSilentResearch = true;
			sub_684AC3();
			gSilentResearch = false;
		}
		scenery_set_default_placement_configuration();
		window_new_ride_init_vars();

		gWindowUpdateTicks = 0;
		gfx_invalidate_screen();
		console_writeline("Object file loaded.");
	}

	return 0;
}

static sint32 cc_object_count(const utf8 **argv, sint32 argc) {
	const utf8* object_type_names[] = { "Rides", "Small scenery", "Large scenery", "Walls", "Banners", "Paths", "Path Additions", "Scenery groups", "Park entrances", "Water" };
	for (sint32 i = 0; i < 10; i++) {

		sint32 entryGroupIndex = 0;
		for (; entryGroupIndex < object_entry_group_counts[i]; entryGroupIndex++){
			if (object_entry_groups[i].chunks[entryGroupIndex] == (uint8*)-1){
				break;
			}
		}
		console_printf("%s: %d/%d", object_type_names[i], entryGroupIndex, object_entry_group_counts[i]);
	}

	return 0;
}

static sint32 cc_reset_user_strings(const utf8 **argv, sint32 argc)
{
	reset_user_strings();
	return 0;
}

static sint32 cc_fix_banner_count(const utf8 **argv, sint32 argc)
{
	banner_reset_broken_index();
	return 0;
}

static sint32 cc_open(const utf8 **argv, sint32 argc) {
	if (argc > 0) {
		bool title = (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) != 0;
		bool invalidTitle = false;
		if (strcmp(argv[0], "object_selection") == 0 && invalidArguments(&invalidTitle, !title)) {
			// Only this window should be open for safety reasons
			window_close_all();
			window_editor_object_selection_open();
		} else if (strcmp(argv[0], "inventions_list") == 0 && invalidArguments(&invalidTitle, !title)) {
			window_editor_inventions_list_open();
		} else if (strcmp(argv[0], "scenario_options") == 0 && invalidArguments(&invalidTitle, !title)) {
			window_editor_scenario_options_open();
		} else if (strcmp(argv[0], "options") == 0) {
			window_options_open();
		} else if (strcmp(argv[0], "themes") == 0) {
			window_themes_open();
		} else if (strcmp(argv[0], "title_sequences") == 0) {
			window_title_editor_open(0);
		} else if (invalidTitle) {
			console_writeline_error("Cannot open this window in the title screen.");
		} else {
			console_writeline_error("Invalid window.");
		}
	}
	return 0;
}


typedef sint32 (*console_command_func)(const utf8 **argv, sint32 argc);
typedef struct console_command {
	utf8 *command;
	console_command_func func;
	utf8 *help;
	utf8 *usage;
} console_command;

utf8* console_variable_table[] = {
	"park_rating",
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
utf8* console_window_table[] = {
	"object_selection",
	"inventions_list",
	"scenario_options",
	"options",
	"themes",
	"title_sequences"
};

console_command console_command_table[] = {
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
	{ "fix_banner_count", cc_fix_banner_count, "Fixes incorrectly appearing 'Too many banners' error by marking every banner entry without a map element as null.", "fix_banner_count" },
	{ "rides", cc_rides, "Ride management.", "rides <subcommand>" },
	{ "staff", cc_staff, "Staff management.", "staff <subcommand>"},
};

static sint32 cc_windows(const utf8 **argv, sint32 argc) {
	for (sint32 i = 0; i < countof(console_window_table); i++)
		console_writeline(console_window_table[i]);
	return 0;
}
static sint32 cc_variables(const utf8 **argv, sint32 argc)
{
	for (sint32 i = 0; i < countof(console_variable_table); i++)
		console_writeline(console_variable_table[i]);
	return 0;
}

static sint32 cc_help(const utf8 **argv, sint32 argc)
{
	if (argc > 0) {
		for (sint32 i = 0; i < countof(console_command_table); i++) {
			if (strcmp(console_command_table[i].command, argv[0]) == 0) {
				console_writeline(console_command_table[i].help);
				console_printf("\nUsage:   %s", console_command_table[i].usage);
			}
		}
	}
	else {
		console_write_all_commands();
	}
	return 0;
}

static void console_write_all_commands()
{
	for (sint32 i = 0; i < countof(console_command_table); i++)
		console_writeline(console_command_table[i].command);
}

void console_execute(const utf8 *src)
{
	console_writeline(src);

	console_execute_silent(src);
}

void console_execute_silent(const utf8 *src)
{
	sint32 argc = 0;
	sint32 argvCapacity = 8;
	utf8 **argv = malloc(argvCapacity * sizeof(utf8*));
	const utf8 *start = src;
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
			utf8 *arg = malloc(length + 1);
			memcpy(arg, start, length);
			arg[length] = 0;

			if (argc >= argvCapacity) {
				argvCapacity *= 2;
				argv = realloc(argv, argvCapacity * sizeof(utf8*));
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
	for (sint32 i = 0; i < countof(console_command_table); i++) {
		if (strcmp(argv[0], console_command_table[i].command) == 0) {
			console_command_table[i].func((const utf8 **)(argv + 1), argc - 1);
			validCommand = true;
			break;
		}
	}

	for (sint32 i = 0; i < argc; i++)
		free(argv[i]);
	free(argv);

	if (!validCommand) {
		utf8 output[128];
		utf8 *dst = output;
		dst = utf8_write_codepoint(dst, FORMAT_TOPAZ);
		safe_strcpy(dst, "Unknown command. Type help to list available commands.", sizeof(output) - (dst - output));
		console_writeline(output);
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
