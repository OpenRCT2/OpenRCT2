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

    #include "ScWidget.hpp"

    #include <openrct2/interface/ColourWithFlags.h>
    #include <openrct2/interface/Window.h>
    #include <openrct2/interface/WindowBase.h>
    #include <openrct2/scripting/Duktape.hpp>

namespace OpenRCT2::Scripting
{
    using namespace OpenRCT2::Ui::Windows;

    class ScWindow
    {
    private:
        WindowClass _class;
        WindowNumber _number;

    public:
        ScWindow(WindowBase* w)
            : ScWindow(w->classification, w->number)
        {
        }

        ScWindow(WindowClass c, WindowNumber n)
            : _class(c)
            , _number(n)
        {
        }

        int32_t classification_get() const
        {
            return static_cast<int32_t>(_class);
        }

        int32_t number_get() const
        {
            return static_cast<int32_t>(_number);
        }

        int32_t x_get() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return w->windowPos.x;
            }
            return 0;
        }
        void x_set(int32_t value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                WindowSetPosition(*w, { value, w->windowPos.y });
            }
        }
        int32_t y_get() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return w->windowPos.y;
            }
            return 0;
        }
        void y_set(int32_t value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                WindowSetPosition(*w, { w->windowPos.x, value });
            }
        }
        int32_t width_get() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return w->width;
            }
            return 0;
        }
        void width_set(int32_t value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                if (w->canBeResized())
                {
                    WindowResizeByDelta(*w, value - w->width, 0);
                }
                else
                {
                    WindowSetResize(*w, { value, w->minHeight }, { value, w->maxHeight });
                }
            }
        }
        int32_t height_get() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return w->height - w->getTitleBarDiffNormal();
            }
            return 0;
        }
        void height_set(int32_t value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                value += w->getTitleBarDiffNormal();
                if (w->canBeResized())
                {
                    WindowResizeByDelta(*w, 0, value - w->height);
                }
                else
                {
                    WindowSetResize(*w, { w->minWidth, value }, { w->maxWidth, value });
                }
            }
        }
        int32_t minWidth_get() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return w->minWidth;
            }
            return 0;
        }
        void minWidth_set(int32_t value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                WindowSetResize(*w, { value, w->minHeight }, { w->maxWidth, w->maxHeight });
            }
        }
        int32_t maxWidth_get() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return w->maxWidth;
            }
            return 0;
        }
        void maxWidth_set(int32_t value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                WindowSetResize(*w, { w->minWidth, w->minHeight }, { value, w->maxHeight });
            }
        }
        int32_t minHeight_get() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return w->minHeight - w->getTitleBarDiffNormal();
            }
            return 0;
        }
        void minHeight_set(int32_t value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                value += w->getTitleBarDiffNormal();
                WindowSetResize(*w, { w->minWidth, value }, { w->maxWidth, w->maxHeight });
            }
        }
        int32_t maxHeight_get() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return w->maxHeight - w->getTitleBarDiffNormal();
            }
            return 0;
        }
        void maxHeight_set(int32_t value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                value += w->getTitleBarDiffNormal();
                WindowSetResize(*w, { w->minWidth, w->minHeight }, { w->maxWidth, value });
            }
        }
        bool isSticky_get() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return w->flags.hasAny(WindowFlag::stickToBack, WindowFlag::stickToFront);
            }
            return false;
        }

        std::vector<DukValue> widgets_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();

            std::vector<DukValue> result;
            auto w = GetWindow();
            if (w != nullptr)
            {
                for (WidgetIndex widgetIndex = 0; widgetIndex < w->widgets.size(); widgetIndex++)
                {
                    result.push_back(ScWidget::ToDukValue(ctx, w, widgetIndex));
                }
            }
            return result;
        }

        std::vector<int32_t> colours_get() const
        {
            std::vector<int32_t> result;
            auto w = GetWindow();
            if (w != nullptr)
            {
                result.reserve(std::size(w->colours));
                for (auto c : w->colours)
                {
                    auto colour = EnumValue(c.colour);
                    if (c.flags.has(ColourFlag::translucent))
                        colour |= kLegacyColourFlagTranslucent;
                    result.push_back(colour);
                }
            }
            return result;
        }
        void colours_set(std::vector<int32_t> colours)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                for (size_t i = 0; i < std::size(w->colours); i++)
                {
                    auto c = ColourWithFlags{ Drawing::Colour::black };
                    if (i < colours.size())
                    {
                        auto colour = (colours[i] & ~kLegacyColourFlagTranslucent) % Drawing::kColourNumTotal;
                        bool isTranslucent = (colours[i] & kLegacyColourFlagTranslucent);
                        c.colour = static_cast<Drawing::Colour>(colour);
                        c.flags.set(ColourFlag::translucent, isTranslucent);
                    }
                    w->colours[i] = c;
                }
                w->invalidate();
            }
        }

        std::string title_get() const
        {
            auto w = GetWindow();
            if (w != nullptr && w->classification == WindowClass::custom)
            {
                return GetWindowTitle(w);
            }
            return {};
        }
        void title_set(std::string value)
        {
            auto w = GetWindow();
            if (w != nullptr && w->classification == WindowClass::custom)
            {
                UpdateWindowTitle(w, value);
            }
        }

        int32_t tabIndex_get() const
        {
            auto w = GetWindow();
            if (w != nullptr && w->classification == WindowClass::custom)
            {
                return w->page;
            }
            return 0;
        }
        void tabIndex_set(int32_t tab)
        {
            auto w = GetWindow();
            if (w != nullptr && w->classification == WindowClass::custom)
            {
                UpdateWindowTab(w, tab);
            }
        }

        void close()
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                auto* windowMgr = Ui::GetWindowManager();
                windowMgr->Close(*w);
            }
        }

        DukValue findWidget(std::string name) const
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

        void bringToFront()
        {
            auto* w = GetWindow();
            if (w != nullptr)
            {
                auto* windowMgr = Ui::GetWindowManager();
                w = windowMgr->BringToFront(*w);
                w->flash();
            }
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScWindow::classification_get, nullptr, "classification");
            dukglue_register_property(ctx, &ScWindow::number_get, nullptr, "number");
            dukglue_register_property(ctx, &ScWindow::x_get, &ScWindow::x_set, "x");
            dukglue_register_property(ctx, &ScWindow::y_get, &ScWindow::y_set, "y");
            dukglue_register_property(ctx, &ScWindow::width_get, &ScWindow::width_set, "width");
            dukglue_register_property(ctx, &ScWindow::height_get, &ScWindow::height_set, "height");
            dukglue_register_property(ctx, &ScWindow::minWidth_get, &ScWindow::minWidth_set, "minWidth");
            dukglue_register_property(ctx, &ScWindow::maxWidth_get, &ScWindow::maxWidth_set, "maxWidth");
            dukglue_register_property(ctx, &ScWindow::minHeight_get, &ScWindow::minHeight_set, "minHeight");
            dukglue_register_property(ctx, &ScWindow::maxHeight_get, &ScWindow::maxHeight_set, "maxHeight");
            dukglue_register_property(ctx, &ScWindow::isSticky_get, nullptr, "isSticky");
            dukglue_register_property(ctx, &ScWindow::widgets_get, nullptr, "widgets");
            dukglue_register_property(ctx, &ScWindow::colours_get, &ScWindow::colours_set, "colours");
            dukglue_register_property(ctx, &ScWindow::title_get, &ScWindow::title_set, "title");
            dukglue_register_property(ctx, &ScWindow::tabIndex_get, &ScWindow::tabIndex_set, "tabIndex");

            dukglue_register_method(ctx, &ScWindow::close, "close");
            dukglue_register_method(ctx, &ScWindow::findWidget, "findWidget");
            dukglue_register_method(ctx, &ScWindow::bringToFront, "bringToFront");
        }

    private:
        WindowBase* GetWindow() const
        {
            auto* windowMgr = Ui::GetWindowManager();
            return windowMgr->FindByNumber(_class, _number);
        }
    };
} // namespace OpenRCT2::Scripting

#endif
