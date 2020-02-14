/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../interface/Window.h"

#include <memory>
#include <openrct2/Context.h>
#include <openrct2/common.h>
#include <openrct2/interface/Viewport.h>
#include <openrct2/scripting/Duktape.hpp>
#include <openrct2/scripting/ScriptEngine.h>
#include <openrct2/world/Map.h>

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
        int32_t left_get()
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                return viewport->view_x;
            }
            return 0;
        }
        void left_set(int32_t value)
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                SetViewLeftTop(value, viewport->view_y);
            }
        }

        int32_t top_get()
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                return viewport->view_y;
            }
            return 0;
        }
        void top_set(int32_t value)
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                SetViewLeftTop(viewport->view_x, value);
            }
        }

        int32_t right_get()
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                return viewport->view_x + viewport->view_width;
            }
            return 0;
        }
        void right_set(int32_t value)
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                SetViewLeftTop(value - viewport->view_width, viewport->view_y);
            }
        }

        int32_t bottom_get()
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                return viewport->view_y + viewport->view_height;
            }
            return 0;
        }
        void bottom_set(int32_t value)
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                SetViewLeftTop(viewport->view_x, value - viewport->view_height);
            }
        }

        int32_t rotation_get()
        {
            return get_current_rotation();
        }
        void rotation_set(int32_t value)
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

        int32_t zoom_get()
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                return viewport->zoom;
            }
            return 0;
        }
        void zoom_set(int32_t value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                window_zoom_set(w, value, false);
            }
        }

        uint32_t visibilityFlags_get()
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

        DukValue getCentrePosition()
        {
            auto viewport = GetViewport();
            if (viewport != nullptr)
            {
                auto centreX = viewport->view_x + (viewport->view_width / 2);
                auto centreY = viewport->view_y + (viewport->view_height / 2);
                auto coords = viewport_coord_to_map_coord(centreX, centreY, 24);

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
                        auto screenCoords = translate_3d_to_2d_with_z(get_current_rotation(), *coords);
                        auto left = screenCoords.x - (viewport->view_width / 2);
                        auto top = screenCoords.y - (viewport->view_height / 2);
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
                    window_scroll_to_location(w, coords->x, coords->y, coords->z);
                }
            }
        }

    public:
        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &left_get, &left_set, "left");
            dukglue_register_property(ctx, &top_get, &top_set, "top");
            dukglue_register_property(ctx, &right_get, &right_set, "right");
            dukglue_register_property(ctx, &bottom_get, &bottom_set, "bottom");
            dukglue_register_property(ctx, &rotation_get, &rotation_set, "rotation");
            dukglue_register_property(ctx, &zoom_get, &zoom_set, "zoom");
            dukglue_register_property(ctx, &visibilityFlags_get, &visibilityFlags_set, "visibilityFlags");
            dukglue_register_method(ctx, &getCentrePosition, "getCentrePosition");
            dukglue_register_method(ctx, &moveTo, "moveTo");
            dukglue_register_method(ctx, &scrollTo, "scrollTo");
        }

    private:
        rct_window* GetWindow()
        {
            if (_class == WC_MAIN_WINDOW)
                return window_get_main();
            else
                return window_find_by_number(_class, _number);
        }

        rct_viewport* GetViewport()
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
                    viewport->view_x = left;
                    viewport->view_y = top;
                    viewport->flags &= ~WF_SCROLLING_TO_LOCATION;
                    w->saved_view_x = viewport->view_x;
                    w->saved_view_y = viewport->view_y;
                }
            }
        }

        std::optional<CoordsXYZ> GetCoordsFromObject(DukValue position)
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
                    else
                    {
                        auto z = tile_element_height(CoordsXY(x, y));
                        return CoordsXYZ(x, y, z);
                    }
                }
            }
            return {};
        }
    };
} // namespace OpenRCT2::Scripting
