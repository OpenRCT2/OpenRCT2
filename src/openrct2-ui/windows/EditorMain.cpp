/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>

/**
 * Creates the main editor window that holds the main viewport.
 *  rct2: 0x0066EF38
 */
rct_window* WindowEditorMainOpen()
{
    WindowMainOpen();

    context_open_window(WC_TOP_TOOLBAR);
    context_open_window_view(WV_EDITOR_BOTTOM_TOOLBAR);

    return window_get_main();
}
