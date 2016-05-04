#ifndef _EDITOR_H_
#define _EDITOR_H_

#include "rct2.h"
#include "addresses.h"

typedef enum {
	EDITOR_STEP_OBJECT_SELECTION,			// 0
	EDITOR_STEP_LANDSCAPE_EDITOR,			// 1
	EDITOR_STEP_INVENTIONS_LIST_SET_UP,		// 2
	EDITOR_STEP_OPTIONS_SELECTION,			// 3
	EDITOR_STEP_OBJECTIVE_SELECTION,		// 4
	EDITOR_STEP_SAVE_SCENARIO,				// 5
	EDITOR_STEP_ROLLERCOASTER_DESIGNER,		// 6
	EDITOR_STEP_TRACK_DESIGNS_MANAGER		// 7
} RCT2_EDITOR_STEP;

#define g_editor_step RCT2_GLOBAL(0x0141F570, uint8)

void editor_load();
void editor_convert_save_to_scenario();
void trackdesigner_load();
void trackmanager_load();
bool editor_load_landscape(const utf8 *path);

void editor_open_windows_for_current_step();

bool editor_check_park();
int editor_check_object_selection();

#endif
