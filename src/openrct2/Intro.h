/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _INTRO_H_
#define _INTRO_H_

#include "common.h"

struct rct_drawpixelinfo;

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

extern uint8_t gIntroState;

void intro_update();
void intro_draw(rct_drawpixelinfo *dpi);

#endif
