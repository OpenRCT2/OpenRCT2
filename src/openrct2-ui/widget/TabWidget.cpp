/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TabWidget.h"

#include <openrct2-ui/interface/Widget.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/drawing/ColourMap.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/FilterPaletteIds.h>
#include <openrct2/localisation/StringIds.h>

using namespace OpenRCT2::Drawing;

namespace OpenRCT2::Ui::Widgets
{
    void Tab::draw(Drawing::RenderTarget& rt, Widget& widget, const WidgetIndex widgetIndex, const WindowBase& w)
    {
        if (widgetIsDisabled(w, widgetIndex))
            return;

        // TODO: old hack, remove
        if (widget.image.GetIndex() == kImageIndexUndefined)
        {
            // Set standard tab sprite to use.
            widget.image = ImageId(SPR_TAB, FilterPaletteID::paletteNull);
        }

        WidgetDrawImage(rt, w, widgetIndex);
    }
} // namespace OpenRCT2::Ui::Widgets
