#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "../common.h"

extern bool gConsoleOpen;

void console_open();
void console_close();
void console_toggle();

void console_update();
void console_draw(rct_drawpixelinfo *dpi);

void console_input(int c);
void console_write(const char *src);
void console_writeline(const char *src);
void console_writeline_error(const char *src);
void console_writeline_warning(const char *src);
void console_printf(const char *format, ...);
void console_execute(const char *src);
void console_execute_silent(const char *src);
void console_clear();
void console_clear_line();
void console_refresh_caret();
void console_scroll(int delta);

#endif