/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2/drawing/ImageId.hpp>
#include <openrct2/interface/Widget.h>

ImageId GetColourButtonImage(colour_t colour);
rct_widget* GetWidgetByIndex(const rct_window& w, WidgetIndex widgetIndex);
