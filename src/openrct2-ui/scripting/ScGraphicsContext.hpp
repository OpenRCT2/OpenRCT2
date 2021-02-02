/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include <openrct2/drawing/Drawing.h>
#    include <openrct2/scripting/Duktape.hpp>

namespace OpenRCT2::Scripting
{
    class ScGraphicsContext
    {
    private:
        duk_context* _ctx{};
        rct_drawpixelinfo _dpi{};

        colour_t _colour{};
        uint8_t _stroke{};
        uint8_t _fill{};

    public:
        ScGraphicsContext(duk_context* ctx, const rct_drawpixelinfo& dpi)
            : _ctx(ctx)
            , _dpi(dpi)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScGraphicsContext::colour_get, &ScGraphicsContext::colour_set, "colour");
            dukglue_register_property(ctx, &ScGraphicsContext::fill_get, &ScGraphicsContext::fill_set, "fill");
            dukglue_register_property(ctx, &ScGraphicsContext::stroke_get, &ScGraphicsContext::stroke_set, "stroke");
            dukglue_register_method(ctx, &ScGraphicsContext::box, "box");
            dukglue_register_method(ctx, &ScGraphicsContext::clear, "clear");
            dukglue_register_method(ctx, &ScGraphicsContext::clip, "clip");
            dukglue_register_method(ctx, &ScGraphicsContext::line, "line");
            dukglue_register_method(ctx, &ScGraphicsContext::rect, "rect");
            dukglue_register_method(ctx, &ScGraphicsContext::well, "well");
        }

    private:
        colour_t colour_get() const
        {
            return _colour;
        }

        void colour_set(colour_t value)
        {
            _colour = value;
        }

        uint8_t fill_get() const
        {
            return _fill;
        }

        void fill_set(uint8_t value)
        {
            _fill = value;
        }

        uint8_t stroke_get() const
        {
            return _stroke;
        }

        void stroke_set(uint8_t value)
        {
            _stroke = value;
        }

        void box(int32_t x, int32_t y, int32_t width, int32_t height)
        {
            gfx_fill_rect_inset(&_dpi, { x, y, x + width - 1, y + height - 1 }, _colour, 0);
        }

        void well(int32_t x, int32_t y, int32_t width, int32_t height)
        {
            gfx_fill_rect_inset(&_dpi, { x, y, x + width - 1, y + height - 1 }, _colour, INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_DONT_LIGHTEN);
        }

        void clear()
        {
            gfx_clear(&_dpi, _fill);
        }

        void clip(int32_t x, int32_t y, int32_t width, int32_t height)
        {
            rct_drawpixelinfo newDpi;
            clip_drawpixelinfo(&newDpi, &_dpi, { x, y }, width, height);
            _dpi = newDpi;
        }

        void line(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
        {
            gfx_draw_line(&_dpi, { { x1, y1 }, { x2, y2 } }, _stroke);
        }

        void rect(int32_t x, int32_t y, int32_t width, int32_t height)
        {
            gfx_fill_rect(&_dpi, { x, y, x + width - 1, y + height - 1 }, _fill);
        }
    };
} // namespace OpenRCT2::Scripting

#endif
