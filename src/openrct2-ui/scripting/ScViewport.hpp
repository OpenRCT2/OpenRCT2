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

#    include "../interface/Window.h"

#    include <memory>
#    include <openrct2/Context.h>
#    include <openrct2/common.h>
#    include <openrct2/interface/Viewport.h>
#    include <openrct2/scripting/Duktape.hpp>
#    include <openrct2/scripting/ScriptEngine.h>
#    include <openrct2/world/Map.h>

namespace OpenRCT2::Scripting
{
    class ScViewport
    {
    private:
        rct_windowclass _class{};
        rct_windownumber _number{};

    public:
        ScViewport(rct_windowclass c, rct_windownumber n = 0)
            : _class(c)
            , _number(n)
        {
        }

    private:
        int32_t LeftGet() const
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                return viewport->viewPos.x;
            }
            return 0;
        }
        void LeftSet(int32_t value)
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                SetViewLeftTop(value, viewport->viewPos.y);
            }
        }

        int32_t TopGet() const
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                return viewport->viewPos.y;
            }
            return 0;
        }
        void TopSet(int32_t value)
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                SetViewLeftTop(viewport->viewPos.x, value);
            }
        }

        int32_t RightGet() const
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                return viewport->viewPos.x + viewport->view_width;
            }
            return 0;
        }
        void RightSet(int32_t value)
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                SetViewLeftTop(value - viewport->view_width, viewport->viewPos.y);
            }
        }

        int32_t BottomGet() const
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                return viewport->viewPos.y + viewport->view_height;
            }
            return 0;
        }
        void BottomSet(int32_t value)
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                SetViewLeftTop(viewport->viewPos.x, value - viewport->view_height);
            }
        }

        int32_t RotationGet() const
        {
            return get_current_rotation();
        }
        void RotationSet(int32_t value)
        {
            if (value >= 0 && value < 4)
            {
                auto w = GetWindow();
                if (w != nullptr)
                {
                    while (get_current_rotation() != value)
                    {
                        window_rotate_camera(w, 1);
                    }
                }
            }
        }

        int32_t ZoomGet() const
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                return static_cast<int8_t>(viewport->zoom);
            }
            return 0;
        }
        void ZoomSet(int32_t value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                window_zoom_set(w, value, false);
            }
        }

        uint32_t VisibilityFlagsGet() const
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                return viewport->flags;
            }
            return 0;
        }
        void VisibilityFlagsSet(uint32_t value)
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

        DukValue GetCentrePosition() const
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                auto centre = viewport->viewPos + ScreenCoordsXY{ viewport->view_width / 2, viewport->view_height / 2 };
                auto coords = viewport_coord_to_map_coord(centre, 24);

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

        void MoveTo(DukValue position)
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
                        auto screenCoords = translate_3d_to_2d_with_z(get_current_rotation(), *coords);
                        auto left = screenCoords.x - (viewport->view_width / 2);
                        auto top = screenCoords.y - (viewport->view_height / 2);
                        SetViewLeftTop(left, top);
                    }
                }
            }
        }

        void ScrollTo(DukValue position)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                auto coords = GetCoordsFromObject(position);
                if (coords)
                {
                    window_scroll_to_location(w, *coords);
                }
            }
        }

    public:
        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScViewport::LeftGet, &ScViewport::LeftSet, "left");
            dukglue_register_property(ctx, &ScViewport::TopGet, &ScViewport::TopSet, "top");
            dukglue_register_property(ctx, &ScViewport::RightGet, &ScViewport::RightSet, "right");
            dukglue_register_property(ctx, &ScViewport::BottomGet, &ScViewport::BottomSet, "bottom");
            dukglue_register_property(ctx, &ScViewport::RotationGet, &ScViewport::RotationSet, "rotation");
            dukglue_register_property(ctx, &ScViewport::ZoomGet, &ScViewport::ZoomSet, "zoom");
            dukglue_register_property(ctx, &ScViewport::VisibilityFlagsGet, &ScViewport::VisibilityFlagsSet, "visibilityFlags");
            dukglue_register_method(ctx, &ScViewport::GetCentrePosition, "getCentrePosition");
            dukglue_register_method(ctx, &ScViewport::MoveTo, "moveTo");
            dukglue_register_method(ctx, &ScViewport::ScrollTo, "scrollTo");
        }

    private:
        rct_window* GetWindow() const
        {
            if (_class == WC_MAIN_WINDOW)
                return window_get_main();

            return window_find_by_number(_class, _number);
        }

        rct_viewport* GetViewport() const
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

                    auto z = tile_element_height(CoordsXY(x, y));
                    return CoordsXYZ(x, y, z);
                }
            }
            return std::nullopt;
        }
    };
} // namespace OpenRCT2::Scripting

#endif
