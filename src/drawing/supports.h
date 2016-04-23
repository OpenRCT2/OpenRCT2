#ifndef _DRAWING_SUPPORTS_H_
#define _DRAWING_SUPPORTS_H_

#include "../common.h"

bool wooden_a_supports_paint_setup(int supportType, int special, int height, uint32 imageColourFlags, bool* underground);
bool metal_a_supports_paint_setup(int supportType, int segment, int special, int height, uint32 imageColourFlags);
#endif
