#pragma once

extern "C" {
#include "../common.h"
}

/**
 * Class to accuately measure elapsed time with high precision.
 */
class Stopwatch
{
private:
	/** Number of ticks in a second. */
	static uint64 Frequency;

	uint64 _total;
	uint64 _last;
	bool _isRunning;

	static uint64 QueryFrequency();
	static uint64 QueryCurrentTicks();

public:
	bool IsRunning() { return _isRunning; }

	Stopwatch();

	uint64 GetElapsedTicks();
	uint64 GetElapsedMilliseconds();

	void Reset();
	void Start();
	void Restart();
	void Stop();
};
