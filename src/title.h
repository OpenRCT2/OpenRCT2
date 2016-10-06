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

#ifndef _TITLE_H_
#define _TITLE_H_

#include <SDL.h>

enum {
	TITLE_SCRIPT_WAIT,
	TITLE_SCRIPT_LOADMM,
	TITLE_SCRIPT_LOCATION,
	TITLE_SCRIPT_ROTATE,
	TITLE_SCRIPT_ZOOM,
	TITLE_SCRIPT_RESTART,
	TITLE_SCRIPT_LOAD,
	TITLE_SCRIPT_END,
	TITLE_SCRIPT_SPEED,
	TITLE_SCRIPT_LOOP,
	TITLE_SCRIPT_ENDLOOP,
	TITLE_SCRIPT_LOADRCT1,
};

extern bool gTitleHideVersionInfo;

extern sint32 gTitleScriptCommand;
extern uint8 gTitleScriptSave;
extern sint32 gTitleScriptSkipTo;
extern sint32 gTitleScriptSkipLoad;

void title_load();
void title_create_windows();
void title_update();
void title_skip_from_beginning();
void title_script_get_line(SDL_RWops *file, char *parts);
bool title_refresh_sequence();
void title_fix_location();
void DrawOpenRCT2(rct_drawpixelinfo *dpi, int x, int y);

#endif
