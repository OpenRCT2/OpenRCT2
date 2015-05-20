#include <stdarg.h>
#include <SDL_scancode.h>
#include "../addresses.h"
#include "../drawing/drawing.h"
#include "../localisation/localisation.h"
#include "../platform/platform.h"
#include "../world/park.h"
#include "../config.h"
#include "../cursors.h"
#include "../game.h"
#include "../input.h"
#include "../object.h"
#include "console.h"
#include "window.h"

#define CONSOLE_BUFFER_SIZE 8192
#define CONSOLE_BUFFER_2_SIZE 256
#define CONSOLE_HISTORY_SIZE 64
#define CONSOLE_INPUT_SIZE 256

bool gConsoleOpen = false;

static bool _consoleInitialised = false;
static int _consoleLeft, _consoleTop, _consoleRight, _consoleBottom;
static int _lastMainViewportX, _lastMainViewportY;
static char _consoleBuffer[CONSOLE_BUFFER_SIZE] = { 0 };
static char *_consoleBufferPointer = _consoleBuffer;
static char *_consoleViewBufferStart = _consoleBuffer;
static char _consoleCurrentLine[CONSOLE_INPUT_SIZE];
static char *_consoleCurrentLinePointer = _consoleCurrentLine;
static int _consoleCaretTicks;
static char _consolePrintfBuffer[CONSOLE_BUFFER_2_SIZE];
static char _consoleErrorBuffer[CONSOLE_BUFFER_2_SIZE];
static int _consoleScrollPos = 0;

static char _consoleHistory[CONSOLE_HISTORY_SIZE][CONSOLE_INPUT_SIZE];
static int _consoleHistoryIndex = 0;
static int _consoleHistoryCount = 0;

static void console_invalidate();
static void console_write_prompt();
static void console_update_scroll();
static void console_clear_input();
static void console_history_add(const char *src);
static void console_write_all_commands();
static int console_parse_int(const char *src, bool *valid);
static double console_parse_double(const char *src, bool *valid);

static int cc_variables(const char **argv, int argc);
static int cc_help(const char **argv, int argc);

#define SET_FLAG(variable, flag, value) {if (value) variable |= flag; else variable &= ~flag;}

void console_open()
{
	gConsoleOpen = true;
	_consoleScrollPos = 0;
	console_refresh_carot();
	console_update_scroll();
	platform_start_text_input(_consoleCurrentLine, sizeof(_consoleCurrentLine));
}

void console_close()
{
	gConsoleOpen = false;
	console_invalidate();
	platform_stop_text_input();
}

void console_toggle()
{
	if (gConsoleOpen)
		console_close();
	else
		console_open();
}

void console_init()
{
	_consoleInitialised = true;
	console_writeline(OPENRCT2_NAME " " OPENRCT2_VERSION);
	console_writeline("");
	console_write_prompt();
}

void console_update()
{
	if (!_consoleInitialised)
		console_init();

	_consoleLeft = 0;
	_consoleTop = 0;
	_consoleRight = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16);
	_consoleBottom = 322;

	if (gConsoleOpen) {
		console_invalidate();

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

		// Remove unwated characters in console input
		unsigned char *ch = (unsigned char*)_consoleCurrentLine;
		while (*ch != 0) {
			if (*ch < 32 || *ch > 126)
				*ch = ' ';
			ch++;
		}
	}

	// Flash the caret
	_consoleCaretTicks = (_consoleCaretTicks + 1) % 30;
}

void console_draw(rct_drawpixelinfo *dpi)
{
	if (!gConsoleOpen)
		return;
	int lines = 0;
	int maxLines = ((_consoleBottom - 22 - _consoleTop) / 10) - 1;
	char *ch = strchr(_consoleBuffer, 0);
	while (ch > _consoleBuffer) {
		ch--;
		if (*ch == '\n')
			lines++;
	}

	// Set font
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = (gConfigInterface.console_small_font ? 0 : 224);
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_FLAGS, uint16) = 0;

	// Background
	gfx_fill_rect(dpi, _consoleLeft, _consoleTop, _consoleRight, _consoleBottom, 10);

	int x = _consoleLeft + 4;
	int y = _consoleTop + 4;
	
	// Draw previous lines
	char lineBuffer[1 + 256];
	ch = _consoleViewBufferStart;
	int currentLine = 0;
	int drawLines = 0;
	while (*ch != 0) {
		// Find line break or null terminator
		char *nextLine = ch;
		while (*nextLine != 0 && *nextLine != '\n') {
			nextLine++;
		}

		currentLine++;
		if (currentLine < (lines - maxLines + 4) - _consoleScrollPos) {
			if (*nextLine == '\n') {
				ch = nextLine + 1;
				x = _consoleLeft + 4;
				//y += 10;
			}
			else {
				break;
			}
			continue;
		}

		if (drawLines >= maxLines)
			break;
		drawLines++;

		int lineLength = min(sizeof(lineBuffer) - 1, nextLine - ch);
		strncpy(lineBuffer + 1, ch, lineLength);
		lineBuffer[0] = FORMAT_GREEN;
		lineBuffer[1 + lineLength] = 0;

		gfx_draw_string(dpi, lineBuffer, 255, x, y);

		x = gLastDrawStringX;

		if (*nextLine == '\n') {
			ch = nextLine + 1;
			x = _consoleLeft + 4;
			y += 10;
		} else {
			break;
		}
	}
	x = _consoleLeft + 4;
	y = _consoleBottom - 15;
	// Draw current line
	strcpy(lineBuffer + 1, _consoleCurrentLine);
	lineBuffer[0] = FORMAT_GREEN;
	gfx_draw_string(dpi, lineBuffer, 255, x, y);

	// Draw caret
	if (_consoleCaretTicks < 15) {
		memcpy(lineBuffer, _consoleCurrentLine, gTextInputCursorPosition);
		lineBuffer[gTextInputCursorPosition] = 0;
		int caretX = x + gfx_get_string_width(lineBuffer);
		int caretY = y + 10;

		gfx_fill_rect(dpi, caretX, caretY, caretX + 6, caretY + 1, FORMAT_GREEN);
	}
	gfx_fill_rect(dpi, _consoleLeft, _consoleBottom - 21, _consoleRight, _consoleBottom - 21, 14);
	gfx_fill_rect(dpi, _consoleLeft, _consoleBottom - 20, _consoleRight, _consoleBottom - 20, 11);

	gfx_fill_rect(dpi, _consoleLeft, _consoleBottom - 1, _consoleRight, _consoleBottom - 1, 14);
	gfx_fill_rect(dpi, _consoleLeft, _consoleBottom - 0, _consoleRight, _consoleBottom - 0, 12);
}

void console_input(int c)
{
	switch (c) {
	case SDL_SCANCODE_ESCAPE:
		console_clear_input();
		console_refresh_carot();
		break;
	case SDL_SCANCODE_RETURN:
		if (_consoleCurrentLine[0] != 0) {
			console_history_add(_consoleCurrentLine);
			console_execute(_consoleCurrentLine);
			console_write_prompt();
			console_clear_input();
			console_refresh_carot();
		}
		break;
	case SDL_SCANCODE_UP:
		if (_consoleHistoryIndex > 0) {
			_consoleHistoryIndex--;
			memcpy(_consoleCurrentLine, _consoleHistory[_consoleHistoryIndex], 256);
		}
		gTextInputCursorPosition = strlen(_consoleCurrentLine);
		gTextInputLength = gTextInputCursorPosition;
		break;
	case SDL_SCANCODE_DOWN:
		if (_consoleHistoryIndex < _consoleHistoryCount - 1) {
			_consoleHistoryIndex++;
			memcpy(_consoleCurrentLine, _consoleHistory[_consoleHistoryIndex], 256);
			gTextInputCursorPosition = strlen(_consoleCurrentLine);
			gTextInputLength = gTextInputCursorPosition;
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

void console_write(const char *src)
{
	int charactersRemainingInBuffer = CONSOLE_BUFFER_SIZE - (_consoleBufferPointer - _consoleBuffer) - 1;
	int charactersToWrite = strlen(src);
	int bufferShift = charactersToWrite - charactersRemainingInBuffer;
	if (charactersToWrite > charactersRemainingInBuffer) {
		memmove(_consoleBuffer, _consoleBuffer + bufferShift, CONSOLE_BUFFER_SIZE - bufferShift);
		_consoleBufferPointer -= bufferShift;
	}
	strcpy(_consoleBufferPointer, src);
	_consoleBufferPointer += charactersToWrite;
	console_update_scroll();
}

void console_writeline(const char *src)
{
	console_write(src);
	console_write("\n");
}

void console_writeline_error(const char *src)
{
	strcpy(_consoleErrorBuffer + 1, src);
	_consoleErrorBuffer[0] = FORMAT_RED;
	console_writeline(_consoleErrorBuffer);
}

void console_writeline_warning(const char *src)
{
	strcpy(_consoleErrorBuffer + 1, src);
	_consoleErrorBuffer[0] = FORMAT_YELLOW;
	console_writeline(_consoleErrorBuffer);
}

void console_printf(const char *format, ...)
{
	va_list list;
	va_start(list, format);
	
	vsprintf(_consolePrintfBuffer, format, list);
	console_writeline(_consolePrintfBuffer);
}

int console_parse_int(const char *src, bool *valid) {
	char *end;
	int value;
	value = strtol(src, &end, 10); *valid &= (*end == '\0');
	return value;
}

double console_parse_double(const char *src, bool *valid) {
	char *end;
	double value;
	value = strtod(src, &end); *valid &= (*end == '\0');
	return value;
}

static void console_update_scroll()
{
	/*int lines = 0;
	int maxLines = ((_consoleBottom - 22 - _consoleTop) / 10) - 1;
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

void console_scroll(int delta)
{
	int speed = 3;
	int lines = 0;
	int maxLines = ((_consoleBottom - 22 - _consoleTop) / 10) - 1;
	char *ch = strchr(_consoleBuffer, 0);
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
	console_refresh_carot();
}

void console_refresh_carot()
{
	_consoleCaretTicks = 0;
}

static void console_clear_input()
{
	_consoleCurrentLine[0] = 0;
	gTextInputCursorPosition = 0;
	gTextInputLength = 0;
}

static void console_history_add(const char *src)
{
	if (_consoleHistoryCount >= CONSOLE_HISTORY_SIZE) {
		for (int i = 0; i < _consoleHistoryCount - 1; i++)
			memcpy(_consoleHistory[i], _consoleHistory[i + 1], CONSOLE_INPUT_SIZE);
		_consoleHistoryCount--;
	}
	memcpy(_consoleHistory[_consoleHistoryCount++], src, CONSOLE_INPUT_SIZE);
	_consoleHistoryIndex = _consoleHistoryCount;
}

static int cc_clear(const char **argv, int argc)
{
	console_clear();
	return 0;
}

static int cc_echo(const char **argv, int argc)
{
	if (argc > 0)
		console_writeline(argv[0]);
	return 0;
}

static int cc_get(const char **argv, int argc)
{
	if (argc > 0) {
		if (strcmp(argv[0], "park_rating") == 0) {
			console_printf("park_rating %d", RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PARK_RATING, sint16));
		}
		else if (strcmp(argv[0], "money") == 0) {
			console_printf("money %d.%d0", DECRYPT_MONEY(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONEY_ENCRYPTED, money32)) / 10, DECRYPT_MONEY(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONEY_ENCRYPTED, money32)) % 10);
		}
		else if (strcmp(argv[0], "current_loan") == 0) {
			console_printf("current_loan %d", RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_LOAN, money32) / 10);
		}
		else if (strcmp(argv[0], "max_loan") == 0) {
			console_printf("max_loan %d", RCT2_GLOBAL(RCT2_ADDRESS_MAXIMUM_LOAN, money32) / 10);
		}
		else if (strcmp(argv[0], "guest_initial_cash") == 0) {
			console_printf("guest_initial_cash %d.%d0", RCT2_GLOBAL(RCT2_ADDRESS_GUEST_INITIAL_CASH, money16) / 10, RCT2_GLOBAL(RCT2_ADDRESS_GUEST_INITIAL_CASH, money16) % 10);
		}
		else if (strcmp(argv[0], "guest_initial_happiness") == 0) {
			uint32 current_happiness = RCT2_GLOBAL(RCT2_ADDRESS_GUEST_INITIAL_HAPPINESS, uint8);
			for (int i = 15; i <= 99; i++) {
				if (i == 99) {
					console_printf("guest_initial_happiness %d%%  (%d)", 15, RCT2_GLOBAL(RCT2_ADDRESS_GUEST_INITIAL_HAPPINESS, uint8));
				}
				else if (current_happiness == calculate_guest_initial_happiness(i)) {
					console_printf("guest_initial_happiness %d%%  (%d)", i, RCT2_GLOBAL(RCT2_ADDRESS_GUEST_INITIAL_HAPPINESS, uint8));
					break;
				}
			}
		}
		else if (strcmp(argv[0], "guest_initial_hunger") == 0) {
			console_printf("guest_initial_hunger %d%%  (%d)", ((255 - RCT2_GLOBAL(RCT2_ADDRESS_GUEST_INITIAL_HUNGER, uint8)) * 100) / 255, RCT2_GLOBAL(RCT2_ADDRESS_GUEST_INITIAL_HUNGER, uint8));
		}
		else if (strcmp(argv[0], "guest_initial_thirst") == 0) {
			console_printf("guest_initial_thirst %d%%  (%d)", ((255 - RCT2_GLOBAL(RCT2_ADDRESS_GUEST_INITIAL_THIRST, uint8)) * 100) / 255, RCT2_GLOBAL(RCT2_ADDRESS_GUEST_INITIAL_THIRST, uint8));
		}
		else if (strcmp(argv[0], "guest_prefer_less_intense_rides") == 0) {
			console_printf("guest_prefer_less_intense_rides %d", (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_PREF_LESS_INTENSE_RIDES) != 0);
		}
		else if (strcmp(argv[0], "guest_prefer_more_intense_rides") == 0) {
			console_printf("guest_prefer_more_intense_rides %d", (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_PREF_MORE_INTENSE_RIDES) != 0);
		}
		else if (strcmp(argv[0], "forbid_marketing_campagns") == 0) {
			console_printf("forbid_marketing_campagns %d", (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_FORBID_MARKETING_CAMPAIGN) != 0);
		}
		else if (strcmp(argv[0], "forbid_landscape_changes") == 0) {
			console_printf("forbid_landscape_changes %d", (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_FORBID_LANDSCAPE_CHANGES) != 0);
		}
		else if (strcmp(argv[0], "forbid_tree_removal") == 0) {
			console_printf("forbid_tree_removal %d", (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_FORBID_TREE_REMOVAL) != 0);
		}
		else if (strcmp(argv[0], "forbid_high_construction") == 0) {
			console_printf("forbid_high_construction %d", (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_FORBID_HIGH_CONSTRUCTION) != 0);
		}
		else if (strcmp(argv[0], "pay_for_rides") == 0) {
			console_printf("pay_for_rides %d", (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_PARK_FREE_ENTRY) != 0);
		}
		else if (strcmp(argv[0], "no_money") == 0) {
			console_printf("no_money %d", (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY) != 0);
		}
		else if (strcmp(argv[0], "difficult_park_rating") == 0) {
			console_printf("difficult_park_rating %d", (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_DIFFICULT_PARK_RATING) != 0);
		}
		else if (strcmp(argv[0], "difficult_guest_generation") == 0) {
			console_printf("difficult_guest_generation %d", (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_DIFFICULT_GUEST_GENERATION) != 0);
		}
		else if (strcmp(argv[0], "park_open") == 0) {
			console_printf("park_open %d", (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_PARK_OPEN) != 0);
		}
		else if (strcmp(argv[0], "land_rights_cost") == 0) {
			console_printf("land_rights_cost %d.%d0", RCT2_GLOBAL(RCT2_ADDRESS_LAND_COST, money16) / 10, RCT2_GLOBAL(RCT2_ADDRESS_LAND_COST, money16) % 10);
		}
		else if (strcmp(argv[0], "construction_rights_cost") == 0) {
			console_printf("construction_rights_cost %d.%d0", RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCTION_RIGHTS_COST, money32) / 10, RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCTION_RIGHTS_COST, money32) % 10);
		}
		else if (strcmp(argv[0], "climate") == 0) {
			const char* climate_names[] = { "cool_and_wet", "warm", "hot_and_dry", "cold" };
			console_printf("climate %s  (%d)", climate_names[RCT2_GLOBAL(RCT2_ADDRESS_CLIMATE, sint8)], RCT2_GLOBAL(RCT2_ADDRESS_CLIMATE, sint8));
		}
		else if (strcmp(argv[0], "game_speed") == 0) {
			console_printf("game_speed %d", gGameSpeed);
		}
		else if (strcmp(argv[0], "console_small_font") == 0) {
			console_printf("console_small_font %d", gConfigInterface.console_small_font);
		}
		else {
			console_writeline_warning("Invalid variable.");
		}
	}
	return 0;
}
static int cc_set(const char **argv, int argc)
{
	if (argc > 1) {
		bool int_valid = true, double_valid = true;

		int int_val = console_parse_int(argv[1], &int_valid);
		double double_val = console_parse_double(argv[1], &double_valid);

		if (strcmp(argv[0], "money") == 0 && double_valid) {
			RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONEY_ENCRYPTED, money32) = ENCRYPT_MONEY(MONEY((int)double_val, ((int)(double_val * 100)) % 100));
			console_execute_silent("get money");
		}
		else if (strcmp(argv[0], "current_loan") == 0 && int_valid) {
			RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_LOAN, money32) = clamp(MONEY(int_val - (int_val % 1000), 0), MONEY(0, 0), RCT2_GLOBAL(RCT2_ADDRESS_MAXIMUM_LOAN, money32));
			console_execute_silent("get current_loan");
		}
		else if (strcmp(argv[0], "max_loan") == 0 && int_valid) {
			RCT2_GLOBAL(RCT2_ADDRESS_MAXIMUM_LOAN, money32) = clamp(MONEY(int_val - (int_val % 1000), 0), MONEY(0, 0), MONEY(5000000, 0));
			console_execute_silent("get max_loan");
		}
		else if (strcmp(argv[0], "guest_initial_cash") == 0 && double_valid) {
			RCT2_GLOBAL(RCT2_ADDRESS_GUEST_INITIAL_CASH, money16) = clamp(MONEY((int)double_val, ((int)(double_val * 100)) % 100), MONEY(0, 0), MONEY(15000, 0));
			console_execute_silent("get guest_initial_cash");
		}
		else if (strcmp(argv[0], "guest_initial_happiness") == 0 && int_valid) {
			RCT2_GLOBAL(RCT2_ADDRESS_GUEST_INITIAL_HAPPINESS, uint8) = calculate_guest_initial_happiness((uint8)int_val);
			console_execute_silent("get guest_initial_happiness");
		}
		else if (strcmp(argv[0], "guest_initial_hunger") == 0 && int_valid) {
			RCT2_GLOBAL(RCT2_ADDRESS_GUEST_INITIAL_HUNGER, uint8) = (clamp(int_val, 1, 84) * 255 / 100 - 255) * -1;
			console_execute_silent("get guest_initial_hunger");
		}
		else if (strcmp(argv[0], "guest_initial_thirst") == 0 && int_valid) {
			RCT2_GLOBAL(RCT2_ADDRESS_GUEST_INITIAL_THIRST, uint8) = (clamp(int_val, 1, 84) * 255 / 100 - 255) * -1;
			console_execute_silent("get guest_initial_thirst");
		}
		else if (strcmp(argv[0], "guest_prefer_less_intense_rides") == 0 && int_valid) {
			SET_FLAG(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32), PARK_FLAGS_PREF_LESS_INTENSE_RIDES, int_val);
			console_execute_silent("get guest_prefer_less_intense_rides");
		}
		else if (strcmp(argv[0], "guest_prefer_more_intense_rides") == 0 && int_valid) {
			SET_FLAG(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32), PARK_FLAGS_PREF_MORE_INTENSE_RIDES, int_val);
			console_execute_silent("get guest_prefer_more_intense_rides");
		}
		else if (strcmp(argv[0], "forbid_marketing_campagns") == 0 && int_valid) {
			SET_FLAG(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32), PARK_FLAGS_FORBID_MARKETING_CAMPAIGN, int_val);
			console_execute_silent("get forbid_marketing_campagns");
		}
		else if (strcmp(argv[0], "forbid_landscape_changes") == 0 && int_valid) {
			SET_FLAG(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32), PARK_FLAGS_FORBID_LANDSCAPE_CHANGES, int_val);
			console_execute_silent("get forbid_landscape_changes");
		}
		else if (strcmp(argv[0], "forbid_tree_removal") == 0 && int_valid) {
			SET_FLAG(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32), PARK_FLAGS_FORBID_TREE_REMOVAL, int_val);
			console_execute_silent("get forbid_tree_removal");
		}
		else if (strcmp(argv[0], "forbid_high_construction") == 0 && int_valid) {
			SET_FLAG(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32), PARK_FLAGS_FORBID_HIGH_CONSTRUCTION, int_val);
			console_execute_silent("get forbid_high_construction");
		}
		else if (strcmp(argv[0], "pay_for_rides") == 0 && int_valid) {
			SET_FLAG(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32), PARK_FLAGS_PARK_FREE_ENTRY, int_val);
			console_execute_silent("get pay_for_rides");
		}
		else if (strcmp(argv[0], "no_money") == 0 && int_valid) {
			SET_FLAG(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32), PARK_FLAGS_NO_MONEY, int_val);
			console_execute_silent("get no_money");
		}
		else if (strcmp(argv[0], "difficult_park_rating") == 0 && int_valid) {
			SET_FLAG(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32), PARK_FLAGS_DIFFICULT_PARK_RATING, int_val);
			console_execute_silent("get difficult_park_rating");
		}
		else if (strcmp(argv[0], "difficult_guest_generation") == 0 && int_valid) {
			SET_FLAG(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32), PARK_FLAGS_DIFFICULT_GUEST_GENERATION, int_val);
			console_execute_silent("get difficult_guest_generation");
		}
		else if (strcmp(argv[0], "park_open") == 0 && int_valid) {
			SET_FLAG(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32), PARK_FLAGS_PARK_OPEN, int_val);
			console_execute_silent("get park_open");
		}
		else if (strcmp(argv[0], "land_rights_cost") == 0 && double_valid) {
			RCT2_GLOBAL(RCT2_ADDRESS_LAND_COST, money16) = clamp(MONEY((int)double_val, ((int)(double_val * 100)) % 100), MONEY(0, 0), MONEY(15000, 0));
			console_execute_silent("get land_rights_cost");
		}
		else if (strcmp(argv[0], "construction_rights_cost") == 0 && double_valid) {
			RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCTION_RIGHTS_COST, money16) = clamp(MONEY((int)double_val, ((int)(double_val * 100)) % 100), MONEY(0, 0), MONEY(15000, 0));
			console_execute_silent("get construction_rights_cost");
		}
		else if (strcmp(argv[0], "climate") == 0) {
			if (int_valid) {
				RCT2_GLOBAL(RCT2_ADDRESS_CLIMATE, sint8) = clamp(int_val, 0, 3);
			}
			else {
				char* climate_names[] = { "cool_and_wet", "warm", "hot_and_dry", "cold" };
				for (int i = 0; i < 4; i++) {
					if (strcmp(argv[1], climate_names[i]) == 0) {
						RCT2_GLOBAL(RCT2_ADDRESS_CLIMATE, sint8) = i;
						break;
					}
				}
			}
			console_execute_silent("get climate");
		}
		else if (strcmp(argv[0], "game_speed") == 0 && int_valid) {
			gGameSpeed = clamp(int_val, 1, 8);
			console_execute_silent("get game_speed");
		}
		else if (strcmp(argv[0], "console_small_font") == 0 && int_valid) {
			gConfigInterface.console_small_font = (int_val != 0);
			config_save_default();
			console_execute_silent("get console_small_font");
		}
		else {
			console_writeline_error("Invalid variable or value.");
		}
	}
	return 0;
}


typedef int (*console_command_func)(const char **argv, int argc);
typedef struct {
	char *command;
	console_command_func func;
	char *help;
} console_command;

char* console_variable_table[] = {
	"park_rating",
	"money",
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
	"console_small_font"
};

console_command console_command_table[] = {
	{ "clear", cc_clear, "Clears the console." },
	{ "echo", cc_echo, "Echos the text to the console.\necho text" },
	{ "help", cc_help, "Lists commands or info about a command.\nhelp [command]" },
	{ "get", cc_get, "Gets the value of the specified variable.\nget variable" },
	{ "set", cc_set, "Sets the variable to the specified value.\nset variable value" },
	{ "variables", cc_variables, "Lists all the variables that can be used with get and sometimes set." }
};

static int cc_variables(const char **argv, int argc)
{
	for (int i = 0; i < countof(console_variable_table); i++)
		console_writeline(console_variable_table[i]);
	return 0;
}

static int cc_help(const char **argv, int argc)
{
	if (argc > 0) {
		for (int i = 0; i < countof(console_command_table); i++) {
			if (strcmp(console_command_table[i].command, argv[0]) == 0)
				console_writeline(console_command_table[i].help);
		}
	}
	else {
		console_write_all_commands();
	}
	return 0;
}

static void console_write_all_commands()
{
	for (int i = 0; i < countof(console_command_table); i++)
		console_writeline(console_command_table[i].command);
}

void console_execute(const char *src)
{
	console_writeline(src);

	console_execute_silent(src);
}

void console_execute_silent(const char *src)
{
	int argc = 0;
	int argvCapacity = 8;
	char **argv = malloc(argvCapacity * sizeof(char*));
	const char *start = src;
	const char *end;
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
		int length = end - start;

		if (length > 0) {
			char *arg = malloc(length + 1);
			memcpy(arg, start, length);
			arg[length] = 0;

			if (argc >= argvCapacity) {
				argvCapacity *= 2;
				argv = realloc(argv, argvCapacity * sizeof(char*));
			}
			argv[argc] = arg;
			argc++;
		}

		start = end;
	} while (*end != 0);

	if (argc == 0)
		return;

	bool validCommand = false;
	for (int i = 0; i < countof(console_command_table); i++) {
		if (strcmp(argv[0], console_command_table[i].command) == 0) {
			console_command_table[i].func(argv + 1, argc - 1);
			validCommand = true;
			break;
		}
	}

	for (int i = 0; i < argc; i++)
		free(argv[i]);
	free(argv);

	if (!validCommand) {
		char output[] = { FORMAT_RED, "Unknown command. Type help to list available commands." };
		console_writeline(output);
	}
}