/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "../interface/Widget.h"
    #include "CustomListView.h"
    #include "CustomWindow.h"
    #include "ScViewport.hpp"
    #include "ScWindow.h"

    #include <memory>
    #include <openrct2/Context.h>
    #include <openrct2/scripting/IconNames.hpp>
    #include <openrct2/scripting/ScriptEngine.h>
    #include <openrct2/scripting/bindings/game/ScContext.hpp>
    #include <openrct2/ui/WindowManager.h>

namespace OpenRCT2::Scripting
{
    class ScWidget;
    extern ScWidget gScWidget;

    struct WidgetData
    {
        WindowClass _class{};
        rct_windownumber _number{};
        WidgetIndex _widgetIndex{};
    };

    class ScWidget : public ScBase
    {
    private:
        static JSValue window_get(JSContext* ctx, JSValue thisVal)
        {
            WidgetData* data = gScWidget.GetOpaque<WidgetData*>(thisVal);
            return gScWindow.New(ctx, data->_class, data->_number);
        }

        static JSValue name_get(JSContext* ctx, JSValue thisVal)
        {
            WidgetData* data = gScWidget.GetOpaque<WidgetData*>(thisVal);
            auto w = GetWindow(data->_class, data->_number);
            if (w != nullptr && IsCustomWindow(w))
            {
                return JSFromStdString(ctx, OpenRCT2::Ui::Windows::GetWidgetName(w, data->_widgetIndex));
            }
            return JSFromStdString(ctx, "");
        }

        static JSValue name_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            WidgetData* data = gScWidget.GetOpaque<WidgetData*>(thisVal);
            auto w = GetWindow(data->_class, data->_number);
            if (w != nullptr && IsCustomWindow(w))
            {
                OpenRCT2::Ui::Windows::SetWidgetName(w, data->_widgetIndex, JSToStdString(ctx, value));
            }
            return JS_UNDEFINED;
        }

        static JSValue type_get(JSContext* ctx, JSValue thisVal)
        {
            auto widget = GetWidget(thisVal);
            if (widget != nullptr)
            {
                switch (widget->type)
                {
                    case WidgetType::frame:
                        return JSFromStdString(ctx, "frame");
                    case WidgetType::resize:
                        return JSFromStdString(ctx, "resize");
                    case WidgetType::imgBtn:
                    case WidgetType::trnBtn:
                    case WidgetType::flatBtn:
                    case WidgetType::button:
                    case WidgetType::closeBox:
                        return JSFromStdString(ctx, "button");
                    case WidgetType::colourBtn:
                        return JSFromStdString(ctx, "colourpicker");
                    case WidgetType::tab:
                        return JSFromStdString(ctx, "tab");
                    case WidgetType::labelCentred:
                    case WidgetType::label:
                        return JSFromStdString(ctx, "label");
                    case WidgetType::tableHeader:
                        return JSFromStdString(ctx, "table_header");
                    case WidgetType::spinner:
                        return JSFromStdString(ctx, "spinner");
                    case WidgetType::dropdownMenu:
                        return JSFromStdString(ctx, "dropdown");
                    case WidgetType::viewport:
                        return JSFromStdString(ctx, "viewport");
                    case WidgetType::groupbox:
                        return JSFromStdString(ctx, "groupbox");
                    case WidgetType::caption:
                        return JSFromStdString(ctx, "caption");
                    case WidgetType::scroll:
                        return JSFromStdString(ctx, "listview");
                    case WidgetType::checkbox:
                        return JSFromStdString(ctx, "checkbox");
                    case WidgetType::textBox:
                        return JSFromStdString(ctx, "textbox");
                    case WidgetType::empty:
                        return JSFromStdString(ctx, "empty");
                    case WidgetType::placeholder:
                        return JSFromStdString(ctx, "placeholder");
                    case WidgetType::progressBar:
                        return JSFromStdString(ctx, "progress_bar");
                    case WidgetType::horizontalSeparator:
                        return JSFromStdString(ctx, "horizontal_separator");
                    case WidgetType::custom:
                        return JSFromStdString(ctx, "custom");
                }
            }
            return JSFromStdString(ctx, "unknown");
        }

        static JSValue x_get(JSContext* ctx, JSValue thisVal)
        {
            auto widget = GetWidget(thisVal);
            if (widget != nullptr)
            {
                return JS_NewInt32(ctx, widget->left);
            }
            return JS_NewInt32(ctx, 0);
        }
        static JSValue x_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_INT32(valueInt, ctx, value);

            auto widget = GetWidget(thisVal);
            if (widget != nullptr)
            {
                auto delta = valueInt - widget->left;

                Invalidate(thisVal);
                widget->left += delta;
                widget->right += delta;

                if (widget->type == WidgetType::dropdownMenu)
                {
                    auto buttonWidget = widget + 1;
                    buttonWidget->left += delta;
                    buttonWidget->right += delta;
                }
                else if (widget->type == WidgetType::spinner)
                {
                    auto upWidget = widget + 1;
                    upWidget->left += delta;
                    upWidget->right += delta;
                    auto downWidget = widget + 2;
                    downWidget->left += delta;
                    downWidget->right += delta;
                }

                Invalidate(thisVal, widget);
            }
            return JS_UNDEFINED;
        }

        static JSValue y_get(JSContext* ctx, JSValue thisVal)
        {
            auto widget = GetWidget(thisVal);
            if (widget != nullptr)
            {
                auto w = GetWindow(thisVal);
                return JS_NewInt32(ctx, widget->top - w->getTitleBarDiffNormal());
            }
            return JS_NewInt32(ctx, 0);
        }
        static JSValue y_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_INT32(valueInt, ctx, value);

            auto widget = GetWidget(thisVal);
            if (widget != nullptr)
            {
                auto w = GetWindow(thisVal);
                valueInt += w->getTitleBarDiffNormal();
                auto delta = valueInt - widget->top;

                Invalidate(thisVal);
                widget->top += delta;
                widget->bottom += delta;

                if (widget->type == WidgetType::dropdownMenu)
                {
                    auto buttonWidget = widget + 1;
                    buttonWidget->top += delta;
                    buttonWidget->bottom += delta;
                }
                else if (widget->type == WidgetType::spinner)
                {
                    auto upWidget = widget + 1;
                    upWidget->top += delta;
                    upWidget->bottom += delta;
                    auto downWidget = widget + 2;
                    downWidget->top += delta;
                    downWidget->bottom += delta;
                }

                Invalidate(thisVal, widget);
            }
            return JS_UNDEFINED;
        }

        static JSValue width_get(JSContext* ctx, JSValue thisVal)
        {
            auto widget = GetWidget(thisVal);
            if (widget != nullptr)
            {
                return JS_NewInt32(ctx, widget->width() + 1);
            }
            return JS_NewInt32(ctx, 0);
        }
        static JSValue width_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_INT32(valueInt, ctx, value);

            auto widget = GetWidget(thisVal);
            if (widget != nullptr)
            {
                auto delta = widget->left + valueInt - (widget->right + 1);

                Invalidate(thisVal);
                widget->right += delta;

                if (widget->type == WidgetType::dropdownMenu)
                {
                    auto buttonWidget = widget + 1;
                    buttonWidget->left += delta;
                    buttonWidget->right += delta;
                }
                else if (widget->type == WidgetType::spinner)
                {
                    auto upWidget = widget + 1;
                    upWidget->left += delta;
                    upWidget->right += delta;
                    auto downWidget = widget + 2;
                    downWidget->left += delta;
                    downWidget->right += delta;
                }

                Invalidate(thisVal, widget);
            }
            return JS_UNDEFINED;
        }

        static JSValue height_get(JSContext* ctx, JSValue thisVal)
        {
            auto widget = GetWidget(thisVal);
            if (widget != nullptr)
            {
                return JS_NewInt32(ctx, widget->height() + 1);
            }
            return JS_NewInt32(ctx, 0);
        }
        static JSValue height_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_INT32(valueInt, ctx, value);

            auto widget = GetWidget(thisVal);
            if (widget != nullptr)
            {
                auto delta = widget->top + valueInt - (widget->bottom + 1);

                Invalidate(thisVal);
                widget->bottom += delta;

                if (widget->type == WidgetType::dropdownMenu)
                {
                    auto buttonWidget = widget + 1;
                    buttonWidget->bottom += delta;
                }
                else if (widget->type == WidgetType::spinner)
                {
                    auto upWidget = widget + 1;
                    upWidget->bottom += delta;
                    auto downWidget = widget + 2;
                    downWidget->bottom += delta;
                }

                Invalidate(thisVal, widget);
            }
            return JS_UNDEFINED;
        }

        static JSValue tooltip_get(JSContext* ctx, JSValue thisVal)
        {
            WidgetData* data = gScWidget.GetOpaque<WidgetData*>(thisVal);
            auto w = GetWindow(data->_class, data->_number);
            if (w != nullptr && IsCustomWindow(w))
            {
                return JSFromStdString(ctx, OpenRCT2::Ui::Windows::GetWidgetTooltip(w, data->_widgetIndex));
            }
            return JSFromStdString(ctx, {});
        }
        static JSValue tooltip_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_STR(valueStr, ctx, value);

            WidgetData* data = gScWidget.GetOpaque<WidgetData*>(thisVal);
            auto w = GetWindow(data->_class, data->_number);
            if (w != nullptr && IsCustomWindow(w))
            {
                OpenRCT2::Ui::Windows::SetWidgetTooltip(w, data->_widgetIndex, valueStr);
            }
            return JS_UNDEFINED;
        }

        static JSValue isDisabled_get(JSContext* ctx, JSValue thisVal)
        {
            WidgetData* data = gScWidget.GetOpaque<WidgetData*>(thisVal);
            auto w = GetWindow(data->_class, data->_number);
            if (w != nullptr)
            {
                return JS_NewBool(ctx, Ui::widgetIsDisabled(*w, data->_widgetIndex));
            }
            return JS_NewBool(ctx, false);
        }
        static JSValue isDisabled_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value);

            WidgetData* data = gScWidget.GetOpaque<WidgetData*>(thisVal);
            auto w = GetWindow(data->_class, data->_number);
            if (w != nullptr)
            {
                Ui::widgetSetDisabled(*w, data->_widgetIndex, valueBool);

                auto widget = GetWidget(w, data->_widgetIndex);
                if (widget != nullptr)
                {
                    if (widget->type == WidgetType::dropdownMenu)
                    {
                        Ui::widgetSetDisabled(*w, data->_widgetIndex + 1, valueBool);
                    }
                    else if (widget->type == WidgetType::spinner)
                    {
                        Ui::widgetSetDisabled(*w, data->_widgetIndex + 1, valueBool);
                        Ui::widgetSetDisabled(*w, data->_widgetIndex + 2, valueBool);
                    }
                }
                Invalidate(thisVal, widget);
            }
            return JS_UNDEFINED;
        }

        static JSValue isVisible_get(JSContext* ctx, JSValue thisVal)
        {
            WidgetData* data = gScWidget.GetOpaque<WidgetData*>(thisVal);
            auto w = GetWindow(data->_class, data->_number);
            if (w != nullptr)
            {
                return JS_NewBool(ctx, Ui::widgetIsVisible(*w, data->_widgetIndex));
            }
            return JS_NewBool(ctx, false);
        }
        static JSValue isVisible_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value);

            WidgetData* data = gScWidget.GetOpaque<WidgetData*>(thisVal);
            auto w = GetWindow(data->_class, data->_number);
            if (w != nullptr)
            {
                Ui::widgetSetVisible(*w, data->_widgetIndex, valueBool);

                auto widget = GetWidget(w, data->_widgetIndex);
                if (widget != nullptr)
                {
                    if (widget->type == WidgetType::dropdownMenu)
                    {
                        Ui::widgetSetVisible(*w, data->_widgetIndex + 1, valueBool);
                    }
                    else if (widget->type == WidgetType::spinner)
                    {
                        Ui::widgetSetVisible(*w, data->_widgetIndex + 1, valueBool);
                        Ui::widgetSetVisible(*w, data->_widgetIndex + 2, valueBool);
                    }
                }
                Invalidate(thisVal, widget);
            }
            return JS_UNDEFINED;
        }

    protected:
        static JSValue text_get(JSContext* ctx, JSValue thisVal)
        {
            WidgetData* data = gScWidget.GetOpaque<WidgetData*>(thisVal);
            auto w = GetWindow(data->_class, data->_number);
            if (IsCustomWindow(w))
            {
                auto widget = GetWidget(w, data->_widgetIndex);
                if (widget != nullptr && (widget->flags.has(WidgetFlag::textIsString)) && widget->string != nullptr)
                {
                    return JSFromStdString(ctx, widget->string);
                }
            }
            return JSFromStdString(ctx, "");
        }

        static JSValue text_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_STR(valueStr, ctx, value);

            WidgetData* data = gScWidget.GetOpaque<WidgetData*>(thisVal);
            auto w = GetWindow(data->_class, data->_number);
            if (w != nullptr && IsCustomWindow(w))
            {
                Ui::Windows::UpdateWidgetText(w, data->_widgetIndex, valueStr);
            }
            return JS_UNDEFINED;
        }

    private:
        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF("window", ScWidget::window_get, nullptr),
            JS_CGETSET_DEF("name", ScWidget::name_get, ScWidget::name_set),
            JS_CGETSET_DEF("type", ScWidget::type_get, nullptr),
            JS_CGETSET_DEF("x", ScWidget::x_get, ScWidget::x_set),
            JS_CGETSET_DEF("y", ScWidget::y_get, ScWidget::y_set),
            JS_CGETSET_DEF("width", ScWidget::width_get, ScWidget::width_set),
            JS_CGETSET_DEF("height", ScWidget::height_get, &ScWidget::height_set),
            JS_CGETSET_DEF("tooltip", ScWidget::tooltip_get, &ScWidget::tooltip_set),
            JS_CGETSET_DEF("isDisabled", ScWidget::isDisabled_get, &ScWidget::isDisabled_set),
            JS_CGETSET_DEF("isVisible", ScWidget::isVisible_get, &ScWidget::isVisible_set),
        };

    public:
        JSValue New(JSContext* ctx, WindowBase* w, WidgetIndex widgetIndex);

        void Register(JSContext* ctx)
        {
            RegisterBaseStr(ctx, "Widget", Finalize);
        }

    private:
        static void Finalize(JSRuntime* rt, JSValue thisVal)
        {
            WidgetData* data = gScWidget.GetOpaque<WidgetData*>(thisVal);
            if (data)
                delete data;
        }

    protected:
        static WidgetData GetWidgetData(JSValue thisVal)
        {
            return *gScWidget.GetOpaque<WidgetData*>(thisVal);
        }

        static WindowBase* GetWindow(const WindowClass wClass, const rct_windownumber number)
        {
            if (wClass == WindowClass::MainWindow)
                return WindowGetMain();

            auto* windowMgr = Ui::GetWindowManager();
            return windowMgr->FindByNumber(wClass, number);
        }

        static WindowBase* GetWindow(JSValue thisVal)
        {
            WidgetData* data = gScWidget.GetOpaque<WidgetData*>(thisVal);
            return GetWindow(data->_class, data->_number);
        }

        static Widget* GetWidget(WindowBase* w, const WidgetIndex wIdx)
        {
            if (w != nullptr)
            {
                return &w->widgets[wIdx];
            }
            return nullptr;
        }
        static Widget* GetWidget(JSValue thisVal)
        {
            WidgetData* data = gScWidget.GetOpaque<WidgetData*>(thisVal);
            auto w = GetWindow(data->_class, data->_number);
            return GetWidget(w, data->_widgetIndex);
        }

        static bool IsCustomWindow(WindowBase* w)
        {
            if (w != nullptr)
            {
                return w->classification == WindowClass::Custom;
            }
            return false;
        }

        static void Invalidate(JSValue thisVal, const Widget* widget)
        {
            WidgetData* data = gScWidget.GetOpaque<WidgetData*>(thisVal);
            auto* windowMgr = Ui::GetWindowManager();
            if (widget != nullptr)
            {
                if (widget->type == WidgetType::dropdownMenu)
                {
                    windowMgr->InvalidateWidgetByNumber(data->_class, data->_number, data->_widgetIndex + 1);
                }
                else if (widget->type == WidgetType::spinner)
                {
                    windowMgr->InvalidateWidgetByNumber(data->_class, data->_number, data->_widgetIndex + 1);
                    windowMgr->InvalidateWidgetByNumber(data->_class, data->_number, data->_widgetIndex + 2);
                }
            }
            windowMgr->InvalidateWidgetByNumber(data->_class, data->_number, data->_widgetIndex);
        }

        static void Invalidate(JSValue thisVal)
        {
            WidgetData* data = gScWidget.GetOpaque<WidgetData*>(thisVal);
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->InvalidateWidgetByNumber(data->_class, data->_number, data->_widgetIndex);
        }
    };

    class ScButtonWidget : public ScWidget
    {
    public:
        static void AddFuncs(JSContext* ctx, JSValue obj)
        {
            // In the future it might be worth properly subclassing the widget type here.
            // Not just for this button class but also for all the other widget subclasses.
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("border", ScButtonWidget::border_get, ScButtonWidget::border_set),
                JS_CGETSET_DEF("isPressed", ScButtonWidget::isPressed_get, ScButtonWidget::isPressed_set),
                JS_CGETSET_DEF("image", ScButtonWidget::image_get, ScButtonWidget::image_set),
                JS_CGETSET_DEF("text", ScWidget::text_get, ScWidget::text_set),
            };
            JS_SetPropertyFunctionList(ctx, obj, funcs, std::size(funcs));
        }

    private:
        static JSValue border_get(JSContext* ctx, JSValue thisVal)
        {
            auto widget = GetWidget(thisVal);
            if (widget != nullptr)
            {
                return JS_NewBool(ctx, widget->type == WidgetType::imgBtn);
            }
            return JS_NewBool(ctx, false);
        }
        static JSValue border_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value);

            auto widget = GetWidget(thisVal);
            if (widget != nullptr && (widget->type == WidgetType::flatBtn || widget->type == WidgetType::imgBtn))
            {
                if (valueBool)
                    widget->type = WidgetType::imgBtn;
                else
                    widget->type = WidgetType::flatBtn;
                Invalidate(thisVal);
            }
            return JS_UNDEFINED;
        }

        static JSValue isPressed_get(JSContext* ctx, JSValue thisVal)
        {
            WidgetData data = GetWidgetData(thisVal);
            auto w = GetWindow(data._class, data._number);

            if (w != nullptr)
            {
                return JS_NewBool(ctx, Ui::widgetIsPressed(*w, data._widgetIndex));
            }
            return JS_NewBool(ctx, false);
        }
        static JSValue isPressed_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value);

            WidgetData data = GetWidgetData(thisVal);
            auto w = GetWindow(data._class, data._number);
            if (w != nullptr)
            {
                Ui::widgetSetCheckboxValue(*w, data._widgetIndex, valueBool ? 1 : 0);
                Invalidate(thisVal);
            }
            return JS_UNDEFINED;
        }

        static JSValue image_get(JSContext* ctx, JSValue thisVal)
        {
            auto widget = GetWidget(thisVal);
            if (widget != nullptr && (widget->type == WidgetType::flatBtn || widget->type == WidgetType::imgBtn))
            {
                if (GetTargetAPIVersion() <= kApiVersionG2Reorder)
                {
                    return JS_NewUint32(ctx, LegacyIconIndex(widget->image.GetIndex()));
                }
                return JS_NewUint32(ctx, widget->image.GetIndex());
            }
            return JS_NewUint32(ctx, 0);
        }

        static JSValue image_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            auto widget = GetWidget(thisVal);
            if (widget != nullptr && (widget->type == WidgetType::flatBtn || widget->type == WidgetType::imgBtn))
            {
                widget->image = ImageId(ImageFromJSValue(ctx, value));
                Invalidate(thisVal);
            }
            return JS_UNDEFINED;
        }
    };

    class ScCheckBoxWidget : public ScWidget
    {
    public:
        static void AddFuncs(JSContext* ctx, JSValue obj)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("isChecked", ScCheckBoxWidget::isChecked_get, ScCheckBoxWidget::isChecked_set),
                JS_CGETSET_DEF("text", ScWidget::text_get, ScWidget::text_set),
            };
            JS_SetPropertyFunctionList(ctx, obj, funcs, std::size(funcs));
        }

    private:
        static JSValue isChecked_get(JSContext* ctx, JSValue thisVal)
        {
            WidgetData data = GetWidgetData(thisVal);
            auto w = GetWindow(data._class, data._number);
            if (w != nullptr)
            {
                return JS_NewBool(ctx, Ui::widgetIsPressed(*w, data._widgetIndex));
            }
            return JS_NewBool(ctx, false);
        }
        static JSValue isChecked_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value);

            WidgetData data = GetWidgetData(thisVal);
            auto w = GetWindow(data._class, data._number);
            if (w != nullptr)
            {
                Ui::widgetSetCheckboxValue(*w, data._widgetIndex, valueBool ? 1 : 0);
                Invalidate(thisVal);
            }
            return JS_UNDEFINED;
        }
    };

    class ScColourPickerWidget : public ScWidget
    {
    public:
        static void AddFuncs(JSContext* ctx, JSValue obj)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("colour", ScColourPickerWidget::colour_get, ScColourPickerWidget::colour_set),
            };
            JS_SetPropertyFunctionList(ctx, obj, funcs, std::size(funcs));
        }

    private:
        static JSValue colour_get(JSContext* ctx, JSValue thisVal)
        {
            WidgetData data = GetWidgetData(thisVal);
            auto w = GetWindow(data._class, data._number);
            if (w != nullptr)
            {
                return JS_NewInt32(ctx, Ui::Windows::GetWidgetColour(w, data._widgetIndex));
            }
            return JS_NewInt32(ctx, COLOUR_BLACK);
        }
        static JSValue colour_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_INT32(valueInt, ctx, value);

            WidgetData data = GetWidgetData(thisVal);
            auto w = GetWindow(data._class, data._number);
            if (w != nullptr)
            {
                Ui::Windows::UpdateWidgetColour(w, data._widgetIndex, valueInt);
                Invalidate(thisVal);
            }
            return JS_UNDEFINED;
        }
    };

    class ScDropdownWidget : public ScWidget
    {
    public:
        static void AddFuncs(JSContext* ctx, JSValue obj)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("items", ScDropdownWidget::items_get, ScDropdownWidget::items_set),
                JS_CGETSET_DEF("selectedIndex", ScDropdownWidget::selectedIndex_get, ScDropdownWidget::selectedIndex_set),
                JS_CGETSET_DEF("text", ScWidget::text_get, ScWidget::text_set),
            };
            JS_SetPropertyFunctionList(ctx, obj, funcs, std::size(funcs));
        }

    private:
        static JSValue selectedIndex_get(JSContext* ctx, JSValue thisVal)
        {
            WidgetData data = GetWidgetData(thisVal);
            auto w = GetWindow(data._class, data._number);
            if (w != nullptr)
            {
                return JS_NewInt32(ctx, Ui::Windows::GetWidgetSelectedIndex(w, data._widgetIndex));
            }
            return JS_NewInt32(ctx, -1);
        }
        static JSValue selectedIndex_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_INT32(valueInt, ctx, value);

            WidgetData data = GetWidgetData(thisVal);
            auto w = GetWindow(data._class, data._number);
            if (w != nullptr)
            {
                Ui::Windows::UpdateWidgetSelectedIndex(w, data._widgetIndex, valueInt);
            }
            return JS_UNDEFINED;
        }

        static JSValue items_get(JSContext* ctx, JSValue thisVal)
        {
            JSValue arr = JS_NewArray(ctx);
            WidgetData data = GetWidgetData(thisVal);
            auto w = GetWindow(data._class, data._number);
            if (w != nullptr)
            {
                auto items = Ui::Windows::GetWidgetItems(w, data._widgetIndex);
                for (size_t i = 0; i < items.size(); i++)
                {
                    JS_SetPropertyInt64(ctx, arr, i, JSFromStdString(ctx, items[i]));
                }
            }
            return arr;
        }

        static JSValue items_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            if (!JS_IsArray(value))
            {
                JS_ThrowTypeError(ctx, "Expected array");
                return JS_EXCEPTION;
            }

            WidgetData data = GetWidgetData(thisVal);
            auto w = GetWindow(data._class, data._number);
            if (w != nullptr)
            {
                std::vector<std::string> items;
                JSIterateArray(
                    ctx, value, "items", [&items](JSContext* ctx, JSValue val) { items.push_back(JSToStdString(ctx, val)); });
                Ui::Windows::UpdateWidgetItems(w, data._widgetIndex, items);
            }
            return JS_UNDEFINED;
        }
    };

    class ScGroupBoxWidget : public ScWidget
    {
    public:
        static void AddFuncs(JSContext* ctx, JSValue obj)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("text", ScWidget::text_get, ScWidget::text_set),
            };
            JS_SetPropertyFunctionList(ctx, obj, funcs, std::size(funcs));
        }
    };

    class ScLabelWidget : public ScWidget
    {
    public:
        static void AddFuncs(JSContext* ctx, JSValue obj)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("text", ScWidget::text_get, ScWidget::text_set),
                JS_CGETSET_DEF("textAlign", ScLabelWidget::textAlign_get, ScLabelWidget::textAlign_set),
            };
            JS_SetPropertyFunctionList(ctx, obj, funcs, std::size(funcs));
        }

    private:
        static JSValue textAlign_get(JSContext* ctx, JSValue thisVal)
        {
            auto* widget = GetWidget(thisVal);
            if (widget != nullptr)
            {
                if (widget->type == WidgetType::labelCentred)
                {
                    return JSFromStdString(ctx, "centred");
                }
            }
            return JSFromStdString(ctx, "left");
        }

        static JSValue textAlign_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_STR(valueStr, ctx, value);

            auto* widget = GetWidget(thisVal);
            if (widget != nullptr)
            {
                if (valueStr == "centred")
                    widget->type = WidgetType::labelCentred;
                else
                    widget->type = WidgetType::label;
            }
            return JS_UNDEFINED;
        }
    };
    /* TODO (mber)
    class ScListViewWidget : public ScWidget
    {
    public:
        ScListViewWidget(WindowClass c, rct_windownumber n, WidgetIndex widgetIndex)
            : ScWidget(c, n, widgetIndex)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_set_base_class<ScWidget, ScListViewWidget>(ctx);
            dukglue_register_property(ctx, &ScListViewWidget::canSelect_get, &ScListViewWidget::canSelect_set, "canSelect");
            dukglue_register_property(ctx, &ScListViewWidget::isStriped_get, &ScListViewWidget::isStriped_set, "isStriped");
            dukglue_register_property(ctx, &ScListViewWidget::scrollbars_get, &ScListViewWidget::scrollbars_set, "scrollbars");
            dukglue_register_property(
                ctx, &ScListViewWidget::showColumnHeaders_get, &ScListViewWidget::showColumnHeaders_set, "showColumnHeaders");
            dukglue_register_property(ctx, &ScListViewWidget::highlightedCell_get, nullptr, "highlightedCell");
            dukglue_register_property(
                ctx, &ScListViewWidget::selectedCell_get, &ScListViewWidget::selectedCell_set, "selectedCell");
            dukglue_register_property(ctx, &ScListViewWidget::columns_get, &ScListViewWidget::columns_set, "columns");
            dukglue_register_property(ctx, &ScListViewWidget::items_get, &ScListViewWidget::items_set, "items");
        }

    private:
        bool canSelect_get() const
        {
            auto listView = GetListView();
            if (listView != nullptr)
            {
                return listView->CanSelect;
            }
            return false;
        }

        void canSelect_set(bool value)
        {
            auto listView = GetListView();
            if (listView != nullptr)
            {
                listView->CanSelect = value;
            }
        }

        bool isStriped_get() const
        {
            auto listView = GetListView();
            if (listView != nullptr)
            {
                return listView->IsStriped;
            }
            return false;
        }

        void isStriped_set(bool value)
        {
            auto listView = GetListView();
            if (listView != nullptr)
            {
                listView->IsStriped = value;
            }
        }

        DukValue scrollbars_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto scrollType = ScrollbarType::None;
            auto listView = GetListView();
            if (listView != nullptr)
            {
                scrollType = listView->GetScrollbars();
            }
            return ToDuk(ctx, scrollType);
        }

        void scrollbars_set(const DukValue& value)
        {
            auto listView = GetListView();
            if (listView != nullptr)
            {
                listView->SetScrollbars(FromDuk<ScrollbarType>(value));
            }
        }

        bool showColumnHeaders_get() const
        {
            auto listView = GetListView();
            if (listView != nullptr)
            {
                return listView->ShowColumnHeaders;
            }
            return false;
        }

        void showColumnHeaders_set(bool value)
        {
            auto listView = GetListView();
            if (listView != nullptr)
            {
                listView->ShowColumnHeaders = value;
            }
        }

        DukValue highlightedCell_get()
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto listView = GetListView();
            if (listView != nullptr)
            {
                return ToDuk(ctx, listView->LastHighlightedCell);
            }
            return ToDuk(ctx, nullptr);
        }

        DukValue selectedCell_get()
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto listView = GetListView();
            if (listView != nullptr)
            {
                return ToDuk(ctx, listView->SelectedCell);
            }
            return ToDuk(ctx, nullptr);
        }

        void selectedCell_set(const DukValue& value)
        {
            auto listView = GetListView();
            if (listView != nullptr)
            {
                listView->SelectedCell = FromDuk<std::optional<RowColumn>>(value);
            }
        }

        std::vector<std::vector<std::string>> items_get()
        {
            std::vector<std::vector<std::string>> result;
            auto listView = GetListView();
            if (listView != nullptr)
            {
                for (const auto& item : listView->GetItems())
                {
                    result.push_back(item.Cells);
                }
            }
            return result;
        }

        void items_set(const DukValue& value)
        {
            auto listView = GetListView();
            if (listView != nullptr)
            {
                listView->SetItems(FromDuk<std::vector<ListViewItem>>(value));
            }
        }

        std::vector<DukValue> columns_get()
        {
            std::vector<DukValue> result;
            auto listView = GetListView();
            if (listView != nullptr)
            {
                auto ctx = GetContext()->GetScriptEngine().GetContext();
                for (const auto& column : listView->GetColumns())
                {
                    result.push_back(ToDuk(ctx, column));
                }
            }
            return result;
        }

        void columns_set(const DukValue& value)
        {
            auto listView = GetListView();
            if (listView != nullptr)
            {
                listView->SetColumns(FromDuk<std::vector<ListViewColumn>>(value));
            }
        }

        CustomListView* GetListView() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return GetCustomListView(w, _widgetIndex);
            }
            return nullptr;
        }
    };
    */
    class ScSpinnerWidget : public ScWidget
    {
    public:
        static void AddFuncs(JSContext* ctx, JSValue obj)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("text", ScWidget::text_get, ScWidget::text_set),
            };
            JS_SetPropertyFunctionList(ctx, obj, funcs, std::size(funcs));
        }
    };

    class ScTextBoxWidget : public ScWidget
    {
    public:
        static void AddFuncs(JSContext* ctx, JSValue obj)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("maxLength", ScTextBoxWidget::maxLength_get, ScTextBoxWidget::maxLength_set),
                JS_CGETSET_DEF("text", ScWidget::text_get, ScWidget::text_set), JS_CFUNC_DEF("focus", 0, ScTextBoxWidget::focus)
            };
            JS_SetPropertyFunctionList(ctx, obj, funcs, std::size(funcs));
        }

    private:
        static JSValue maxLength_get(JSContext* ctx, JSValue thisVal)
        {
            WidgetData data = GetWidgetData(thisVal);
            auto w = GetWindow(data._class, data._number);
            if (IsCustomWindow(w))
            {
                return JS_NewInt32(ctx, Ui::Windows::GetWidgetMaxLength(w, data._widgetIndex));
            }
            return JS_NewInt32(ctx, 0);
        }

        static JSValue maxLength_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_INT32(valueInt, ctx, value);

            WidgetData data = GetWidgetData(thisVal);
            auto w = GetWindow(data._class, data._number);
            if (IsCustomWindow(w))
            {
                Ui::Windows::SetWidgetMaxLength(w, data._widgetIndex, valueInt);
            }
            return JS_UNDEFINED;
        }

        static JSValue focus(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            WidgetData data = GetWidgetData(thisVal);
            auto w = GetWindow(data._class, data._number);
            if (IsCustomWindow(w))
            {
                Ui::Windows::WindowStartTextbox(
                    *w, data._widgetIndex, GetWidget(thisVal)->string, Ui::Windows::GetWidgetMaxLength(w, data._widgetIndex));
            }
            return JS_UNDEFINED;
        }
    };

    class ScViewportWidget : public ScWidget
    {
    public:
        static void AddFuncs(JSContext* ctx, JSValue obj)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("viewport", ScViewportWidget::viewport_get, nullptr),
            };
            JS_SetPropertyFunctionList(ctx, obj, funcs, std::size(funcs));
        }

    private:
        static JSValue viewport_get(JSContext* ctx, JSValue thisVal)
        {
            WidgetData data = GetWidgetData(thisVal);
            auto w = GetWindow(data._class, data._number);
            if (IsCustomWindow(w))
            {
                auto widget = GetWidget(w, data._widgetIndex);
                if (widget != nullptr && widget->type == WidgetType::viewport)
                {
                    // TODO (mber)
                    // return std::make_shared<ScViewport>(w->classification, w->number);
                }
            }
            return JS_NULL;
        }
    };

    inline JSValue ScWidget::New(JSContext* ctx, WindowBase* w, WidgetIndex widgetIndex)
    {
        JSValue newObj = MakeWithOpaque(ctx, funcs, new WidgetData{ w->classification, w->number, widgetIndex });
        switch (w->widgets[widgetIndex].type)
        {
            case WidgetType::button:
            case WidgetType::flatBtn:
            case WidgetType::imgBtn:
                ScButtonWidget::AddFuncs(ctx, newObj);
                break;
            case WidgetType::checkbox:
                ScCheckBoxWidget::AddFuncs(ctx, newObj);
                break;
            case WidgetType::colourBtn:
                ScColourPickerWidget::AddFuncs(ctx, newObj);
                break;
            case WidgetType::dropdownMenu:
                ScDropdownWidget::AddFuncs(ctx, newObj);
                break;
            case WidgetType::groupbox:
                ScGroupBoxWidget::AddFuncs(ctx, newObj);
                break;
            case WidgetType::label:
            case WidgetType::labelCentred:
                ScLabelWidget::AddFuncs(ctx, newObj);
                break;
            case WidgetType::scroll:
                // TODO (mber)
                // return GetObjectAsDukValue(ctx, std::make_shared<ScListViewWidget>(c, n, widgetIndex));
                break;
            case WidgetType::spinner:
                ScSpinnerWidget::AddFuncs(ctx, newObj);
                break;
            case WidgetType::textBox:
                ScTextBoxWidget::AddFuncs(ctx, newObj);
                break;
            case WidgetType::viewport:
                ScViewportWidget::AddFuncs(ctx, newObj);
                break;
            default:
                break;
        }
        return newObj;
    }

} // namespace OpenRCT2::Scripting

#endif
