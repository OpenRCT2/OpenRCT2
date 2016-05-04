#ifndef _TITLE_SEQUENCES_H_
#define _TITLE_SEQUENCES_H_

#include "../common.h"
#include "window.h"
#include "../config.h"
#include "../title.h"

#define TITLE_SEQUENCE_DEFAULT_PRESETS 2

title_command TitleScriptMakeCommand(int command, int parameter1, int parameter2);

#define TITLE_WAIT(t)			TitleScriptMakeCommand(TITLE_SCRIPT_WAIT,		t, 0)
#define TITLE_LOADMM()			TitleScriptMakeCommand(TITLE_SCRIPT_LOADMM,		0, 0)
#define TITLE_LOCATION(x, y)	TitleScriptMakeCommand(TITLE_SCRIPT_LOCATION,	x, y)
#define TITLE_ROTATE(n)			TitleScriptMakeCommand(TITLE_SCRIPT_ROTATE,		n, 0)
#define TITLE_ZOOM(d)			TitleScriptMakeCommand(TITLE_SCRIPT_ZOOM,		d, 0)
#define TITLE_RESTART()			TitleScriptMakeCommand(TITLE_SCRIPT_RESTART,	0, 0)
#define TITLE_LOAD(i)			TitleScriptMakeCommand(TITLE_SCRIPT_LOAD,		i, 0)

// The index of the current title sequence being edited
extern uint16 gCurrentTitleSequence;
// The index of the current title sequence being shown
extern uint16 gCurrentPreviewTitleSequence;

bool title_sequence_name_exists(const char *name);
bool title_sequence_save_exists(int preset, const char *name);

// Presets
void title_sequence_change_preset(int preset);
void title_sequence_create_preset(const char *name);
void title_sequence_duplicate_preset(int duplicate, const char *name);
void title_sequence_delete_preset(int preset);
void title_sequence_rename_preset(int preset, const char *newName);

// Saves
void title_sequence_add_save(int preset, const char *path, const char *newName);
void title_sequence_remove_save(int preset, int index);
void title_sequence_rename_save(int preset, int index, const char *newName);

// Commands
void title_sequence_add_command(int preset, title_command command);
void title_sequence_insert_command(int preset, int index, title_command command);
void title_sequence_delete_command(int preset, int index);
void title_sequence_move_down_command(int preset, int index);
void title_sequence_move_up_command(int preset, int index);

#endif
