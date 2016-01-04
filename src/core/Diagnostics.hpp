#pragma once

#if _WIN32
#include <debugapi.h>
#endif

/**
 * Utility methods for asserting and logging.
 */
namespace Debug
{
	void Break()
	{
#if DEBUG
#if _WIN32
		if (IsDebuggerPresent()) {
			DebugBreak();
		}
#endif
#endif
	}
}
