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

#    include "ScWidget.hpp"

#    include <openrct2/common.h>
#    include <openrct2/interface/Window.h>
#    include <openrct2/interface/Window_internal.h>
#    include <openrct2/localisation/Language.h>
#    include <openrct2/scripting/Duktape.hpp>

namespace OpenRCT2::Scripting
{
    using namespace OpenRCT2::Ui::Windows;

    class ScWindow
    {
    private:
        rct_windowclass _class;
        rct_windownumber _number;

    public:
        ScWindow(rct_window* w)
            : ScWindow(w->classification, w->number)
        {
        }

        ScWindow(rct_windowclass c, rct_windownumber n)
            : _class(c)
            , _number(n)
        {
        }

        int32_t ClassificationGet() const
        {
            return static_cast<int32_t>(_class);
        }

        int32_t NumberGet() const
        {
            return static_cast<int32_t>(_number);
        }

        int32_t XGet() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return w->windowPos.x;
            }
            return 0;
        }
        void XSet(int32_t value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                window_set_position(w, { value, w->windowPos.y });
            }
        }
        int32_t YGet() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return w->windowPos.y;
            }
            return 0;
        }
        void YSet(int32_t value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                window_set_position(w, { w->windowPos.x, value });
            }
        }
        int32_t WidthGet() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return w->width;
            }
            return 0;
        }
        void WidthSet(int32_t value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                window_resize(w, value - w->width, 0);
            }
        }
        int32_t HeightGet() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return w->height;
            }
            return 0;
        }
        void HeightSet(int32_t value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                window_resize(w, 0, value - w->height);
            }
        }
        int32_t MinWidthGet() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return w->min_width;
            }
            return 0;
        }
        void MinWidthSet(int32_t value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                window_set_resize(w, value, w->min_height, w->max_width, w->max_height);
            }
        }
        int32_t MaxWidthGet() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return w->max_width;
            }
            return 0;
        }
        void MaxWidthSet(int32_t value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                window_set_resize(w, w->min_width, w->min_height, value, w->max_height);
            }
        }
        int32_t MinHeightGet() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return w->min_height;
            }
            return 0;
        }
        void MinHeightSet(int32_t value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                window_set_resize(w, w->min_width, value, w->max_width, w->max_height);
            }
        }
        int32_t MaxHeightGet() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return w->max_height;
            }
            return 0;
        }
        void MaxHeightSet(int32_t value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                window_set_resize(w, w->min_width, w->min_height, w->max_width, value);
            }
        }
        bool IsStickyGet() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return (w->flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT)) != 0;
            }
            return false;
        }

        std::vector<DukValue> WidgetsGet() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();

            std::vector<DukValue> result;
            auto w = GetWindow();
            if (w != nullptr)
            {
                rct_widgetindex widgetIndex = 0;
                for (auto widget = w->widgets; widget->type != WindowWidgetType::Last; widget++)
                {
                    result.push_back(ScWidget::ToDukValue(ctx, w, widgetIndex));
                    widgetIndex++;
                }
            }
            return result;
        }

        std::vector<int32_t> ColoursGet() const
        {
            std::vector<int32_t> result;
            auto w = GetWindow();
            if (w != nullptr)
            {
                result.reserve(std::size(w->colours));
                for (auto c : w->colours)
                {
                    result.push_back(c);
                }
            }
            return result;
        }
        void ColoursSet(std::vector<int32_t> colours)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                for (size_t i = 0; i < std::size(w->colours); i++)
                {
                    int32_t c = COLOUR_BLACK;
                    if (i < colours.size())
                    {
                        c = std::clamp<int32_t>(BASE_COLOUR(colours[i]), COLOUR_BLACK, COLOUR_COUNT - 1);
                        if (colours[i] & COLOUR_FLAG_TRANSLUCENT)
                        {
                            c = TRANSLUCENT(c);
                        }
                    }
                    w->colours[i] = c;
                }
            }
        }

        std::string TitleGet() const
        {
            auto w = GetWindow();
            if (w != nullptr && w->classification == WC_CUSTOM)
            {
                return GetWindowTitle(w);
            }
            return {};
        }
        void TitleSet(std::string value)
        {
            auto w = GetWindow();
            if (w != nullptr && w->classification == WC_CUSTOM)
            {
                UpdateWindowTitle(w, value);
            }
        }

        int32_t TabIndexGet() const
        {
            auto w = GetWindow();
            if (w != nullptr && w->classification == WC_CUSTOM)
            {
                return w->page;
            }
            return 0;
        }

        void Close()
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                window_close(w);
            }
        }

        DukValue FindWidget(std::string name) const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto w = GetWindow();
            if (w != nullptr)
            {
                auto widgetIndex = FindWidgetIndexByName(w, name);
                if (widgetIndex)
                {
                    return ScWidget::ToDukValue(ctx, w, *widgetIndex);
                }
            }
            return GetObjectAsDukValue<ScWidget>(ctx, nullptr);
        }

        void BringToFront()
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                window_bring_to_front(w);
                w->flags |= WF_WHITE_BORDER_MASK;
            }
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScWindow::ClassificationGet, nullptr, "classification");
            dukglue_register_property(ctx, &ScWindow::NumberGet, nullptr, "number");
            dukglue_register_property(ctx, &ScWindow::XGet, &ScWindow::XSet, "x");
            dukglue_register_property(ctx, &ScWindow::YGet, &ScWindow::YSet, "y");
            dukglue_register_property(ctx, &ScWindow::WidthGet, &ScWindow::WidthSet, "width");
            dukglue_register_property(ctx, &ScWindow::HeightGet, &ScWindow::HeightSet, "height");
            dukglue_register_property(ctx, &ScWindow::MinWidthGet, &ScWindow::MinWidthSet, "minWidth");
            dukglue_register_property(ctx, &ScWindow::MaxWidthGet, &ScWindow::MaxWidthSet, "maxWidth");
            dukglue_register_property(ctx, &ScWindow::MinHeightGet, &ScWindow::MinHeightSet, "minHeight");
            dukglue_register_property(ctx, &ScWindow::MaxHeightGet, &ScWindow::MaxHeightSet, "maxHeight");
            dukglue_register_property(ctx, &ScWindow::IsStickyGet, nullptr, "isSticky");
            dukglue_register_property(ctx, &ScWindow::WidgetsGet, nullptr, "widgets");
            dukglue_register_property(ctx, &ScWindow::ColoursGet, &ScWindow::ColoursSet, "colours");
            dukglue_register_property(ctx, &ScWindow::TitleGet, &ScWindow::TitleSet, "title");
            dukglue_register_property(ctx, &ScWindow::TabIndexGet, nullptr, "tabIndex");

            dukglue_register_method(ctx, &ScWindow::Close, "close");
            dukglue_register_method(ctx, &ScWindow::FindWidget, "findWidget");
            dukglue_register_method(ctx, &ScWindow::BringToFront, "bringToFront");
        }

    private:
        rct_window* GetWindow() const
        {
            return window_find_by_number(_class, _number);
        }
    };
} // namespace OpenRCT2::Scripting

#endif
