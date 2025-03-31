/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once
#include <openrct2-ui/interface/Window.h>

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "ScWidget.hpp"

    #include <openrct2/interface/Window.h>
    #include <openrct2/interface/Window_internal.h>
    #include <openrct2/ui/WindowManager.h>

namespace OpenRCT2::Scripting
{
    using namespace OpenRCT2::Ui::Windows;

    class ScWindow;
    extern ScWindow gScWindow;

    class ScWindow final : public ScBase
    {
    private:
        using OpaqueType = std::tuple<WindowClass, rct_windownumber>;

        static JSValue classification_get(JSContext* ctx, JSValue thisVal)
        {
            OpaqueType* data = gScWindow.GetOpaque<OpaqueType*>(thisVal);
            return JS_NewInt32(ctx, static_cast<int32_t>(std::get<0>(*data)));
        }

        static JSValue number_get(JSContext* ctx, JSValue thisVal)
        {
            OpaqueType* data = gScWindow.GetOpaque<OpaqueType*>(thisVal);
            return JS_NewInt32(ctx, std::get<1>(*data));
        }

        static JSValue x_get(JSContext* ctx, JSValue thisVal)
        {
            auto w = GetWindow(thisVal);
            if (w != nullptr)
            {
                return JS_NewInt32(ctx, w->windowPos.x);
            }
            return JS_NewInt32(ctx, 0);
        }
        static JSValue x_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            auto w = GetWindow(thisVal);
            if (w != nullptr && JS_IsNumber(value))
            {
                int32_t valueInt = -1;
                JS_ToInt32(ctx, &valueInt, value);
                WindowSetPosition(*w, { valueInt, w->windowPos.y });
            }
            return JS_UNDEFINED;
        }
        static JSValue y_get(JSContext* ctx, JSValue thisVal)
        {
            auto w = GetWindow(thisVal);
            if (w != nullptr)
            {
                return JS_NewInt32(ctx, w->windowPos.y);
            }
            return JS_NewInt32(ctx, 0);
        }
        static JSValue y_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            auto w = GetWindow(thisVal);
            if (w != nullptr && JS_IsNumber(value))
            {
                int32_t valueInt = -1;
                JS_ToInt32(ctx, &valueInt, value);
                WindowSetPosition(*w, { w->windowPos.x, valueInt });
            }
            return JS_UNDEFINED;
        }
        static JSValue width_get(JSContext* ctx, JSValue thisVal)
        {
            auto w = GetWindow(thisVal);
            if (w != nullptr)
            {
                return JS_NewInt32(ctx, w->width);
            }
            return JS_NewInt32(ctx, 0);
        }
        static JSValue width_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            auto w = GetWindow(thisVal);
            if (w != nullptr && JS_IsNumber(value))
            {
                int32_t valueInt = -1;
                JS_ToInt32(ctx, &valueInt, value);
                if (WindowCanResize(*w))
                {
                    WindowResize(*w, valueInt - w->width, 0);
                }
                else
                {
                    WindowSetResize(*w, { valueInt, w->min_height }, { valueInt, w->max_height });
                }
            }
            return JS_UNDEFINED;
        }
        static JSValue height_get(JSContext* ctx, JSValue thisVal)
        {
            auto w = GetWindow(thisVal);
            if (w != nullptr)
            {
                return JS_NewInt32(ctx, w->height);
            }
            return JS_NewInt32(ctx, 0);
        }
        static JSValue height_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            auto w = GetWindow(thisVal);
            if (w != nullptr && JS_IsNumber(value))
            {
                int32_t valueInt = -1;
                JS_ToInt32(ctx, &valueInt, value);
                if (WindowCanResize(*w))
                {
                    WindowResize(*w, 0, valueInt - w->height);
                }
                else
                {
                    WindowSetResize(*w, { w->min_width, valueInt }, { w->max_width, valueInt });
                }
            }
            return JS_UNDEFINED;
        }
        static JSValue minWidth_get(JSContext* ctx, JSValue thisVal)
        {
            auto w = GetWindow(thisVal);
            if (w != nullptr)
            {
                return JS_NewInt32(ctx, w->min_width);
            }
            return JS_NewInt32(ctx, 0);
        }
        static JSValue minWidth_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            auto w = GetWindow(thisVal);
            if (w != nullptr && JS_IsNumber(value))
            {
                int32_t valueInt = -1;
                JS_ToInt32(ctx, &valueInt, value);
                WindowSetResize(*w, { valueInt, w->min_height }, { w->max_width, w->max_height });
            }
            return JS_UNDEFINED;
        }
        static JSValue maxWidth_get(JSContext* ctx, JSValue thisVal)
        {
            auto w = GetWindow(thisVal);
            if (w != nullptr)
            {
                return JS_NewInt32(ctx, w->max_width);
            }
            return JS_NewInt32(ctx, 0);
        }
        static JSValue maxWidth_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            auto w = GetWindow(thisVal);
            if (w != nullptr && JS_IsNumber(value))
            {
                int32_t valueInt = -1;
                JS_ToInt32(ctx, &valueInt, value);
                WindowSetResize(*w, { w->min_width, w->min_height }, { valueInt, w->max_height });
            }
            return JS_UNDEFINED;
        }
        static JSValue minHeight_get(JSContext* ctx, JSValue thisVal)
        {
            auto w = GetWindow(thisVal);
            if (w != nullptr)
            {
                return JS_NewInt32(ctx, w->min_height);
            }
            return JS_NewInt32(ctx, 0);
        }
        static JSValue minHeight_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            auto w = GetWindow(thisVal);
            if (w != nullptr && JS_IsNumber(value))
            {
                int32_t valueInt = -1;
                JS_ToInt32(ctx, &valueInt, value);
                WindowSetResize(*w, { w->min_width, valueInt }, { w->max_width, w->max_height });
            }
            return JS_UNDEFINED;
        }
        static JSValue maxHeight_get(JSContext* ctx, JSValue thisVal)
        {
            auto w = GetWindow(thisVal);
            if (w != nullptr)
            {
                return JS_NewInt32(ctx, w->max_height);
            }
            return JS_NewInt32(ctx, 0);
        }
        static JSValue maxHeight_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            auto w = GetWindow(thisVal);
            if (w != nullptr && JS_IsNumber(value))
            {
                int32_t valueInt = -1;
                JS_ToInt32(ctx, &valueInt, value);
                WindowSetResize(*w, { w->min_width, w->min_height }, { w->max_width, valueInt });
            }
            return JS_UNDEFINED;
        }
        static JSValue isSticky_get(JSContext* ctx, JSValue thisVal)
        {
            auto w = GetWindow(thisVal);
            if (w != nullptr)
            {
                return JS_NewBool(ctx, (w->flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT)) != 0);
            }
            return JS_NewBool(ctx, false);
        }

        static JSValue widgets_get(JSContext* ctx, JSValue thisVal)
        {
            JSValue result = JS_NewArray(ctx);
            auto w = GetWindow(thisVal);
            if (w != nullptr)
            {
                for (WidgetIndex widgetIndex = 0; widgetIndex < w->widgets.size(); widgetIndex++)
                {
                    // TODO (mber)
                    // result.push_back(ScWidget::ToDukValue(ctx, w, widgetIndex));
                }
            }
            return result;
        }

        static JSValue colours_get(JSContext* ctx, JSValue thisVal)
        {
            JSValue result = JS_NewArray(ctx);
            auto w = GetWindow(thisVal);
            if (w != nullptr)
            {
                int64_t i = 0;
                for (auto c : w->colours)
                {
                    auto colour = c.colour;
                    if (c.hasFlag(ColourFlag::translucent))
                        colour |= kLegacyColourFlagTranslucent;

                    JS_SetPropertyInt64(ctx, result, i++, JS_NewInt32(ctx, colour));
                }
            }
            return result;
        }
        static JSValue colours_set(JSContext* ctx, JSValue thisVal, JSValue colours)
        {
            auto w = GetWindow(thisVal);
            if (w != nullptr && JS_IsArray(colours))
            {
                int64_t coloursLen = -1;
                JS_GetLength(ctx, colours, &coloursLen);
                for (int64_t i = 0; i < std::ssize(w->colours); i++)
                {
                    auto c = ColourWithFlags{ COLOUR_BLACK };
                    if (i < coloursLen)
                    {
                        JSValue elem = JS_GetPropertyInt64(ctx, colours, i);
                        if (JS_IsNumber(elem))
                        {
                            int32_t colorInt = -1;
                            JS_ToInt32(ctx, &colorInt, elem);
                            colour_t colour = colorInt & ~kLegacyColourFlagTranslucent;
                            auto isTranslucent = (colorInt & kLegacyColourFlagTranslucent);
                            c.colour = std::clamp<colour_t>(colour, COLOUR_BLACK, COLOUR_COUNT - 1);
                            c.flags = (isTranslucent ? EnumToFlag(ColourFlag::translucent) : 0);
                        }
                        JS_FreeValue(ctx, elem);
                    }
                    w->colours[i] = c;
                }
                w->Invalidate();
            }
            return JS_UNDEFINED;
        }

        static JSValue title_get(JSContext* ctx, JSValue thisVal)
        {
            auto w = GetWindow(thisVal);
            if (w != nullptr && w->classification == WindowClass::Custom)
            {
                // TODO (mber)
                // return GetWindowTitle(w);
            }
            return JS_NULL;
        }
        static JSValue title_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            if (!JS_IsString(value))
            {
                JS_ThrowTypeError(ctx, "Expected string");
                return JS_EXCEPTION;
            }
            auto w = GetWindow(thisVal);
            if (w != nullptr && w->classification == WindowClass::Custom)
            {
                const char* valueStr = JS_ToCString(ctx, value);
                UpdateWindowTitle(w, valueStr);
                JS_FreeCString(ctx, valueStr);
            }
            return JS_UNDEFINED;
        }

        static JSValue tabIndex_get(JSContext* ctx, JSValue thisVal)
        {
            auto w = GetWindow(thisVal);
            if (w != nullptr && w->classification == WindowClass::Custom)
            {
                return JS_NewInt32(ctx, w->page);
            }
            return JS_NewInt32(ctx, 0);
        }
        static JSValue tabIndex_set(JSContext* ctx, JSValue thisVal, JSValue tab)
        {
            if (JS_IsNumber(tab))
            {
                auto w = GetWindow(thisVal);
                if (w != nullptr && w->classification == WindowClass::Custom)
                {
                    int32_t tabNumber = -1;
                    JS_ToInt32(ctx, &tabNumber, tab);
                    // TODO (mber)
                    // UpdateWindowTab(w, tabNumber);
                }
            }
            return JS_UNDEFINED;
        }

        static JSValue close(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            auto w = GetWindow(thisVal);
            if (w != nullptr)
            {
                auto* windowMgr = Ui::GetWindowManager();
                windowMgr->Close(*w);
            }
            return JS_UNDEFINED;
        }

        static JSValue findWidget(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            throw new std::runtime_error("Not implemented");
            // TODO (mber) widgets
            // auto w = GetWindow(thisVal);
            // if (w != nullptr)
            // {
            //     auto widgetIndex = FindWidgetIndexByName(w, name);
            //     if (widgetIndex)
            //     {
            //         return ScWidget::ToDukValue(ctx, w, *widgetIndex);
            //     }
            // }
            // return GetObjectAsDukValue<ScWidget>(ctx, nullptr);
        }

        static JSValue bringToFront(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            auto* w = GetWindow(thisVal);
            if (w != nullptr)
            {
                auto* windowMgr = Ui::GetWindowManager();
                w = windowMgr->BringToFront(*w);
                w->flags |= WF_WHITE_BORDER_MASK;
            }
            return JS_UNDEFINED;
        }

        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF("classification", ScWindow::classification_get, nullptr),
            JS_CGETSET_DEF("number", ScWindow::number_get, nullptr),
            JS_CGETSET_DEF("x", ScWindow::x_get, ScWindow::x_set),
            JS_CGETSET_DEF("y", ScWindow::y_get, ScWindow::y_set),
            JS_CGETSET_DEF("width", ScWindow::width_get, ScWindow::width_set),
            JS_CGETSET_DEF("height", ScWindow::height_get, ScWindow::height_set),
            JS_CGETSET_DEF("minWidth", ScWindow::minWidth_get, ScWindow::minWidth_set),
            JS_CGETSET_DEF("maxWidth", ScWindow::maxWidth_get, ScWindow::maxWidth_set),
            JS_CGETSET_DEF("minHeight", ScWindow::minHeight_get, ScWindow::minHeight_set),
            JS_CGETSET_DEF("maxHeight", ScWindow::maxHeight_get, ScWindow::maxHeight_set),
            JS_CGETSET_DEF("isSticky", ScWindow::isSticky_get, nullptr),
            JS_CGETSET_DEF("widgets", ScWindow::widgets_get, nullptr),
            JS_CGETSET_DEF("colours", ScWindow::colours_get, ScWindow::colours_set),
            JS_CGETSET_DEF("title", ScWindow::title_get, ScWindow::title_set),
            JS_CGETSET_DEF("tabIndex", ScWindow::tabIndex_get, ScWindow::tabIndex_set),

            JS_CFUNC_DEF("close", 0, ScWindow::close),
            JS_CFUNC_DEF("findWidget", 1, ScWindow::findWidget),
            JS_CFUNC_DEF("bringToFront", 0, ScWindow::bringToFront)
        };

    public:
        JSValue New(JSContext* ctx, WindowBase* w)
        {
            return New(ctx, w->classification, w->number);
        }

        JSValue New(JSContext* ctx, WindowClass wClass, rct_windownumber wNum)
        {
            return MakeWithOpaque(ctx, funcs, new OpaqueType{ wClass, wNum });
        }

        void Register(JSContext* ctx)
        {
            RegisterBaseStr(ctx, "Window", Finalize);
        }

    private:
        static void Finalize(JSRuntime* rt, JSValue thisVal)
        {
            OpaqueType* data = gScWindow.GetOpaque<OpaqueType*>(thisVal);
            if (data)
                delete data;
        }

        static WindowBase* GetWindow(JSValue thisVal)
        {
            OpaqueType* data = gScWindow.GetOpaque<OpaqueType*>(thisVal);
            if (!data)
                return nullptr;
            auto* windowMgr = Ui::GetWindowManager();
            return windowMgr->FindByNumber(std::get<0>(*data), std::get<1>(*data));
        }
    };
} // namespace OpenRCT2::Scripting

#endif
