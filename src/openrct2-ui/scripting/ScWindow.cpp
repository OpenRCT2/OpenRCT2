/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "ScWindow.h"

    #include "ScWidget.hpp"

    #include <openrct2/interface/Window_internal.h>
    #include <openrct2/ui/WindowManager.h>

namespace OpenRCT2::Scripting
{
    using namespace OpenRCT2::Ui::Windows;

    extern ScWindow gScWindow;

    using OpaqueType = std::tuple<WindowClass, rct_windownumber>;

    JSValue ScWindow::classification_get(JSContext* ctx, JSValue thisVal)
    {
        OpaqueType* data = gScWindow.GetOpaque<OpaqueType*>(thisVal);
        return JS_NewInt32(ctx, static_cast<int32_t>(std::get<0>(*data)));
    }

    JSValue ScWindow::number_get(JSContext* ctx, JSValue thisVal)
    {
        OpaqueType* data = gScWindow.GetOpaque<OpaqueType*>(thisVal);
        return JS_NewInt32(ctx, std::get<1>(*data));
    }

    JSValue ScWindow::x_get(JSContext* ctx, JSValue thisVal)
    {
        auto w = GetWindow(thisVal);
        if (w != nullptr)
        {
            return JS_NewInt32(ctx, w->windowPos.x);
        }
        return JS_NewInt32(ctx, 0);
    }
    JSValue ScWindow::x_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_INT32(valueInt, ctx, value);

        auto w = GetWindow(thisVal);
        if (w != nullptr)
        {
            WindowSetPosition(*w, { valueInt, w->windowPos.y });
        }
        return JS_UNDEFINED;
    }
    JSValue ScWindow::y_get(JSContext* ctx, JSValue thisVal)
    {
        auto w = GetWindow(thisVal);
        if (w != nullptr)
        {
            return JS_NewInt32(ctx, w->windowPos.y);
        }
        return JS_NewInt32(ctx, 0);
    }
    JSValue ScWindow::y_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_INT32(valueInt, ctx, value);

        auto w = GetWindow(thisVal);
        if (w != nullptr)
        {
            WindowSetPosition(*w, { w->windowPos.x, valueInt });
        }
        return JS_UNDEFINED;
    }
    JSValue ScWindow::width_get(JSContext* ctx, JSValue thisVal)
    {
        auto w = GetWindow(thisVal);
        if (w != nullptr)
        {
            return JS_NewInt32(ctx, w->width);
        }
        return JS_NewInt32(ctx, 0);
    }
    JSValue ScWindow::width_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_INT32(valueInt, ctx, value);

        auto w = GetWindow(thisVal);
        if (w != nullptr)
        {
            if (WindowCanResize(*w))
            {
                WindowResizeByDelta(*w, valueInt - w->width, 0);
            }
            else
            {
                WindowSetResize(*w, { valueInt, w->min_height }, { valueInt, w->max_height });
            }
        }
        return JS_UNDEFINED;
    }
    JSValue ScWindow::height_get(JSContext* ctx, JSValue thisVal)
    {
        auto w = GetWindow(thisVal);
        if (w != nullptr)
        {
            return JS_NewInt32(ctx, w->height - w->getTitleBarDiffNormal());
        }
        return JS_NewInt32(ctx, 0);
    }
    JSValue ScWindow::height_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_INT32(valueInt, ctx, value);

        auto w = GetWindow(thisVal);
        if (w != nullptr)
        {
            valueInt += w->getTitleBarDiffNormal();
            if (WindowCanResize(*w))
            {
                WindowResizeByDelta(*w, 0, valueInt - w->height);
            }
            else
            {
                WindowSetResize(*w, { w->min_width, valueInt }, { w->max_width, valueInt });
            }
        }
        return JS_UNDEFINED;
    }
    JSValue ScWindow::minWidth_get(JSContext* ctx, JSValue thisVal)
    {
        auto w = GetWindow(thisVal);
        if (w != nullptr)
        {
            return JS_NewInt32(ctx, w->min_width);
        }
        return JS_NewInt32(ctx, 0);
    }
    JSValue ScWindow::minWidth_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_INT32(valueInt, ctx, value);

        auto w = GetWindow(thisVal);
        if (w != nullptr)
        {
            WindowSetResize(*w, { valueInt, w->min_height }, { w->max_width, w->max_height });
        }
        return JS_UNDEFINED;
    }
    JSValue ScWindow::maxWidth_get(JSContext* ctx, JSValue thisVal)
    {
        auto w = GetWindow(thisVal);
        if (w != nullptr)
        {
            return JS_NewInt32(ctx, w->max_width);
        }
        return JS_NewInt32(ctx, 0);
    }
    JSValue ScWindow::maxWidth_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_INT32(valueInt, ctx, value);

        auto w = GetWindow(thisVal);
        if (w != nullptr)
        {
            WindowSetResize(*w, { w->min_width, w->min_height }, { valueInt, w->max_height });
        }
        return JS_UNDEFINED;
    }
    JSValue ScWindow::minHeight_get(JSContext* ctx, JSValue thisVal)
    {
        auto w = GetWindow(thisVal);
        if (w != nullptr)
        {
            return JS_NewInt32(ctx, w->min_height - w->getTitleBarDiffNormal());
        }
        return JS_NewInt32(ctx, 0);
    }
    JSValue ScWindow::minHeight_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_INT32(valueInt, ctx, value);

        auto w = GetWindow(thisVal);
        if (w != nullptr)
        {
            valueInt += w->getTitleBarDiffNormal();
            WindowSetResize(*w, { w->min_width, valueInt }, { w->max_width, w->max_height });
        }
        return JS_UNDEFINED;
    }
    JSValue ScWindow::maxHeight_get(JSContext* ctx, JSValue thisVal)
    {
        auto w = GetWindow(thisVal);
        if (w != nullptr)
        {
            return JS_NewInt32(ctx, w->max_height - w->getTitleBarDiffNormal());
        }
        return JS_NewInt32(ctx, 0);
    }
    JSValue ScWindow::maxHeight_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_INT32(valueInt, ctx, value);

        auto w = GetWindow(thisVal);
        if (w != nullptr)
        {
            valueInt += w->getTitleBarDiffNormal();
            WindowSetResize(*w, { w->min_width, w->min_height }, { w->max_width, valueInt });
        }
        return JS_UNDEFINED;
    }
    JSValue ScWindow::isSticky_get(JSContext* ctx, JSValue thisVal)
    {
        auto w = GetWindow(thisVal);
        if (w != nullptr)
        {
            return JS_NewBool(ctx, (w->flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT)) != 0);
        }
        return JS_NewBool(ctx, false);
    }

    JSValue ScWindow::widgets_get(JSContext* ctx, JSValue thisVal)
    {
        JSValue result = JS_NewArray(ctx);
        auto w = GetWindow(thisVal);
        if (w != nullptr)
        {
            for (WidgetIndex widgetIndex = 0; widgetIndex < w->widgets.size(); widgetIndex++)
            {
                JS_SetPropertyInt64(ctx, result, widgetIndex, gScWidget.New(ctx, w, widgetIndex));
            }
        }
        return result;
    }

    JSValue ScWindow::colours_get(JSContext* ctx, JSValue thisVal)
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
    JSValue ScWindow::colours_set(JSContext* ctx, JSValue thisVal, JSValue colours)
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

    JSValue ScWindow::title_get(JSContext* ctx, JSValue thisVal)
    {
        auto w = GetWindow(thisVal);
        if (w != nullptr && w->classification == WindowClass::Custom)
        {
            return JSFromStdString(ctx, GetWindowTitle(w));
        }
        return JSFromStdString(ctx, {});
    }
    JSValue ScWindow::title_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_STR(valueStr, ctx, value);

        auto w = GetWindow(thisVal);
        if (w != nullptr && w->classification == WindowClass::Custom)
        {
            UpdateWindowTitle(w, valueStr);
        }
        return JS_UNDEFINED;
    }

    JSValue ScWindow::tabIndex_get(JSContext* ctx, JSValue thisVal)
    {
        auto w = GetWindow(thisVal);
        if (w != nullptr && w->classification == WindowClass::Custom)
        {
            return JS_NewInt32(ctx, w->page);
        }
        return JS_NewInt32(ctx, 0);
    }
    JSValue ScWindow::tabIndex_set(JSContext* ctx, JSValue thisVal, JSValue tab)
    {
        JS_UNPACK_INT32(tabNumber, ctx, tab);

        auto w = GetWindow(thisVal);
        if (w != nullptr && w->classification == WindowClass::Custom)
        {
            UpdateWindowTab(w, tabNumber);
        }
        return JS_UNDEFINED;
    }

    JSValue ScWindow::close(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        auto w = GetWindow(thisVal);
        if (w != nullptr)
        {
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->Close(*w);
        }
        return JS_UNDEFINED;
    }

    JSValue ScWindow::findWidget(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_UNPACK_STR(name, ctx, argv[0]);

        auto w = GetWindow(thisVal);
        if (w != nullptr)
        {
            auto widgetIndex = FindWidgetIndexByName(w, name);
            if (widgetIndex)
            {
                return gScWidget.New(ctx, w, *widgetIndex);
            }
        }
        return JS_NULL;
    }

    JSValue ScWindow::bringToFront(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
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

    JSValue ScWindow::New(JSContext* ctx, WindowBase* w)
    {
        return New(ctx, w->classification, w->number);
    }

    JSValue ScWindow::New(JSContext* ctx, WindowClass wClass, rct_windownumber wNum)
    {
        return MakeWithOpaque(ctx, funcs, new OpaqueType{ wClass, wNum });
    }

    void ScWindow::Register(JSContext* ctx)
    {
        RegisterBaseStr(ctx, "Window", Finalize);
    }

    void ScWindow::Finalize(JSRuntime* rt, JSValue thisVal)
    {
        OpaqueType* data = gScWindow.GetOpaque<OpaqueType*>(thisVal);
        if (data)
            delete data;
    }

    WindowBase* ScWindow::GetWindow(JSValue thisVal)
    {
        OpaqueType* data = gScWindow.GetOpaque<OpaqueType*>(thisVal);
        if (!data)
            return nullptr;
        auto* windowMgr = Ui::GetWindowManager();
        return windowMgr->FindByNumber(std::get<0>(*data), std::get<1>(*data));
    }
} // namespace OpenRCT2::Scripting

#endif
