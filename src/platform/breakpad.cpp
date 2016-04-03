#include "breakpad.h"

#ifdef USE_BREAKPAD
#include "client/linux/handler/exception_handler.h"
#include <stdio.h>

static bool dumpCallback(const google_breakpad::MinidumpDescriptor& descriptor, void* context, bool succeeded) {
	  printf("Dump path: %s\n", descriptor.path());
	  return succeeded;
}

extern "C" CExceptionHandler newCExceptionHandlerSimple(void)
{
	printf("init Simple breakpad\n");
	google_breakpad::MinidumpDescriptor descriptor("/tmp");
	//google_breakpad::ExceptionHandler eh(descriptor, NULL, dumpCallback, NULL, true, -1);
	return reinterpret_cast<void*>(new google_breakpad::ExceptionHandler(descriptor, NULL, dumpCallback, NULL, true, -1));
}
#endif // USE_BREAKPAD
