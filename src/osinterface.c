/*****************************************************************************
 * Copyright (c) 2014 Ted John, Alexander Overvoorde
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 * 
 * This file is part of OpenRCT2.
 * 
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include <stdio.h>
#include <shlobj.h>
#include <tchar.h>
#include <windows.h>
#include <SDL.h>
#include <SDL_syswm.h>

#include "addresses.h"
#include "gfx.h"
#include "osinterface.h"
#include "rct2.h"

typedef void(*update_palette_func)(char*, int, int);

openrct2_cursor gCursorState;
const unsigned char *gKeysState;
unsigned char *gKeysPressed;
unsigned int gLastKeyPressed;

static void osinterface_create_window();
static void osinterface_close_window();
static void osinterface_resize(int width, int height);
static void osinterface_update_palette(char* colours, int start_index, int num_colours);

static SDL_Window *_window;
static SDL_Surface *_surface;
static SDL_Palette *_palette;

static int _screenBufferSize;
static void *_screenBuffer;

void osinterface_init()
{
	osinterface_create_window();

	gKeysPressed = malloc(sizeof(unsigned char) * 256);
	memset(gKeysPressed, 0, sizeof(unsigned char) * 256);

	// RCT2_CALLPROC(0x00404584); // dinput_init()
}

static void osinterface_create_window()
{
	SDL_SysWMinfo wmInfo;
	HWND hWnd;
	int width, height;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		RCT2_ERROR("SDL_Init %s", SDL_GetError());
		exit(-1);
	}

	// stuff
	{
		RCT2_CALLPROC_EBPSAFE(0x0068352C);
		RCT2_CALLPROC_EBPSAFE(0x0068371D);

		width = RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_RESOLUTION_WIDTH, sint16);
		height = RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_RESOLUTION_HEIGHT, sint16);

		width = 640;
		height = 480;
	}

	RCT2_GLOBAL(0x009E2D8C, sint32) = 0;


	_window = SDL_CreateWindow("OpenRCT2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_RESIZABLE);
	if (!_window) {
		RCT2_ERROR("SDL_CreateWindow failed %s", SDL_GetError());
		exit(-1);
	}

	// Get the HWND context
	if (SDL_GetWindowWMInfo(_window, &wmInfo) != SDL_TRUE) {
		RCT2_ERROR("SDL_GetWindowWMInfo failed %s", SDL_GetError());
		exit(-1);
	}
	hWnd = wmInfo.info.win.window;
	RCT2_GLOBAL(0x009E2D70, HWND) = hWnd;

	// Set the update palette function pointer
	RCT2_GLOBAL(0x009E2BE4, update_palette_func) = osinterface_update_palette;

	// Initialise the surface, palette and draw buffer
	osinterface_resize(width, height);
}

static void osinterface_resize(int width, int height)
{
	rct_drawpixelinfo *screenDPI;
	int newScreenBufferSize;
	void *newScreenBuffer;

	if (_surface != NULL)
		SDL_FreeSurface(_surface);
	if (_palette != NULL)
		SDL_FreePalette(_palette);

	_surface = SDL_CreateRGBSurface(0, width, height, 8, 0, 0, 0, 0);
	_palette = SDL_AllocPalette(256);

	if (!_surface || !_palette) {
		RCT2_ERROR("%p || %p == NULL %s", _surface, _palette, SDL_GetError());
		exit(-1);
	}

	if (SDL_SetSurfacePalette(_surface, _palette)) {
		RCT2_ERROR("SDL_SetSurfacePalette failed %s", SDL_GetError());
		exit(-1);
	}

	newScreenBufferSize = _surface->pitch * _surface->h;
	newScreenBuffer = malloc(newScreenBufferSize);
	if (_screenBuffer == NULL) {
		memset(newScreenBuffer, 0, newScreenBufferSize);
	} else {
		memcpy(newScreenBuffer, _screenBuffer, min(_screenBufferSize, newScreenBufferSize));
		if (newScreenBufferSize - _screenBufferSize > 0)
			memset((uint8*)newScreenBuffer + _screenBufferSize, 0, newScreenBufferSize - _screenBufferSize);
		free(_screenBuffer);
	}

	_screenBuffer = newScreenBuffer;
	_screenBufferSize = newScreenBufferSize;

	RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16) = width;
	RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, sint16) = height;

	screenDPI = RCT2_ADDRESS(RCT2_ADDRESS_SCREEN_DPI, rct_drawpixelinfo);
	screenDPI->bits = _screenBuffer;
	screenDPI->x = 0;
	screenDPI->y = 0;
	screenDPI->width = width;
	screenDPI->height = height;
	screenDPI->pitch = _surface->pitch - _surface->w;

	RCT2_GLOBAL(0x009ABDF0, uint8) = 6;
	RCT2_GLOBAL(0x009ABDF1, uint8) = 3;
	RCT2_GLOBAL(0x009ABDF2, uint8) = 1;
	RCT2_GLOBAL(RCT2_ADDRESS_DIRTY_BLOCK_WIDTH, sint16) = 64;
	RCT2_GLOBAL(RCT2_ADDRESS_DIRTY_BLOCK_HEIGHT, sint16) = 8;
	RCT2_GLOBAL(RCT2_ADDRESS_DIRTY_BLOCK_COLUMNS, sint32) = (width >> 6) + 1;
	RCT2_GLOBAL(RCT2_ADDRESS_DIRTY_BLOCK_ROWS, sint32) = (height >> 3) + 1;

	RCT2_CALLPROC_EBPSAFE(0x0066B905); // resize_gui()
	gfx_invalidate_screen();
}

static void osinterface_update_palette(char* colours, int start_index, int num_colours)
{
	SDL_Color base[256];
	SDL_Surface *surface;
	int i;

	surface = SDL_GetWindowSurface(_window);
	if (!surface) {
		RCT2_ERROR("SDL_GetWindowSurface failed %s", SDL_GetError());
		exit(1);
	}

	for (i = 0; i < 256; i++) {
		base[i].r = colours[2];
		base[i].g = colours[1];
		base[i].b = colours[0];
		base[i].a = 0;
		colours += 4;
	}

	if (SDL_SetPaletteColors(_palette, base, 0, 256)) {
		RCT2_ERROR("SDL_SetPaletteColors failed %s", SDL_GetError());
		exit(1);
	}
}

void osinterface_draw()
{
	// Lock the surface before setting its pixels
	if (SDL_MUSTLOCK(_surface))
		if (SDL_LockSurface(_surface) < 0) {
			RCT2_ERROR("locking failed %s", SDL_GetError());
			return;
		}

	// Copy pixels from the virtual screen buffer to the surface
	memcpy(_surface->pixels, _screenBuffer, _surface->pitch * _surface->h);

	// Unlock the surface
	if (SDL_MUSTLOCK(_surface))
		SDL_UnlockSurface(_surface);

	// Copy the surface to the window
	if (SDL_BlitSurface(_surface, NULL, SDL_GetWindowSurface(_window), NULL)) {
		RCT2_ERROR("SDL_BlitSurface %s", SDL_GetError());
		exit(1);
	}
	if (SDL_UpdateWindowSurface(_window)) {
		RCT2_ERROR("SDL_UpdateWindowSurface %s", SDL_GetError());
		exit(1);
	}
}

void osinterface_process_messages()
{
	SDL_Event e;

	gLastKeyPressed = 0;
	// gCursorState.wheel = 0;
	gCursorState.left &= ~CURSOR_CHANGED;
	gCursorState.middle &= ~CURSOR_CHANGED;
	gCursorState.right &= ~CURSOR_CHANGED;
	gCursorState.old = 0;

	while (SDL_PollEvent(&e)) {
		switch (e.type) {
		case SDL_QUIT:
			rct2_finish();
			break;
		case SDL_WINDOWEVENT:
			if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				osinterface_resize(e.window.data1, e.window.data2);
			break;
		case SDL_MOUSEMOTION:
			RCT2_GLOBAL(0x0142406C, int) = e.motion.x;
			RCT2_GLOBAL(0x01424070, int) = e.motion.y;

			gCursorState.x = e.motion.x;
			gCursorState.y = e.motion.y;
			break;
		case SDL_MOUSEWHEEL:
			gCursorState.wheel += e.wheel.y * 128;
			break;
		case SDL_MOUSEBUTTONDOWN:
			RCT2_GLOBAL(0x01424318, int) = e.button.x;
			RCT2_GLOBAL(0x0142431C, int) = e.button.y;
			switch (e.button.button) {
			case SDL_BUTTON_LEFT:
				RCT2_CALLPROC_1(0x00406C96, int, 1);
				gCursorState.left = CURSOR_PRESSED;
				gCursorState.old = 1;
				break;
			case SDL_BUTTON_MIDDLE:
				gCursorState.middle = CURSOR_PRESSED;
				break;
			case SDL_BUTTON_RIGHT:
				RCT2_CALLPROC_1(0x00406C96, int, 3);
				gCursorState.right = CURSOR_PRESSED;
				gCursorState.old = 2;
				break;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			RCT2_GLOBAL(0x01424318, int) = e.button.x;
			RCT2_GLOBAL(0x0142431C, int) = e.button.y;
			switch (e.button.button) {
			case SDL_BUTTON_LEFT:
				RCT2_CALLPROC_1(0x00406C96, int, 2);
				gCursorState.left = CURSOR_RELEASED;
				gCursorState.old = 3;
				break;
			case SDL_BUTTON_MIDDLE:
				gCursorState.middle = CURSOR_RELEASED;
				break;
			case SDL_BUTTON_RIGHT:
				RCT2_CALLPROC_1(0x00406C96, int, 4);
				gCursorState.right = CURSOR_RELEASED;
				gCursorState.old = 4;
				break;
			}
			break;
		case SDL_KEYDOWN:
			gLastKeyPressed = e.key.keysym.sym;
			gKeysPressed[e.key.keysym.scancode] = 1;
			break;
		default:
			break;
		}
	}

	gCursorState.any = gCursorState.left | gCursorState.middle | gCursorState.right;

	// Updates the state of the keys
	int numKeys = 256;
	gKeysState = SDL_GetKeyboardState(&numKeys);
}

static void osinterface_close_window()
{
	if (_window != NULL)
		SDL_DestroyWindow(_window);
	if (_surface != NULL)
		SDL_FreeSurface(_surface);
	if (_palette != NULL)
		SDL_FreePalette(_palette);
}

void osinterface_free()
{
	free(gKeysPressed);

	osinterface_close_window();
	SDL_Quit();
}

/**
 * 
 *  rct2: 0x004080EA
 */
int osinterface_open_common_file_dialog(int type, char *title, char *filename, char *filterPattern, char *filterName)
{
	char initialDirectory[MAX_PATH], *dotAddress, *slashAddress;
	OPENFILENAME openFileName;
	BOOL result;
	int tmp;

	// Get directory path from given filename
	strcpy(initialDirectory, filename);
	dotAddress = strrchr(filename, '.');
	if (dotAddress != NULL) {
		slashAddress = strrchr(filename, '\\');
		if (slashAddress < dotAddress)
			*(slashAddress + 1) = 0;
	}

	// Clear filename
	*filename = 0;

	// Set open file name options
	memset(&openFileName, 0, sizeof(OPENFILENAME));
	openFileName.lStructSize = sizeof(OPENFILENAME);
	openFileName.hwndOwner = RCT2_GLOBAL(0x009E2D70, HWND);
	openFileName.lpstrFile = filename;
	openFileName.nMaxFile = MAX_PATH;
	openFileName.lpstrInitialDir = initialDirectory;
	openFileName.lpstrTitle = title;

	// Copy filter name
	strcpy((char*)0x01423800, filterName);

	// Copy filter pattern
	strcpy((char*)0x01423800 + strlen(filterName) + 1, filterPattern);
	*((char*)(0x01423800 + strlen(filterName) + 1 + strlen(filterPattern) + 1)) = 0;
	openFileName.lpstrFilter = (char*)0x01423800;

	// 
	tmp = RCT2_GLOBAL(0x009E2C74, uint32);
	if (RCT2_GLOBAL(0x009E2BB8, uint32) == 2 && RCT2_GLOBAL(0x009E1AF8, uint32) == 1)
		RCT2_GLOBAL(0x009E2C74, uint32) = 1;

	// Open dialog
	if (type == 0) {
		openFileName.Flags = OFN_EXPLORER | OFN_CREATEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
		result = GetSaveFileName(&openFileName);
	} else if (type == 1) {
		openFileName.Flags = OFN_EXPLORER | OFN_NONETWORKBUTTON | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
		result = GetOpenFileName(&openFileName);
	}

	// 
	RCT2_GLOBAL(0x009E2C74, uint32) = tmp;

	return result;
}

void osinterface_show_messagebox(char* message){
	MessageBox(NULL, message, "OpenRCT2", MB_OK);
}

char* osinterface_open_directory_browser(char *title) {
	BROWSEINFO      bi;
	char            pszBuffer[MAX_PATH];
	LPITEMIDLIST    pidl;
	LPMALLOC        lpMalloc;

	// Initialize COM
	if (CoInitializeEx(0, COINIT_APARTMENTTHREADED) != S_OK) {
		MessageBox(NULL, _T("Error opening browse window"), _T("ERROR"), MB_OK);
		CoUninitialize();
		return 0;
	}

	// Get a pointer to the shell memory allocator
	if (SHGetMalloc(&lpMalloc) != S_OK) {
		MessageBox(NULL, _T("Error opening browse window"), _T("ERROR"), MB_OK);
		CoUninitialize();
		return 0;
	}

	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = pszBuffer;
	bi.lpszTitle = _T(title);
	bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.lParam = 0;

	char *outPath = "C:\\";

	if (pidl = SHBrowseForFolder(&bi)) {
		// Copy the path directory to the buffer
		if (SHGetPathFromIDList(pidl, pszBuffer)) {
			// Store pszBuffer (and the path) in the outPath
			outPath = (char*) malloc(strlen(pszBuffer)+1);
			strcpy(outPath, pszBuffer);
		}
	}
	CoUninitialize();
	return outPath;
}
