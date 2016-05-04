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

extern sint32 gTitleScriptCommand;
extern uint8 gTitleScriptSave;
extern sint32 gTitleScriptSkipTo;
extern sint32 gTitleScriptSkipLoad;

void title_load();
void title_update();
void title_skip_from_beginning();
void title_script_get_line(SDL_RWops *file, char *parts);
bool title_refresh_sequence();
void title_fix_location();
void DrawOpenRCT2(int x, int y);

#endif
