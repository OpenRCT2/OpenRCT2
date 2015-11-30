#ifndef _CHAT_H_
#define _CHAT_H_

#include "../common.h"
#include "../drawing/drawing.h"

extern bool gChatOpen;

void chat_open();
void chat_close();
void chat_toggle();

void chat_init();
void chat_update();
void chat_draw();

void chat_history_add(const char *src);
void chat_input(int c);


#endif