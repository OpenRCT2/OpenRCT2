#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "../common.h"
#include "../drawing/drawing.h"

extern bool gConsoleOpen;

void console_open();
void console_close();
void console_toggle();

void console_update();
void console_draw(rct_drawpixelinfo *dpi);

void console_input(int c);
void console_write(const utf8 *src);
void console_writeline(const utf8 *src);
void console_writeline_error(const utf8 *src);
void console_writeline_warning(const utf8 *src);
void console_printf(const utf8 *format, ...);
void console_execute(const utf8 *src);
void console_execute_silent(const utf8 *src);
void console_clear();
void console_clear_line();
void console_refresh_caret();
void console_scroll(int delta);

#endif