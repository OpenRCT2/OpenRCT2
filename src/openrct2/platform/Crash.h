/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _OPENRCT2_CRASH_
#define _OPENRCT2_CRASH_

using CExceptionHandler = void *;

extern bool gOpenRCT2SilentBreakpad;
CExceptionHandler crash_init();

#endif  /* _OPENRCT2_CRASH_ */
