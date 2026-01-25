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

    #include "../interface/Window.h"

    #include <openrct2/Context.h>
    #include <openrct2/interface/Viewport.h>
    #include <openrct2/scripting/ScriptEngine.h>
    #include <openrct2/ui/WindowManager.h>
    #include <openrct2/world/Map.h>

namespace OpenRCT2::Scripting
{
    class ScViewport;
    extern ScViewport gScViewport;
    class ScViewport final : public ScBase
    {
    private:
        struct OpaqueWindowData
        {
            WindowClass _class{};
            WindowNumber _number{};
        };

        static JSValue left_get(JSContext* ctx, JSValue thisVal)
        {
            auto viewport = GetViewport(thisVal);
            if (viewport != nullptr)
            {
                return JS_NewInt32(ctx, viewport->viewPos.x);
            }
            return JS_NewInt32(ctx, 0);
        }
        static JSValue left_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_INT32(valueInt, ctx, value);

            WindowBase* w;
            auto viewport = GetViewport(thisVal, &w);
            if (w != nullptr && viewport != nullptr)
            {
                SetViewLeftTop(w, viewport, valueInt, viewport->viewPos.y);
            }
            return JS_UNDEFINED;
        }

        static JSValue top_get(JSContext* ctx, JSValue thisVal)
        {
            auto viewport = GetViewport(thisVal);
            if (viewport != nullptr)
            {
                return JS_NewInt32(ctx, viewport->viewPos.y);
            }
            return JS_NewInt32(ctx, 0);
        }
        static JSValue top_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_INT32(valueInt, ctx, value);

            WindowBase* w;
            auto viewport = GetViewport(thisVal, &w);
            if (w != nullptr && viewport != nullptr)
            {
                SetViewLeftTop(w, viewport, viewport->viewPos.x, valueInt);
            }
            return JS_UNDEFINED;
        }

        static JSValue right_get(JSContext* ctx, JSValue thisVal)
        {
            auto viewport = GetViewport(thisVal);
            if (viewport != nullptr)
            {
                return JS_NewInt32(ctx, viewport->viewPos.x + viewport->ViewWidth());
            }
            return JS_NewInt32(ctx, 0);
        }
        static JSValue right_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_INT32(valueInt, ctx, value);

            WindowBase* w;
            auto viewport = GetViewport(thisVal, &w);
            if (w != nullptr && viewport != nullptr)
            {
                SetViewLeftTop(w, viewport, valueInt - viewport->ViewWidth(), viewport->viewPos.y);
            }
            return JS_UNDEFINED;
        }

        static JSValue bottom_get(JSContext* ctx, JSValue thisVal)
        {
            auto viewport = GetViewport(thisVal);
            if (viewport != nullptr)
            {
                return JS_NewInt32(ctx, viewport->viewPos.y + viewport->ViewHeight());
            }
            return JS_NewInt32(ctx, 0);
        }
        static JSValue bottom_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_INT32(valueInt, ctx, value);

            WindowBase* w;
            auto viewport = GetViewport(thisVal, &w);
            if (w != nullptr && viewport != nullptr)
            {
                SetViewLeftTop(w, viewport, viewport->viewPos.x, valueInt - viewport->ViewHeight());
            }
            return JS_UNDEFINED;
        }

        static JSValue rotation_get(JSContext* ctx, JSValue thisVal)
        {
            auto viewport = GetViewport(thisVal);
            if (viewport != nullptr)
            {
                return JS_NewInt32(ctx, viewport->rotation);
            }
            return JS_NewInt32(ctx, 0);
        }
        static JSValue rotation_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_INT32(valueInt, ctx, value);

            if (valueInt >= 0 && valueInt < 4)
            {
                auto w = GetWindow(thisVal);
                if (w != nullptr)
                {
                    while (w->viewport->rotation != valueInt)
                    {
                        ViewportRotateSingle(w, 1);
                    }
                }
            }
            return JS_UNDEFINED;
        }

        static JSValue zoom_get(JSContext* ctx, JSValue thisVal)
        {
            auto viewport = GetViewport(thisVal);
            if (viewport != nullptr)
            {
                return JS_NewInt32(ctx, static_cast<int8_t>(viewport->zoom));
            }
            return JS_NewInt32(ctx, 0);
        }
        static JSValue zoom_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_INT32(valueInt, ctx, value);

            auto w = GetWindow(thisVal);
            if (w != nullptr)
            {
                auto i8Value = static_cast<int8_t>(valueInt);
                WindowZoomSet(*w, ZoomLevel{ i8Value }, false);
            }
            return JS_UNDEFINED;
        }

        static JSValue visibilityFlags_get(JSContext* ctx, JSValue thisVal)
        {
            auto viewport = GetViewport(thisVal);
            if (viewport != nullptr)
            {
                return JS_NewInt64(ctx, viewport->flags);
            }
            return JS_NewInt32(ctx, 0);
        }
        static JSValue visibilityFlags_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_UINT32(valueUint, ctx, value)

            auto w = GetWindow(thisVal);
            if (w != nullptr)
            {
                auto viewport = w->viewport;
                if (viewport != nullptr)
                {
                    if (viewport->flags != valueUint)
                    {
                        viewport->flags = valueUint;
                        w->invalidate();
                    }
                }
            }
            return JS_UNDEFINED;
        }

        static JSValue getCentrePosition(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            auto viewport = GetViewport(thisVal);
            if (viewport != nullptr)
            {
                auto centre = viewport->viewPos + ScreenCoordsXY{ viewport->ViewWidth() / 2, viewport->ViewHeight() / 2 };
                auto coords = ViewportPosToMapPos(centre, 24, viewport->rotation);

                JSValue obj = JS_NewObject(ctx);
                JS_SetPropertyStr(ctx, obj, "x", JS_NewInt32(ctx, coords.x));
                JS_SetPropertyStr(ctx, obj, "y", JS_NewInt32(ctx, coords.y));
                return obj;
            }
            return JS_UNDEFINED;
        }

        static JSValue moveTo(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            auto w = GetWindow(thisVal);
            if (w != nullptr)
            {
                auto viewport = w->viewport;
                if (viewport != nullptr)
                {
                    auto coords = GetCoordsFromObject(ctx, argv[0]);
                    if (coords)
                    {
                        auto screenCoords = Translate3DTo2DWithZ(viewport->rotation, *coords);
                        auto left = screenCoords.x - (viewport->ViewWidth() / 2);
                        auto top = screenCoords.y - (viewport->ViewHeight() / 2);
                        SetViewLeftTop(w, viewport, left, top);
                    }
                }
            }
            return JS_UNDEFINED;
        }

        static JSValue scrollTo(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            auto w = GetWindow(thisVal);
            if (w != nullptr)
            {
                auto coords = GetCoordsFromObject(ctx, argv[0]);
                if (coords)
                {
                    WindowScrollToLocation(*w, *coords);
                }
            }
            return JS_UNDEFINED;
        }

    public:
        JSValue New(JSContext* ctx, WindowClass wClass, WindowNumber wNum = 0)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("left", ScViewport::left_get, ScViewport::left_set),
                JS_CGETSET_DEF("top", ScViewport::top_get, ScViewport::top_set),
                JS_CGETSET_DEF("right", ScViewport::right_get, ScViewport::right_set),
                JS_CGETSET_DEF("bottom", ScViewport::bottom_get, ScViewport::bottom_set),
                JS_CGETSET_DEF("rotation", ScViewport::rotation_get, ScViewport::rotation_set),
                JS_CGETSET_DEF("zoom", ScViewport::zoom_get, ScViewport::zoom_set),
                JS_CGETSET_DEF("visibilityFlags", ScViewport::visibilityFlags_get, ScViewport::visibilityFlags_set),
                JS_CFUNC_DEF("getCentrePosition", 0, ScViewport::getCentrePosition),
                JS_CFUNC_DEF("moveTo", 1, ScViewport::moveTo),
                JS_CFUNC_DEF("scrollTo", 1, ScViewport::scrollTo),
            };
            return MakeWithOpaque(ctx, funcs, new OpaqueWindowData{ wClass, wNum });
        }

        void Register(JSContext* ctx)
        {
            RegisterBaseStr(ctx, "Viewport", Finalize);
        }

        static void Finalize(JSRuntime* rt, JSValue thisVal)
        {
            OpaqueWindowData* data = gScViewport.GetOpaque<OpaqueWindowData*>(thisVal);
            if (data)
                delete data;
        }

    private:
        static WindowBase* GetWindow(JSValue thisVal)
        {
            OpaqueWindowData* data = gScViewport.GetOpaque<OpaqueWindowData*>(thisVal);
            if (!data)
                return nullptr;
            auto* windowMgr = Ui::GetWindowManager();
            return windowMgr->FindByNumber(data->_class, data->_number);
        }

        static Viewport* GetViewport(JSValue thisVal, WindowBase** wOut = nullptr)
        {
            auto w = GetWindow(thisVal);
            if (wOut)
                *wOut = w;
            if (w != nullptr)
            {
                return w->viewport;
            }
            return nullptr;
        }

        static void SetViewLeftTop(WindowBase* w, Viewport* viewport, int32_t left, int32_t top)
        {
            if (viewport->viewPos != ScreenCoordsXY(left, top))
            {
                viewport->viewPos.x = left;
                viewport->viewPos.y = top;
                w->flags.unset(WindowFlag::scrollingToLocation);
                w->savedViewPos.x = viewport->viewPos.x;
                w->savedViewPos.y = viewport->viewPos.y;
                viewport->Invalidate();
            }
        }

        static std::optional<CoordsXYZ> GetCoordsFromObject(JSContext* ctx, JSValue position)
        {
            if (JS_IsObject(position))
            {
                auto x = JSToOptionalInt(ctx, position, "x");
                auto y = JSToOptionalInt(ctx, position, "y");
                auto z = JSToOptionalInt(ctx, position, "z");
                if (x.has_value() && y.has_value())
                {
                    if (z.has_value())
                    {
                        return CoordsXYZ(x.value(), y.value(), z.value());
                    }

                    auto zTile = TileElementHeight(CoordsXY(x.value(), y.value()));
                    return CoordsXYZ(x.value(), y.value(), zTile);
                }
            }
            return std::nullopt;
        }
    };
} // namespace OpenRCT2::Scripting

#endif
