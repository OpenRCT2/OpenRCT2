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

typedef struct stopwatch_t {
	void *context;
} stopwatch_t;

void stopwatch_create(stopwatch_t *stopwatch);
void stopwatch_dispose(stopwatch_t *stopwatch);

uint64 stopwatch_GetElapsedTicks(stopwatch_t *stopwatch);
uint64 stopwatch_GetElapsedMilliseconds(stopwatch_t *stopwatch);
void stopwatch_Reset(stopwatch_t *stopwatch);
void stopwatch_Start(stopwatch_t *stopwatch);
void stopwatch_Restart(stopwatch_t *stopwatch);
void stopwatch_Stop(stopwatch_t *stopwatch);

#endif
