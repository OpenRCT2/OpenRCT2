#include <stdarg.h>
#include <SDL_scancode.h>
#include "../addresses.h"
#include "../drawing/drawing.h"
#include "../localisation/localisation.h"
#include "../platform/platform.h"
#include "../object.h"
#include "console.h"
#include "window.h"

#define CONSOLE_BUFFER_SIZE 1024
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
static char _consolePrintfBuffer[CONSOLE_BUFFER_SIZE];

static char _consoleHistory[CONSOLE_HISTORY_SIZE][CONSOLE_INPUT_SIZE];
static int _consoleHistoryIndex = 0;
static int _consoleHistoryCount = 0;

static void console_invalidate();
static void console_write_prompt();
static void console_update_scroll();
static void console_clear_input();
static void console_history_add(const char *src);
static void console_write_all_commands();

static int cc_help(const char **argv, int argc);

void console_open()
{
	gConsoleOpen = true;
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
	_consoleBottom = 300;

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

	// Set font
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = 224;
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_FLAGS, uint16) = 0;

	// Background
	gfx_fill_rect(dpi, _consoleLeft, _consoleTop, _consoleRight, _consoleBottom, 0);

	int x = _consoleLeft + 4;
	int y = _consoleTop + 4;
	
	// Draw previous lines
	char lineBuffer[1 + 256];
	char *ch = _consoleViewBufferStart;
	while (*ch != 0) {
		// Find line break or null terminator
		char *nextLine = ch;
		while (*nextLine != 0 && *nextLine != '\n') {
			nextLine++;
		}

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

	// Draw current line
	gfx_draw_string(dpi, _consoleCurrentLine, 255, x, y);

	// Draw caret
	if (_consoleCaretTicks < 15) {
		memcpy(lineBuffer, _consoleCurrentLine, gTextInputCursorPosition);
		lineBuffer[gTextInputCursorPosition] = 0;
		int caretX = x + gfx_get_string_width(lineBuffer);
		int caretY = y + 10;

		gfx_fill_rect(dpi, caretX, caretY, caretX + 6, caretY + 1, FORMAT_GREEN);
	}
	gfx_fill_rect(dpi, _consoleLeft, _consoleBottom + 1, _consoleRight, _consoleBottom + 1, 14);
	gfx_fill_rect(dpi, _consoleLeft, _consoleBottom + 2, _consoleRight, _consoleBottom + 2, 11);
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
	console_write("OpenRCT2 > ");
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

void console_printf(const char *format, ...)
{
	va_list list;
	va_start(list, format);
	
	vsprintf(_consolePrintfBuffer, format, list);
	console_writeline(_consolePrintfBuffer);
}

static void console_update_scroll()
{
	int lines = 0;
	int maxLines = ((_consoleBottom - _consoleTop) / 10) - 1;
	char *ch = strchr(_consoleBuffer, 0);
	while (ch > _consoleBuffer && lines < maxLines) {
		ch--;
		if (*ch == '\n')
			lines++;
	}

	if (*ch == '\n')
		ch++;
	_consoleViewBufferStart = ch;
}

void console_clear()
{
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

static int cc_object_count(const char **argv, int argc)
{
	console_printf("%d total rides\n%d total large scenery", object_entry_group_counts[0], object_entry_group_counts[1]);
	return 0;
}


typedef int (*console_command_func)(const char **argv, int argc);
typedef struct {
	char *command;
	console_command_func func;
	char *help;
} console_command;

console_command console_command_table[] = {
	{ "clear", cc_clear, "Clears the console." },
	{ "echo", cc_echo, "Echos the text to the console.\necho text" },
	{ "help", cc_help, "Lists commands or info about a command.\nhelp [command]" },
	{ "object_count", cc_object_count, "Lists the total objecst in the scenario.\nobject_count" }
};

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
	console_writeline(_consoleCurrentLine);

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
		} else {
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