#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include "../platform/platform.h"
#endif

#include "Stopwatch.hpp"

uint64 Stopwatch::Frequency = 0;

Stopwatch::Stopwatch()
{
	Reset();
}

uint64 Stopwatch::GetElapsedTicks()
{
	uint64 result = _total;
	if (_isRunning)
	{
		uint64 ticks = QueryCurrentTicks();
		if (ticks != 0) {
			result += QueryCurrentTicks() - _last;
		}
	}

	return _total;
}

uint64 Stopwatch::GetElapsedMilliseconds()
{
	if (Frequency == 0)
	{
		Frequency = QueryFrequency();
		if (Frequency == 0)
		{
			return 0;
		}
	}

	return (GetElapsedTicks() * 1000) / Frequency;
}

void Stopwatch::Reset()
{
	_isRunning = false;
	_total = 0;
	_last = 0;
}

void Stopwatch::Start()
{
	if (_isRunning) return;

	uint64 ticks = QueryCurrentTicks();
	if (ticks != 0)
	{
		_last = ticks;
		_isRunning = true;
	}
}

void Stopwatch::Restart()
{
	Reset();
	Start();
}

void Stopwatch::Stop()
{
	uint64 ticks = QueryCurrentTicks();
	if (ticks != 0) {
		_total += QueryCurrentTicks() - _last;
	}
	_isRunning = false;
}

uint64 Stopwatch::QueryFrequency()
{
#ifdef _WIN32
	LARGE_INTEGER frequency;
	if (QueryPerformanceFrequency(&frequency))
	{
		return frequency.QuadPart;
	}
	else
	{
		log_error("QueryPerformanceFrequency failed.");
		return 0;
	}
#else
	STUB();
	return 0;
#endif
}

uint64 Stopwatch::QueryCurrentTicks()
{
#ifdef _WIN32
	LARGE_INTEGER counter;
	if (QueryPerformanceCounter(&counter))
	{
		return counter.QuadPart;
	}
	else
	{
		log_error("QueryPerformanceCounter failed.");
		return 0;
	}
#else
	STUB();
	return 0;
#endif
}

extern "C" {
	#include "stopwatch.h"

	void stopwatch_create(stopwatch *stopwatch)
	{
		stopwatch->context = new Stopwatch();
	}

	void stopwatch_dispose(stopwatch *stopwatch)
	{
		delete ((Stopwatch*)stopwatch->context);
	}

	uint64 stopwatch_GetElapsedTicks(stopwatch *stopwatch)
	{
		Stopwatch *ctx = (Stopwatch*)stopwatch->context;
		return ctx->GetElapsedTicks();
	}

	uint64 stopwatch_GetElapsedMilliseconds(stopwatch *stopwatch)
	{
		Stopwatch *ctx = (Stopwatch*)stopwatch->context;
		return ctx->GetElapsedMilliseconds();
	}

	void stopwatch_Reset(stopwatch *stopwatch)
	{
		Stopwatch *ctx = (Stopwatch*)stopwatch->context;
		return ctx->Reset();
	}

	void stopwatch_Start(stopwatch *stopwatch)
	{
		Stopwatch *ctx = (Stopwatch*)stopwatch->context;
		return ctx->Start();
	}

	void stopwatch_Restart(stopwatch *stopwatch)
	{
		Stopwatch *ctx = (Stopwatch*)stopwatch->context;
		return ctx->Restart();
	}

	void stopwatch_Stop(stopwatch *stopwatch)
	{
		Stopwatch *ctx = (Stopwatch*)stopwatch->context;
		return ctx->Stop();
	}
}
