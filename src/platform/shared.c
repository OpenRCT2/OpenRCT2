/*****************************************************************************
 * Copyright (c) 2014 Ted John
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

#include <math.h>
#include <SDL.h>
#include "../addresses.h"
#include "../config.h"
#include "../cursors.h"
#include "../drawing/drawing.h"
#include "../game.h"
#include "../interface/console.h"
#include "../interface/keyboard_shortcut.h"
#include "../interface/window.h"
#include "../input.h"
#include "../localisation/localisation.h"
#include "../openrct2.h"
#include "platform.h"

typedef void(*update_palette_func)(char*, int, int);

openrct2_cursor gCursorState;
const unsigned char *gKeysState;
unsigned char *gKeysPressed;
unsigned int gLastKeyPressed;
utf8 *gTextInput;
int gTextInputLength;
int gTextInputMaxLength;
int gTextInputCursorPosition = 0;

int gNumResolutions = 0;
resolution *gResolutions = NULL;
int gResolutionsAllowAnyAspectRatio = 0;

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;
SDL_Texture *gBufferTexture = NULL;
SDL_PixelFormat *gBufferTextureFormat = NULL;
SDL_Color gPalette[256];
uint32 gPaletteHWMapped[256];

static SDL_Surface *_surface;
static SDL_Palette *_palette;

static void *_screenBuffer;
static int _screenBufferSize;
static int _screenBufferWidth;
static int _screenBufferHeight;
static int _screenBufferPitch;

static SDL_Cursor* _cursors[CURSOR_COUNT];
static const int _fullscreen_modes[] = { 0, SDL_WINDOW_FULLSCREEN, SDL_WINDOW_FULLSCREEN_DESKTOP };
static unsigned int _lastGestureTimestamp;
static float _gestureRadius;

static void platform_create_window();
static void platform_load_cursors();
static void platform_unload_cursors();

static void platform_refresh_screenbuffer(int width, int height, int pitch);

int resolution_sort_func(const void *pa, const void *pb)
{
	const resolution *a = (resolution*)pa;
	const resolution *b = (resolution*)pb;

	int areaA = a->width * a->height;
	int areaB = b->width * b->height;

	if (areaA == areaB) return 0;
	if (areaA < areaB) return -1;
	return 1;
}

void platform_update_fullscreen_resolutions()
{
	int i, displayIndex, numDisplayModes;
	SDL_DisplayMode mode;
	resolution *resLook, *resPlace;
	float desktopAspectRatio, aspectRatio;

	// Query number of display modes
	displayIndex = SDL_GetWindowDisplayIndex(gWindow);
	numDisplayModes = SDL_GetNumDisplayModes(displayIndex);

	// Get desktop aspect ratio
	SDL_GetDesktopDisplayMode(displayIndex, &mode);
	desktopAspectRatio = (float)mode.w / mode.h;

	if (gResolutions != NULL)
		free(gResolutions);

	// Get resolutions
	gNumResolutions = numDisplayModes;
	gResolutions = malloc(gNumResolutions * sizeof(resolution));

	gNumResolutions = 0;
	for (i = 0; i < numDisplayModes; i++) {
		SDL_GetDisplayMode(displayIndex, i, &mode);
		
		aspectRatio = (float)mode.w / mode.h;
		if (gResolutionsAllowAnyAspectRatio || fabs(desktopAspectRatio - aspectRatio) < 0.0001f) {
			gResolutions[gNumResolutions].width = mode.w;
			gResolutions[gNumResolutions].height = mode.h;
			gNumResolutions++;
		}
	}

	// Sort by area
	qsort(gResolutions, gNumResolutions, sizeof(resolution), resolution_sort_func);

	// Remove duplicates
	resPlace = &gResolutions[0];
	for (int i = 1; i < gNumResolutions; i++) {
		resLook = &gResolutions[i];
		if (resLook->width != resPlace->width || resLook->height != resPlace->height)
			*++resPlace = *resLook;
	}

	gNumResolutions = (int)(resPlace - &gResolutions[0]) + 1;

	// Update config fullscreen resolution if not set
	if (gConfigGeneral.fullscreen_width == -1 || gConfigGeneral.fullscreen_height == -1) {
		gConfigGeneral.fullscreen_width = gResolutions[gNumResolutions - 1].width;
		gConfigGeneral.fullscreen_height = gResolutions[gNumResolutions - 1].height;
	}
}

void platform_get_closest_resolution(int inWidth, int inHeight, int *outWidth, int *outHeight)
{
	int i, destinationArea, areaDiff, closestAreaDiff, closestWidth, closestHeight;

	closestAreaDiff = -1;
	destinationArea = inWidth * inHeight;
	for (i = 0; i < gNumResolutions; i++) {
		// Check if exact match
		if (gResolutions[i].width == inWidth && gResolutions[i].height == inHeight) {
			closestWidth = gResolutions[i].width;
			closestHeight = gResolutions[i].height;
			closestAreaDiff = 0;
			break;
		}

		// Check if area is closer to best match
		areaDiff = abs((gResolutions[i].width * gResolutions[i].height) - destinationArea);
		if (closestAreaDiff == -1 || areaDiff < closestAreaDiff) {
			closestAreaDiff = areaDiff;
			closestWidth = gResolutions[i].width;
			closestHeight = gResolutions[i].height;
		}
	}

	if (closestAreaDiff != -1) {
		*outWidth = closestWidth;
		*outHeight = closestHeight;
	} else {
		*outWidth = 640;
		*outHeight = 480;
	}
}

void platform_draw()
{
	int width = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16);
	int height = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16);

	if (gConfigGeneral.hardware_display) {
		void *pixels;
		int pitch;
		if (SDL_LockTexture(gBufferTexture, NULL, &pixels, &pitch) == 0) {
			uint8 *src = (uint8*)_screenBuffer;
			int padding = pitch - (width * 4);
			if (pitch == width * 4) {
				uint32 *dst = pixels;
				for (int i = width * height; i > 0; i--) { *dst++ = *(uint32 *)(&gPaletteHWMapped[*src++]); }
			} else
			if (pitch == (width * 2) + padding) {
				uint16 *dst = pixels;
				for (int y = height; y > 0; y++) {
					for (int x = width; x > 0; x--) { *dst++ = *(uint16 *)(&gPaletteHWMapped[*src++]); }
					dst = (uint16*)(((uint8 *)dst) + padding);
				}
			} else
			if (pitch == width + padding) {
				uint8 *dst = pixels;
				for (int y = height; y > 0; y++) {
					for (int x = width; x > 0; x--) { *dst++ = *(uint8 *)(&gPaletteHWMapped[*src++]); }
					dst += padding;
				}
			}
			SDL_UnlockTexture(gBufferTexture);
		}

		SDL_RenderCopy(gRenderer, gBufferTexture, NULL, NULL);
		SDL_RenderPresent(gRenderer);
	} else {
		// Lock the surface before setting its pixels
		if (SDL_MUSTLOCK(_surface)) {
			if (SDL_LockSurface(_surface) < 0) {
				log_error("locking failed %s", SDL_GetError());
				return;
			}
		}

		// Copy pixels from the virtual screen buffer to the surface
		memcpy(_surface->pixels, _screenBuffer, _surface->pitch * _surface->h);

		// Unlock the surface
		if (SDL_MUSTLOCK(_surface))
			SDL_UnlockSurface(_surface);

		// Copy the surface to the window
		if (SDL_BlitSurface(_surface, NULL, SDL_GetWindowSurface(gWindow), NULL)) {
			log_fatal("SDL_BlitSurface %s", SDL_GetError());
			exit(1);
		}
		if (SDL_UpdateWindowSurface(gWindow)) {
			log_fatal("SDL_UpdateWindowSurface %s", SDL_GetError());
			exit(1);
		}
	}
}

static void platform_resize(int width, int height)
{
	uint32 flags;

	RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16) = width;
	RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16) = height;

	platform_refresh_video();

	flags = SDL_GetWindowFlags(gWindow);

	if ((flags & SDL_WINDOW_MINIMIZED) == 0) {
		window_resize_gui(width, height);
		window_relocate_windows(width, height);
	}

	gfx_invalidate_screen();

	// Check if the window has been resized in windowed mode and update the config file accordingly
	// This is called in rct2_update_2 and is only called after resizing a window has finished
	if ((flags & (SDL_WINDOW_MAXIMIZED | SDL_WINDOW_MINIMIZED |
		SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP)) == 0) {
		if (width != gConfigGeneral.window_width || height != gConfigGeneral.window_height) {
			gConfigGeneral.window_width = width;
			gConfigGeneral.window_height = height;
			config_save_default();
		}
	}
}

static uint8 soft_light(uint8 a, uint8 b)
{
	float fa = a / 255.0f;
	float fb = b / 255.0f;
	float fr;
	if (fb < 0.5f) {
		fr = (2 * fa * fb) + ((fa * fa) * (1 - (2 * fb)));
	} else {
		fr = (2 * fa * (1 - fb)) + (sqrtf(fa) * ((2 * fb) - 1));
	}
	return (uint8)(clamp(0.0f, fr, 1.0f) * 255.0f);
}

static uint8 lerp(uint8 a, uint8 b, float t)
{
	if (t <= 0) return a;
	if (t >= 1) return b;

	int range = b - a;
	int amount = (int)(range * t);
	return (uint8)(a + amount);
}

void platform_update_palette(char* colours, int start_index, int num_colours)
{
	SDL_Surface *surface;
	int i;

	for (i = 0; i < 256; i++) {
		gPalette[i].r = colours[2];
		gPalette[i].g = colours[1];
		gPalette[i].b = colours[0];
		gPalette[i].a = 0;

		float night = gDayNightCycle;
		if (night >= 0 && RCT2_GLOBAL(RCT2_ADDRESS_LIGHTNING_ACTIVE, uint8) == 0) {
			gPalette[i].r = lerp(gPalette[i].r, soft_light(gPalette[i].r, 8), night);
			gPalette[i].g = lerp(gPalette[i].g, soft_light(gPalette[i].g, 8), night);
			gPalette[i].b = lerp(gPalette[i].b, soft_light(gPalette[i].b, 128), night);
		}

		colours += 4;
		if (gBufferTextureFormat != NULL) {
			gPaletteHWMapped[i] = SDL_MapRGB(gBufferTextureFormat, gPalette[i].r, gPalette[i].g, gPalette[i].b);
		}
	}

	if (!gOpenRCT2Headless && !gConfigGeneral.hardware_display) {
		surface = SDL_GetWindowSurface(gWindow);
		if (!surface) {
			log_fatal("SDL_GetWindowSurface failed %s", SDL_GetError());
			exit(1);
		}

		if (_palette != NULL && SDL_SetPaletteColors(_palette, gPalette, 0, 256)) {
			log_fatal("SDL_SetPaletteColors failed %s", SDL_GetError());
			exit(1);
		}
	}
}

void platform_process_messages()
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
// 			rct2_finish();
			rct2_quit();
			break;
		case SDL_WINDOWEVENT:
			if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				platform_resize(e.window.data1, e.window.data2);
			break;
		case SDL_MOUSEMOTION:
			RCT2_GLOBAL(0x0142406C, int) = e.motion.x;
			RCT2_GLOBAL(0x01424070, int) = e.motion.y;

			gCursorState.x = e.motion.x;
			gCursorState.y = e.motion.y;
			break;
		case SDL_MOUSEWHEEL:
			if (gConsoleOpen) {
				console_scroll(e.wheel.y);
				break;
			}
			gCursorState.wheel += e.wheel.y * 128;
			break;
		case SDL_MOUSEBUTTONDOWN:
			RCT2_GLOBAL(0x01424318, int) = e.button.x;
			RCT2_GLOBAL(0x0142431C, int) = e.button.y;
			switch (e.button.button) {
			case SDL_BUTTON_LEFT:
				store_mouse_input(1);
				gCursorState.left = CURSOR_PRESSED;
				gCursorState.old = 1;
				break;
			case SDL_BUTTON_MIDDLE:
				gCursorState.middle = CURSOR_PRESSED;
				break;
			case SDL_BUTTON_RIGHT:
				store_mouse_input(3);
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
				store_mouse_input(2);
				gCursorState.left = CURSOR_RELEASED;
				gCursorState.old = 3;
				break;
			case SDL_BUTTON_MIDDLE:
				gCursorState.middle = CURSOR_RELEASED;
				break;
			case SDL_BUTTON_RIGHT:
				store_mouse_input(4);
				gCursorState.right = CURSOR_RELEASED;
				gCursorState.old = 4;
				break;
			}
			break;
		case SDL_KEYDOWN:
			if (e.key.keysym.sym == SDLK_KP_ENTER){
				// Map Keypad enter to regular enter.
				e.key.keysym.scancode = SDL_SCANCODE_RETURN;
			}

			gLastKeyPressed = e.key.keysym.sym;
			gKeysPressed[e.key.keysym.scancode] = 1;
			if (e.key.keysym.sym == SDLK_RETURN && e.key.keysym.mod & KMOD_ALT) {
				int targetMode = gConfigGeneral.fullscreen_mode == 0 ? 2 : 0;
				platform_set_fullscreen_mode(targetMode);
				gConfigGeneral.fullscreen_mode = targetMode;
				config_save_default();
				break;
			}

			// Text input

			// If backspace and we have input text with a cursor position none zero
			if (e.key.keysym.sym == SDLK_BACKSPACE && gTextInputLength > 0 && gTextInput != NULL && gTextInputCursorPosition) {
				int dstIndex = gTextInputCursorPosition;
				do {
					if (dstIndex == 0) break;
					dstIndex--;
				} while (!utf8_is_codepoint_start(&gTextInput[dstIndex]));
				int removedCodepointSize = gTextInputCursorPosition - dstIndex;

				// When at max length don't shift the data left
				// as it would buffer overflow.
				if (gTextInputCursorPosition != gTextInputMaxLength) {
					memmove(gTextInput + dstIndex, gTextInput + gTextInputCursorPosition, gTextInputMaxLength - dstIndex);
				}
				gTextInput[gTextInputLength - removedCodepointSize] = '\0';
				gTextInputCursorPosition -= removedCodepointSize;
				gTextInputLength -= removedCodepointSize;
				console_refresh_caret();
				window_update_textbox();
			}
			if (e.key.keysym.sym == SDLK_END){
				gTextInputCursorPosition = gTextInputLength;
				console_refresh_caret();
			}
			if (e.key.keysym.sym == SDLK_HOME) {
				gTextInputCursorPosition = 0;
				console_refresh_caret();
			}
			if (e.key.keysym.sym == SDLK_DELETE && gTextInputLength > 0 && gTextInput != NULL && gTextInputCursorPosition != gTextInputLength) {
				int dstIndex = gTextInputCursorPosition;
				do {
					if (dstIndex == gTextInputLength) break;
					dstIndex++;
				} while (!utf8_is_codepoint_start(&gTextInput[dstIndex]));
				int removedCodepointSize = dstIndex - gTextInputCursorPosition;

				memmove(gTextInput + gTextInputCursorPosition, gTextInput + dstIndex, gTextInputMaxLength - dstIndex);
				gTextInput[gTextInputMaxLength - removedCodepointSize] = '\0';
				gTextInputLength -= removedCodepointSize;
				console_refresh_caret();
				window_update_textbox();
			}
			if (e.key.keysym.sym == SDLK_RETURN && gTextInput != NULL) {
				window_cancel_textbox();
			}
			if (e.key.keysym.sym == SDLK_LEFT && gTextInput != NULL) {
				do {
					if (gTextInputCursorPosition == 0) break;
					gTextInputCursorPosition--;
				} while (!utf8_is_codepoint_start(&gTextInput[gTextInputCursorPosition]));
				console_refresh_caret();
			}
			else if (e.key.keysym.sym == SDLK_RIGHT && gTextInput != NULL) {
				do {
					if (gTextInputCursorPosition == gTextInputLength) break;
					gTextInputCursorPosition++;
				} while (!utf8_is_codepoint_start(&gTextInput[gTextInputCursorPosition]));
				console_refresh_caret();
			}
			// Checks GUI modifier key for MACs otherwise CTRL key
#ifdef MAC
			else if (e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_GUI && gTextInput != NULL) {
#else
			else if (e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL && gTextInput != NULL) {
#endif
				if (SDL_HasClipboardText()) {
					utf8 *text = SDL_GetClipboardText();
					for (int i = 0; text[i] != '\0' && gTextInputLength < gTextInputMaxLength; i++) {
						// If inserting in center of string make space for new letter
						if (gTextInputLength > gTextInputCursorPosition){
							memmove(gTextInput + gTextInputCursorPosition + 1, gTextInput + gTextInputCursorPosition, gTextInputMaxLength - gTextInputCursorPosition - 1);
							gTextInput[gTextInputCursorPosition] = text[i];
							gTextInputLength++;
						} else {
							gTextInput[gTextInputLength++] = text[i];
						}
						gTextInputCursorPosition++;
					}
					window_update_textbox();
				}
			}
			break;
		case SDL_MULTIGESTURE:
			if (e.mgesture.numFingers == 2) {
				if (e.mgesture.timestamp > _lastGestureTimestamp + 1000)
					_gestureRadius = 0;
				_lastGestureTimestamp = e.mgesture.timestamp;
				_gestureRadius += e.mgesture.dDist;

				// Zoom gesture
				const int tolerance = 128;
				int gesturePixels = (int)(_gestureRadius * RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16));
				if (gesturePixels > tolerance) {
					_gestureRadius = 0;
					keyboard_shortcut_handle_command(SHORTCUT_ZOOM_VIEW_IN);
				} else if (gesturePixels < -tolerance) {
					_gestureRadius = 0;
					keyboard_shortcut_handle_command(SHORTCUT_ZOOM_VIEW_OUT);
				}
			}
			break;

		case SDL_TEXTINPUT:
			if (gTextInputLength < gTextInputMaxLength && gTextInput){
				// Convert the utf-8 code into rct ascii
				char new_char;
				if (e.text.text[0] == '`' && gConsoleOpen)
					break;
				if (!(e.text.text[0] & 0x80))
					new_char = *e.text.text;
				else if (!(e.text.text[0] & 0x20))
					new_char = ((e.text.text[0] & 0x1F) << 6) | (e.text.text[1] & 0x3F);

				// If inserting in center of string make space for new letter
				if (gTextInputLength > gTextInputCursorPosition){
					memmove(gTextInput + gTextInputCursorPosition + 1, gTextInput + gTextInputCursorPosition, gTextInputMaxLength - gTextInputCursorPosition - 1);
					gTextInput[gTextInputCursorPosition] = new_char;
					gTextInputLength++;
				} else {
					gTextInput[gTextInputLength++] = new_char;
					gTextInput[gTextInputLength] = 0;
				}

				gTextInputCursorPosition++;
				console_refresh_caret();
				window_update_textbox();
			}
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

static void platform_close_window()
{
	if (gWindow != NULL)
		SDL_DestroyWindow(gWindow);
	if (_surface != NULL)
		SDL_FreeSurface(_surface);
	if (_palette != NULL)
		SDL_FreePalette(_palette);
	platform_unload_cursors();
}

void platform_init()
{
	platform_create_window();
	gKeysPressed = malloc(sizeof(unsigned char) * 256);
	memset(gKeysPressed, 0, sizeof(unsigned char) * 256);
}

static void platform_create_window()
{
	int width, height;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		log_fatal("SDL_Init %s", SDL_GetError());
		exit(-1);
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);
	SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, gConfigGeneral.minimize_fullscreen_focus_loss ? "1" : "0");

	platform_load_cursors();

	// TODO This should probably be called somewhere else. It has nothing to do with window creation and can be done as soon as
	// g1.dat is loaded.
	sub_68371D();

	// Get window size
	width = gConfigGeneral.window_width;
	height = gConfigGeneral.window_height;
	if (width == -1) width = 640;
	if (height == -1) height = 480;

	RCT2_GLOBAL(0x009E2D8C, sint32) = 0;

	// Create window in window first rather than fullscreen so we have the display the window is on first
	gWindow = SDL_CreateWindow(
		"OpenRCT2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_RESIZABLE
	);
	if (!gWindow) {
		log_fatal("SDL_CreateWindow failed %s", SDL_GetError());
		exit(-1);
	}

	// Set the update palette function pointer
	RCT2_GLOBAL(0x009E2BE4, update_palette_func) = platform_update_palette;

	// Initialise the surface, palette and draw buffer
	platform_resize(width, height);

	platform_update_fullscreen_resolutions();
	platform_set_fullscreen_mode(gConfigGeneral.fullscreen_mode);
}

int platform_scancode_to_rct_keycode(int sdl_key)
{
	char keycode = (char)SDL_GetKeyFromScancode((SDL_Scancode)sdl_key);

	// Until we reshufle the text files to use the new positions 
	// this will suffice to move the majority to the correct positions.
	// Note any special buttons PgUp PgDwn are mapped wrong.
	if (keycode >= 'a' && keycode <= 'z')
		keycode = toupper(keycode);

	return keycode;
}

void platform_free()
{
	free(gKeysPressed);

	platform_close_window();
	SDL_Quit();
}

void platform_start_text_input(char* buffer, int max_length)
{
	SDL_StartTextInput();
	gTextInputMaxLength = max_length - 1;
	gTextInput = buffer;
	gTextInputCursorPosition = strnlen(gTextInput, max_length);
	gTextInputLength = gTextInputCursorPosition;
}

void platform_stop_text_input()
{
	SDL_StopTextInput();
	gTextInput = NULL;
}

SDL_RWops* platform_sdl_rwfromfile(const char* filename, const char* mode)
{
	utf8 utf8filename[512];
	win1252_to_utf8(utf8filename, filename, sizeof(utf8filename));
	return SDL_RWFromFile(utf8filename, mode);
}

static void platform_unload_cursors()
{
	for (int i = 0; i < CURSOR_COUNT; i++)
		if (_cursors[i] != NULL)
			SDL_FreeCursor(_cursors[i]);
}

void platform_set_fullscreen_mode(int mode)
{
	int width, height;

	mode = _fullscreen_modes[mode];

	// HACK Changing window size when in fullscreen usually has no effect
	if (mode == SDL_WINDOW_FULLSCREEN)
		SDL_SetWindowFullscreen(gWindow, 0);

	// Set window size
	if (mode == SDL_WINDOW_FULLSCREEN) {
		platform_update_fullscreen_resolutions();
		platform_get_closest_resolution(gConfigGeneral.fullscreen_width, gConfigGeneral.fullscreen_height, &width, &height);
		SDL_SetWindowSize(gWindow, width, height);
	} else if (mode == 0) {
		SDL_SetWindowSize(gWindow, gConfigGeneral.window_width, gConfigGeneral.window_height);
	}

	if (SDL_SetWindowFullscreen(gWindow, mode)) {
		log_fatal("SDL_SetWindowFullscreen %s", SDL_GetError());
		exit(1);

		// TODO try another display mode rather than just exiting the game
	}
}

/**
 *  This is not quite the same as the below function as we don't want to
 *  derfererence the cursor before the function.
 *  rct2: 0x0407956
 */
void platform_set_cursor(char cursor)
{
	RCT2_GLOBAL(RCT2_ADDRESS_CURENT_CURSOR, uint8) = cursor;
	SDL_SetCursor(_cursors[cursor]);
}
/**
 *
 * rct2: 0x0068352C
 */
static void platform_load_cursors()
{
	RCT2_GLOBAL(0x14241BC, uint32) = 2;
	HINSTANCE hInst = RCT2_GLOBAL(RCT2_ADDRESS_HINSTANCE, HINSTANCE);
	RCT2_GLOBAL(RCT2_ADDRESS_HCURSOR_ARROW,				HCURSOR) = LoadCursor(hInst, MAKEINTRESOURCE(0x74));
	RCT2_GLOBAL(RCT2_ADDRESS_HCURSOR_BLANK,				HCURSOR) = LoadCursor(hInst, MAKEINTRESOURCE(0xA1));
	RCT2_GLOBAL(RCT2_ADDRESS_HCURSOR_UP_ARROW,			HCURSOR) = LoadCursor(hInst, MAKEINTRESOURCE(0x6D));
	RCT2_GLOBAL(RCT2_ADDRESS_HCURSOR_UP_DOWN_ARROW,		HCURSOR) = LoadCursor(hInst, MAKEINTRESOURCE(0x6E));
	RCT2_GLOBAL(RCT2_ADDRESS_HCURSOR_HAND_POINT,		HCURSOR) = LoadCursor(hInst, MAKEINTRESOURCE(0x70));
	RCT2_GLOBAL(RCT2_ADDRESS_HCURSOR_ZZZ,				HCURSOR) = LoadCursor(hInst, MAKEINTRESOURCE(0x78));
	RCT2_GLOBAL(RCT2_ADDRESS_HCURSOR_DIAGONAL_ARROWS,	HCURSOR) = LoadCursor(hInst, MAKEINTRESOURCE(0x77));
	RCT2_GLOBAL(RCT2_ADDRESS_HCURSOR_PICKER,			HCURSOR) = LoadCursor(hInst, MAKEINTRESOURCE(0x7C));
	RCT2_GLOBAL(RCT2_ADDRESS_HCURSOR_TREE_DOWN,			HCURSOR) = LoadCursor(hInst, MAKEINTRESOURCE(0x83));
	RCT2_GLOBAL(RCT2_ADDRESS_HCURSOR_FOUNTAIN_DOWN,		HCURSOR) = LoadCursor(hInst, MAKEINTRESOURCE(0x7F));
	RCT2_GLOBAL(RCT2_ADDRESS_HCURSOR_STATUE_DOWN,		HCURSOR) = LoadCursor(hInst, MAKEINTRESOURCE(0x80));
	RCT2_GLOBAL(RCT2_ADDRESS_HCURSOR_BENCH_DOWN,		HCURSOR) = LoadCursor(hInst, MAKEINTRESOURCE(0x81));
	RCT2_GLOBAL(RCT2_ADDRESS_HCURSOR_CROSS_HAIR,		HCURSOR) = LoadCursor(hInst, MAKEINTRESOURCE(0x82));
	RCT2_GLOBAL(RCT2_ADDRESS_HCURSOR_BIN_DOWN,			HCURSOR) = LoadCursor(hInst, MAKEINTRESOURCE(0x84));
	RCT2_GLOBAL(RCT2_ADDRESS_HCURSOR_LAMPPOST_DOWN,		HCURSOR) = LoadCursor(hInst, MAKEINTRESOURCE(0x85));
	RCT2_GLOBAL(RCT2_ADDRESS_HCURSOR_FENCE_DOWN,		HCURSOR) = LoadCursor(hInst, MAKEINTRESOURCE(0x8A));
	RCT2_GLOBAL(RCT2_ADDRESS_HCURSOR_FLOWER_DOWN,		HCURSOR) = LoadCursor(hInst, MAKEINTRESOURCE(0x89));
	RCT2_GLOBAL(RCT2_ADDRESS_HCURSOR_PATH_DOWN,			HCURSOR) = LoadCursor(hInst, MAKEINTRESOURCE(0x8B));
	RCT2_GLOBAL(RCT2_ADDRESS_HCURSOR_DIG_DOWN,			HCURSOR) = LoadCursor(hInst, MAKEINTRESOURCE(0x8D));
	RCT2_GLOBAL(RCT2_ADDRESS_HCURSOR_WATER_DOWN,		HCURSOR) = LoadCursor(hInst, MAKEINTRESOURCE(0x8E));
	RCT2_GLOBAL(RCT2_ADDRESS_HCURSOR_HOUSE_DOWN,		HCURSOR) = LoadCursor(hInst, MAKEINTRESOURCE(0x8F));
	RCT2_GLOBAL(RCT2_ADDRESS_HCURSOR_VOLCANO_DOWN,		HCURSOR) = LoadCursor(hInst, MAKEINTRESOURCE(0x90));
	RCT2_GLOBAL(RCT2_ADDRESS_HCURSOR_WALK_DOWN,			HCURSOR) = LoadCursor(hInst, MAKEINTRESOURCE(0x91));
	RCT2_GLOBAL(RCT2_ADDRESS_HCURSOR_PAINT_DOWN,		HCURSOR) = LoadCursor(hInst, MAKEINTRESOURCE(0x9E));
	RCT2_GLOBAL(RCT2_ADDRESS_HCURSOR_ENTRANCE_DOWN,		HCURSOR) = LoadCursor(hInst, MAKEINTRESOURCE(0x9F));
	RCT2_GLOBAL(RCT2_ADDRESS_HCURSOR_HAND_OPEN,			HCURSOR) = LoadCursor(hInst, MAKEINTRESOURCE(0xA6));
	RCT2_GLOBAL(RCT2_ADDRESS_HCURSOR_HAND_CLOSED,		HCURSOR) = LoadCursor(hInst, MAKEINTRESOURCE(0xA5));

	_cursors[0] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	_cursors[1] = SDL_CreateCursor(blank_cursor_data, blank_cursor_mask, 32, 32, BLANK_CURSOR_HOTX, BLANK_CURSOR_HOTY);
	_cursors[2] = SDL_CreateCursor(up_arrow_cursor_data, up_arrow_cursor_mask, 32, 32, UP_ARROW_CURSOR_HOTX, UP_ARROW_CURSOR_HOTY);
	_cursors[3] = SDL_CreateCursor(up_down_arrow_cursor_data, up_down_arrow_cursor_mask, 32, 32, UP_DOWN_ARROW_CURSOR_HOTX, UP_DOWN_ARROW_CURSOR_HOTY);
	_cursors[4] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
	_cursors[5] = SDL_CreateCursor(zzz_cursor_data, zzz_cursor_mask, 32, 32, ZZZ_CURSOR_HOTX, ZZZ_CURSOR_HOTY);
	_cursors[6] = SDL_CreateCursor(diagonal_arrow_cursor_data, diagonal_arrow_cursor_mask, 32, 32, DIAGONAL_ARROW_CURSOR_HOTX, DIAGONAL_ARROW_CURSOR_HOTY);
	_cursors[7] = SDL_CreateCursor(picker_cursor_data, picker_cursor_mask, 32, 32, PICKER_CURSOR_HOTX, PICKER_CURSOR_HOTY);
	_cursors[8] = SDL_CreateCursor(tree_down_cursor_data, tree_down_cursor_mask, 32, 32, TREE_DOWN_CURSOR_HOTX, TREE_DOWN_CURSOR_HOTY);
	_cursors[9] = SDL_CreateCursor(fountain_down_cursor_data, fountain_down_cursor_mask, 32, 32, FOUNTAIN_DOWN_CURSOR_HOTX, FOUNTAIN_DOWN_CURSOR_HOTY);
	_cursors[10] = SDL_CreateCursor(statue_down_cursor_data, statue_down_cursor_mask, 32, 32, STATUE_DOWN_CURSOR_HOTX, STATUE_DOWN_CURSOR_HOTY);
	_cursors[11] = SDL_CreateCursor(bench_down_cursor_data, bench_down_cursor_mask, 32, 32, BENCH_DOWN_CURSOR_HOTX, BENCH_DOWN_CURSOR_HOTY);
	_cursors[12] = SDL_CreateCursor(cross_hair_cursor_data, cross_hair_cursor_mask, 32, 32, CROSS_HAIR_CURSOR_HOTX, CROSS_HAIR_CURSOR_HOTY);
	_cursors[13] = SDL_CreateCursor(bin_down_cursor_data, bin_down_cursor_mask, 32, 32, BIN_DOWN_CURSOR_HOTX, BIN_DOWN_CURSOR_HOTY);
	_cursors[14] = SDL_CreateCursor(lamppost_down_cursor_data, lamppost_down_cursor_mask, 32, 32, LAMPPOST_DOWN_CURSOR_HOTX, LAMPPOST_DOWN_CURSOR_HOTY);
	_cursors[15] = SDL_CreateCursor(fence_down_cursor_data, fence_down_cursor_mask, 32, 32, FENCE_DOWN_CURSOR_HOTX, FENCE_DOWN_CURSOR_HOTY);
	_cursors[16] = SDL_CreateCursor(flower_down_cursor_data, flower_down_cursor_mask, 32, 32, FLOWER_DOWN_CURSOR_HOTX, FLOWER_DOWN_CURSOR_HOTY);
	_cursors[17] = SDL_CreateCursor(path_down_cursor_data, path_down_cursor_mask, 32, 32, PATH_DOWN_CURSOR_HOTX, PATH_DOWN_CURSOR_HOTY);
	_cursors[18] = SDL_CreateCursor(dig_down_cursor_data, dig_down_cursor_mask, 32, 32, DIG_DOWN_CURSOR_HOTX, DIG_DOWN_CURSOR_HOTY);
	_cursors[19] = SDL_CreateCursor(water_down_cursor_data, water_down_cursor_mask, 32, 32, WATER_DOWN_CURSOR_HOTX, WATER_DOWN_CURSOR_HOTY);
	_cursors[20] = SDL_CreateCursor(house_down_cursor_data, house_down_cursor_mask, 32, 32, HOUSE_DOWN_CURSOR_HOTX, HOUSE_DOWN_CURSOR_HOTY);
	_cursors[21] = SDL_CreateCursor(volcano_down_cursor_data, volcano_down_cursor_mask, 32, 32, VOLCANO_DOWN_CURSOR_HOTX, VOLCANO_DOWN_CURSOR_HOTY);
	_cursors[22] = SDL_CreateCursor(walk_down_cursor_data, walk_down_cursor_mask, 32, 32, WALK_DOWN_CURSOR_HOTX, WALK_DOWN_CURSOR_HOTY);
	_cursors[23] = SDL_CreateCursor(paint_down_cursor_data, paint_down_cursor_mask, 32, 32, PAINT_DOWN_CURSOR_HOTX, PAINT_DOWN_CURSOR_HOTY);
	_cursors[24] = SDL_CreateCursor(entrance_down_cursor_data, entrance_down_cursor_mask, 32, 32, ENTRANCE_DOWN_CURSOR_HOTX, ENTRANCE_DOWN_CURSOR_HOTY);
	_cursors[25] = SDL_CreateCursor(hand_open_cursor_data, hand_open_cursor_mask, 32, 32, HAND_OPEN_CURSOR_HOTX, HAND_OPEN_CURSOR_HOTY);
	_cursors[26] = SDL_CreateCursor(hand_closed_cursor_data, hand_closed_cursor_mask, 32, 32, HAND_CLOSED_CURSOR_HOTX, HAND_CLOSED_CURSOR_HOTY);
	platform_set_cursor(CURSOR_ARROW);
	RCT2_GLOBAL(0x14241BC, uint32) = 0;
}

/**
 * 
 *  rct2: 0x00407D80
 */
int platform_get_cursor_pos(int* x, int* y)
{
	POINT point;
	GetCursorPos(&point);
	*x = point.x;
	*y = point.y;
}

void platform_refresh_video()
{
	int width = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16);
	int height = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16);
	
	SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, gConfigGeneral.minimize_fullscreen_focus_loss ? "1" : "0");

	if (gConfigGeneral.hardware_display) {
		if (gRenderer == NULL)
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

		if (gBufferTexture != NULL)
			SDL_DestroyTexture(gBufferTexture);

		if (gBufferTextureFormat != NULL)
			SDL_FreeFormat(gBufferTextureFormat);

		SDL_RendererInfo rendererinfo;
		SDL_GetRendererInfo(gRenderer, &rendererinfo);
		Uint32 pixelformat = SDL_PIXELFORMAT_UNKNOWN;
		for(unsigned int i = 0; i < rendererinfo.num_texture_formats; i++){
			Uint32 format = rendererinfo.texture_formats[i];
			if(!SDL_ISPIXELFORMAT_FOURCC(format) && !SDL_ISPIXELFORMAT_INDEXED(format) && (pixelformat == SDL_PIXELFORMAT_UNKNOWN || SDL_BYTESPERPIXEL(format) < SDL_BYTESPERPIXEL(pixelformat))){
				pixelformat = format;
			}
		}
	
		gBufferTexture = SDL_CreateTexture(gRenderer, pixelformat, SDL_TEXTUREACCESS_STREAMING, width, height);
		Uint32 format;
		SDL_QueryTexture(gBufferTexture, &format, 0, 0, 0);
		gBufferTextureFormat = SDL_AllocFormat(format);
		platform_refresh_screenbuffer(width, height, width);
	} else {
		if (_surface != NULL)
			SDL_FreeSurface(_surface);
		if (_palette != NULL)
			SDL_FreePalette(_palette);

		_surface = SDL_CreateRGBSurface(0, width, height, 8, 0, 0, 0, 0);
		_palette = SDL_AllocPalette(256);

		if (!_surface || !_palette) {
			log_fatal("%p || %p == NULL %s", _surface, _palette, SDL_GetError());
			exit(-1);
		}

		if (SDL_SetSurfacePalette(_surface, _palette)) {
			log_fatal("SDL_SetSurfacePalette failed %s", SDL_GetError());
			exit(-1);
		}

		platform_refresh_screenbuffer(width, height, _surface->pitch);
	}
}

static void platform_refresh_screenbuffer(int width, int height, int pitch)
{
	int newScreenBufferSize = pitch * height;
	char *newScreenBuffer = (char*)malloc(newScreenBufferSize);
	if (_screenBuffer == NULL) {
		memset(newScreenBuffer, 0, newScreenBufferSize);
	} else {
		if (_screenBufferPitch == pitch) {
			memcpy(newScreenBuffer, _screenBuffer, min(_screenBufferSize, newScreenBufferSize));
		} else {
			char *src = _screenBuffer;
			char *dst = newScreenBuffer;

			int minWidth = min(_screenBufferWidth, width);
			int minHeight = min(_screenBufferHeight, height);
			for (int y = 0; y < minHeight; y++) {
				memcpy(dst, src, minWidth);
				if (pitch - minWidth > 0)
					memset(dst + minWidth, 0, pitch - minWidth);

				src += _screenBufferPitch;
				dst += pitch;
			}
		}
		//if (newScreenBufferSize - _screenBufferSize > 0)
		//	memset((uint8*)newScreenBuffer + _screenBufferSize, 0, newScreenBufferSize - _screenBufferSize);
		free(_screenBuffer);
	}

	_screenBuffer = newScreenBuffer;
	_screenBufferSize = newScreenBufferSize;
	_screenBufferWidth = width;
	_screenBufferHeight = height;
	_screenBufferPitch = pitch;

	rct_drawpixelinfo *screenDPI;
	screenDPI = RCT2_ADDRESS(RCT2_ADDRESS_SCREEN_DPI, rct_drawpixelinfo);
	screenDPI->bits = _screenBuffer;
	screenDPI->x = 0;
	screenDPI->y = 0;
	screenDPI->width = width;
	screenDPI->height = height;
	screenDPI->pitch = _screenBufferPitch - width;

	RCT2_GLOBAL(0x009ABDF0, uint8) = 6;
	RCT2_GLOBAL(0x009ABDF1, uint8) = 3;
	RCT2_GLOBAL(0x009ABDF2, uint8) = 1;
	RCT2_GLOBAL(RCT2_ADDRESS_DIRTY_BLOCK_WIDTH, uint16) = 64;
	RCT2_GLOBAL(RCT2_ADDRESS_DIRTY_BLOCK_HEIGHT, uint16) = 8;
	RCT2_GLOBAL(RCT2_ADDRESS_DIRTY_BLOCK_COLUMNS, uint32) = (width >> 6) + 1;
	RCT2_GLOBAL(RCT2_ADDRESS_DIRTY_BLOCK_ROWS, uint32) = (height >> 3) + 1;
}