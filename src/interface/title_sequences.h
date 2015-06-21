/*****************************************************************************
* Copyright (c) 2014 Ted John, Peter Hill, Duncan Frost
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

#ifndef _TITLE_SEQUENCES_H_
#define _TITLE_SEQUENCES_H_

#include "../common.h"
#include "window.h"
#include "../config.h"

enum {
	TITLE_SCRIPT_WAIT,
	TITLE_SCRIPT_LOADMM,
	TITLE_SCRIPT_LOCATION,
	TITLE_SCRIPT_ROTATE,
	TITLE_SCRIPT_ZOOM,
	TITLE_SCRIPT_RESTART,
	TITLE_SCRIPT_LOAD
} TITLE_SCRIPT_COMMANDS;

title_command MakeCommand(int command, int parameter1, int parameter2);

#define TITLE_WAIT(t)			MakeCommand(TITLE_SCRIPT_WAIT,		t, 0)
#define TITLE_LOADMM()			MakeCommand(TITLE_SCRIPT_LOADMM,	0, 0)
#define TITLE_LOCATION(x, y)	MakeCommand(TITLE_SCRIPT_LOCATION,	x, y)
#define TITLE_ROTATE(n)			MakeCommand(TITLE_SCRIPT_ROTATE,	n, 0)
#define TITLE_ZOOM(d)			MakeCommand(TITLE_SCRIPT_ZOOM,		d, 0)
#define TITLE_RESTART()			MakeCommand(TITLE_SCRIPT_RESTART,	0, 0)
#define TITLE_LOAD(i)			MakeCommand(TITLE_SCRIPT_LOAD,		i, 0)

// The index of the current title sequence
extern uint16 gCurrentTitleSequence;

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

#endif