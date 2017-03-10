#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* OpenRCT2 is the work of many authors, a full list can be found in contributors.md
* For more information, visit https://github.com/OpenRCT2/OpenRCT2
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* A full copy of the GNU General Public License can be found in licence.txt
*****************************************************************************/
#pragma endregion

#include "../network/network.h"

extern "C"
{
    #include "../audio/audio.h"
    #include "../scenario/scenario.h"
    #include "../util/util.h"
    #include "sprite.h"
}

static bool sprite_is_balloon(rct_sprite * sprite)
{
    return sprite != nullptr &&
           sprite->balloon.sprite_identifier == SPRITE_IDENTIFIER_MISC &&
           sprite->balloon.misc_identifier == SPRITE_MISC_BALLOON;
}

static void balloon_pop(rct_balloon * balloon)
{
    balloon->popped = 1;
    balloon->frame = 0;
    audio_play_sound_at_location(SOUND_BALLOON_POP, balloon->x, balloon->y, balloon->z);
}

/**
*
*  rct2: 0x006E88ED
*/
static void balloon_press(rct_balloon * balloon)
{
    if (balloon->popped != 1)
    {
        uint32 random = scenario_rand();
        if ((balloon->sprite_index & 7) || (random & 0xFFFF) < 0x2000)
        {
            balloon_pop(balloon);
        }
        else
        {
            sint16 shift = ((random & 0x80000000) ? -6 : 6);
            sprite_move(balloon->x + shift,
                        balloon->y,
                        balloon->z,
                        (rct_sprite *)balloon);
        }
    }
}

static sint32 balloon_press(uint16 spriteIndex, uint8 flags)
{
    rct_sprite * sprite = try_get_sprite(spriteIndex);
    if (!sprite_is_balloon(sprite))
    {
        log_error("Tried getting invalid sprite for balloon: %u", spriteIndex);
        return MONEY32_UNDEFINED;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        if (sprite_is_balloon(sprite))
        {
            balloon_press(&sprite->balloon);
        }
    }
    return 0;
}

extern "C"
{
    /**
    *
    *  rct2: 0x006736C7
    */
    void create_balloon(sint32 x, sint32 y, sint32 z, sint32 colour, uint8 bl)
    {
        rct_sprite* sprite = create_sprite(2);
        if (sprite != nullptr)
        {
            sprite->balloon.var_14 = 13;
            sprite->balloon.var_09 = 22;
            sprite->balloon.var_15 = 11;
            sprite->balloon.sprite_identifier = SPRITE_IDENTIFIER_MISC;
            sprite_move(x, y, z, sprite);
            sprite->balloon.misc_identifier = SPRITE_MISC_BALLOON;
            sprite->balloon.frame = 0;
            sprite->balloon.colour = colour;
            sprite->balloon.popped = bl;
        }
    }

    /**
    *
    *  rct2: 0x0067342C
    */
    void balloon_update(rct_balloon * balloon)
    {
        invalidate_sprite_2((rct_sprite *)balloon);
        if (balloon->popped == 1)
        {
            balloon->frame += 256;
            if (balloon->frame >= 1280)
            {
                sprite_remove((rct_sprite *)balloon);
            }
        }
        else
        {
            sint32 original_var26a = balloon->var_26a;
            balloon->var_26a += 85;
            if (original_var26a >= 255 - 85)
            {
                balloon->var_26b++;
                sprite_move(balloon->x, balloon->y, balloon->z + 1, (rct_sprite*)balloon);

                sint32 maxZ = 1967 - ((balloon->x ^ balloon->y) & 31);
                if (balloon->z >= maxZ)
                {
                    balloon_pop(balloon);
                }
            }
        }
    }

    void game_command_balloon_press(sint32 * eax, sint32 * ebx, sint32 * ecx, sint32 * edx, sint32 * esi, sint32 * edi, sint32 * ebp)
    {
        *ebx = balloon_press(*eax & 0xFFFF, *ebx);
    }
}
