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

#ifndef _STOPWATCH_H_
#define _STOPWATCH_H_

#include "../common.h"

/////////////////////////////
// C wrapper for Stopwatch //
/////////////////////////////

typedef struct stopwatch {
	void *context;
} stopwatch;

void stopwatch_create(stopwatch *stopwatch);
void stopwatch_dispose(stopwatch *stopwatch);

uint64 stopwatch_GetElapsedTicks(stopwatch *stopwatch);
uint64 stopwatch_GetElapsedMilliseconds(stopwatch *stopwatch);
void stopwatch_Reset(stopwatch *stopwatch);
void stopwatch_Start(stopwatch *stopwatch);
void stopwatch_Restart(stopwatch *stopwatch);
void stopwatch_Stop(stopwatch *stopwatch);

#endif
