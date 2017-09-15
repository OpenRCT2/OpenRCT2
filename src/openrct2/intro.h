#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#ifndef _INTRO_H_
#define _INTRO_H_

#include "drawing/drawing.h"

enum INTRO_STATE {
    INTRO_STATE_NONE,
    INTRO_STATE_PUBLISHER_BEGIN,
    INTRO_STATE_PUBLISHER_SCROLL,
    INTRO_STATE_DEVELOPER_BEGIN,
    INTRO_STATE_DEVELOPER_SCROLL,
    INTRO_STATE_LOGO_FADE_IN,
    INTRO_STATE_LOGO_WAIT,
    INTRO_STATE_LOGO_FADE_OUT,
    INTRO_STATE_DISCLAIMER_1,
    INTRO_STATE_DISCLAIMER_2,
    INTRO_STATE_CLEAR               = 254,
    INTRO_STATE_FINISH              = 255,
};

#ifdef __cplusplus
extern "C" {
#endif

extern uint8 gIntroState;

void intro_update();
void intro_draw(rct_drawpixelinfo *dpi);

#ifdef __cplusplus
}
#endif

#endif
