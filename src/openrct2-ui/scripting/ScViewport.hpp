/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

    #include "../interface/Window.h"

    #include <memory>
    #include <openrct2/Context.h>
    #include <openrct2/interface/Viewport.h>
    #include <openrct2/scripting/Duktape.hpp>
    #include <openrct2/scripting/ScriptEngine.h>
    #include <openrct2/ui/UiContext.h>
    #include <openrct2/ui/WindowManager.h>
    #include <openrct2/world/Map.h>

namespace OpenRCT2::Scripting
{
    class ScViewport
    {
    private:
        WindowClass _class{};
        rct_windownumber _number{};

    public:
        ScViewport(WindowClass c, rct_windownumber n = 0)
            : _class(c)
            , _number(n)
        {
        }

    private:
        int32_t left_get() const
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                return viewport->viewPos.x;
            }
            return 0;
        }
        void left_set(int32_t value)
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                SetViewLeftTop(value, viewport->viewPos.y);
            }
        }

        int32_t top_get() const
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                return viewport->viewPos.y;
            }
            return 0;
        }
        void top_set(int32_t value)
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                SetViewLeftTop(viewport->viewPos.x, value);
            }
        }

        int32_t right_get() const
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                return viewport->viewPos.x + viewport->ViewWidth();
            }
            return 0;
        }
        void right_set(int32_t value)
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                SetViewLeftTop(value - viewport->ViewWidth(), viewport->viewPos.y);
            }
        }

        int32_t bottom_get() const
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                return viewport->viewPos.y + viewport->ViewHeight();
            }
            return 0;
        }
        void bottom_set(int32_t value)
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                SetViewLeftTop(viewport->viewPos.x, value - viewport->ViewHeight());
            }
        }

        int32_t rotation_get() const
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                return viewport->rotation;
            }
            return 0;
        }
        void rotation_set(int32_t value)
        {
            if (value >= 0 && value < 4)
            {
                auto w = GetWindow();
                if (w != nullptr)
                {
                    while (w->viewport->rotation != value)
                    {
                        ViewportRotateSingle(w, 1);
                    }
                }
            }
        }

        int32_t zoom_get() const
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                return static_cast<int8_t>(viewport->zoom);
            }
            return 0;
        }
        void zoom_set(int32_t value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                auto i8Value = static_cast<int8_t>(value);
                WindowZoomSet(*w, ZoomLevel{ i8Value }, false);
            }
        }

        uint32_t visibilityFlags_get() const
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                return viewport->flags;
            }
            return 0;
        }
        void visibilityFlags_set(uint32_t value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                auto viewport = w->viewport;
                if (viewport != nullptr)
                {
                    if (viewport->flags != value)
                    {
                        viewport->flags = value;
                        w->Invalidate();
                    }
                }
            }
        }

        DukValue getCentrePosition() const
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                auto centre = viewport->viewPos + ScreenCoordsXY{ viewport->ViewWidth() / 2, viewport->ViewHeight() / 2 };
                auto coords = ViewportPosToMapPos(centre, 24, viewport->rotation);

                auto ctx = GetContext()->GetScriptEngine().GetContext();
                auto obj = duk_push_object(ctx);
                duk_push_number(ctx, coords.x);
                duk_put_prop_string(ctx, obj, "x");
                duk_push_number(ctx, coords.y);
                duk_put_prop_string(ctx, obj, "y");
                return DukValue::take_from_stack(ctx);
            }
            return {};
        }

        void moveTo(DukValue position)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                auto viewport = w->viewport;
                if (viewport != nullptr)
                {
                    auto coords = GetCoordsFromObject(position);
                    if (coords)
                    {
                        auto screenCoords = Translate3DTo2DWithZ(viewport->rotation, *coords);
                        auto left = screenCoords.x - (viewport->ViewWidth() / 2);
                        auto top = screenCoords.y - (viewport->ViewHeight() / 2);
                        SetViewLeftTop(left, top);
                    }
                }
            }
        }

        void scrollTo(DukValue position)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                auto coords = GetCoordsFromObject(position);
                if (coords)
                {
                    WindowScrollToLocation(*w, *coords);
                }
            }
        }

    public:
        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScViewport::left_get, &ScViewport::left_set, "left");
            dukglue_register_property(ctx, &ScViewport::top_get, &ScViewport::top_set, "top");
            dukglue_register_property(ctx, &ScViewport::right_get, &ScViewport::right_set, "right");
            dukglue_register_property(ctx, &ScViewport::bottom_get, &ScViewport::bottom_set, "bottom");
            dukglue_register_property(ctx, &ScViewport::rotation_get, &ScViewport::rotation_set, "rotation");
            dukglue_register_property(ctx, &ScViewport::zoom_get, &ScViewport::zoom_set, "zoom");
            dukglue_register_property(
                ctx, &ScViewport::visibilityFlags_get, &ScViewport::visibilityFlags_set, "visibilityFlags");
            dukglue_register_method(ctx, &ScViewport::getCentrePosition, "getCentrePosition");
            dukglue_register_method(ctx, &ScViewport::moveTo, "moveTo");
            dukglue_register_method(ctx, &ScViewport::scrollTo, "scrollTo");
        }

    private:
        WindowBase* GetWindow() const
        {
            if (_class == WindowClass::MainWindow)
                return WindowGetMain();

            auto* windowMgr = GetContext()->GetUiContext()->GetWindowManager();
            return windowMgr->FindByNumber(_class, _number);
        }

        Viewport* GetViewport() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return w->viewport;
            }
            return nullptr;
        }

        void SetViewLeftTop(int32_t left, int32_t top)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                auto viewport = w->viewport;
                if (viewport != nullptr)
                {
                    viewport->viewPos.x = left;
                    viewport->viewPos.y = top;
                    viewport->flags &= ~WF_SCROLLING_TO_LOCATION;
                    w->savedViewPos.x = viewport->viewPos.x;
                    w->savedViewPos.y = viewport->viewPos.y;
                    viewport->Invalidate();
                }
            }
        }

        std::optional<CoordsXYZ> GetCoordsFromObject(DukValue position) const
        {
            if (position.type() == DukValue::Type::OBJECT)
            {
                auto dukX = position["x"];
                auto dukY = position["y"];
                auto dukZ = position["z"];
                if (dukX.type() == DukValue::Type::NUMBER && dukY.type() == DukValue::Type::NUMBER)
                {
                    auto x = dukX.as_int();
                    auto y = dukY.as_int();
                    if (dukZ.type() == DukValue::Type::NUMBER)
                    {
                        return CoordsXYZ(x, y, dukZ.as_int());
                    }

                    auto z = TileElementHeight(CoordsXY(x, y));
                    return CoordsXYZ(x, y, z);
                }
            }
            return std::nullopt;
        }
    };
} // namespace OpenRCT2::Scripting

#endif
