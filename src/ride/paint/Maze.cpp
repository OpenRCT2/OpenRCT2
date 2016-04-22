#include "paint.h"

namespace Maze
{
    enum
    {
        SPR_MAZE_BASE_HEDGE = 21938,
        SPR_MAZE_BASE_BRICK = 21951,
        SPR_MAZE_BASE_ICE = 21964,
        SPR_MAZE_BASE_WOOD = 21977,
    };

    enum
    {
        SPR_MAZE_OFFSET_WALL_CENTER = 0,
        SPR_MAZE_OFFSET_WALL_INNER_NE_SW,
        SPR_MAZE_OFFSET_WALL_INNER_NW_SE,
        SPR_MAZE_OFFSET_WALL_TOP_LEFT,
        SPR_MAZE_OFFSET_WALL_TOP_RIGHT,
        SPR_MAZE_OFFSET_WALL_BOTTOM_RIGHT,
        SPR_MAZE_OFFSET_WALL_BOTTOM_LEFT,
        SPR_MAZE_OFFSET_COLUMN_CENTER,
        SPR_MAZE_OFFSET_COLUMN_TOP_RIGHT,
        SPR_MAZE_OFFSET_COLUMN_TOP_LEFT,
        SPR_MAZE_OFFSET_COLUMN_BOTTOM_LEFT,
        SPR_MAZE_OFFSET_COLUMN_BOTTOM_RIGHT,
        SPR_MAZE_OFFSET_COLUMN_CORNER,
    };

    void Draw(rct_ride * ride, uint8 trackType, uint8 trackSequence, uint8 direction, rct_map_element * mapElement, TileDrawingContext * context)
    {
        if (trackType != 101) {
            return;
        }

        uint16 maze_entry = mapElement->properties.track.maze_entry;
        maze_entry = rol16(maze_entry, direction * 4);

        // draw ground
        int image_id = 2485 | RCT2_GLOBAL(0x00F441A0, uint32);
        context->draw_98196C(image_id, 0, 0, 32, 32, 0, 0);

        context->DrawSupports(SUPPORT_STYLE_WOOD, direction, 0, RCT2_GLOBAL(0x00F441A4, uint32));

        RCT2_GLOBAL(0x0141E9B4, uint16) = 0xFFFF;
        RCT2_GLOBAL(0x0141E9B8, uint16) = 0xFFFF;
        RCT2_GLOBAL(0x0141E9BC, uint16) = 0xFFFF;
        RCT2_GLOBAL(0x0141E9C0, uint16) = 0xFFFF;
        RCT2_GLOBAL(0x0141E9C8, uint16) = 0xFFFF;
        RCT2_GLOBAL(0x0141E9CC, uint16) = 0xFFFF;
        RCT2_GLOBAL(0x0141E9D0, uint16) = 0xFFFF;
        RCT2_GLOBAL(0x0141E9D4, uint16) = 0xFFFF;

        int base_image_id;
        switch (ride->track_colour_supports[0]) {
            case 0:
                base_image_id = SPR_MAZE_BASE_BRICK;
                break;
            case 1:
                base_image_id = SPR_MAZE_BASE_HEDGE;
                break;
            case 2:
                base_image_id = SPR_MAZE_BASE_ICE;
                break;
            case 3:
                base_image_id = SPR_MAZE_BASE_WOOD;
                break;
        }

        base_image_id |= RCT2_GLOBAL(0x00F441A0, uint32);


        image_id = base_image_id + SPR_MAZE_OFFSET_WALL_CENTER;
        if (maze_entry & (1 << 3))
            context->draw_98197C(image_id, 2, 2, 10, 10, 9, 0, 3, 3, 2);

        if (maze_entry & (1 << 7))
            context->draw_98197C(image_id, 2, 18, 10, 10, 9, 0, 3, 19, 2);

        if (maze_entry & (1 << 11))
            context->draw_98197C(image_id, 18, 18, 10, 10, 9, 0, 19, 19, 2);

        if (maze_entry & (1 << 15))
            context->draw_98197C(image_id, 18, 2, 10, 10, 9, 0, 19, 3, 2);


        image_id = base_image_id + SPR_MAZE_OFFSET_WALL_TOP_LEFT;
        if (maze_entry & (1 << 0))
            context->draw_98197C(image_id, 2, 0, 10, 1, 9, 0, 3, 1, 2);

        if (maze_entry & (1 << 13))
            context->draw_98197C(image_id, 18, 0, 10, 1, 9, 0, 19, 1, 2);


        image_id = base_image_id + SPR_MAZE_OFFSET_WALL_BOTTOM_RIGHT;
        if (maze_entry & (1 << 5))
            context->draw_98197C(image_id, 2, 30, 10, 1, 9, 0, 3, 30, 2);

        if (maze_entry & (1 << 8))
            context->draw_98197C(image_id, 18, 30, 10, 1, 9, 0, 19, 30, 2);


        image_id = base_image_id + SPR_MAZE_OFFSET_WALL_TOP_RIGHT;
        if (maze_entry & (1 << 1))
            context->draw_98197C(image_id, 0, 2, 1, 10, 9, 0, 1, 3, 2);

        if (maze_entry & (1 << 4))
            context->draw_98197C(image_id, 0, 18, 1, 10, 9, 0, 1, 19, 2);


        image_id = base_image_id + SPR_MAZE_OFFSET_WALL_BOTTOM_LEFT;
        if (maze_entry & (1 << 12))
            context->draw_98197C(image_id, 30, 2, 1, 10, 9, 0, 30, 3, 2);

        if (maze_entry & (1 << 9))
            context->draw_98197C(image_id, 30, 18, 1, 10, 9, 0, 30, 19, 2);


        image_id = base_image_id + SPR_MAZE_OFFSET_WALL_INNER_NE_SW;
        if (maze_entry & (1 << 2))
            context->draw_98197C(image_id, 2, 14, 10, 4, 9, 0, 3, 14, 2);

        if (maze_entry & (1 << 10))
            context->draw_98197C(image_id, 18, 14, 10, 4, 9, 0, 19, 14, 2);


        image_id = base_image_id + SPR_MAZE_OFFSET_WALL_INNER_NW_SE;
        if (maze_entry & (1 << 14))
            context->draw_98197C(image_id, 14, 2, 4, 10, 9, 0, 14, 3, 2);

        if (maze_entry & (1 << 6))
            context->draw_98197C(image_id, 14, 18, 4, 10, 9, 0, 14, 19, 2);


        image_id = base_image_id + SPR_MAZE_OFFSET_COLUMN_CORNER;
        if (maze_entry & (1 << 0 | 1 << 1))
            context->draw_98197C(image_id, 0, 0, 1, 1, 9, 0, 1, 1, 2);

        if (maze_entry & (1 << 4 | 1 << 5))
            context->draw_98197C(image_id, 0, 30, 1, 1, 9, 0, 1, 30, 2);

        if (maze_entry & (1 << 8 | 1 << 9))
            context->draw_98197C(image_id, 30, 30, 1, 1, 9, 0, 30, 30, 2);

        if (maze_entry & (1 << 12 | 1 << 13))
            context->draw_98197C(image_id, 30, 0, 1, 1, 9, 0, 30, 1, 2);


        if (maze_entry & (1 << 0 | 1 << 13 | 1 << 14))
            context->draw_98197C(base_image_id + SPR_MAZE_OFFSET_COLUMN_TOP_LEFT, 14, 0, 2, 1, 9, 0, 15, 1, 2);


        if (maze_entry & (1 << 5 | 1 << 6 | 1 << 8))
            context->draw_98197C(base_image_id + SPR_MAZE_OFFSET_COLUMN_BOTTOM_RIGHT, 14, 30, 2, 1, 9, 0, 15, 30, 2);


        if (maze_entry & (1 << 1 | 1 << 2 | 1 << 4))
            context->draw_98197C(base_image_id + SPR_MAZE_OFFSET_COLUMN_TOP_RIGHT, 0, 14, 1, 2, 9, 0, 1, 15, 2);


        if (maze_entry & (1 << 9 | 1 << 10 | 1 << 12))
            context->draw_98197C(base_image_id + SPR_MAZE_OFFSET_COLUMN_BOTTOM_LEFT, 30, 14, 1, 2, 9, 0, 30, 15, 2);


        if (maze_entry & (1 << 2 | 1 << 6 | 1 << 10 | 1 << 14)) {
            context->draw_98197C(base_image_id + SPR_MAZE_OFFSET_COLUMN_CENTER, 14, 14, 2, 2, 8, 0, 15, 15, 2);

            RCT2_GLOBAL(0x141E9C4, uint16) = context->Z + 12;
            RCT2_GLOBAL(0x141E9C6, uint8) = 0x20;
        }

        context->UpdateTileMaxHeight(context->Z + 32, 0x20);
    }
}
