/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2/CmdlineSprite.h>

/**
 *
 */
int main(int argc, const char** argv)
{
    return cmdline_for_sprite(argv + 1, argc - 1);
}
