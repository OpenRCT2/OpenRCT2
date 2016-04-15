#ifndef _STOPWATCH_H_
#define _STOPWATCH_H_

#include "../common.h"

/////////////////////////////
// C wrapper for Stopwatch //
/////////////////////////////

typedef struct {
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
