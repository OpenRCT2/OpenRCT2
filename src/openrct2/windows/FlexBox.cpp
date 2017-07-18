
#include <Yoga.h>
#include "../common.h"
#include "../Context.h"
#include "../core/Math.hpp"


extern "C" {
#include "../interface/widget.h"
}


#define YGSizeMake(width, height) ((YGSize){(width), (height)})


namespace UI {
    class Paint {

    };

    class Canvas {
        void drawText(const utf8 *, float x, float y, Paint paint) {

        }
    };

    interface IView {
        virtual ~IView() {};


        virtual YGSize Measure(float width, YGMeasureMode widthMode,
                               float height, YGMeasureMode heightMode) {
            return {width, height};
        };

        virtual void Draw(rct_drawpixelinfo *dpi, float x, float y) abstract;
    };


    class ImageView : IView {
    private:
        uint32 _sprite;

    public:
        ImageView(uint32 sprite) {
            _sprite = sprite;
        }

        virtual void Draw(rct_drawpixelinfo *dpi, float x, float y) {
            rct_drawpixelinfo clipped = {};
            clip_drawpixelinfo(&clipped, dpi, x, y, 344, 194);

            for (int _y = 0; _y < 194;) {
                for (int _x = 0; _x < 344;) {
                    gfx_draw_sprite(&clipped, _sprite + 4, _x, _y, 0);
                    _x += g1Elements[_sprite].width;
                }

                _y += g1Elements[_sprite].height;
            }
        }
    };

    class TextView : IView {
    private:
        utf8string _text;

    public:
        TextView(const utf8 *text) {
            _text = (utf8string) text;
        }

        virtual YGSize Measure(float width, YGMeasureMode widthMode,
                               float height, YGMeasureMode heightMode) {

            gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
            if (widthMode == YGMeasureModeUndefined) {
                width = gfx_get_string_width(_text);
            } else if (widthMode == YGMeasureModeAtMost) {
                const float preferredWidth = gfx_get_string_width(_text);
                width = Math::Min(width, preferredWidth);
            }

            if (heightMode == YGMeasureModeUndefined) {
                height = string_get_height_raw(_text);
            } else if (heightMode == YGMeasureModeAtMost) {
                const float preferredHeight = string_get_height_raw(_text);
                height = Math::Min(height, preferredHeight);
            }

            return YGSizeMake(width, height);
        }

        virtual void Draw(rct_drawpixelinfo *dpi, float x, float y) {
            text_palette[0] = 0;
            text_palette[1] = ColourMapA[COLOUR_WHITE].colour_11; // Main colour
            text_palette[2] = 0; // Top shadow
            text_palette[3] = 0; // bottom shadow
            text_palette[4] = 0;
            text_palette[5] = 0;
            text_palette[6] = 0;
            text_palette[7] = 0;

            printf("%f, %f\n", x, y);
            gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
            gfx_draw_string(dpi, _text, TEXT_COLOUR_255, x, y);
        }
    };

    interface IWindow {
    public:
        virtual ~IWindow() {};
        // virtual Node render() abstract;
    };
}

namespace FlexBox {

    static rct_widget _flexboxWidgets[] = {
            {WIDGETS_END},
    };

    static void window_flexbox_paint(rct_window *w, rct_drawpixelinfo *dpi);

    static void window_flexbox_resize(rct_window *w);

    static void window_flexbox_invalidate(rct_window *w);

    static rct_window_event_list window_flexbox_events = {
            .close =            nullptr,
            .mouse_up =         nullptr,
            .resize =           nullptr,
            .mouse_down =       nullptr,
            .dropdown =         nullptr,
            .unknown_05 =       nullptr,
            .update =           nullptr,
            .unknown_07 =       nullptr,
            .unknown_08 =       nullptr,
            .tool_update =      nullptr,
            .tool_down =        nullptr,
            .tool_drag =        nullptr,
            .tool_up =          nullptr,
            .tool_abort =       nullptr,
            .unknown_0E =       nullptr,
            .get_scroll_size =  nullptr,
            .scroll_mousedown = nullptr,
            .scroll_mousedrag = nullptr,
            .scroll_mouseover = nullptr,
            .text_input =       nullptr,
            .viewport_rotate =  nullptr,
            .unknown_15 =       nullptr,
            .tooltip =          nullptr,
            .cursor =           nullptr,
            .moved =            nullptr,
            .invalidate =       nullptr,
            .paint =            window_flexbox_paint,
            .scroll_paint =     nullptr
    };

/**
 * Creates the window containing the menu buttons on the title screen.
 *  rct2: 0x0066B5C0 (part of 0x0066B3E8)
 */
    void Open() {
        rct_window *window;

        window = window_create(
                320, 32,
                320, 180,
                &window_flexbox_events,
                WC_WIDGETS_YOGA,
                WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_NO_BACKGROUND
        );
        window->widgets = _flexboxWidgets;
    }

    void fillRect(rct_drawpixelinfo *dpi, int x, int y, int width, int height, uint32 colour) {
        gfx_fill_rect(dpi, x, y, x + width - 1, y + height - 1, colour);
    }


    void drawNode(rct_window *window, rct_drawpixelinfo *dpi, YGNodeRef node, uint8 colour) {
        const float nodeTop = YGNodeLayoutGetTop(node);
        const float nodeLeft = YGNodeLayoutGetLeft(node);
        const float nodeWidth = YGNodeLayoutGetWidth(node);
        const float nodeHeight = YGNodeLayoutGetHeight(node);

        fillRect(
                dpi,
                window->x + nodeLeft,
                window->y + nodeTop,
                nodeWidth,
                nodeHeight,
                colour
        );

        return;

        const uint32 paddingFill = ColourMapA[COLOUR_BRIGHT_GREEN].mid_light | 0x1000000;
        const uint32 marginFill = ColourMapA[COLOUR_LIGHT_ORANGE].mid_light | 0x1000000;

        const float leftPadding = YGNodeLayoutGetPadding(node, YGEdgeLeft);
        const float rightPadding = YGNodeLayoutGetPadding(node, YGEdgeRight);
        const float topPadding = YGNodeLayoutGetPadding(node, YGEdgeTop);
        const float bottomPadding = YGNodeLayoutGetPadding(node, YGEdgeBottom);
        if (leftPadding != 0) {
            fillRect(
                    dpi,
                    window->x + nodeLeft,
                    window->y + nodeTop,
                    leftPadding,
                    nodeHeight,
                    paddingFill
            );
        }
        if (rightPadding != 0) {
            fillRect(
                    dpi,
                    window->x + nodeLeft + nodeWidth - rightPadding,
                    window->y + nodeTop,
                    rightPadding,
                    nodeHeight,
                    paddingFill
            );
        }

        if (topPadding != 0) {
            fillRect(
                    dpi,
                    window->x + nodeLeft + leftPadding,
                    window->y + nodeTop,
                    nodeWidth - leftPadding - topPadding,
                    topPadding,
                    paddingFill
            );
        }

        if (bottomPadding != 0) {
            fillRect(
                    dpi,
                    window->x + nodeLeft + leftPadding,
                    window->y + nodeTop + nodeHeight - bottomPadding,
                    nodeWidth - leftPadding - topPadding,
                    bottomPadding,
                    paddingFill
            );
        }

        const float leftMargin = YGNodeLayoutGetMargin(node, YGEdgeLeft);
        if (leftMargin != 0) {
            fillRect(
                    dpi,
                    window->x + nodeLeft - leftMargin,
                    window->y + nodeTop,
                    leftMargin,
                    nodeHeight,
                    marginFill
            );
        }

        const float rightMargin = YGNodeLayoutGetMargin(node, YGEdgeRight);
        if (rightMargin != 0) {
            fillRect(
                    dpi,
                    window->x + nodeLeft + nodeWidth,
                    window->y + nodeTop,
                    rightMargin,
                    nodeHeight,
                    marginFill
            );
        }
    }

/**
*
*  rct2: 0x0068D7DC
*/
    static void window_flexbox_resize(rct_window *w) {
        window_set_resize(w, w->min_width, w->min_height, w->max_width, w->max_height);
    }


    static void window_flexbox_invalidate(rct_window *w) {
        //window_flexbox_widgets[WIDX_BACKGROUND].right = w->width - 1;
        //window_flexbox_widgets[WIDX_BACKGROUND].bottom = w->height - 1;
    }

    static YGSize measureYGNode(YGNodeRef node,
                                float width,
                                YGMeasureMode widthMode,
                                float height,
                                YGMeasureMode heightMode) {
        UI::IView *ctx = (UI::IView *) YGNodeGetContext(node);

        return ctx->Measure(width, widthMode, height, heightMode);
    }

    uint8 _currentColour = COLOUR_WHITE;

    static void drawNodes(rct_drawpixelinfo *dpi, float x, float y, YGNodeRef node) {
        const float nodeTop = YGNodeLayoutGetTop(node);
        const float nodeLeft = YGNodeLayoutGetLeft(node);
        const float nodeWidth = YGNodeLayoutGetWidth(node);
        const float nodeHeight = YGNodeLayoutGetHeight(node);

        UI::IView *view = (UI::IView *) YGNodeGetContext(node);
        if (view != nullptr) {
            fillRect(
                    dpi,
                    x + nodeLeft,
                    y + nodeTop,
                    nodeWidth,
                    nodeHeight,
                    ColourMapA[_currentColour].mid_light
            );
            view->Draw(
                    dpi,
                    x + nodeLeft + YGNodeLayoutGetBorder(node, YGEdgeLeft) + YGNodeLayoutGetPadding(node, YGEdgeLeft),
                    y + nodeTop + YGNodeLayoutGetBorder(node, YGEdgeTop) + YGNodeLayoutGetPadding(node, YGEdgeTop)
            );
        }


        const int childCount = YGNodeGetChildCount(node);
        _currentColour++;
        for (int i = 0; i < childCount; i++) {
            drawNodes(dpi, x + nodeLeft, y + nodeTop, YGNodeGetChild(node, i));
        }
        _currentColour--;
    }

    static void window_flexbox_paint(rct_window *w, rct_drawpixelinfo *dpi) {

        auto action1View = UI::TextView("REALLY LONG BUTTON TEXT");
        auto action2View = UI::TextView("ACTION 2");
        auto bodyView = UI::TextView("Greyhound divisively hello coldy wonderfully marginally far" "\x05" "upon excluding.");
      //  auto imageView = UI::ImageView(SPR_TERRAIN_PATTERN_GRASS);

        YGNodeRef card = YGNodeNew();
        YGNodeStyleSetWidth(card, 344);
        YGNodeStyleSetFlexDirection(card, YGFlexDirectionColumn);

        YGNodeRef image = YGNodeNew();
        YGNodeStyleSetAspectRatio(image, 16.0 / 9.0);
        YGNodeInsertChild(card, image, 0);
        //YGNodeSetContext(image, &imageView);

        YGNodeRef actionsComponent = YGNodeNew();
        YGNodeStyleSetPadding(actionsComponent, YGEdgeAll, 8);
        YGNodeStyleSetFlexDirection(actionsComponent, YGFlexDirectionRow);
        YGNodeInsertChild(card, actionsComponent, 1);

        YGNodeRef bodyComponent = YGNodeNew();
        YGNodeStyleSetPadding(bodyComponent, YGEdgeAll, 16);
        YGNodeSetContext(bodyComponent, &bodyView);
        YGNodeSetMeasureFunc(bodyComponent, measureYGNode);
        YGNodeInsertChild(card, bodyComponent, 1);

        YGNodeRef action1 = YGNodeNew();
        YGNodeStyleSetPadding(action1, YGEdgeHorizontal, 8);
        YGNodeStyleSetPadding(action1, YGEdgeVertical, 4);
        YGNodeStyleSetMargin(action1, YGEdgeRight, 8);
        YGNodeStyleSetAlignSelf(action1, YGAlignCenter);
        YGNodeSetContext(action1, &action1View);
        YGNodeSetMeasureFunc(action1, measureYGNode);
        YGNodeInsertChild(actionsComponent, action1, 0);

        YGNodeRef action2 = YGNodeNew();
        YGNodeStyleSetPadding(action2, YGEdgeHorizontal, 8);
        YGNodeStyleSetPadding(action2, YGEdgeVertical, 4);
        YGNodeStyleSetMargin(action2, YGEdgeRight, 8);
        YGNodeStyleSetAlignSelf(action2, YGAlignCenter);
        YGNodeSetContext(action2, &action2View);
        YGNodeSetMeasureFunc(action2, measureYGNode);
        YGNodeInsertChild(actionsComponent, action2, 1);


        YGNodeCalculateLayout(card, YGUndefined, YGUndefined, YGDirectionLTR);


        drawNode(w, dpi, card, ColourMapA[COLOUR_WHITE].colour_11);
        drawNode(w, dpi, image, ColourMapA[COLOUR_GREY].colour_10);


        drawNodes(dpi, w->x, w->y, card);


        YGNodeFreeRecursive(card);
    }

}


extern "C" {
void window_flexbox_open() {
    FlexBox::Open();
}
}