#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include "diagnostic.h"

int _log_levels[DIAGNOSTIC_LEVEL_COUNT] = { 1, 1, 1, 0, 1 };
int _log_location_enabled = 1;

const char * _level_strings[] = {
	"FATAL",
	"ERROR",
	"WARNING",
	"VERBOSE",
	"INFO"
};

void diagnostic_log(int diagnosticLevel, const char *format, ...)
{
	FILE *stream;
	va_list args;

	if (!_log_levels[diagnosticLevel])
		return;

	stream = stderr;

	// Level
	fprintf(stream, "%s: ", _level_strings[diagnosticLevel]);

	// Message
	va_start(args, format);
	vfprintf(stream, format, args);
	va_end(args);

	// Line terminator
	fprintf(stream, "\n");
}

void diagnostic_log_with_location(int diagnosticLevel, const char *file, const char *function, int line, const char *format, ...)
{
	FILE *stream;
	va_list args;

	if (!_log_levels[diagnosticLevel])
		return;

	stream = stderr;

	// Level and source code information
	if (_log_location_enabled)
		fprintf(stream, "%s[%s:%d (%s)]: ", _level_strings[diagnosticLevel], file, line, function);
	else
		fprintf(stream, "%s: ", _level_strings[diagnosticLevel]);

	// Message
	va_start(args, format);
	vfprintf(stream, format, args);
	va_end(args);

	// Line terminator
	fprintf(stream, "\n");
}
