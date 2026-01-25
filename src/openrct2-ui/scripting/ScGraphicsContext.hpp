/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

    #include "CustomImages.h"

    #include <openrct2/drawing/Drawing.h>
    #include <openrct2/drawing/Rectangle.h>
    #include <openrct2/drawing/RenderTarget.h>
    #include <openrct2/scripting/ScriptEngine.h>
    #include <quickjs.h>

using namespace OpenRCT2::Drawing;

namespace OpenRCT2::Scripting
{
    class ScGraphicsContext;
    extern ScGraphicsContext gScGraphicsContext;
    class ScGraphicsContext final : public ScBase
    {
    private:
        struct GraphicsData
        {
            RenderTarget _rt{};

            std::optional<colour_t> _colour{};
            std::optional<colour_t> _secondaryColour{};
            std::optional<colour_t> _tertiaryColour{};
            std::optional<uint8_t> _paletteId{};
            PaletteIndex _stroke{};
            PaletteIndex _fill{};
        };

    public:
        void Register(JSContext* ctx)
        {
            RegisterBaseStr(ctx, "GraphicsContext", Finalize);
        }

        static void Finalize(JSRuntime* rt, JSValue thisVal)
        {
            GraphicsData* data = gScGraphicsContext.GetOpaque<GraphicsData*>(thisVal);
            if (data)
                delete data;
        }

        JSValue New(JSContext* ctx, const RenderTarget& rt)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("colour", ScGraphicsContext::colour_get, ScGraphicsContext::colour_set),
                JS_CGETSET_DEF(
                    "secondaryColour", ScGraphicsContext::secondaryColour_get, ScGraphicsContext::secondaryColour_set),
                JS_CGETSET_DEF("ternaryColour", ScGraphicsContext::tertiaryColour_get, ScGraphicsContext::tertiaryColour_set),
                JS_CGETSET_DEF("tertiaryColour", ScGraphicsContext::tertiaryColour_get, ScGraphicsContext::tertiaryColour_set),
                JS_CGETSET_DEF("paletteId", ScGraphicsContext::paletteId_get, ScGraphicsContext::paletteId_set),
                JS_CGETSET_DEF("fill", ScGraphicsContext::fill_get, ScGraphicsContext::fill_set),
                JS_CGETSET_DEF("stroke", ScGraphicsContext::stroke_get, ScGraphicsContext::stroke_set),
                JS_CGETSET_DEF("width", ScGraphicsContext::width_get, nullptr),
                JS_CGETSET_DEF("height", ScGraphicsContext::height_get, nullptr),

                JS_CFUNC_DEF("getImage", 1, ScGraphicsContext::getImage),
                JS_CFUNC_DEF("measureText", 1, ScGraphicsContext::measureText),

                JS_CFUNC_DEF("box", 4, ScGraphicsContext::box),
                JS_CFUNC_DEF("clear", 0, ScGraphicsContext::clear),
                JS_CFUNC_DEF("clip", 4, ScGraphicsContext::clip),
                JS_CFUNC_DEF("image", 3, ScGraphicsContext::image),
                JS_CFUNC_DEF("line", 4, ScGraphicsContext::lineJS),
                JS_CFUNC_DEF("rect", 4, ScGraphicsContext::rect),
                JS_CFUNC_DEF("text", 3, ScGraphicsContext::text),
                JS_CFUNC_DEF("well", 4, ScGraphicsContext::well),
            };
            return MakeWithOpaque(ctx, funcs, new GraphicsData{ rt });
        }

    private:
        static JSValue colour_get(JSContext* ctx, JSValue thisVal)
        {
            GraphicsData* data = gScGraphicsContext.GetOpaque<GraphicsData*>(thisVal);
            return ToJSValue(ctx, data->_colour);
        }

        static JSValue colour_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            GraphicsData* data = gScGraphicsContext.GetOpaque<GraphicsData*>(thisVal);
            if (JS_IsNumber(value))
                data->_colour = static_cast<colour_t>(JSToInt(ctx, value));
            else
                data->_colour = {};
            return JS_UNDEFINED;
        }

        static JSValue secondaryColour_get(JSContext* ctx, JSValue thisVal)
        {
            GraphicsData* data = gScGraphicsContext.GetOpaque<GraphicsData*>(thisVal);
            return ToJSValue(ctx, data->_secondaryColour);
        }

        static JSValue secondaryColour_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            GraphicsData* data = gScGraphicsContext.GetOpaque<GraphicsData*>(thisVal);
            if (JS_IsNumber(value))
                data->_secondaryColour = static_cast<colour_t>(JSToInt(ctx, value));
            else
                data->_secondaryColour = {};
            return JS_UNDEFINED;
        }

        static JSValue tertiaryColour_get(JSContext* ctx, JSValue thisVal)
        {
            GraphicsData* data = gScGraphicsContext.GetOpaque<GraphicsData*>(thisVal);
            return ToJSValue(ctx, data->_tertiaryColour);
        }

        static JSValue tertiaryColour_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            GraphicsData* data = gScGraphicsContext.GetOpaque<GraphicsData*>(thisVal);
            if (JS_IsNumber(value))
                data->_tertiaryColour = static_cast<colour_t>(JSToInt(ctx, value));
            else
                data->_tertiaryColour = {};
            return JS_UNDEFINED;
        }

        static JSValue paletteId_get(JSContext* ctx, JSValue thisVal)
        {
            GraphicsData* data = gScGraphicsContext.GetOpaque<GraphicsData*>(thisVal);
            return ToJSValue(ctx, data->_paletteId);
        }

        static JSValue paletteId_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            GraphicsData* data = gScGraphicsContext.GetOpaque<GraphicsData*>(thisVal);
            if (JS_IsNumber(value))
                data->_paletteId = static_cast<uint8_t>(JSToInt(ctx, value));
            else
                data->_paletteId = {};
            return JS_UNDEFINED;
        }

        static JSValue fill_get(JSContext* ctx, JSValue thisVal)
        {
            GraphicsData* data = gScGraphicsContext.GetOpaque<GraphicsData*>(thisVal);
            return JS_NewInt32(ctx, EnumValue(data->_fill));
        }

        static JSValue fill_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_INT32(valueInt, ctx, value)
            GraphicsData* data = gScGraphicsContext.GetOpaque<GraphicsData*>(thisVal);
            data->_fill = static_cast<PaletteIndex>(valueInt);
            return JS_UNDEFINED;
        }

        static JSValue stroke_get(JSContext* ctx, JSValue thisVal)
        {
            GraphicsData* data = gScGraphicsContext.GetOpaque<GraphicsData*>(thisVal);
            return JS_NewInt32(ctx, EnumValue(data->_stroke));
        }

        static JSValue stroke_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_INT32(valueInt, ctx, value);
            GraphicsData* data = gScGraphicsContext.GetOpaque<GraphicsData*>(thisVal);
            data->_stroke = static_cast<PaletteIndex>(valueInt);
            return JS_UNDEFINED;
        }

        static JSValue width_get(JSContext* ctx, JSValue thisVal)
        {
            GraphicsData* data = gScGraphicsContext.GetOpaque<GraphicsData*>(thisVal);
            return JS_NewInt32(ctx, data->_rt.width);
        }

        static JSValue height_get(JSContext* ctx, JSValue thisVal)
        {
            GraphicsData* data = gScGraphicsContext.GetOpaque<GraphicsData*>(thisVal);
            return JS_NewInt32(ctx, data->_rt.height);
        }

        static JSValue getImage(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_UINT32(id, ctx, argv[0]);

            return JSGetImageInfo(ctx, id);
        }

        static JSValue measureText(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_STR(text, ctx, argv[0])

            auto width = GfxGetStringWidth(text, FontStyle::medium);
            auto height = StringGetHeightRaw(text.c_str(), FontStyle::medium);
            return ToJSValue(ctx, ScreenSize{ width, height });
        }

        static JSValue box(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_INT32(x, ctx, argv[0]);
            JS_UNPACK_INT32(y, ctx, argv[1]);
            JS_UNPACK_INT32(width, ctx, argv[2]);
            JS_UNPACK_INT32(height, ctx, argv[3]);
            GraphicsData* data = gScGraphicsContext.GetOpaque<GraphicsData*>(thisVal);

            Rectangle::fillInset(data->_rt, { x, y, x + width - 1, y + height - 1 }, { data->_colour.value_or(0) });
            return JS_UNDEFINED;
        }

        static JSValue well(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_INT32(x, ctx, argv[0]);
            JS_UNPACK_INT32(y, ctx, argv[1]);
            JS_UNPACK_INT32(width, ctx, argv[2]);
            JS_UNPACK_INT32(height, ctx, argv[3]);
            GraphicsData* data = gScGraphicsContext.GetOpaque<GraphicsData*>(thisVal);

            Rectangle::fillInset(
                data->_rt, { x, y, x + width - 1, y + height - 1 }, { data->_colour.value_or(0) },
                Rectangle::BorderStyle::inset, Rectangle::FillBrightness::light, Rectangle::FillMode::dontLightenWhenInset);
            return JS_UNDEFINED;
        }

        static JSValue clear(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            GraphicsData* data = gScGraphicsContext.GetOpaque<GraphicsData*>(thisVal);
            GfxClear(data->_rt, data->_fill);
            return JS_UNDEFINED;
        }

        static JSValue clip(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_INT32(x, ctx, argv[0]);
            JS_UNPACK_INT32(y, ctx, argv[1]);
            JS_UNPACK_INT32(width, ctx, argv[2]);
            JS_UNPACK_INT32(height, ctx, argv[3]);
            GraphicsData* data = gScGraphicsContext.GetOpaque<GraphicsData*>(thisVal);

            RenderTarget newRT;
            ClipRenderTarget(newRT, data->_rt, { x, y }, width, height);
            data->_rt = newRT;
            return JS_UNDEFINED;
        }

        static JSValue image(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_UINT32(id, ctx, argv[0]);
            JS_UNPACK_INT32(x, ctx, argv[1]);
            JS_UNPACK_INT32(y, ctx, argv[2]);
            GraphicsData* data = gScGraphicsContext.GetOpaque<GraphicsData*>(thisVal);

            ImageId img;
            img = img.WithIndex(id);
            if (data->_paletteId)
            {
                img = img.WithRemap(*data->_paletteId);
            }
            else
            {
                if (data->_colour)
                {
                    img = img.WithPrimary(*data->_colour);
                }
                if (data->_secondaryColour)
                {
                    img = img.WithSecondary(*data->_secondaryColour);
                }
            }

            GfxDrawSprite(data->_rt, img.WithTertiary(data->_tertiaryColour.value_or(0)), { x, y });
            return JS_UNDEFINED;
        }

        static void line(GraphicsData* data, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
        {
            GfxDrawLine(data->_rt, { { x1, y1 }, { x2, y2 } }, data->_stroke);
        }

        static JSValue lineJS(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_INT32(x1, ctx, argv[0]);
            JS_UNPACK_INT32(y1, ctx, argv[1]);
            JS_UNPACK_INT32(x2, ctx, argv[2]);
            JS_UNPACK_INT32(y2, ctx, argv[3]);
            GraphicsData* data = gScGraphicsContext.GetOpaque<GraphicsData*>(thisVal);
            line(data, x1, y1, x2, y2);
            return JS_UNDEFINED;
        }

        static JSValue rect(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_INT32(x, ctx, argv[0]);
            JS_UNPACK_INT32(y, ctx, argv[1]);
            JS_UNPACK_INT32(width, ctx, argv[2]);
            JS_UNPACK_INT32(height, ctx, argv[3]);
            GraphicsData* data = gScGraphicsContext.GetOpaque<GraphicsData*>(thisVal);

            if (data->_stroke != PaletteIndex::transparent)
            {
                line(data, x, y, x + width, y);
                line(data, x + width - 1, y + 1, x + width - 1, y + height - 1);
                line(data, x, y + height - 1, x + width, y + height - 1);
                line(data, x, y + 1, x, y + height - 1);

                x++;
                y++;
                width -= 2;
                height -= 2;
            }
            if (data->_fill != PaletteIndex::transparent)
            {
                Rectangle::fill(data->_rt, { x, y, x + width - 1, y + height - 1 }, data->_fill);
            }
            return JS_UNDEFINED;
        }

        static JSValue text(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_STR(text, ctx, argv[0]);
            JS_UNPACK_INT32(x, ctx, argv[1]);
            JS_UNPACK_INT32(y, ctx, argv[2]);
            GraphicsData* data = gScGraphicsContext.GetOpaque<GraphicsData*>(thisVal);
            DrawText(data->_rt, { x, y }, { data->_colour.value_or(0) }, text.c_str());
            return JS_UNDEFINED;
        }
    };
} // namespace OpenRCT2::Scripting

#endif
