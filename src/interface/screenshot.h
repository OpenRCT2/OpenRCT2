#ifndef _SCREENSHOT_H_
#define _SCREENSHOT_H_

void screenshot_check();
int screenshot_dump();

void screenshot_giant();
int cmdline_for_screenshot(const char **argv, int argc);

#endif
