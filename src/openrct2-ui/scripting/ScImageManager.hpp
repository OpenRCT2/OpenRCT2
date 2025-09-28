/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "CustomImages.h"

    #include <openrct2/Context.h>
    #include <openrct2/SpriteIds.h>
    #include <openrct2/drawing/Image.h>

namespace OpenRCT2::Scripting
{
    class ScImageManager;
    extern ScImageManager gScImageManager;

    class ScImageManager final : public ScBase
    {
    public:
        void Register(JSContext* ctx)
        {
            RegisterBaseStr(ctx, "ImageManager");
        }

        JSValue New(JSContext* ctx)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CFUNC_DEF("getPredefinedRange", 1, ScImageManager::getPredefinedRange),
                JS_CFUNC_DEF("getAvailableAllocationRanges", 0, ScImageManager::getAvailableAllocationRanges),
                JS_CFUNC_DEF("allocate", 1, ScImageManager::allocate),
                JS_CFUNC_DEF("free", 1, ScImageManager::free),
                JS_CFUNC_DEF("getImageInfo", 1, ScImageManager::getImageInfo),
                JS_CFUNC_DEF("getPixelData", 1, ScImageManager::getPixelData),
                JS_CFUNC_DEF("setPixelData", 2, ScImageManager::setPixelData),
                JS_CFUNC_DEF("draw", 3, ScImageManager::draw)
            };
            return MakeWithOpaque(ctx, funcs, nullptr);
        }

    private:
        static JSValue getPredefinedRange(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_STR(name, ctx, argv[0]);
            if (name == "g1")
            {
                return CreateImageIndexRange(ctx, 0, SPR_G1_END);
            }
            else if (name == "g2")
            {
                return CreateImageIndexRange(ctx, SPR_G2_BEGIN, SPR_G2_END - SPR_G2_BEGIN);
            }
            else if (name == "fonts")
            {
                return CreateImageIndexRange(ctx, SPR_FONTS_BEGIN, SPR_FONTS_END - SPR_FONTS_BEGIN);
            }
            else if (name == "tracks")
            {
                return CreateImageIndexRange(ctx, SPR_TRACKS_BEGIN, SPR_TRACKS_END - SPR_TRACKS_BEGIN);
            }
            else if (name == "csg")
            {
                return CreateImageIndexRange(ctx, SPR_CSG_BEGIN, SPR_CSG_END - SPR_CSG_BEGIN);
            }
            else if (name == "allocated")
            {
                return CreateImageIndexRange(ctx, SPR_IMAGE_LIST_BEGIN, SPR_IMAGE_LIST_END - SPR_IMAGE_LIST_BEGIN);
            }
            else
            {
                return JS_NULL;
            }
        }

        static JSValue getAvailableAllocationRanges(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            auto ranges = GetAvailableAllocationRanges();
            JSValue arr = JS_NewArray(ctx);
            int64_t index = 0;
            for (const auto& range : ranges)
            {
                JS_SetPropertyInt64(ctx, arr, index, CreateImageIndexRange(ctx, range.BaseId, range.Count));
                index++;
            }
            return arr;
        }

        static JSValue allocate(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_INT32(count, ctx, argv[0])

            auto& scriptEngine = GetContext()->GetScriptEngine();
            auto plugin = scriptEngine.GetExecInfo().GetCurrentPlugin();
            auto range = AllocateCustomImages(plugin, count);
            return range ? CreateImageIndexRange(ctx, range->BaseId, range->Count) : JS_UNDEFINED;
        }

        static JSValue free(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            auto start = JSToOptionalInt64(ctx, argv[0], "start");
            auto count = JSToOptionalInt64(ctx, argv[0], "count");
            if (!start.has_value() || !count.has_value())
            {
                JS_ThrowPlainError(ctx, "Invalid argument");
                return JS_EXCEPTION;
            }

            ImageList range(start.value(), count.value());

            auto& scriptEngine = GetContext()->GetScriptEngine();
            auto plugin = scriptEngine.GetExecInfo().GetCurrentPlugin();
            if (!FreeCustomImages(plugin, range))
            {
                JS_ThrowPlainError(ctx, "This plugin did not allocate the specified image range.");
                return JS_EXCEPTION;
            }

            return JS_UNDEFINED;
        }

        static JSValue getImageInfo(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_INT32(id, ctx, argv[0]);

            return JSGetImageInfo(ctx, id);
        }

        static JSValue getPixelData(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_INT32(id, ctx, argv[0]);

            return JSGetImagePixelData(ctx, id);
        }

        static JSValue setPixelData(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_INT32(id, ctx, argv[0]);
            JSValue pixelData = argv[1];

            auto& scriptEngine = GetContext()->GetScriptEngine();
            auto plugin = scriptEngine.GetExecInfo().GetCurrentPlugin();
            if (!DoesPluginOwnImage(plugin, id))
            {
                JS_ThrowPlainError(ctx, "This plugin did not allocate the specified image range.");
                return JS_EXCEPTION;
            }

            try
            {
                JSSetPixelData(ctx, id, pixelData);
            }
            catch (const std::runtime_error& e)
            {
                JS_ThrowInternalError(ctx, "%s", e.what());
                return JS_EXCEPTION;
            }
            return JS_UNDEFINED;
        }

        static JSValue draw(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_INT32(id, ctx, argv[0])
            auto width = JSToOptionalInt64(ctx, argv[1], "width");
            auto height = JSToOptionalInt64(ctx, argv[1], "height");
            if (!width.has_value() || !height.has_value())
            {
                JS_ThrowPlainError(ctx, "Invalid size argument");
                return JS_EXCEPTION;
            }
            JSCallback callback(ctx, argv[2]);

            auto& scriptEngine = GetContext()->GetScriptEngine();
            auto plugin = scriptEngine.GetExecInfo().GetCurrentPlugin();
            if (!DoesPluginOwnImage(plugin, id))
            {
                JS_ThrowPlainError(ctx, "This plugin did not allocate the specified image range.");
                return JS_EXCEPTION;
            }

            JSDrawCustomImage(
                scriptEngine, id, { static_cast<int32_t>(width.value()), static_cast<int32_t>(height.value()) }, callback);
            return JS_UNDEFINED;
        }

        static JSValue CreateImageIndexRange(JSContext* ctx, size_t start, size_t count)
        {
            JSValue obj = JS_NewObject(ctx);
            JS_SetPropertyStr(ctx, obj, "start", JS_NewInt64(ctx, static_cast<uint32_t>(start)));
            JS_SetPropertyStr(ctx, obj, "count", JS_NewInt64(ctx, static_cast<uint32_t>(count)));
            return obj;
        }
    };
} // namespace OpenRCT2::Scripting

#endif
